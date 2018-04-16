//
// Created by hongchao1 on 2018/4/13.
//

#include <boost/typeof/typeof.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <glog/logging.h>

#include "hlzo_conf.h"

using namespace boost::property_tree;

namespace HLZO {

    void HLZOConf::addResource(const string &conf_file) {
        ptree pt;
        xml_parser::read_xml(conf_file, pt);

        string strTmp;

        BOOST_AUTO(child, pt.get_child("configuration"));
        for (BOOST_AUTO(pos, child.begin()); pos != child.end(); ++pos) {
            strTmp.clear();
            if ("<xmlattr>" == pos->first) {
                ;
            } else if ("<xmlcomment>" == pos->first) {
                ;
            } else {
                string name = "";
                string value = "";
                BOOST_AUTO(nextchild, pos->second.get_child(""));
                for (BOOST_AUTO(nextpos, nextchild.begin()); nextpos != nextchild.end(); ++nextpos) {
                    strTmp.clear();
                    if ("<xmlattr>" == nextpos->first) {
                        ;
                    } else if ("<xmlcomment>" == nextpos->first) {
                        ;
                    } else {
                        if ("name" == nextpos->first)
                            name = nextpos->second.data();
                        else if ("value" == nextpos->first)
                            value = nextpos->second.data();
                    }
                }

                if (!name.empty()) {
                    _conf.insert(make_pair(name, value));
                }
            }
        }
    }

    const char *HLZOConf::getConf(const string& key) {
        map<string, string>::iterator it = _conf.find(key);
        if (it == _conf.end()) {
            return "";
        } else {
            return it->second.c_str();
        }
    }
}