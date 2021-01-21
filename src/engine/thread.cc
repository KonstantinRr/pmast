/// MIT License
/// 
/// Copyright (c) 2020 Konstantin Rolf
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
/// 
/// Written by Konstantin Rolf (konstantin.rolf@gmail.com)
/// July 2020

#include <engine/thread.hpp>

NYREM_USE_NAMESPACE

// ---- ConcurrencyManager ---- //

struct ConcurrencyManager::ThreadManagerImpl {
    
    ThreadManagerImpl(size_t n) { }
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
}