#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <thread>
#include <memory>
#include <mutex>
#include <vector>
#include <deque>
#include <map>
#include <string>
#include <atomic>
#include <functional>
#include <condition_variable>
#include <msgpack/msgpack.hpp>
#include <xasio/asio.hpp>

#include "xelect2/xelect_intf.h"
#include "xconfig.hpp"

NS_BEG2(top, elect2)

typedef std::atomic_bool abool_t;
typedef std::thread thread_t;
typedef std::shared_ptr<thread_t> thread_ptr_t;
typedef std::recursive_mutex mutex_t; // TODO: optimize
typedef std::lock_guard<mutex_t> guard_t;
typedef std::condition_variable condv_t;

typedef msgpack::packer<msgpack::sbuffer> packer_t;

// TODO: digest and sign
struct xnetann {
    uint64_t m_ann_no;
    uint64_t m_ann_index; // index of last block + 1
    xnode m_node;
    // TODO: for testnet begin
    // willing to be in (zone/shard), enabled when m_node.m_free_node is true
    int32_t m_zone_id{-1};
    int32_t m_shard_id{-1};
    // TODO: for testnet end
    MSGPACK_DEFINE(m_ann_no, m_ann_index, m_node, m_zone_id, m_shard_id);
};
typedef std::shared_ptr<xnetann> xnetann_ptr_t;

// TODO: use ann insteady of?
// ann info
struct xann {
    xnetann_ptr_t m_netann_ptr;
};
typedef std::deque<xann> deq_ann_t; // ann queue
typedef std::shared_ptr<deq_ann_t> deq_ann_ptr_t;

// TODO: send to elect function
struct xannset {
    uint64_t m_res_index;
    time_t m_timestamp;
    uint64_t m_diff;
    deq_ann_ptr_t m_anns_ptr{std::make_shared<deq_ann_t>()}; // TODO:remove shared_ptr
};
typedef std::shared_ptr<xannset> xannset_ptr_t;

typedef std::shared_ptr<xelect_result> xelect_result_ptr_t;

// main/f/xtest::~xtest
class xtrace {
public:
    xtrace(const std::string &str_full_name) : m_name(get_func_name(str_full_name)) {
        xinfo2("<enter> '%s'", m_name.c_str());
    }

    ~xtrace() {
        xinfo2("<leave> '%s'", m_name.c_str());
    }

    std::string get_func_name(const std::string &str) {
        auto p1 = str.find('(');
        assert(p1 != str.npos);
        auto p2 = str.rfind(' ', p1);
        if (p2 == str.npos) // ctor/dtor
            p2 = 0;
        else
            p2 += 1;
        assert(p1 > p2);
        return str.substr(p2, p1 - p2);
    }

private:
    std::string m_name;
};

#define ELECT_TRACE() xtrace trace_(__PRETTY_FUNCTION__)

class xcondv {
    typedef std::mutex mutex_t;
    typedef std::condition_variable condv_t;
    typedef std::unique_lock<mutex_t> lock_t;
public:
    void wait() {
        lock_t lock(m_mutex);
        m_condv.wait(lock);
    }

    void notify() {
        m_condv.notify_all();
    }

private:
    mutex_t m_mutex;
    condv_t m_condv;
};

typedef std::function<void ()> thread_proc_t;

class xthread : public std::enable_shared_from_this<xthread> {
public:
    xthread(const std::string &str_name="[unnamed]") : m_name(str_name) {}

    void start(thread_proc_t proc) {
        assert(proc);
        guard_t guard(m_mutex);
        assert(!m_started);
        m_proc = proc;
        m_running = true;
        m_started = true;
        m_thread_ptr = std::make_shared<thread_t>(&xthread::thread_proc, shared_from_this()); // be last
        xinfo2("thread(%s) started", m_name.c_str());
    }

    void stop() {
        {
            guard_t guard(m_mutex);
            if (!m_running)
                return;

            xinfo2("thread(%s) stopping ...", m_name.c_str());
            m_running = false;
        }

        m_thread_ptr->join(); // wait exit or m_is_cancel == true
        m_proc = nullptr;
    }

    bool is_running() {
        return m_running;
    }

private:
    void thread_proc() {
        xinfo2("thread(%s) running ...", m_name.c_str());
        thread_proc_t proc;
        {
            guard_t guard(m_mutex);
            if (!m_running)
                return;

            proc = m_proc;
        }

        if (proc)
            proc();

        xinfo2("thread(%s) stopped", m_name.c_str());
    }

private:
    std::string m_name;
    mutex_t m_mutex;
    bool m_started{false};
    abool_t m_running{false};
    thread_ptr_t m_thread_ptr;
    thread_proc_t m_proc;
};
typedef std::shared_ptr<xthread> xthread_ptr_t;

NS_END2
