#pragma once

#include "xdefine.h"
#include "xblock.h"
#include <time.h>

NS_BEG2(top, elect2)

class xutil {
public:
    static std::string format_str(const std::string &fmt ...);

    // for struct xelect_result
    static std::string to_string(const xshard &shard);
    static std::string to_string(const xzone &zone);
    static std::string to_string(const xelect_result &res);
    static void to_json(xJson::Value &root, const xshard &shard);
    static void to_json(xJson::Value &root, const xzone &zone);
    static void to_json(xJson::Value &root, const xelect_result &res);

    static std::string get_gmtime(time_t sec);
    static std::string get_gmtime(struct timeval &tv);
    static std::string get_gmtime();
    static std::string get_localtime(time_t sec);
    static std::string get_localtime(struct timeval &tv);
    static std::string get_localtime();

    static void dump(const xelect_result &res);
    static std::string to_string(const xnetann &netann);
    static std::string to_string(const deq_ann_t &deq);

    static std::string pack_to(const xelect_result &res);
    static bool unpack_from(xelect_result &res, const std::string &str_buf);
    static std::string pack_to(const xblock &b);
    static bool unpack_from(xblock &b, const std::string &str_buf);

    static std::string pack_to(const xnetann &netann);
    static bool unpack_from(xnetann &netann, const std::string &str_buf);

    // 1 zone 1 shard: (0, 0)
    static void make_unique_shard(xelect_result &res, const std::string &local_account, const xannset &annset);
    // at most 3 zones and 2 shards every zone and scatter free nodes
    // just for testnet 11-10
    static void make_1110_shards(xelect_result &res, const std::string &local_account, const xannset &annset);
};

NS_END2
