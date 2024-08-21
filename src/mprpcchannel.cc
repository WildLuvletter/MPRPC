//
// Created by turing on 24-8-20.
//

#include "mprpcchannel.h"

#include <rpcheader.pb.h>
#include <zookeeperutil.h>
#include <netinet/in.h>
#include "mprpcapplication.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "errno.h"
#include "arpa/inet.h"
#include "netinet/in.h"
#include "unistd.h"
#include "mprpccontroller.h"

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                              google::protobuf::RpcController *controller,
                              const google::protobuf::Message *request, google::protobuf::Message *response,
                              google::protobuf::Closure *done) {
    const google::protobuf::ServiceDescriptor *sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    //获取参数序列化字符串长度args_size
    std::string args_str;
    uint32_t args_size = 0;
    if (request->SerializeToString(&args_str)) {
        args_size = args_str.size();
    } else {
        controller->SetFailed("Failed to serialize request message");
        return;
    }

    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_args_size(args_size);

    std::string rpc_header_str;
    uint32_t header_size = 0;
    if (rpcHeader.SerializeToString(&rpc_header_str)) {
        header_size = rpc_header_str.size();
    } else {
        controller->SetFailed("Failed to serialize rpc header");
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char *) &header_size,4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;

    std::cout << "===================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "===================================================" << std::endl;

    int clientfd = socket(AF_INET,SOCK_STREAM, 0);
    if (clientfd == -1) {
        // std::cout << "Failed to create socket " << errno << std::endl;
        controller->SetFailed("Failed to create socket");
        return;
    }

    // std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    // uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    ZkClient zkCli;
    zkCli.Start();
    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host_data=zkCli.GetData(method_path.c_str());
    if(host_data=="") {
        controller->SetFailed("Failed to get host data");
        return;
    }
    int idx=host_data.find(":");
    if(idx==-1) {
        controller->SetFailed("address is invalid");
        return;
    }
    std::string ip=host_data.substr(0,idx);
    int port=atoi(host_data.substr(idx+1).c_str());

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    //连接rpc服务节点
    if (-1 == connect(clientfd, (struct sockaddr *) &server_addr, sizeof(server_addr))) {
        std::cout << "Failed to connect " << errno << std::endl;
        close(clientfd);
        exit(EXIT_FAILURE);
    }
    //发送rpc请求
    if (-1 == send(clientfd, send_rpc_str.c_str(), send_rpc_str.size(), 0)) {
        // std::cout << "Failed to send " << errno << std::endl;
        controller->SetFailed("Failed to send");
        close(clientfd);
        return;
    }
    //接收rpc请求的相应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1 == (recv_size = recv(clientfd, recv_buf, 1024, 0))) {
        // std::cout << "Failed to recv " << errno << std::endl;
        controller->SetFailed("Failed to recv");
        close(clientfd);
        return;
    }

    // std::string response_str(recv_buf, 0,recv_size);
    // if(!response->ParseFromString(response_str)) {
        if(!response->ParseFromArray(recv_buf, recv_size)) {
        std::cout << "Failed to parse response" << std::endl;
        close(clientfd);
        return;
    }
    close(clientfd);
}
