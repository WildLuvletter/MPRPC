//
// Created by turing on 24-8-20.
//

#ifndef RPCPROVIDER_H
#define RPCPROVIDER_H
#include <bits/stdc++.h>
#include "google/protobuf/service.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpConnection.h"

//框架提供专门发布rpc服务的网络对象类
class RpcProvider {
public:
    void NotifyService(google::protobuf::Service *service);

    void run();
private:

    //服务类型信息
    //包含服务对象指针和服务的所有方法
    struct ServiceInfo {
        //保存服务对象
        google::protobuf::Service* m_service;
        //保存服务方法
        //方法名字:方法
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*>m_methodMap;
    };
    //存储注册成功的服务对象和其服务方法的所有信息
    //服务对象名字:服务对象信息
    std::unordered_map<std::string,ServiceInfo>m_serviceMap;
    muduo::net::EventLoop m_eventLoop;

    void onConnection(const muduo::net::TcpConnectionPtr &conn);

    void onMessage(const muduo::net::TcpConnectionPtr &conn,muduo::net::Buffer*,muduo::Timestamp);

    //Closure的回调操作,用于序列化rpc相应和网络发送
    void sendRpcResponse(const muduo::net::TcpConnectionPtr &conn,google::protobuf::Message*);
};


#endif //RPCPROVIDER_H
