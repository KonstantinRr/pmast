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

#ifndef LISTENER_HPP
#define LISTENER_HPP

#include <engine/internal.hpp>

#include <functional>
#include <vector>

NYREM_NAMESPACE_BEGIN

template<typename Type>
struct CallbackForm {
	int32_t id;
	std::function<Type> function;

	template<typename Param>
	CallbackForm(int32_t id, Param && func)
		: id(id), function(func) { }
};

template<typename Type>
struct CallbackReturn {
	using ThisType = CallbackReturn<Type>;
	using CallbackReturnType = ThisType;

	std::vector<CallbackForm<Type>> *parent;
	int32_t id;
	bool autoRemove;

	CallbackReturn() : id(-1), autoRemove(false) { }
	CallbackReturn(int32_t id,
		std::vector<CallbackForm<Type>>* parent, bool autoRemove=false)
		: id(id), parent(parent), autoRemove(autoRemove) { }

	CallbackReturn(const ThisType&) = delete;
	CallbackReturn(ThisType &&mv) :
		parent(std::exchange(mv.parent, nullptr)),
		id(std::exchange(mv.id, -1)),
		autoRemove(std::exchange(mv.autoRemove, false)) { }

	ThisType& operator=(const ThisType &) = delete;
	ThisType& operator=(ThisType &&mv) {
		parent = std::exchange(mv.parent, nullptr);
		id = std::exchange(mv.id, -1);
		autoRemove = std::exchange(mv.autoRemove, false);
		return *this;
	}

	~CallbackReturn() {
		if (autoRemove)
			remove();
	}

	bool isActive() const { return id != -1; }
	void remove() {
		if (id != -1) {
			for (size_t i = 0; i < parent->size();) {
				if ((*parent)[i].id == id)
					parent->erase(parent->begin() + i);
				else
					i++;
			}
			id = -1;
		}
	}
};

template<typename Type>
class Listener {
protected:
	std::vector<CallbackForm<Type>> callbacks;
	int32_t lastIndex = 1000;

public:
	template<typename FType>
	CallbackReturn<Type> listen(bool autoRemove, FType && function) {
		int32_t id = lastIndex++;
		callbacks.push_back(
			CallbackForm<Type>(
				id, std::function<Type>(function)
			)
		);
		return CallbackReturn<Type>(id, &callbacks, autoRemove);
	}

	template<typename ... Args>
	void trigger(Args&& ... args) {
		for (const auto & ref : callbacks) {
			ref.function(std::forward<Args>(args)...);
		}
	}

	bool empty() { return callbacks.empty(); }
	void clear() { callbacks.clear(); }
};

NYREM_NAMESPACE_END

#endif
