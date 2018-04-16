//
// Created by hongchao1 on 2018/4/8.
//

#include <glog/logging.h>

#include "hlzo_def.h"
#include "hlzo_hdfs_connector.h"

namespace HLZO {

    vector<string> HLZOHdfsConnector::_connect_conf_param = {"fs.default.name", "dfs.nameservices"};

    HLZOHdfsConnector::HLZOHdfsConnector() : _fs(NULL), _hdfsBuilder(NULL) {
    }


    HLZOHdfsConnector::~HLZOHdfsConnector() {
        disconnectHdfs();
    }

    bool HLZOHdfsConnector::isEffectConf(HLZOConf& conf) {
        for (int i = 0; i < _connect_conf_param.size(); ++i) {
            if (!conf.isExist(_connect_conf_param[i])) {
                LOG(ERROR) << "conf fail, " << _connect_conf_param[i] << " no exist";
                return false;
            }
        }

        /* 下面的配置在连接hdfs时也需要(特别针对HA方式) */
        string nameservice = conf.getConf("dfs.nameservices");
        if (!conf.isExist("dfs.ha.namenodes." + nameservice)) {
            LOG(ERROR) << "conf fail, dfs.ha.namenodes." + nameservice << " no exist";
            return false;
        }
        if (!conf.isExist("dfs.namenode.rpc-address." + nameservice + ".nn1")) {
            LOG(ERROR) << "conf fail, dfs.namenode.rpc-address." + nameservice + ".nn1" << " no exist";
            return false;
        }
        if (!conf.isExist("dfs.namenode.rpc-address." + nameservice + ".nn2")) {
            LOG(ERROR) << "conf fail, dfs.namenode.rpc-address." + nameservice + ".nn2" << " no exist";
            return false;
        }
        if (!conf.isExist("dfs.client.failover.proxy.provider." + nameservice)) {
            LOG(ERROR) << "dfs.client.failover.proxy.provider." + nameservice << " no exist";
            return false;
        }

        return true;
    }

    bool HLZOHdfsConnector::initHdfsBuilder(HLZOConf& conf) {
        LOG(ERROR) << "init hdfs builder";

        _hdfsBuilder = hdfsNewBuilder();
        if (_hdfsBuilder == NULL) {
            LOG(ERROR) << "new hdfs builder fail";
            return false;
        }

        hdfsBuilderSetForceNewInstance(_hdfsBuilder);

        hdfsBuilderSetNameNode(_hdfsBuilder, conf.getConf("fs.default.name"));
        hdfsBuilderConfSetStr(_hdfsBuilder, "dfs.nameservices", conf.getConf("dfs.nameservices"));
        hdfsBuilderConfSetStr(_hdfsBuilder, "dfs.ha.namenodes.eosdip", conf.getConf("dfs.ha.namenodes.eosdip"));
        hdfsBuilderConfSetStr(_hdfsBuilder, "dfs.namenode.rpc-address.eosdip.nn1", conf.getConf("dfs.namenode.rpc-address.eosdip.nn1"));
        hdfsBuilderConfSetStr(_hdfsBuilder, "dfs.namenode.rpc-address.eosdip.nn2", conf.getConf("dfs.namenode.rpc-address.eosdip.nn2"));
        hdfsBuilderConfSetStr(_hdfsBuilder, "dfs.client.failover.proxy.provider.eosdip", conf.getConf("dfs.client.failover.proxy.provider.eosdip"));

        return true;
    }

    bool HLZOHdfsConnector::connectHdfs() {
        LOG(INFO) << "start connect hdfs";
        if (_hdfsBuilder == NULL) {
            LOG(ERROR) << "empty hdfs builder, can't connect";
            return false;
        }

        _fs = hdfsBuilderConnect(_hdfsBuilder);
        /* The HDFS builder will be freeed, whether or not the connection was successful. */
        _hdfsBuilder = NULL;
        if (!_fs) {
            LOG(ERROR) << "hdfs connect failed";
            return false;
        }

        LOG(INFO) << "success connect hdfs";
        return true;
    }

    void HLZOHdfsConnector::disconnectHdfs() {
        LOG(INFO) << "stop connect hdfs";
        if (_hdfsBuilder) {
            hdfsFreeBuilder(_hdfsBuilder);
            _hdfsBuilder = NULL;
        }

        if (_fs) {
            hdfsDisconnect(_fs);
            _fs = NULL;
        }

        return;
    }
}
