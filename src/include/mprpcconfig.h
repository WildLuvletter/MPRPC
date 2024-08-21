//
// Created by turing on 24-8-20.
//

#ifndef MPRPCCONFIG_H
#define MPRPCCONFIG_H

#include <bits/stdc++.h>

class MprpcConfig {
public:
    //负责解析加载配置文件
    void LoadConfigFile(const char *config_file);

    //查询配置项信息
    std::string Load(const std::string &key);

private:
    std::unordered_map<std::string, std::string> m_configMap;

    void Trim(std::string &src_buf);
};


#endif //MPRPCCONFIG_H
