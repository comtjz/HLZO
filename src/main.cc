#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "hlzo.h"

#include "hlzo_hdfs.h"
#include "hlzo_lzo_index.h"

using namespace std;

/* 入口 */
int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;

#if 1
    HLZO::HLZO hlzo;
    hlzo.addHdfsConf("./config/core-site.xml");
    hlzo.addHdfsConf("./config/hdfs-site.xml");
    hlzo.addHdfsConf("./config/yarn-site.xml");
    if (!hlzo.connectHdfs()) {
        cout << "connect hdfs fail" << endl;
    } else {
        //vector<vector<HLZO::LzoSplit>> splits = hlzo.getLzoSplit("hdfs://eosdip/user/topweibo/training/tian-gan/samples/2018-03-19/data");
        vector<vector<HLZO::LzoSplit>> splits = hlzo.getLzoSplit("hdfs://eosdip/user/topweibo/training/tian-gan/samples/2018-03-19/data/part-00000.lzo", 5);
        cout << "splits.size = " << splits.size() << endl;

        for (int i = 0; i < splits.size(); ++i) {
            bool flag = false;
            for (int j = 0; j < splits[i].size(); ++j) {
                if (!flag) {
                    cout << "path:" << splits[i][j]._filename << endl;
                    flag = true;
                }
                cout << "\tstart:" << splits[i][j]._start << "\tlength:" << splits[i][j]._length << endl;
            }
        }
    }
#endif
#if 0
    HLZO::HLZOHdfs hdfs;
    if (!hdfs.connectHdfs("./config/hdfs_config")) {
        std::cout << "connect hdfs fail" << std::endl;
    } else {
        //vector<string> infos = hdfs.listHdfsDirectory("hdfs://eosdip/user/topweibo/training/tian-gan/samples/2018-03-19/data");
        //for (int i = 0; i < infos.size(); ++i) {
        //    cout << infos[i] << endl;
        //}
        hdfs.downHdfsFile("hdfs://eosdip/user/topweibo/training/tian-gan/samples/2018-03-19/data/part-00287.lzo.index");
        hdfs.showHdfsFileInfo("hdfs://eosdip/user/topweibo/training/tian-gan/samples/2018-03-19/data/part-00287.lzo");
    }
#endif
#if 0
    HLZO::HLZO hlzo;
    if (!hlzo.connectHdfs("./config/hdfs_config")) {
        std::cout << "connect hdfs fail" << std::endl;
    } else {
        int r = hlzo.setReadHdfs("hdfs://eosdip/user/topweibo/training/tian-gan/samples/2018-03-19/data/part-00000.lzo",
                                 671237057,
                                 41901738);

        if (r == HLZO_OK) {
            string out;
            ssize_t ret;
            ofstream outfile;
            outfile.open("./test.txt", ios::out|ios::binary);
            do {
                ret = hlzo.getNext(out);
                //std::cout << out.length() << std::endl;
                outfile << out;
            } while (ret > 0);
            outfile.close();
        }
    }
#endif

    std::cout << "Bye, World!" << std::endl;
}