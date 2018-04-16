//
// Created by hongchao1 on 2018/4/11.
//

#ifndef HLZO_HLZO_LZO_INDEX_H
#define HLZO_HLZO_LZO_INDEX_H

#include <string>

#include "hlzo_hdfs_connector.h"

using namespace std;

namespace HLZO {

    typedef struct {
        string    _filename;
        u_int64_t _start;
        u_int64_t _length;
    } LzoSplit;

    class HLZOLzoIndex {
    public:
        HLZOLzoIndex(HLZOHdfsConnector *hdfsConnector, u_int64_t block_size = 134217728)
                : _block_size(block_size), _hdfsConnector(hdfsConnector) {
        }

        ~HLZOLzoIndex() {
        }


        /* 根据index文件切分分片 */
        /* 当split_num > 0 时,且出指定个数分片 */
        vector<LzoSplit> getSplits(const string& lzo_file, u_int64_t data_len, u_int32_t split_num = 0);

    private:
        u_int64_t _block_size;

        HLZOHdfsConnector *_hdfsConnector;
    };
}

#endif //HLZO_HLZO_LZO_INDEX_H
