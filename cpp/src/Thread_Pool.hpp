/**
 * @file    Thread_Pool.hpp
 * @author  Marvin Smith
 * @date    12/20/2020
 * 
 * @note I stole this entirely from: 
 *       https://vorbrodt.blog/2019/02/27/advanced-thread-pool/
 */

// C++ Libraries
#include <atomic>
#include <cassert>
#include <functional>
#include <future>
#include <thread>
#include <vector>

// Project Libraries
#include "Blocking_Queue.hpp"

/**
 * @class Thread_Pool
 */
class Thread_Pool
{
    public:
        explicit Thread_Pool(unsigned int threads = std::thread::hardware_concurrency())
          : m_queues(threads), m_count(threads)
        {
            if(!threads)
                throw std::invalid_argument("Invalid thread count!");

            auto worker = [this](auto i)
            {
                while(true)
                {
                    Proc f;
                    for(auto n = 0; n < m_count * K; ++n)
                        if(m_queues[(i + n) % m_count].try_pop(f))
                            break;
                    if(!f && !m_queues[i].pop(f))
                        break;
                    f();
                }
            };

            for(auto i = 0; i < threads; ++i)
                m_threads.emplace_back(worker, i);
        }

        ~Thread_Pool()
        {
            for(auto& queue : m_queues)
                queue.done();
            for(auto& thread : m_threads)
                thread.join();
        }

        template<typename F, typename... Args>
        void enqueue_work(F&& f, Args&&... args)
        {
            auto work = [p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() { std::apply(p, t); };
            auto i = m_index++;

            for(auto n = 0; n < m_count * K; ++n)
                if(m_queues[(i + n) % m_count].try_push(work))
                    return;

            m_queues[i % m_count].push(std::move(work));
        }

        template<typename F, typename... Args>
        [[nodiscard]] auto enqueue_task(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>>
        {
            using task_return_type = std::invoke_result_t<F, Args...>;
            using task_type = std::packaged_task<task_return_type()>;

            auto task = std::make_shared<task_type>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
            auto work = [task]() { (*task)(); };
            auto result = task->get_future();
            auto i = m_index++;

            for(auto n = 0; n < m_count * K; ++n)
                if(m_queues[(i + n) % m_count].try_push(work))
                    return result;

            m_queues[i % m_count].push(std::move(work));

            return result;
        }

    private:
        using Proc = std::function<void(void)>;
        using Queue = Blocking_Queue<Proc>;
        using Queues = std::vector<Queue>;
        Queues m_queues;

        using Threads = std::vector<std::thread>;
        Threads m_threads;

        const unsigned int m_count;
        std::atomic_uint m_index = 0;

        inline static const unsigned int K = 2;
};