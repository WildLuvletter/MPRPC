//
// Created by turing on 24-8-20.
//

#include "mprpcconfig.h"

void MprpcConfig::LoadConfigFile(const char *config_file) {
    FILE *pf = fopen(config_file, "r");
    if (pf == nullptr) {
        std::cerr << "Error opening config file " << config_file << std::endl;
        exit(EXIT_FAILURE);
    }
    //1.注释  2.正确的配置项 3.去掉空格
    while (!feof(pf)) {
        char buf[512] = {0};
        fgets(buf, 512, pf);

        //去掉多余的空格
        std::string src_buf(buf);
        Trim(src_buf);
        if (src_buf[0] == '#' || src_buf.empty())continue;

        int idx = src_buf.find('=');
        if (idx == -1) {
            continue;
        }
        std::string key, value;
        key = src_buf.substr(0, idx);
        Trim(key);
        int endidx=src_buf.find('\n',idx);
        value = src_buf.substr(idx + 1,endidx-idx-1);
        Trim(value);
        m_configMap.insert({key, value});
    }
    fclose(pf);
}

std::string MprpcConfig::Load(const std::string &key) {
    auto it = m_configMap.find(key);
    if (it == m_configMap.end()) {
        return "";
    }
    return it->second;
}

void MprpcConfig::Trim(std::string &src_buf) {
    int idx = src_buf.find_first_not_of(' ');
    if (idx != -1) {
        src_buf.substr(idx);
    }
    idx = src_buf.find_last_not_of(' ');
    if (idx != -1) {
        src_buf.substr(0, idx + 1);
    }
}
