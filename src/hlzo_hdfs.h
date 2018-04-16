//
// Created by hongchao1 on 2018/4/11.
//

#ifndef HLZO_HLZO_HDFS_H
#define HLZO_HLZO_HDFS_H

#include <string>
#include <vector>

#include "hlzo_hdfs_connector.h"

using namespace std;

namespace HLZO {

    typedef enum {
        KindEmpty = 'E',
        KindFile = 'F',
        KindDirectory = 'D',
    } HLZOObjectKind;

    typedef struct {
        bool  _flag;
        HLZOObjectKind _kind;
        u_int64_t  _size;
    } HLZOHdfsFileInfo;

    class HLZOHdfs {
    public:
        HLZOHdfs(HLZOHdfsConnector *hdfsConnector) : _hdfsConnector(hdfsConnector) {};

        ~HLZOHdfs() {};


        vector<string> listHdfsDirectory(const string& path);

        int downHdfsFile(const string& path);

        HLZOHdfsFileInfo getHdfsFileInfo(const string& path);

        u_int64_t getFileLength(const string& path);
        HLZOObjectKind getFileKind(const string& path);

    private:
        HLZOHdfsConnector  *_hdfsConnector;
    };
}

#endif //HLZO_HLZO_HDFS_H
