#include "xsender.h"

NS_BEG2(top, elect2)

void xsender::start(xdispatcher_intf_ptr_t dispatcher_ptr, xconfig_intf_ptr_t config_ptr) {
    assert(dispatcher_ptr);
    assert(config_ptr);
    guard_t guard(m_mutex);
    assert(!m_started);
    m_running = true;
    m_started = true;
    m_dispatcher_ptr = dispatcher_ptr;
    m_config_ptr = config_ptr;
}

void xsender::stop() {
    guard_t guard(m_mutex);
    if (!m_running)
        return;

    m_running = false;
    m_config_ptr = nullptr;
    m_dispatcher_ptr = nullptr;
}

void xsender::on_ann_tick_proc() {
    guard_t guard(m_mutex);

    // as free node first
    xnetann netann;
    netann.m_ann_no = m_ann_no++;
    netann.m_ann_index = m_ann_index;
    netann.m_node.m_account = m_config_ptr->get_local_account();
    netann.m_node.m_type = elect::enum_node_type_consensus;
    netann.m_node.m_free_node = 1;

    if (!m_config_ptr->is_free_node()) {
        netann.m_node.m_free_node = 0;
        if (m_config_ptr->is_edge_node())
            netann.m_node.m_type = elect::enum_node_type_rpc;
        else
            m_config_ptr->get_configed_shard_info(netann.m_zone_id, netann.m_shard_id);
    }

    // only pbft node send ann
    // TODO: edge node can participate election later
    if (elect::enum_node_type_consensus == netann.m_node.m_type)
        send_ann(netann);
}

void xsender::send_block(std::string str_block, xblock &block) {
    guard_t guard(m_mutex);
    xtop_message_t msg;
    msg.m_msg_type = enum_xtop_msg_type_elect_result;
    msg.m_content = xutil::pack_to(block);
    m_dispatcher_ptr->broadcast(msg);
    xkinfo2("broadcast %s(%s)", str_block.c_str(), block.m_head.m_hash.c_str());
}

void xsender::update_index(uint64_t index) {
    guard_t guard(m_mutex);
    if (m_ann_index != index) {
        xinfo2("update ann sender's index(%ld) -> %ld", (long)m_ann_index, (long)index);
        m_ann_index = index;
    } else {
        xwarn2("update ann sender's equal index(%ld)", (long)index);
    }
}

void xsender::send_ann(const xnetann &netann) {
    xtop_message_t msg;
    msg.m_msg_type = enum_xtop_msg_type_elect_announce;
    msg.m_content = xutil::pack_to(netann);
    xinfo2("broadcast ann(index=%ld,account=%s) [%ld] ..."
        , (long)netann.m_ann_index, netann.m_node.m_account.c_str(), (long)netann.m_ann_no);
    m_dispatcher_ptr->broadcast(msg);
}

NS_END2
