//
// Created by hongchao1 on 2018/4/11.
//

#include <glog/logging.h>

#include "hlzo_lzo_index.h"

namespace HLZO {

    vector<LzoSplit> HLZOLzoIndex::getSplits(const string& lzo_file, u_int64_t data_len, u_int32_t split_num) {
        vector<LzoSplit> ret;
        if (!_hdfsConnector) {
            LOG(ERROR) << "Please connect hdfs";
            return ret;
        }

        /* 打开hdfs文件 */
        hdfsFile readFile = hdfsOpenFile(_hdfsConnector->getHdfsFS(), lzo_file.c_str(), O_RDONLY, 0, 0, 0);
        if (!readFile) {
            LOG(ERROR) << "open hdfs file fail";
            return ret;
        }

        char buf[1024 * 256] = {0};
        ssize_t readnum = hdfsRead(_hdfsConnector->getHdfsFS(), readFile, buf, 1024 * 256);
        int blocks = readnum / 8;

        if (split_num > 0) {
            int each_block = blocks / split_num;
            /* 这里根据blocks个数切分,有个问题就是切出的文件大小可能偏差很大 */
            /* (可根据文件大小切,然后根据每段的大小去blocks中找最接近的切分点) */
            if (each_block == 0) {
                each_block = 1;
            }

            int64_t start = 0;
            int64_t last = 0;
            for (int i = 0; i < split_num - 1 && i < blocks - 1; ++i) {
                last = *(int64_t *) (buf + (i + 1)* each_block * 8);
                last = ntohll(last);

                LzoSplit split;
                split._filename = lzo_file;
                split._length = last - start;
                split._start = start;

                ret.push_back(split);

                start = last;
            }

            LzoSplit split;
            split._filename = lzo_file;
            split._length = data_len - start;
            split._start = start;
            ret.push_back(split);

        } else {
            int64_t start = 0;
            int64_t length = 0;
            int64_t offset;

            int i = 0;
            while (i < blocks) {
                offset = *(int64_t *) (buf + i * 8);
                offset = ntohll(offset);
                if ((offset - start) >= _block_size) {
                    LzoSplit split;
                    split._filename = lzo_file;
                    split._length = offset - start;
                    split._start = start;

                    ret.push_back(split);

                    start = offset;
                }

                i++;
            }

            LzoSplit split;
            split._filename = lzo_file;
            split._length = data_len - start;
            split._start = start;
            ret.push_back(split);
        }

        return ret;
    }
}