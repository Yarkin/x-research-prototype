#pragma once

#include "xdefine.h"

NS_BEG2(top, elect2)

// received blocks
class xqueue_blocks {
public:
    void push_back(xblock_ptr_t block_ptr) {
        assert(block_ptr);

        // xinfo2("receive block");

        guard_t guard(m_mutex);
        m_deq.push_back(block_ptr);
    }

    void swap(deq_block_t &deq) {
        guard_t guard(m_mutex);
        deq.swap(m_deq);
    }

private:
    mutex_t m_mutex;
    deq_block_t m_deq;
};

// condv queue
template <typename T>
class xqueue_condv {
    typedef std::deque<T> deq_t;
    typedef std::mutex mutex_t;
    typedef std::condition_variable condv_t;
    typedef std::unique_lock<mutex_t> lock_t;
public:
    void push_back(T t) {
        assert(t);
        lock_t lock(m_mutex);
        if (!m_running)
            return;

        m_deq.push_back(t);
        m_condv.notify_all();
    }

    // nullptr meens stop
    T get_front() {
        lock_t lock(m_mutex);
        if (!m_running)
            return T();

        if (m_deq.empty()) { // wait until not empty
            m_condv.wait(lock, [this] { return !m_running || !m_deq.empty(); } );
        }

        if (!m_running)
            return T();

        auto t = m_deq.front();
        m_deq.pop_front();
        return t;
    }

    void stop() {
        lock_t lock(m_mutex);
        if (m_running) {
            m_running = false;
            m_deq.clear();
            m_condv.notify_all();
        }
    }

private:
    mutex_t m_mutex;
    condv_t m_condv;
    bool m_running{true};
    deq_t m_deq;
};

typedef std::function<void ()> task_t;

// task queue
class xqueue_task : public std::enable_shared_from_this<xqueue_task> {
    typedef asio::io_context ioctx_t;
    typedef asio::io_context::work work_t;
    typedef std::shared_ptr<work_t> work_ptr_t;
public:
    xqueue_task(const std::string &str_name="[unnamed]") : m_str_name(str_name) {}

    void start() {
        guard_t guard(m_mutex);
        assert(!m_started);
        m_running = true;
        m_started = true;
        m_work_ptr = std::make_shared<work_t>(m_ioctx);
        m_thread_ptr = std::make_shared<std::thread>(&xqueue_task::thread_proc, shared_from_this());
        xinfo2("xqueue_task(%s) started", m_str_name.c_str());
    }

    // call without start
    void stop() {
        {
            guard_t guard(m_mutex);
            if (!m_running)
                return;

            m_running = false;
        }

        m_work_ptr.reset();
        m_thread_ptr->join();
        m_thread_ptr = nullptr;
    }

    // call with start
    void post(task_t task) {
        assert(task);
        {
            guard_t guard(m_mutex);
            assert(m_started);
            if (!m_running)
                return;
        }
        m_ioctx.post(task);
        xinfo2("xqueue_task(%s) post task", m_str_name.c_str());
    }

private:
    void thread_proc() {
        xinfo2("xqueue_task(%s) running", m_str_name.c_str());
        m_ioctx.run();
    }

private:
    std::string m_str_name;
    mutex_t m_mutex;
    bool m_started{false};
    bool m_running{false};
    thread_ptr_t m_thread_ptr;
    asio::io_context m_ioctx;
    work_ptr_t m_work_ptr;
};
typedef std::shared_ptr<xqueue_task> xqueue_task_ptr_t;

NS_END2
