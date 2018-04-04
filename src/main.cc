#include <iostream>
#include <getopt.h>
#include <glog/logging.h>

#include "hdfs_worker.h"
#include "my_log.h"


#include "hlzo_lzo_file.h"
#include "hlzo_lzo_parser.h"


/* 入口 */
int main(int argc, char* argv[]) {
    std::cout << "Hello, World!" << std::endl;

    HLZO::HLZOLzoparser lzoparser;
    lzoparser.decompressFile("/Users/hongchao1/cpp_workspace/test_hdfs/out.lzo");
#if 0
    HLZO::HLZOLzofile lzofile;
    bool succ = lzofile.f_open("/Users/hongchao1/cpp_workspace/test_hdfs/out_01.lzo");
    if (succ) {
        std::cout << "open success" << std::endl;
    } else {
        std::cout << "open fail" << std::endl;
    }
    int ret = lzofile.magic();
    if (ret == 0) {
        std::cout << "magic success" << std::endl;
    } else {
        std::cout << "magic fail, ret = " << ret << std::endl;
    }
#endif
#if 0
    int opt;
    std::string conf_file;
    std::string hdfs_conf_file;
    while ((opt = getopt(argc, argv, "c:h:")) != -1) {
        switch (opt) {
            case 'c':
                conf_file = optarg;
                break;
            case 'h':
                hdfs_conf_file = optarg;
                break;
            default:
                break;
        }
    }

    if (hdfs_conf_file.empty()) {
        std::cerr << "Usage: " << argv[0] << " -c config_file -h hdfs_conf_file" << std::endl;
        return -1;
    }

    HLZOMyLog hlzoLog(argv[0]);
    hlzoLog.setLogDetails("./log", false);

    HdfsWorker hdfsworker("./data");
    hdfsworker.initHdfsBuilderByFile(hdfs_conf_file);
    hdfsworker.connectHdfs();
    hdfsworker.downloadHdfsFile("hdfs://eosdip/user/topweibo", 939535068, 134221229);
    hdfsworker.disconnectHdfs();
#endif
    std::cout << "GoodBye, World!" << std::endl;
    return 0;
}