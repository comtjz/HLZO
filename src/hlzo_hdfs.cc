//
// Created by hongchao1 on 2018/4/11.
//

#include <glog/logging.h>
#include <fstream>
#include <iostream>

#include "hlzo_def.h"
#include "hdfs.h"
#include "hlzo_hdfs.h"

using namespace std;

namespace HLZO {

    vector<string> HLZOHdfs::listHdfsDirectory(const string& path) {
        vector<string> ret;
        if (path.empty()) {
            return ret;
        }

        int numEntries;
        hdfsFileInfo *fileInfos = hdfsListDirectory(_hdfsConnector->getHdfsFS(), path.c_str(), &numEntries);
        if (NULL == fileInfos) {
            return ret;
        }

        for (int i = 0; i < numEntries; ++i) {
            hdfsFileInfo *fileInfo = fileInfos + i;
            ret.push_back(fileInfo->mName);
        }

        hdfsFreeFileInfo(fileInfos, numEntries);

        return ret;
    }

    int HLZOHdfs::downHdfsFile(const string& path) {
        int pos = path.rfind('/');
        if (pos < - 1) {
            LOG(ERROR) << "file path error";
            return HLZO_ERR_PARAM;
        }

        string filename = path.substr(pos+1);
        ofstream outfile;
        outfile.open("./" + filename, ios::out|ios::binary);

        /* 打开hdfs文件 */
        hdfsFile readFile = hdfsOpenFile(_hdfsConnector->getHdfsFS(), path.c_str(), O_RDONLY, 0, 0, 0);
        if (!readFile) {
            LOG(ERROR) << "open hdfs file fail";
            return HLZO_ERR_OPEN;
        }
#if 1
        char buf[1024 * 256];
        ssize_t readnum = hdfsRead(_hdfsConnector->getHdfsFS(), readFile, buf, 1024 * 256);
        int blocks = readnum / 8;
        LOG(INFO) << "readnum = " << readnum;
        for (int i = 0; i < blocks; i++) {
            //string t((char *)(buf + i * 4), 4);
            long long ll = *(long long *)(buf + i * 8);
            //LOG(INFO) << "ll = " << htonll(ll);
            outfile << htonll(ll) << endl;
        }
#else
        char buf[1024 * 256];
        ssize_t readnum = hdfsRead(_hdfsConnector->getHdfsFS(), readFile, buf, 1024*256);
        while (readnum > 0) {
            outfile << string(buf, readnum);
            readnum = hdfsRead(_hdfsConnector->getHdfsFS(), readFile, buf, 1024*4);
        }
#endif
        outfile.close();

        return 1;
    }

    HLZOHdfsFileInfo HLZOHdfs::getHdfsFileInfo(const string& path) {
        HLZOHdfsFileInfo fileInfo;
        fileInfo._flag = false;

        hdfsFileInfo *info = hdfsGetPathInfo(_hdfsConnector->getHdfsFS(), path.c_str());
        if (info == nullptr) {
            LOG(ERROR) << "ge path info fail";
            return fileInfo;
        }

        fileInfo._flag = true;
        fileInfo._kind = (HLZOObjectKind) info->mKind;
        fileInfo._size = (u_int64_t) info->mSize;

        hdfsFreeFileInfo(info, 1);

        return fileInfo;
    }

    u_int64_t HLZOHdfs::getFileLength(const string &path) {
        HLZOHdfsFileInfo fileInfo = getHdfsFileInfo(path);

        if (fileInfo._flag == false || fileInfo._kind == HLZOObjectKind::KindDirectory) {
            return 0;
        }
        return (u_int64_t)fileInfo._size;
    }

    HLZOObjectKind HLZOHdfs::getFileKind(const string& path) {
        HLZOHdfsFileInfo fileInfo = getHdfsFileInfo(path);

        return fileInfo._flag ? fileInfo._kind : HLZOObjectKind::KindEmpty;
    }
}