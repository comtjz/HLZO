//
// Created by hongchao1 on 2018/4/8.
//

#ifndef HLZO_HLZO_H
#define HLZO_HLZO_H

#include <string>

#include "hlzo_hdfs_connector.h"
#include "hlzo_lzo_parser.h"

using std::string;

namespace HLZO {

    class HLZO {
    public:
        HLZO();

        virtual ~HLZO();

        /* 指定hdfs相关配置文件并主动打开hdfs连接 */
        /* TODO 这里通过配置文件的方式传入待连接hdfs的相关信息
         * 需要考虑是否应由调用者解析自己的hdfs配置,以传递参数的方式来调用HLZO */
        bool connectHdfs(const string& hdfs_config_file);

        /* 主动关闭hdfs连接 */
        void disconnectHdfs();

        /* 设定读取hdfs文件信息
         * 会覆盖之前设置的关于hdfs文件信息 */
        void setReadHdfs(string hdfs_file, u_int32_t start, u_int32_t len);

        /* 返回值 > 0, 本次读取并解析数据长度
         * 返回值 = 0, 指定长度文件解析完成
         * 返回值 < 0, 错误 */
        int64_t getNext(string& out);

    private:
        HLZOHdfsConnector  _hlzo_hdfs_connector;
        HLZOLzoparser      _hlzo_lzo_parser;

        string    _hdfs_file;
        u_int32_t _start;
        u_int32_t _length;

        u_int32_t _cur_start;
        u_int32_t _cur_length;

        std::mutex  _lock;
    };
}

#endif //HLZO_HLZO_H
