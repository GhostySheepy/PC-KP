#pragma once

#include <iostream>
#include <shared_mutex>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <queue>
#include <vector>
#include <random>
#include <chrono>
#include <utility>



using read_write_lock = std::shared_mutex;
using read_lock = std::shared_lock<read_write_lock>;
using write_lock = std::unique_lock<read_write_lock>;

template<typename T>
class Queue
{
private:
    using tasks_queue = std::queue<T>;

    std::queue<T> m_q;
    mutable read_write_lock m_rw_lock;
public:
    Queue() = default;
    ~Queue()
    {
        clear();
    }


    bool empty() const
    {
        read_lock lock(m_rw_lock);

        return m_q.empty();
    }

    size_t size() const
    {
        read_lock lock(m_rw_lock);

        return m_q.size();
    }

    void clear()
    {
        write_lock lock(m_rw_lock);

        while (!m_q.empty())
        {
            m_q.pop();
        }
    }

    // Return 1 if the queue is full, 0 - the task is added
    int push(T& new_task)
    {
        write_lock lock(m_rw_lock);
        m_q.push(new_task);

        return 0;
    }

    bool pop(T& task)
    {
        write_lock lock(m_rw_lock);

        if (m_q.empty())
        {
            return false;
        }

        task = std::move(m_q.front());
        m_q.pop();

        return true;
    }

public:
    Queue(Queue& other_q) = delete;
    Queue(Queue&& other_q) = delete;
    Queue& operator=(Queue& rhs) = delete;
    Queue& operator=(Queue&& rhs) = delete;
};

template<typename TaskType>
class Thread_Pool
{
private:
    Queue<TaskType> m_task_queue_0;
    std::vector<std::thread> m_workers_0;
    mutable std::condition_variable_any m_task_queue_0_waiter;

    Queue<TaskType> m_task_queue_1;
    std::vector<std::thread> m_workers_1;
    mutable std::condition_variable_any m_task_queue_1_waiter;

    bool m_initialized = false;
    bool m_terminated = false;
    bool m_cancelled = false;
    bool m_paused = false;

    std::mutex m_cout;
    mutable read_write_lock m_rw_lock;

    mutable std::once_flag m_initialized_once;
public:
    Thread_Pool() = default;
    ~Thread_Pool()
    {
        terminate();
    }

    void initialize(size_t workers_num)
    {
        call_once(m_initialized_once, [this, workers_num]() {
            write_lock lock(m_rw_lock);

            m_workers_0.reserve(workers_num);

            for (int i = 0; i < workers_num; ++i)
            {
                m_workers_0.emplace_back(&Thread_Pool::run, this, i, 0, std::ref(m_task_queue_0), std::ref(m_task_queue_0_waiter));
            }


            m_workers_1.reserve(workers_num);

            for (int i = 0; i < workers_num; ++i)
            {
                m_workers_1.emplace_back(&Thread_Pool::run, this, i, 1, std::ref(m_task_queue_1), std::ref(m_task_queue_1_waiter));
            }

            m_initialized = true;
            });
    }

    template<typename F, typename... Args>
    int add_task(F&& func, Args&&... args)
    {
        {
            read_lock lock(m_rw_lock);

            if (!is_running_unsafe() || m_paused)
            {
                return 1;
            }
        }

        std::function<void()> bound_func = std::bind(std::forward<F>(func), std::forward<Args>(args)...);

        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<int> dist(0, 1);
        int result = 0;
        if (m_task_queue_0.size() < m_task_queue_1.size()) {
            result = m_task_queue_0.push(bound_func);

            if (result == 0) {
                m_task_queue_0_waiter.notify_one();
            }
        }
        else {
            result = m_task_queue_1.push(bound_func);

            if (result == 0) {
                m_task_queue_1_waiter.notify_one();
            }
        }

        return result;
    }

    void terminate()
    {
        {
            write_lock lock(m_rw_lock);

            if (!is_running_unsafe())
            {
                m_workers_0.clear();
                m_workers_1.clear();

                m_terminated = false;
                m_cancelled = false;
                m_initialized = false;

                return;
            }

            m_terminated = true;
        }

        m_task_queue_0_waiter.notify_all();
        m_task_queue_1_waiter.notify_all();

        for (auto& worker : m_workers_0)
        {
            worker.join();
        }

        for (auto& worker : m_workers_1)
        {
            worker.join();
        }

        m_workers_0.clear();
        m_workers_1.clear();

        m_terminated = false;
        m_cancelled = false;
        m_initialized = false;
    }

    void cancel()
    {
        {
            write_lock lock(m_rw_lock);

            if (!is_running_unsafe())
            {
                m_workers_0.clear();
                m_workers_1.clear();

                m_terminated = false;
                m_cancelled = false;
                m_initialized = false;

                return;
            }

            m_cancelled = true;
        }

        m_task_queue_0.clear();
        m_task_queue_1.clear();

        m_task_queue_0_waiter.notify_all();
        m_task_queue_1_waiter.notify_all();

        for (auto& worker : m_workers_0)
        {
            worker.join();
        }

        for (auto& worker : m_workers_1)
        {
            worker.join();
        }

        m_workers_0.clear();
        m_workers_1.clear();

        m_terminated = false;
        m_cancelled = false;
        m_initialized = false;
    }

    void pause()
    {
        write_lock lock(m_rw_lock);

        if (m_paused)
        {
            return;
        }

        m_paused = true;
    }

    void resume()
    {
        write_lock lock(m_rw_lock);

        if (!m_paused)
        {
            return;
        }

        m_paused = false;

        m_task_queue_0_waiter.notify_all();
    }

    bool is_initialized() const
    {
        read_lock lock(m_rw_lock);

        return m_initialized;
    }

    bool is_running_safe() const
    {
        read_lock lock(m_rw_lock);

        return is_running_unsafe();
    }

    bool is_paused() const
    {
        read_lock lock(m_rw_lock);

        return m_paused;
    }

    size_t workers_size() const
    {
        read_lock lock(m_rw_lock);

        return m_workers_0.size() + m_workers_1.size();
    }

    size_t queue_size() const
    {
        read_lock lock(m_rw_lock);

        return m_task_queue_0.size() + m_task_queue_1.size();
    }
private:
    void run(int thread_id, int queue_id, Queue<TaskType>& labouring_queue, std::condition_variable_any& labouring_waiter)
    {
        while (true)
        {
            bool popped = false;
            std::function<void()> task;

            {
                write_lock lock(m_rw_lock);

                labouring_waiter.wait(lock, [this, &popped, &task, &labouring_queue] {
                    if (!m_paused)
                    {
                        popped = labouring_queue.pop(task);
                    }

                    return m_cancelled || m_terminated || popped || (!m_paused && popped);
                    });
            }

            if (m_cancelled || (m_terminated))
            {
                std::cout << std::endl << "THREAD GONE" << std::endl;
                return;
            }

            if (!m_paused && popped) {
                task();
            }
            else {
                std::random_device rd;
                std::mt19937 mt(rd());
                int st = 0;
                int ed = 200;
                std::uniform_int_distribution<int> dist(st, ed);
                std::this_thread::sleep_for(std::chrono::milliseconds(dist(mt)));
            }

        }
    }

    bool is_running_unsafe() const
    {
        return m_initialized && !m_terminated && !m_cancelled;
    }
public:
    Thread_Pool(Thread_Pool& other) = delete;
    Thread_Pool(Thread_Pool&& other) = delete;
    Thread_Pool& operator=(Thread_Pool& rhs) = delete;
    Thread_Pool& operator=(Thread_Pool&& rhs) = delete;
};

