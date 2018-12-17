#include "xblock.h"
#include <sstream>
#include "xhash.h"
#include "xutil.h"

NS_BEG2(top, elect2)

void xblock_head::dump() {
    xinfo2("block: %s", to_string().c_str());
}

std::string xblock_head::get_str() {
    return get_str(m_nonce);
}

std::string xblock_head::get_str(uint64_t nonce) {
    std::string str_ret = get_str_pre();
    str_ret += std::to_string(nonce);
    return str_ret;
}

std::string xblock_head::get_str_pre() {
    std::string str_ret;
    str_ret += std::to_string(m_index);
    str_ret += m_timestamp;
    str_ret += m_value_hash;
    str_ret += m_prev_hash;
    str_ret += std::to_string(m_difficulty);
    return str_ret;
}

std::string xblock_head::to_string() {
    std::string str_ret;
    str_ret += "index:" + std::to_string(m_index);
    str_ret += ",time:'" + m_timestamp + "'";
    str_ret += ",value:'" + m_value_hash + "'";
    str_ret += ",hash:'" + m_hash + "'";
    str_ret += ",prev:'" + m_prev_hash + "'";
    str_ret += ",diff:" + std::to_string(m_difficulty);
    str_ret += ",nonce:" + std::to_string(m_nonce);
    return str_ret;
}

void xblock_head::to_json(xJson::Value &root) {
    root = xJson::Value();
    root["index"] = (xJson::UInt64)m_index;
    root["time"] = m_timestamp;
    root["value"] = m_value_hash;
    root["hash"] = m_hash;
    root["prev"] = m_prev_hash;
    root["diff"] = (xJson::UInt64)m_difficulty;
    root["nonce"] = (xJson::UInt64)m_nonce;
}

bool xblock_head::check_nonce(uint64_t nonce) {
    m_nonce = nonce;
    const auto str = get_str(nonce);
    m_hash = sha256(str);
    return is_hash_valid(m_hash, m_difficulty);
}

static char hex_to_char(uint8_t hex) {
    if (hex > 9)
        return char(hex - 10) + 'a';
    else
        return char(hex) + '0';
}

static std::string bytes_to_str(const uint8_t *bytes, int len) {
    std::string buf;
    buf.resize(len * 2);
    for (int i = 0; i < len; ++i) {
        const uint8_t high = bytes[i] >> 4;
        const uint8_t low = bytes[i] & 0x0F;
        buf[i*2] = hex_to_char(high);
        buf[i*2+1] = hex_to_char(low);
    }

    return buf;
}

// TODO: leading-zero num(diffi)
bool xblock_head::is_hash_valid(const std::string &hash, int diff) {
    assert(!hash.empty());
    const std::string prefix = std::string(diff, '0');
	const auto pos = hash.find(prefix);
    return 0 == pos;
}

bool xblock_head::check_hash() {
    const auto record = get_str();
	const auto str_hash = sha256(record);
    if (str_hash != m_hash) {
        xinfo2("block head check failed: str_hash(%s) != hash(%s)", str_hash.c_str(), m_hash.c_str());
        return false;
    }

    return true;
}

void xblock_head::init_as_genesis_block(uint64_t diff) {
    m_index = 0;
    m_timestamp = "t";
    m_value_hash = "v";
    m_hash = "h";
    m_prev_hash = "p";
    m_difficulty = diff;
    m_nonce = 201810161515;
}

void xblock_head::init_from_last_block(const xblock_head &bh) {
    m_index = bh.m_index + 1;
    m_timestamp = xutil::get_gmtime();
    // m_value_hash
    // m_hash
    m_prev_hash = bh.m_hash;
    // m_nonce
}

bool xblock_head::is_genesis_block() {
    return "h" == m_hash; // TODO: special hash value
}

std::string xblock_head::sha256(const std::string &str) {
    // unsigned char buf[SHA256_DIGEST_LENGTH];
    // SHA256_CTX ctx;
    // SHA256_Init(&ctx);
    // SHA256_Update(&ctx, str.data(), str.size());
    // SHA256_Final(buf, &ctx);

    // std::ostringstream strout;
    // for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
    //     strout << std::hex << std::setw(2) << std::setfill('0') << (int)buf[i];
    // }

    // return strout.str();

    const uint256_t hash = top::utl::xsha2_256_t::digest(str.data(), str.size());
	const auto str_hash = bytes_to_str(hash.data(), hash.size());
    return str_hash;
}

std::string xblock::to_string() {
    std::string str_ret("{");
    str_ret += m_head.to_string();
    str_ret += "|" + xutil::to_string(*m_res_ptr);
    str_ret += "}";
    return str_ret;
}

void xblock::to_json(xJson::Value &root) {
    root = xJson::Value();
    m_head.to_json(root["head"]);
    xutil::to_json(root["body"], *m_res_ptr);
}

NS_END2
