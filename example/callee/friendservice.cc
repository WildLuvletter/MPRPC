//
// Created by turing on 24-8-21.
//
#include <bits/stdc++.h>
#include "mprpcconfig.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "friend.pb.h"

class FriendService :public fixbug::FriendServiceRpc{
public:
    std::vector<std::string>GetFriendsList(uint32_t userid) {
        std::cout << "FriendService::GetFriendsList" << std::endl;
        std::vector<std::string> vec;
        vec.push_back("gtrhrthtrh");
        vec.push_back("trhtrh");
        vec.push_back("gao yang");
        return vec;
    }

    void GetFriendLists(google::protobuf::RpcController *controller,
        const fixbug::GetFriendListsRequest *request,
        fixbug::GetFriendListsResponse *response,
        google::protobuf::Closure *done) {

        uint32_t userid = request->userid();
        std::vector<std::string> friends = GetFriendsList(userid);
        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for(std::string& name:friends) {
            std::string* p=response->add_friends();
            *p=name;
        }
        done->Run();
    }

};

int main(int argc, char **argv) {
    MprpcApplication::Init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new FriendService());
    provider.run();
}