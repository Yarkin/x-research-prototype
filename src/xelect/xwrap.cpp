#include "xwrap.h"

NS_BEG2(top, elect2)

// static std::once_flag g_once_flag;
// xdispatcher_wrap *g_dispatch_wrap;

// xdispatcher_wrap *xdispatcher_wrap::get_instance() {
//     std::call_once(g_once_flag, []{ g_dispatch_wrap = new xdispatcher_wrap; });
//     return g_dispatch_wrap;
// }

void xdispatcher_wrap::register_handler(const std::string &name, enum_xtop_msg_type msg_type, xmessage_handler_ptr_t handler_ptr) {
    xmessage_dispatcher::get_instance().register_handler(msg_type, handler_ptr);
    guard_t guard(m_mutex);
    m_handler_ptr = handler_ptr;
}

void xdispatcher_wrap::unregister_handler(const std::string &name, enum_xtop_msg_type msg_type) {
    // TODO: not provided
}

void xdispatcher_wrap::broadcast(const xtop_message_t &msg) {
    // send to self first
    {
        guard_t guard(m_mutex);
        if (m_handler_ptr) {
            xtop_node node;
            m_handler_ptr->on_message(msg, node);
        };
    }
    xmessage_dispatcher::get_instance().broadcast(msg);
}

std::string xconfig_wrap::get_local_account() {
    return xconfig::get_instance().m_account;
}

bool xconfig_wrap::is_free_node() {
    return xconfig::get_instance().m_free_node;
}

bool xconfig_wrap::is_edge_node() {
    return elect::enum_node_type_rpc == xconfig::get_instance().m_node_type;
}

void xconfig_wrap::get_configed_shard_info(int32_t &zone_id, int32_t &shard_id) {
    zone_id = xconfig::get_instance().m_zone_id;
    shard_id = xconfig::get_instance().m_shard_id;
}

int xconfig_wrap::get_elect_cycle() {
    return xconfig::get_instance().m_elect_cycle;
}

NS_END2
