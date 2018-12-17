#include "xchain.h"

NS_BEG2(top, elect2)

xchain::xchain(uint64_t diff) {
    assert(diff > 0); // TODO: check 0 is ok
    auto block_ptr = std::make_shared<xblock>();
    block_ptr->m_head.init_as_genesis_block(diff);
    // block_ptr->m_body; // TODO: init first block body?
    m_blocks.push_back(block_ptr);
    m_cache.insert(block_ptr);
    xkinfo2("genesis block: %s", block_ptr->m_head.to_string().c_str());
}

bool xchain::append_new_block(xblock_ptr_t block_ptr) {
    assert(block_ptr);
    guard_t guard(m_mutex);

    xkinfo2("get new_block: %s", block_ptr->to_string().c_str());
    if (!append_block("new_block", block_ptr)) {
        xkinfo2("drop new_block(%s)", block_ptr->m_head.m_hash.c_str());
        return false;
    }

    return true;
}

bool xchain::append_net_block(xblock_ptr_t block_ptr) {
    assert(block_ptr);
    guard_t guard(m_mutex);

    if (m_cache.check_exist(block_ptr)) // don't print and broadcast received block again
        return false;

    xkinfo2("get net_block: %s", block_ptr->to_string().c_str());
    if (!append_block("net_block", block_ptr)) {
        xkinfo2("drop net_block(%s)", block_ptr->m_head.m_hash.c_str());
        return false;
    }

    return true;
}

bool xchain::append_block(const std::string &str_block, xblock_ptr_t block_ptr) {
    assert(block_ptr);
    assert(str_block == "net_block" || str_block == "new_block");

    auto last_block_ptr = *m_blocks.rbegin();
    auto &head = block_ptr->m_head;
    auto &last_head = last_block_ptr->m_head;
    
    // value hash
    const auto str_hash = xblock_head::sha256(block_ptr->m_body);
    if (str_hash != head.m_value_hash) {
        xinfo2("%s(%s): hash(body)(%s) != value_hash(%s)"
            , str_block.c_str(), block_ptr->m_head.m_hash.c_str(), str_hash.c_str(), head.m_value_hash.c_str());
        return false;
    }

    // hash
    if (!head.check_hash()) {
        xinfo2("%s(%s): check hash failed", str_block.c_str(), block_ptr->m_head.m_hash.c_str());
        return false;
    }

    // TODO: accept any block(remote index > local index)
    if (head.m_index <= last_head.m_index) { // net <= local, invalid
        xinfo2("%s(%s): out-of-date block: index(%ld) <= last_index(%ld)"
            , str_block.c_str(), block_ptr->m_head.m_hash.c_str(), (long)head.m_index, (long)last_head.m_index);
        return false;
    } else if (head.m_index > last_head.m_index + 1) { // net > local + 1, TODO: accept anyway so far
        xkinfo2("accept %s(%s) and update chain", str_block.c_str(), block_ptr->m_head.m_hash.c_str());
        m_blocks.clear();
        m_blocks.push_back(block_ptr);
        m_cache.clear();
        m_cache.insert(block_ptr);
        return true;
    }

    // net == local + 1
    // index
    assert(last_head.m_index + 1 == head.m_index);
    // prev hash
    if (head.m_prev_hash != last_head.m_hash) {
        xinfo2("%s(%s): prev_hash(%s) != last_hash(%s)"
            , str_block.c_str(), block_ptr->m_head.m_hash.c_str(), head.m_prev_hash.c_str(), last_head.m_hash.c_str());
        return false;
    }

    xkinfo2("accept %s(%s)", str_block.c_str(), block_ptr->m_head.m_hash.c_str());
    m_blocks.push_back(block_ptr);
    m_cache.insert(block_ptr);
    adjust_blocks();
    return true;
}

xblock_ptr_t xchain::get_last_block() {
    guard_t guard(m_mutex);
    return *m_blocks.rbegin();
}

void xchain::adjust_blocks() {
    // xinfo2("adjust blocks");
    if (m_blocks.size() > 30) { // TODO: store chain
        auto block_ptr = m_blocks.front();
        m_blocks.pop_front();
        m_cache.remove(block_ptr);
        xinfo2("adjust drop block(index='%ld',hash='%s'))", (long)block_ptr->m_head.m_index, block_ptr->m_head.m_hash.c_str());
    }
}

std::string xchain::to_string() {
    guard_t guard(m_mutex);
    std::string str_ret;
    for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it) {
        auto &block_ptr = *it;
        auto &head = block_ptr->m_head;
        str_ret += "[" + std::to_string(head.m_index) + "]: " + block_ptr->to_string();
    }
    return str_ret;
}

void xchain::to_json(xJson::Value &root) {
    root = xJson::Value(); // as array
    guard_t guard(m_mutex);
    unsigned page = 0;
    for (auto it = m_blocks.begin(); it != m_blocks.end(); ++it) {
        auto &block_ptr = *it;
        block_ptr->to_json(root[page++]);
    }
}

NS_END2
