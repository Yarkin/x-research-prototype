#pragma once

#include "xdefine.h"
#include "xblock.h"

NS_BEG2(top, elect2)

class xhash_cache {
    typedef std::map<std::string, std::string> map_t;
public:
    // true: inserted, false: exist
    bool check_exist(xblock_ptr_t block_ptr) {
        assert(block_ptr);
        const std::string &hash = block_ptr->m_head.m_hash;
        if (m_map.find(hash) != m_map.end()) // exist
            return true;
        return false;
    }

    void insert(xblock_ptr_t block_ptr) {
        assert(block_ptr);
        const std::string &hash = block_ptr->m_head.m_hash;
        m_map[hash] = "";
    }

    void remove(xblock_ptr_t block_ptr) {
        assert(block_ptr);
        const std::string &hash = block_ptr->m_head.m_hash;
        m_map.erase(hash);
    }

    void clear() {
        m_map.clear();
    }

private:
    map_t m_map;
};

class xchain { // TODO: implement blockchain
    typedef std::deque<xblock_ptr_t> deq_block_t;
public:
    xchain(uint64_t diff);

    bool append_new_block(xblock_ptr_t block_ptr);
    bool append_net_block(xblock_ptr_t block_ptr);

    xblock_ptr_t get_last_block();

    std::string to_string();
    void to_json(xJson::Value &root); // array

private:
    // maintain at most 30 blocks(>= 24 hours)
    void adjust_blocks();

    // check if valid and append
    // str_block: net_block/new_block
    // enum BLOCK_N{ NET_BLOCK, NEW_BLOCK };
    bool append_block(const std::string &str_block, xblock_ptr_t block_ptr);

private:
    mutex_t m_mutex;
    deq_block_t m_blocks;
    xhash_cache m_cache;
};
typedef std::shared_ptr<xchain> xchain_ptr_t;

NS_END2
