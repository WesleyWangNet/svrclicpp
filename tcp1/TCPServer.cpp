#include "TCPServer.hpp"
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace std;

int TCPServer::Setup(int port, vector<int> opts)
{
    int opt = 1;
    isonline = false;
    last_closed = -1;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&svrAddr, 0, sizeof(struct sockaddr_in));

    for (int val: opts) {
        if (setsockopt(sockfd, SOL_SOCKET, val, (char *)&opt, sizeof(opt)) < 0) {
            cerr << "socket set failed" << endl;
            return -1;
        }
    }

    svrAddr.sin_family = AF_INET;
    svrAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    svrAddr.sin_port = htons(port);

    if (::bind(sockfd, (struct sockaddr *)&svrAddr, sizeof(svrAddr)) < 0) {
        cerr << "Error opt" << endl;
        return -1;
    }

    if (listen(sockfd, 5) < 0) {
        cerr << "Error listen failed" << endl;
    }

    num_client = 0;
    isonline = 0;
   
    
    return 0;
}


void TCPServer::Task(void *arg)
{
    DescriptSocket* desc = static_cast<DescriptSocket *>(arg);
    int n = 0;
    #define MSG_SIZE 1024
    char msg[MSG_SIZE] = {0};
    while(1) {
        n = recv(desc->sockfd, msg, sizeof(msg), 0);
        if (n != -1) {
            if (n == 0) {
                isonline = false;
                close(desc->sockfd);
                last_closed = desc->sockfd;
                int id = desc->id;
                sockfdSets.erase(std::remove_if(sockfdSets.begin(), sockfdSets.end(), [id](DescriptSocket *dev){
                    return id == dev->id;
                }),sockfdSets.end());
                messageSets.erase(std::remove_if(messageSets.begin(), messageSets.end(), [id](DescriptSocket *dev){
                    return id == dev->id;
                }),messageSets.end());
                delete desc;
                desc = nullptr;
                num_client--;
                break;
            } else {
                msg[n] = 0x0;
                desc->message = string(msg);
                lock_guard<std::mutex> guard(mt);
                messageSets.push_back(desc);
            }
            
        }
    }
    std::cout << "ont thread exit" << std::endl;
}


void TCPServer::Accepted()
{
    socklen_t sosize = sizeof(cliAddr);
    DescriptSocket *desc_sock = new DescriptSocket();
    desc_sock->sockfd = accept(sockfd, (struct sockaddr *)&cliAddr, &sosize);
    desc_sock->id = num_client;
    desc_sock->ip = inet_ntoa(cliAddr.sin_addr);
    sockfdSets.push_back(desc_sock);


    thread t([this, desc_sock](){
        this->Task(static_cast<void*>(desc_sock));
    });
    t.detach();
    sockthreadSets.emplace_back(move(t));
//    sockthreadSets.emplace_back(Task, desc_sock);
    isonline = true;    
    num_client++;
}

vector<DescriptSocket *>& TCPServer::GetMessage()
{
    return messageSets;
}


void TCPServer::Send(string msg, int id)
{
    send(sockfdSets[id]->sockfd, msg.c_str(), msg.length(), 0);
}


void TCPServer::Closed(void)
{
    close(sockfd);
    sockfd = -1;
}

bool TCPServer::IsOnline()
{
    return isonline;
}

string TCPServer::GetIpaddr(int id)
{
    return messageSets[id]->ip;
}

TCPServer::~TCPServer()
{

    for (auto& val:sockfdSets) {
//    for (DescriptSocket *val:sockfdSets) {
        delete val;
    }
}


