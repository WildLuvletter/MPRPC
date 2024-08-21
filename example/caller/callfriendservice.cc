//
// Created by turing on 24-8-21.
//#include <user.pb.h>

#include <friend.pb.h>

#include "bits/stdc++.h"
#include "mprpcapplication.h"
#include "mprpcchannel.h"
#include "mprpccontroller.h"

int main(int argc, char **argv) {
    MprpcApplication::Init(argc, argv);

    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());

    fixbug::GetFriendListsRequest request;
    request.set_userid(2000);

    fixbug::GetFriendListsResponse response;
    MprpcController controller;
    stub.GetFriendLists(&controller, &request, &response, nullptr);

    if(controller.Failed()) {
        std::cout << controller.ErrorText() << std::endl;
    }
    else {
        if (response.result().errcode() == 0) {
            std::cout << "Get ok "  << std::endl;
            for(auto name:response.friends()) {
                std::cout << name << std::endl;
            }
        } else {
            std::cout << "get error " << response.result().errmsg() << std::endl;
        }
    }



    return 0;
}
