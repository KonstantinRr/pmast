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


#pragma once

#ifndef NYREM_THREAD_HPP
#define NYREM_THREAD_HPP

#include "internal.hpp"

#include <memory>
#include <future>
#include <functional>

namespace nyrem {
    class ConcurrencyManager {
    public:
        ///<summary>Creates a new Concurrency manager with the default thread size</summary>
        ConcurrencyManager();
        ConcurrencyManager(size_t n);
        
        void resize(size_t n);

        ConcurrencyManager(const ConcurrencyManager&) = delete;
        ConcurrencyManager(ConcurrencyManager &&) = default;

        ConcurrencyManager& operator=(const ConcurrencyManager&) = delete;
        ConcurrencyManager& operator=(ConcurrencyManager &&) = default;

        ~ConcurrencyManager();

        template<typename T, typename ...Args>
        std::promise<T> add(const std::function<T(Args...)> &exec, Args&& ... args) {
            std::promise<T> prom;
            addRaw([...args = std::forward<Args>(args), &exec, &prom](int) {
                try {
                    prom.set_value(exec(std::forward<Args>(args)...));
                } catch (const std::exception &excp) {
                    prom.set_exception(excp);
                } catch (...) {
                    prom.set_exception(std::runtime_error("An unknwon error occurred"));
                }
            });
            return prom;
        }

        void addRaw(const std::function<void(int)> &exec);
        
    protected:
        struct ThreadManagerImpl;
        std::unique_ptr<ThreadManagerImpl> m_pool;
    };
}

#endif
