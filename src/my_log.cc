//
// Created by hongchao1 on 2018/3/30.
//

#include <glog/logging.h>
#include "my_log.h"

HLZOMyLog::HLZOMyLog(char *program) {
    google::InitGoogleLogging(program);
}

HLZOMyLog::~HLZOMyLog() {
    google::ShutdownGoogleLogging();
}

void HLZOMyLog::setLogDetails(const std::string& log_dir, bool logtostderr) {
    if (logtostderr) {
        google::SetStderrLogging(google::INFO);
        FLAGS_colorlogtostderr = true;
    }

    std::string log_info_dest = log_dir + "/INFO_";
    google::SetLogDestination(google::INFO, log_info_dest.c_str());

    std::string log_warn_dest = log_dir + "/WARNING_";
    google::SetLogDestination(google::WARNING, log_warn_dest.c_str());

    std::string log_error_dest = log_dir + "/ERROR_";
    google::SetLogDestination(google::ERROR, log_error_dest.c_str());

    FLAGS_logbufsecs = 0;
    FLAGS_max_log_size = 2000;
    FLAGS_stop_logging_if_full_disk = true;
}

