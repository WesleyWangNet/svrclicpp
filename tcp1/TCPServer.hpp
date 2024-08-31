#ifndef __TCPSERVER_HPP__
#define __TCPSERVER_HPP__


#include <iostream>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <thread>
#include <algorithm>
#include <cctype>
#include <mutex>



class DescriptSocket {
public:
    int sockfd = -1;
    std::string ip = "";
    int id = -1;
    std::string message;
    bool enable_message_runtime = false;
};


class TCPServer
{
public:
    int Setup(int port, std::vector<int> opts = std::vector<int>());
    std::vector<DescriptSocket *>& GetMessage();
    void Accepted();
    void Task(void *arg);
    void Send(std::string msg, int id);
    void Closed(void);
    bool IsOnline(void);
    std::string GetIpaddr(int);
    ~TCPServer();
private:
    bool isonline = false;
    int last_closed = -1;
    int sockfd = -1;
    struct sockaddr_in svrAddr;
    struct sockaddr_in cliAddr;
    int num_client = -1;
    std::vector<DescriptSocket *> sockfdSets;
    std::vector<std::thread> sockthreadSets;
    std::vector<DescriptSocket *> messageSets;
    std::mutex mt;
};

#endif
