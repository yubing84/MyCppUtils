#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <atomic>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <queue>
#include <string>

template <typename T> 
class ThreadSafeQueue 
{
public:
    ThreadSafeQueue() 
    {
        m_bTermination = false;
    }
    ThreadSafeQueue(ThreadSafeQueue const& other)
    {
        std::lock_guard<std::mutex> lk(other.m_Mutex);
        m_Queue = other.m_Queue;
    }

    ~ThreadSafeQueue(void) {
    }

    /*
    * @brief 等待队列不为空压出元素,或者等待m_bTermination为真时压出元素
    * @return 队列为空时返回false，不为空时返回true
    */
    bool WaitAndPop (T& value)
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_ConditionVariable.wait(lock, [this] { return ((!m_Queue.empty()) || m_bTermination); });
        //不为空则出队
        if (!m_Queue.empty()) 
        {
            value = m_Queue.front();
            //value = std::move(m_Queue.front());
            m_Queue.front().reset();
            m_Queue.pop();
            std::cout << "压出" << std::endl;
            return true;
        }
        //队列为空则返回失败
        return false;
    }

    std::shared_ptr<T> WaitAndPop() 
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_ConditionVariable.wait(lock, [this] { return ((!m_Queue.empty()) || m_bTermination); });
        if (!m_Queue.empty())
        {
            std::shared_ptr<T> res(std::make_shared<T>(m_Queue.front()));
            m_Queue.pop();
            std::cout << "压出" << std::endl;
            return res;
        }
        return nullptr;
    }

    /*
    * @brief 直接压出元素
    */
    bool TryPop(T& value)
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Queue.empty())
        {
            return false;
        }
        value = m_Queue.front();
        m_Queue.pop();
        return true;
    }

    //队列为空返回null
    std::shared_ptr<T> TryPop() 
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_Queue.empty()) 
        {
            return nullptr;
        }
        std::shared_ptr<T> res(std::make_shared<T>(m_Queue.front()));
        m_Queue.pop();
        return res;
    }


    void Push(T new_value) 
    {
        if (m_bTermination) 
        {
            return;
        }
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_Queue.push(new_value);
        std::cout << "压入" << std::endl;
        m_ConditionVariable.notify_one();
    }

    bool Empty() 
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Queue.empty();
    }
    int Size()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        return m_Queue.size();
    }
    //设置队列为退出状态。在退出状态下，忽略入队，可以执行出队，但当队列为空时，wait_and_pop不会阻塞。
    void Termination()
    {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_bTermination = true;
        m_ConditionVariable.notify_all();
    }

    // 返回退出状态
    bool IsTermination()
    {
        return m_bTermination;
    }

private:
    std::mutex m_Mutex;
    std::queue<T> m_Queue;
    std::condition_variable m_ConditionVariable;
    std::atomic<bool> m_bTermination;
};

#endif // !THREAD_SAFE_QUEUE_H
