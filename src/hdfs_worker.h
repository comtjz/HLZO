//
// Created by hongchao1 on 2018/3/30.
//

#ifndef HLZO_HDFS_WORKER_H
#define HLZO_HDFS_WORKER_H

#include <string>
#include <map>
#include "hdfs.h"

using std::string;
using std::map;

class HdfsWorker {
public:
    HdfsWorker(const string& data_dir) : fs_(NULL), hdfsBuilder_(NULL), hdfs_lzo_dir_(data_dir) {}
    ~HdfsWorker() {
    }

    bool initHdfsBuilderByFile(const string& conf_file);

    bool connectHdfs();

    void disconnectHdfs();

    void downloadHdfsFile(const string& file_name, size_t pos, size_t length);

    string getlzofile() {
        if (hdfs_lzo_file_.empty()) {
            return "";
        }

        return hdfs_lzo_dir_ + "/" + hdfs_lzo_file_;
    }

    /**
     * 删除lzo文件(每个worker只删除自己下载的)
     */
    void delHdfsFile();

private:
    int load_conf(const string& conf_path);

private:
    const static int buf_size_ = 128 * 1024;

    hdfsFS fs_;
    struct hdfsBuilder *hdfsBuilder_;

    string hdfs_lzo_dir_;  // 存储lzo文件的文件夹
    string hdfs_lzo_file_; // 由hdfs下载的lzo文件名

    map<string, string> hdfsBuilderConf_;
    char buf_[buf_size_];
};

#endif //HLZO_HDFS_WORKER_H
