//
// Created by hongchao1 on 2018/3/30.
//

#ifndef HLZO_MY_LOG_H
#define HLZO_MY_LOG_H

#include <string>

class HLZOMyLog {
public:
    // GLOG 配置
    HLZOMyLog(char *program);

    // GLOG 内存清理
    ~HLZOMyLog();

    void setLogDetails(const std::string& dir, bool logtostderr);
};

#endif //HLZO_MY_LOG_H
