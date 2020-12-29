#include <ctpl.h>

#include <engine/thread.hpp>

using namespace nyrem;

// ---- ConcurrencyManager ---- //

struct ConcurrencyManager::ThreadManagerImpl {
    ctpl::thread_pool pool;
    
    ThreadManagerImpl(size_t n) : pool(n) { }
};

ConcurrencyManager::~ConcurrencyManager() { }

ConcurrencyManager::ConcurrencyManager()
{
    size_t size = std::thread::hardware_concurrency();
    resize(size == 0 ? 8 : size);
}

ConcurrencyManager::ConcurrencyManager(size_t n)
{
    resize(n);
}

void ConcurrencyManager::resize(size_t size)
{
    m_pool = std::make_unique<ThreadManagerImpl>(size);
}

void ConcurrencyManager::addRaw(const std::function<void(int)> &exec) {
    m_pool->pool.push(exec);
}