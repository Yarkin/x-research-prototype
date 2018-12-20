#pragma once

#include "xwrap.h"
#include "xutil.h"

NS_BEG2(top, elect2)

class xsender {
public:
    void start(xdispatcher_intf_ptr_t dispatcher_ptr, xconfig_intf_ptr_t config_ptr);
    void stop();
    void on_ann_tick_proc();
    void send_block(std::string str_block, xblock &block);
    void update_index(uint64_t index);

private:
    void send_ann(const xnetann &netann);

private:
    mutex_t m_mutex;
    bool m_started{false};
    bool m_running{false};
    uint64_t m_ann_no{0};
    uint64_t m_ann_index{1}; // TODO: genesis's index is 0, so first time is 1
    xdispatcher_intf_ptr_t m_dispatcher_ptr;
    xconfig_intf_ptr_t m_config_ptr;
};
typedef std::shared_ptr<xsender> xsender_ptr_t;

NS_END2
