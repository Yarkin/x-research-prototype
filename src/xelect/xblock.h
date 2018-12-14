#pragma once

#include "xdefine.h"

NS_BEG2(top, elect2)

struct xblock_head {
    uint64_t m_index;
    std::string m_timestamp; // TODO: use uint64_t precision to us?
    std::string m_value_hash; // hash for the content
    std::string m_hash;
    std::string m_prev_hash;
    uint64_t m_difficulty;
    uint64_t m_nonce;
    MSGPACK_DEFINE(m_index, m_timestamp, m_value_hash, m_hash, m_prev_hash, m_difficulty, m_nonce);

public:
    void dump();
    std::string get_str(); // for hash // TODO: get_data(without m_hash)
    std::string get_str(uint64_t nonce);
    std::string get_str_pre();
    std::string to_string(); // for print
    void to_json(xJson::Value &root);

    // set nonce/hash and check the difficulty satisfication
    bool check_nonce(uint64_t nonce);

    // check if the hash is sha256 of all others
    bool check_hash(); // nonce/hash valid

    void init_as_genesis_block(uint64_t diff);

    // without value/nonce/hash
    void init_from_last_block(const xblock_head &bh);
    bool is_genesis_block();

// util
public:
    static bool is_hash_valid(const std::string &hash, int diff);
    static std::string sha256(const std::string &str);
};

struct xblock {
    xblock_head m_head;
    std::string m_body; // xelect_result
    MSGPACK_DEFINE(m_head, m_body);

    xelect_result_ptr_t m_res_ptr{std::make_shared<xelect_result>()}; // unpack from m_body, will not pack to xblock
    std::string to_string();
    void to_json(xJson::Value &root);
};
typedef std::shared_ptr<xblock> xblock_ptr_t;
typedef std::deque<xblock_ptr_t> deq_block_t;

NS_END2
