//
// Created by turing on 24-8-20.
//
#include <bits/stdc++.h>
#include "mprpcconfig.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "user.pb.h"
class UserService :public fixbug::UserServiceRpc{
public:
    bool Login(std::string name,std::string pwd) {
        std::cout<<"login: "<<name << " " <<pwd<<std::endl;
        return true;
    }
    bool Register(uint32_t id,std::string name,std::string pwd) {
        std::cout <<"register: "<<id << " " <<name << " " <<pwd<<std::endl;
        return true;
    }
    void Login(google::protobuf::RpcController *controller,
        const fixbug::LoginRequest *request,
        fixbug::LoginResponse *response,
        google::protobuf::Closure *done) {
        std::string name = request->name();
        std::string pwd=request->pwd();

        bool login_result=Login(name,pwd);
        fixbug::ResultCode *code=response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);
        done->Run();
    }
    void Register(google::protobuf::RpcController *controller,
        const fixbug::RegisterRequest *request,
        fixbug::RegisterResponse *response,
        google::protobuf::Closure *done) {
        uint32_t id = request->id();
        std::string name = request->name();
        std::string pwd = request->pwd();
        bool result=Register(id,name,pwd);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(result);
        done->Run();
    }
private:
};

int main(int argc, char **argv) {
    MprpcApplication::Init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new UserService());
    provider.run();
}