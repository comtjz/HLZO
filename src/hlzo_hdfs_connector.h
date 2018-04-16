//
// Created by hongchao1 on 2018/4/8.
//

#ifndef HLZO_HLZO_HDFS_CONNECTOR_H
#define HLZO_HLZO_HDFS_CONNECTOR_H

#include <string>
#include <map>
#include <vector>

#include "hdfs.h"

#include "hlzo_conf.h"

using std::string;
using std::map;
using std::vector;

namespace HLZO {
    class HLZOHdfsConnector {
    public:
        HLZOHdfsConnector();

        ~HLZOHdfsConnector();

        bool isEffectConf(HLZOConf& conf);
        bool initHdfsBuilder(HLZOConf& conf);

        bool connectHdfs();
        void disconnectHdfs();

        hdfsFS getHdfsFS() { return _fs; }

    private:
        /* HDFS相关 */
        hdfsFS _fs;
        struct hdfsBuilder *_hdfsBuilder;

        /* hdfs builder 配置 */
        map<string, string> _hdfsBuilderConf;

        /* 这里定义的连接hdfs的配置名是写死的 */
        /* 这两个配置写死了,连接hdfs还需要几个配置名, 可通过nameservices加规定字符串得出 */
        static vector<string> _connect_conf_param;
    };
}

#endif //HLZO_HLZO_HDFS_CONNECTOR_H
