//
// Created by hongchao1 on 2018/4/8.
//

#ifndef HLZO_HLZO_HDFS_CONNECTOR_H
#define HLZO_HLZO_HDFS_CONNECTOR_H

#include <string>
#include <map>

#include "hdfs.h"
#include "hlzo_lzo_parser.h"
#include "hlzo_hdfs_file.h"

using std::string;
using std::map;

namespace HLZO {
    class HLZOHdfsConnector {
    public:
        HLZOHDfsConnector();

        virtual ~HLZOHdfsConnector();

        /* TODO 这里传入了特定lzo parser,未来如果有多种parser,可以使用继承多态 */
        void setParser(HLZOLzoparser *parser);

        bool initHdfsBuilderByFile(const string& conf_file);

        bool connectHdfs();
        void disconnectHdfs();

        int openHdfsFile(const string& file, size_t pos, size_t length);
        void closeHdfsFile();

        int64_t readHead();
        int64_t readContext(const string& file, size_t pos, size_t length);

    private:
        int load_conf(const string& conf_path);

    private:
        HLZOLzoparser *_parser;

        /* hdfs相关 */
        hdfsFS _fs;
        struct hdfsBuilder *_hdfsBuilder;

        HLZOHdfsFile _hdfsFile;

        map<string, string> _hdfsBuilderConf;
    };
}

#endif //HLZO_HLZO_HDFS_CONNECTOR_H
