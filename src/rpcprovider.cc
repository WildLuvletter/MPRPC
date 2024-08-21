//
// Created by turing on 24-8-20.
//

#include "rpcprovider.h"

#include <mprpcapplication.h>
#include <zookeeperutil.h>

#include "google/protobuf/descriptor.h"
#include "rpcheader.pb.h"
#include "logger.h"
/**
 *
 *搜索过程
 * service_name => ServiceInfo => service* 对象 和 method方法
 */
void RpcProvider::NotifyService(google::protobuf::Service *service) {
    ServiceInfo service_info;
    //获取服务对象的描述信息
    const google::protobuf::ServiceDescriptor *pserviceDesc = service->GetDescriptor();
    //获取服务的名字
    std::string service_name = pserviceDesc->name();
    LOG_INFO("service name: %s", service_name.c_str());
    //获取服务对象的方法数量
    int methodCnt = pserviceDesc->method_count();
    for (int i = 0; i < methodCnt; i++) {
        const google::protobuf::MethodDescriptor *pmethodDesc = pserviceDesc->method(i);
        std::string method_name = pmethodDesc->name();
        service_info.m_methodMap.insert({method_name, pmethodDesc});
        LOG_INFO("method name: %s", method_name.c_str());
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
}

void RpcProvider::run() {
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);
    muduo::net::TcpServer server(&m_eventLoop, address, "RpcProvider");

    server.setConnectionCallback(bind(&RpcProvider::onConnection, this, std::placeholders::_1));
    server.setMessageCallback(bind(&RpcProvider::onMessage, this, std::placeholders::_1,
                                   std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);
    //把当前Rpc节点上要发布的服务全部注册到zk上,让rpc client可以从zk上发现服务
    ZkClient zkCli;
    zkCli.Start();
    for(auto &sp:m_serviceMap) {
        std::string service_path="/"+sp.first;
        zkCli.Create(service_path.c_str(),nullptr,0);
        for(auto &mp:sp.second.m_methodMap) {
            std::string method_path=service_path+"/"+mp.first;
            char method_path_data[128]={0};
            sprintf(method_path_data,"%s:%d",ip.c_str(),port);
            //ZOO_EPHEMERAL临时性节点
            zkCli.Create(method_path.c_str(),method_path_data,strlen(method_path_data),ZOO_EPHEMERAL);
        }
    }

    server.start();
    m_eventLoop.loop();
}

void RpcProvider::onConnection(const muduo::net::TcpConnectionPtr &conn) {
    if (!conn->connected()) {
        conn->shutdown();
    }
}

/**
 *
 * protobuf数据格式
 *  service_name method_name args
 *  数据头包含 service_name method_name args_size
 *  header_size(4个字节)+header_str+args_str
 */
void RpcProvider::onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buffer, muduo::Timestamp) {
    std::string recv_buf = buffer->retrieveAllAsString();
    uint32_t header_size = 0;
    recv_buf.copy((char *) &header_size, 4, 0);
    std::string rpc_header_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size = 0;
    if (rpcHeader.ParseFromString(rpc_header_str)) {
        //数据头反序列化成功
        service_name = rpcHeader.service_name();
        method_name = rpcHeader.method_name();
        args_size = rpcHeader.args_size();
    } else {
        //数据头反序列化失败
        std::cout << "rpcHeader.ParseFromString error" << std::endl;
        return;
    }
    std::string args_str = recv_buf.substr(4 + header_size, args_size);

    std::cout << "===================================================" << std::endl;
    std::cout << "header_size: " << header_size << std::endl;
    std::cout << "rpc_header_str: " << rpc_header_str << std::endl;
    std::cout << "service_name: " << service_name << std::endl;
    std::cout << "method_name: " << method_name << std::endl;
    std::cout << "args_size: " << args_size << std::endl;
    std::cout << "args_str: " << args_str << std::endl;
    std::cout << "===================================================" << std::endl;

    //获取service对象和method对象
    auto it = m_serviceMap.find(service_name);
    if (it == m_serviceMap.end()) {
        std::cout << "service not found" << std::endl;
        return;
    }
    auto mit = it->second.m_methodMap.find(method_name);
    if (mit == it->second.m_methodMap.end()) {
        std::cout << "method not found" << std::endl;
        return;
    }
    google::protobuf::Service *service = it->second.m_service;
    const google::protobuf::MethodDescriptor *method = mit->second;

    //生成rpc方法调用的请求request和相应response参数
    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    if (!request->ParseFromString(args_str)) {
        std::cout << "request parse error: " << args_str << std::endl;
    }
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    //给下面method调用绑定一个Closure回调函数
    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider,
                const muduo::net::TcpConnectionPtr &,
                google::protobuf::Message *>
            (this, &RpcProvider::sendRpcResponse, conn, response);

    //在框架上根据远端rpc请求,调用当前rpc节点上发布的方法
    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::sendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response) {
    std::string response_str;
    if(response->SerializeToString(&response_str)) {
        conn->send(response_str);
        conn->shutdown();
    }
    else {
        std::cout << "response serialize error" << std::endl;
    }
    conn->shutdown();
}
