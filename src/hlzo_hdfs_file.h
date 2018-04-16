//
// Created by hongchao1 on 2018/4/8.
//

#ifndef HLZO_HLZO_HDFS_FILE_H
#define HLZO_HLZO_HDFS_FILE_H

#include "hlzo_file.h"
#include "hlzo_lzo_header.h"
#include "hlzo_lzo_context.h"

namespace HLZO {
    class HLZOHdfsFile : public HLZOFile {
    public:
        HLZOHdfsFile();
        virtual ~HLZOHdfsFile();

        void setHdfsFS(hdfsFS fs) { _fs = fs; }

        virtual int openFile(const string& file, size_t pos, size_t length);
        virtual void closeFile();

        virtual ssize_t tell();

        virtual ssize_t checkMagic();
        virtual ssize_t checkHead();
        virtual ssize_t readContext(string& out);

        virtual ssize_t read_buf(void *vptr, size_t cnt);

    private:
        hdfsFS   _fs;
        hdfsFile _readFile;

        /* lzo文件 */
        HLZOLzoHeader  _lzoHeader;
        HLZOLzoContext _lzoContext;
    };
}

#endif //HLZO_HLZO_HDFS_FILE_H
