//
// Created by turing on 24-8-20.
//
#include <user.pb.h>

#include "bits/stdc++.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"

int main(int argc, char **argv) {
    MprpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");

    fixbug::LoginResponse response;
    stub.Login(nullptr,&request,&response,nullptr);

    if(response.result().errcode()==0) {
        std::cout << "login ok " << response.success() << std::endl;
    }
    else {
        std::cout << "login error " << response.result().errmsg() << std::endl;
    }


    fixbug::RegisterRequest req;
    req.set_id(2000);
    req.set_name("mprpc");
    req.set_pwd("66666660000");
    fixbug::RegisterResponse rsp;
    stub.Register(nullptr,&req,&rsp,nullptr);
    if(rsp.result().errcode()==0) {
        std::cout << "register ok " << rsp.success() << std::endl;
    }
    else {
        std::cout << "register error " << rsp.result().errmsg() << std::endl;
    }
    return 0;
}
