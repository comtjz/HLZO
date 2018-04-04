//
// Created by hongchao1 on 2018/3/30.
//

#include <glog/logging.h>
#include <fstream>

#include "hlzo_def.h"
#include "hdfs_worker.h"

using namespace std;

bool HdfsWorker::initHdfsBuilderByFile(const string& conf_file) {
    LOG(INFO) << "init hdfs builder by file";
    if (load_conf(conf_file) != HLZO_OK) {
        LOG(ERROR) << "load hdfs builder conf fail";
        return false;
    }

    hdfsBuilder_ = hdfsNewBuilder();
    if (hdfsBuilder_ == NULL) {
        LOG(ERROR) << "new hdfs builder fail";
        return false;
    }

    map<string, string>::iterator iter = hdfsBuilderConf_.begin();
    while (iter != hdfsBuilderConf_.end()) {
        LOG(INFO) << iter->first << " : " << iter->second;
        if (iter->first == "fs.default.name") {
            hdfsBuilderSetNameNode(hdfsBuilder_, iter->second.c_str());
        } else {
            int ret = hdfsBuilderConfSetStr(hdfsBuilder_, iter->first.c_str(), iter->second.c_str());
            if (ret != HLZO_OK) {
                LOG(ERROR) << "hdfs builder set conf failed, key:" << iter->first << ", val:" << iter->second;
                return false;
            }
        }

        iter++;
    }

    return true;
}

bool HdfsWorker::connectHdfs() {
    if (hdfsBuilder_ == NULL) {
        LOG(ERROR) << "empty hdfs builder, can't connect";
        return false;
    }

    fs_ = hdfsBuilderConnect(hdfsBuilder_);
    /* The HDFS builder will be freeed, whether or not the connection was successful. */
    hdfsBuilder_ = NULL;
    if (!fs_) {
        LOG(ERROR) << "hdfs connect failed";
        return false;
    }

    return true;
}

void HdfsWorker::disconnectHdfs() {
    LOG(ERROR) << "stop connect hdfs";
    if (hdfsBuilder_) {
        hdfsFreeBuilder(hdfsBuilder_);
        hdfsBuilder_ = NULL;
    }

    if (fs_) {
        hdfsDisconnect(fs_);
        fs_ = NULL;
    }

    return;
}

void HdfsWorker::downloadHdfsFile(const string& file_name, size_t pos, size_t length) {
    /* 判断文件是否存在 */
    int exist = hdfsExists(fs_, file_name.c_str());
    if (exist != HLZO_OK) {
        LOG(ERROR) << "hdfs file not exist, filename:" << file_name;
        return;
    }

    /* 获取文件信息 */
    hdfsFileInfo *fileInfo = hdfsGetPathInfo(fs_, file_name.c_str());
    if (fileInfo) {
        LOG(INFO) << "file or directory: " << (char)fileInfo->mKind;
        LOG(INFO) << "the name of the file: " << fileInfo->mName;
        LOG(INFO) << "the last modification time for the file in seconds: " << fileInfo->mLastMod;
        LOG(INFO) << "the size of the file in bytes: " << fileInfo->mSize;
        LOG(INFO) << "the count of replicas: " << fileInfo->mReplication;
        LOG(INFO) << "the block size for the file: " << fileInfo->mBlockSize;
        LOG(INFO) << "the owner of the file: " << fileInfo->mOwner;
        LOG(INFO) << "the group associated with the file: " << fileInfo->mGroup;
        LOG(INFO) << "the permissions associated with the file: " << fileInfo->mPermissions;
        LOG(INFO) << "the last access time for the file in seconds: " << fileInfo->mLastAccess;
    } else {
        LOG(ERROR) << "hdfs get path info failed";
        return;
    }

    /* 检查待下载文件要求 */
    if (fileInfo->mKind == kObjectKindDirectory) {
        LOG(ERROR) << "hdfs directory not download, path:" << file_name;
        return;
    }

    if (pos > fileInfo->mSize || (pos + length) > fileInfo->mSize) {
        LOG(ERROR) << "hdfs download wrong param, pos:" << pos << ", length:" << length << ", file size:" << fileInfo->mSize;
        return;
    }
    hdfsFreeFileInfo(fileInfo, 1);

    /* 串行执行 */
    hdfsFile readFile = hdfsOpenFile(fs_, file_name.c_str(), O_RDONLY, 0, 0, 0);
    if (!readFile) {
        LOG(ERROR) << "failed to open file, path:" << file_name;
        return;
    }

    int ret = hdfsSeek(fs_, readFile, pos);
    if (ret != HLZO_OK) {
        LOG(ERROR) << "failed to seek to given offset in file, path:" << file_name << ", pos:" << pos;
        return;
    }

    // 获取文件名
    size_t slash_pos = file_name.find_last_of('/');
    string name(file_name.substr(slash_pos + 1));

    hdfs_lzo_file_ = name + "_" + std::to_string(pos) + "_" + std::to_string(length) + ".lzo";
    ofstream out(hdfs_lzo_dir_ + "/" + hdfs_lzo_file_);
    if (!out) {
        LOG(ERROR) << "failed to create lzo file";
        return;
    }

    size_t cur_num = 0;
    while (true) {
        size_t num = hdfsRead(fs_, readFile, buf_, buf_size_);
        if (num > 0) {
            if ((cur_num + num) > length) {
                out.write(buf_, length - cur_num);
            } else {
                out.write(buf_, num);
            }

            cur_num += num;
            LOG(INFO) << "read hdfs success, cur length:" << cur_num;
            if (cur_num > length) {
                break;
            }
        } else if (num == 0) {
            LOG(INFO) << "read hdfs over";
            break;
        } else {
            if (errno == EINTR) {
                continue;
            }

            LOG(ERROR) << "read hdfs occur error";
            break;
        }
    }

    if (cur_num < length) {
        LOG(ERROR) << "read hdfs data fail";
    }

    hdfsCloseFile(fs_, readFile);
    out.close();

    return;
}

void HdfsWorker::delHdfsFile() {
    string del_file = hdfs_lzo_dir_ + "/" + hdfs_lzo_file_;
    LOG(INFO) << "del lzo file, path:" << del_file;
    return;
}

int HdfsWorker::load_conf(const string &conf_path) {
    if (conf_path.empty())
        return HLZO_ERR_PARAM;

    // open config file
    FILE *conf_file = fopen(conf_path.c_str(), "r");
    if (NULL == conf_file) {
        return HLZO_ERR_OPEN;
    }

    // read config file
    size_t line_num = 0;
    char line[HLZO_LINE_MAX_LEN] = {0};
    while (NULL != fgets(line, HLZO_LINE_MAX_LEN, conf_file)) {
        line_num++;

        // empty line or comment line
        if ('\0' == line[0] || '#' == line[0] || '\n' == line[0])
            continue;

        // remove the LF at the end of line
        line[strlen(line) - 1] = '\0';

        // format error line
        char *delimiter = NULL;
        if (NULL == (delimiter = strchr(line, '='))) {
            LOG(ERROR) << "Error format of conf file! line:" << line_num;
            continue;
        }

        string key(line, delimiter - line);
        string value(delimiter + 1);

        //key = strtrim(key);
        //value = strtrim(value);
        if (key.empty() || value.empty()) {
            LOG(ERROR) << "Invalid conf of key! line_num:" << line_num <<", value:" << value;
            continue;
        }

        auto ret = hdfsBuilderConf_.insert(make_pair(key, value));
        if (!ret.second) {
            LOG(ERROR) << "Failed to put conf_map item:<" << key << ", " << value;
            continue;
        }
    }

    fclose(conf_file);
    conf_file = NULL;

    return HLZO_OK;
}