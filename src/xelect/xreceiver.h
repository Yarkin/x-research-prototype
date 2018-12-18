#pragma once

#include "xwrap.h"
#include "xutil.h"
#include "xblock.h"
#include "xqueue.h"

NS_BEG2(top, elect2)

typedef std::function<void (xblock_ptr_t)> on_net_block_t;

struct xmsg {
    enum_xtop_msg_type m_type;
    std::string m_data;
};
typedef std::shared_ptr<xmsg> xmsg_ptr_t;

// received ann
class xreceiver : public xmessage_handler, public std::enable_shared_from_this<xreceiver> {
    typedef xqueue_condv<xmsg_ptr_t> queue_condv_t;
public:
    // handle ann/res msg
    // TODO: hide the declare
    void on_message(const xtop_message_t &msg, const xtop_node &from) override;

public:
    void start(xdispatcher_intf_ptr_t dispatcher_ptr, xconfig_intf_ptr_t config_ptr, on_net_block_t on_net_block);
    void stop();
    void push_back(xnetann_ptr_t netann_ptr);
    void swap(xannset_ptr_t &annset_ptr);
    // TODO: drop invalid ann?
    void update_index(uint64_t index);

    // for ut
    void wait_anns(size_t count);

private:
    void thread_proc();
    void run_once();

private:
    mutex_t m_mutex;
    std::string m_name;
    bool m_started{false};
    bool m_running{false};
    uint64_t m_res_index{1};
    xannset_ptr_t m_annset_ptr{std::make_shared<xannset>()}; // TODO: contain the index?
    xdispatcher_intf_ptr_t m_dispatcher_ptr;
    xconfig_intf_ptr_t m_config_ptr;
    on_net_block_t m_on_net_block;
    xthread_ptr_t m_thread_ptr{std::make_shared<xthread>("xreceiver")};
    queue_condv_t m_queue_condv; // accept msg from on_message(thread safe)
};
typedef std::shared_ptr<xreceiver> xreceiver_ptr_t;

NS_END2
