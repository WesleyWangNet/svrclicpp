#include <iostream>
#include <string>
#include <thread>
#include "TCPServer.hpp"



TCPServer tcp;

void received()
{
    vector<DescriptSocket> desc = tcp.getMessage();

    while(1) {
        for (auto msg:desc) {
            

        }
        

    }
}



int main(int argc, char **argv)
{

    TCPServer tcpsvr;

    std::vector<int> opts={SO_REUSEPORT, SO_REUSEADDR};

    tcp.setup(10010, opts);

    thread t();
    

    
    return 0;
}
