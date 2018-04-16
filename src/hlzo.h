//
// Created by hongchao1 on 2018/4/9.
//

#ifndef HLZO_HLZO_H
#define HLZO_HLZO_H

#include <string>

#include "hlzo_hdfs_connector.h"
#include "hlzo_lzo_index.h"
#include "hlzo_hdfs_file.h"
#include "hlzo_conf.h"

using std::string;

namespace HLZO {

    class HLZO {
    public:
        HLZO();

        ~HLZO();

        /* 添加配置文件 */
        /* 配置文件比较特殊,支持core-site.xml、hdfs-site.xml、yarn-site.xml */
        /* 三个配置文件中很多字段也没有使用,项目只使用了用于连接hdfs所需的配置字段 */
        void addHdfsConf(const string& conf_file);

        /* 主动打开hdfs连接 */
        /* 每调用一次强制创建一个新的hdfs连接实例 */
        /* 项目自身不做竞争处理,需要调用者自己保证安全 */
        bool connectHdfs();

        /* 主动关闭hdfs连接 */
        void disconnectHdfs();

        /* 设定读取hdfs文件信息 */
        /* 会覆盖之前设置的关于hdfs文件信息 */
        int setReadHdfs(const string& hdfs_file, size_t start, size_t len);

        /* 返回值 > 0, 本次读取并解析数据长度
         * 返回值 = 0, 指定长度文件解析完成
         * 返回值 < 0, 错误 */
        ssize_t getNext(string& out);

        /* 传入数据文件目录,分析其中的index文件,获得每个文件的切分 */
        vector<vector<LzoSplit>> getLzoSplit(const string& directory, int split_num = 0);

    private:
        HLZOConf _conf;

        /* 维护hdfs的连接 */
        HLZOHdfsConnector _hlzo_hdfs_connector;

        /* 维护当前处理的hdfs文件 */
        HLZOHdfsFile *_hdfsFile;

        /* 正在读取的文件信息 */
        string    _hdfs_file;
        size_t    _start;
        size_t    _length;

        size_t _cur_start;
        size_t _cur_length;
    };
}

#endif //HLZO_HLZO_H
