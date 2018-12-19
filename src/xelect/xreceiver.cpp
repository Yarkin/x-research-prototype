#include "xreceiver.h"

NS_BEG2(top, elect2)

void xreceiver::on_message(const xtop_message_t &msg, const xtop_node &from) {
    switch (msg.m_msg_type) {
    case enum_xtop_msg_type_elect_announce:
    case enum_xtop_msg_type_elect_result:
        m_queue_condv.push_back(std::make_shared<xmsg>(xmsg{msg.m_msg_type, msg.m_content}));
        break;
    default:
        xwarn2("unsupported msg type(%d)", msg.m_msg_type);
    }
    // run_once();
}

void xreceiver::start(xdispatcher_intf_ptr_t dispatcher_ptr, xconfig_intf_ptr_t config_ptr, on_net_block_t on_net_block) {
    assert(dispatcher_ptr);
    assert(config_ptr);
    assert(on_net_block);
    guard_t guard(m_mutex);
    assert(!m_started);
    m_running = true;
    m_started = true;
    m_dispatcher_ptr = dispatcher_ptr;
    m_config_ptr = config_ptr;
    m_on_net_block = on_net_block;
    auto on_thread = std::bind(&xreceiver::thread_proc, shared_from_this());
    m_thread_ptr->start(on_thread);
    // register handler(outside)
    std::shared_ptr<xmessage_handler> elect_handler(shared_from_this());
    m_name = "xreceiver_" + config_ptr->get_local_account();
    m_dispatcher_ptr->register_handler(m_name, enum_xtop_msg_type_elect_announce, elect_handler);
    m_dispatcher_ptr->register_handler(m_name, enum_xtop_msg_type_elect_result, elect_handler);
    xinfo2("%s started", m_name.c_str());
}

void xreceiver::stop() {
    guard_t guard(m_mutex);
    if (!m_running)
        return;

    m_running = false;
    m_dispatcher_ptr->unregister_handler(m_name, enum_xtop_msg_type_elect_announce);
    m_dispatcher_ptr->unregister_handler(m_name, enum_xtop_msg_type_elect_result);
    m_queue_condv.stop();
    m_thread_ptr->stop();
    m_dispatcher_ptr = nullptr;
    m_on_net_block = nullptr;
    m_name.clear();
    xinfo2("%s stopped", m_name.c_str());
}

void xreceiver::push_back(xnetann_ptr_t netann_ptr) {
    assert(netann_ptr);
    const auto &account = netann_ptr->m_node.m_account;
    /*if (account.empty()) { // drop empty
        // xinfo2("drop ann for account is empty");
        return;
    }*/

    // remove edge
    // if (netann_ptr->m_node.m_type != elect::enum_node_type_consensus) {
    //     xinfo2("drop netann for type(%d) != 'pbft(%d)'", (int)netann_ptr->m_node.m_type, elect::enum_node_type_consensus);
    // }

    // ::printf("receive: ann(%s)\n", account.c_str());

    guard_t guard(m_mutex);
    // TODO: opt
    // if (netann_ptr->m_ann_index != m_res_index) {
    //     xinfo2("drop netann for index(%ld) != 'current(%ld)'", (long)netann_ptr->m_ann_index, (long)m_res_index);
    // }
	xinfo2("received netann(index=%ld, account=%s)", (long)netann_ptr->m_ann_index, (long)netann_ptr->m_node.m_account.c_str());

    const auto index = netann_ptr->m_ann_index;
    auto anns_ptr = m_annset_ptr->m_anns_ptr;
    for (auto it = anns_ptr->begin(); it != anns_ptr->end(); ++it) {
        if (index == it->m_netann_ptr->m_ann_index && account == it->m_netann_ptr->m_node.m_account) // remove duplicate
            return;
    }

    xkinfo2("receive ann from %s", account.c_str());
    anns_ptr->push_back({netann_ptr});
}

void xreceiver::swap(xannset_ptr_t &annset_ptr) {
    assert(annset_ptr);
    guard_t guard(m_mutex);
    m_annset_ptr->m_res_index = m_res_index;
    annset_ptr.swap(m_annset_ptr);
}

void xreceiver::update_index(uint64_t index) {
    guard_t guard(m_mutex);
    if (m_res_index != index) {
        xinfo2("update ann reciever's index(%ld) -> %ld", (long)m_res_index, (long)index);
        m_res_index = index;
        // annset_ptr->m_anns_ptr->clear();
    } else {
        xwarn2("update ann reciever's equal index(%ld)", (long)index);
    }
}

void xreceiver::wait_anns(size_t count) {
    for (;;) {
        ::usleep(1 * 1000);
        guard_t guard(m_mutex);
        if (m_annset_ptr->m_anns_ptr->size() >= count)
            return;
    }
}

void xreceiver::thread_proc() {
    while (m_thread_ptr->is_running()) {
        run_once();
    }
}

void xreceiver::run_once() {
    auto msg_ptr = m_queue_condv.get_front();
    if (!msg_ptr)
        return;

    switch (msg_ptr->m_type) {
    case enum_xtop_msg_type_elect_announce: {
        auto netann_ptr = std::make_shared<xnetann>();
        if (xutil::unpack_from(*netann_ptr, msg_ptr->m_data)) {
            push_back(netann_ptr);
        }
        break;
    }
    case enum_xtop_msg_type_elect_result: {
        auto block_ptr = std::make_shared<xblock>(); // TODO: time point valid?
        if (xutil::unpack_from(*block_ptr, msg_ptr->m_data) && xutil::unpack_from(*block_ptr->m_res_ptr, block_ptr->m_body)) {
            m_on_net_block(block_ptr); // valid when thread is running!
        }
        break;
    }
    default:
        assert(!"check on_message's switch");
    }
}

NS_END2
