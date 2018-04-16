//
// Created by hongchao1 on 2018/4/13.
//

#ifndef HLZO_HLZO_CONF_H
#define HLZO_HLZO_CONF_H

#include <string>

using namespace std;

namespace HLZO {

    class HLZOConf {
    public:
        /* 提供配置文件路径(xml文件) */
        void addResource(const string& conf_file);

        const char *getConf(const string& key);

        /* 检查连接hdfs的配置是否都存在 */
        bool isExist(const string& key) {
            if (_conf.find(key) != _conf.end()) {
                return true;
            }
            return false;
        }

    private:
        map<string, string> _conf;
    };
}

#endif //HLZO_HLZO_CONF_H
