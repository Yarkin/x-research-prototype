#pragma once

#include "xmessage_dispatcher.hpp"
#include "xnet.h"
#include "xdefine.h"
#include "xconfig.hpp"

NS_BEG2(top, elect2)

typedef std::function<void ()> on_message_t;

struct xwrap_message {
    const xtop_message_t &m_msg;
    const xtop_node &m_from;

    xwrap_message(const xtop_message_t &msg, const xtop_node &from) 
        : m_msg(msg), m_from(from) {}
};

typedef std::shared_ptr<xmessage_handler> xmessage_handler_ptr_t;

class xdispatcher_intf {
public:
    virtual void register_handler(const std::string &name, enum_xtop_msg_type msg_type, xmessage_handler_ptr_t handler_ptr) = 0;
    virtual void unregister_handler(const std::string &name, enum_xtop_msg_type msg_type) = 0;
    virtual void broadcast(const xtop_message_t &msg) = 0;
};
typedef std::shared_ptr<xdispatcher_intf> xdispatcher_intf_ptr_t;

class xconfig_intf {
public:
    virtual std::string get_local_account() = 0;
    virtual bool is_free_node() = 0;
    virtual bool is_edge_node() = 0;
    virtual void get_configed_shard_info(int32_t &zone_id, int32_t &shard_id) = 0;
    virtual int get_elect_cycle() = 0;
};
typedef std::shared_ptr<xconfig_intf> xconfig_intf_ptr_t;

// so hard to mock singleton
// class xlog_intf {
// public:
//     virtual void log(int level, const std::string &msg) = 0;
// };
// typedef std::shared_ptr<xlog_intf> xlog_intf_ptr_t;

class xdispatcher_wrap : public xdispatcher_intf {
public:
    void register_handler(const std::string &name, enum_xtop_msg_type msg_type, xmessage_handler_ptr_t handler_ptr) override;
    void unregister_handler(const std::string &name, enum_xtop_msg_type msg_type) override;
    void broadcast(const xtop_message_t &msg) override;

private:
    mutex_t m_mutex;
    xmessage_handler_ptr_t m_handler_ptr;
};
// typedef std::shared_ptr<xdispatcher_wrap> xdispatcher_wrap_ptr_t;

class xconfig_wrap : public xconfig_intf {
public:
    std::string get_local_account() override;
    bool is_free_node() override;
    bool is_edge_node() override;
    void get_configed_shard_info(int32_t &zone_id, int32_t &shard_id) override;
    int get_elect_cycle() override;
};
// typedef std::shared_ptr<xconfig_wrap> xconfig_wrap_ptr_t;

// class xlog_wrap : public xlog_intf {
// };

NS_END2
