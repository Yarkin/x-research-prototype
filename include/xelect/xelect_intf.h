#pragma once

#include <memory>
#include <time.h>
#include <vector>
#include <functional>
#include <stdint.h>
#include <msgpack.hpp>
#include <json/value.h>

NS_BEG2(top, elect2)

// error code
enum enum_error_code {
    enum_error_code_SUCCESS = 0,
    enum_error_code_FAILURE,
};

struct xnode {
    std::string m_account;
    int32_t m_type; // from elect::enum_node_type
    int32_t m_free_node{0}; // 0: false, else: true
    MSGPACK_DEFINE(m_account, m_type, m_free_node);
};

typedef std::vector<xnode> xnodes_t;

struct xshard {
    uint32_t m_shard_id;
    xnodes_t m_normal_work_nodes;
    xnodes_t m_normal_backup_nodes;
    MSGPACK_DEFINE(m_shard_id, m_normal_work_nodes, m_normal_backup_nodes);
};

struct xzone {
    uint32_t m_zone_id;
    xnodes_t m_advance_work_nodes;
    xnodes_t m_advance_backup_nodes;
    std::vector<xshard> m_shards;
    MSGPACK_DEFINE(m_zone_id, m_advance_work_nodes, m_advance_backup_nodes, m_shards);
};

typedef uint64_t round_no_t;

// TODO: digest and sign
struct xelect_result {
    round_no_t m_round_no;
    std::string m_account; // who give the result
    int64_t m_timestamp;
    std::vector<xzone> m_zones;
    MSGPACK_DEFINE(m_round_no, m_account, m_timestamp, m_zones);
};
typedef std::shared_ptr<xelect_result> xelect_result_ptr_t;

// xconfig must have been started
// true: get shard_info
// false: local node is not in the elect result
bool get_local_shard_info(const xelect_result &result, elect::xshard_info &shard_info);

enum enum_notify {
    enum_notify_prepare = 0, // 59:30, with elect_result
    enum_notify_switch, // 00:00, ensure there was a prepare notify before switch
};

struct xnotify {
    enum_notify m_type;
    xelect_result_ptr_t m_result_ptr; // valid in prepare
};

typedef std::function<void (const xnotify &)> on_notify_t;

class xelect_manager_intf;
typedef std::shared_ptr<xelect_manager_intf> xelect_manager_intf_ptr_t;

class xelect_manager_intf {
public:
    // get the instance
    static xelect_manager_intf_ptr_t get_instance();

    // start the service
    virtual int start() = 0;

    // register callback
    virtual int reg_notify(const std::string &name, on_notify_t on_notify) = 0;
    virtual int unreg_notify(const std::string &name) = 0;

    /*
        {"action":"trigger_elect", "method": "dump"} -> {"result" : {}}
    */
    virtual xJson::Value trigger_elect(const xJson::Value &req) = 0;
};

NS_END2
