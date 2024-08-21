//
// Created by turing on 24-8-21.
//

#ifndef MPRPCCONTROLLER_H
#define MPRPCCONTROLLER_H
#include "google/protobuf/service.h"
#include "bits/stdc++.h"

class MprpcController :public google::protobuf::RpcController{
public:
    MprpcController();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);

    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure *callback);
private:
    //RPC方法执行过程中的状态
    bool m_failed;
    //RPC方法执行过程中的错误信息
    std::string m_errText;
};



#endif //MPRPCCONTROLLER_H
