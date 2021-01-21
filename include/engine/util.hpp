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

#ifndef NYREM_UTIL_HPP
#define NYREM_UTIL_HPP

#include <engine/internal.hpp>

#include <vector>
#include <string>

NYREM_NAMESPACE_BEGIN

/// <summary>
/// This class implements a fast string stream (FastSStream).
/// 
/// </summary>
class FastSStream {
public:
	void add(const char *string);
	void add(const char *string, size_t size);
	void add(const std::string &string, bool lives=false);
	void add(std::string &&string);
	void add(char fill, size_t size);

	std::string generate() const;
	void clear();
	inline size_t bufferSize() { return segments.size(); }

	

private:
	enum DataType {
		TYPE_STRING,
		TYPE_STRING_INDEX,
		TYPE_CHAR,
		TYPE_FILL
	};
	
	struct DataSegment {
		union {
			struct {
				size_t size;
				char fill;
			} u_fill;
			struct {
				const char *data;
				size_t size;
			} u_char;
			const std::string *u_string;
			size_t u_index;
		};
		DataType type;
	};

	std::vector<std::string> strings;
	std::vector<DataSegment> segments;
};

template<typename Type>
class DoubleQueue {
public:

	DoubleQueue(size_t size) { }

	void push_front(const Type& type) { }
	void push_front(Type &&type) { }

	void push_back(const Type& type) { }
	void push_back(Type &&type) { }

	template<typename ...Args>
	void emplace_back(Args&&... args) { }

	template<typename ...Args>
	void emplace_front(Args&&... args) { }

	void pop_front() { }
	void pop_back() { }

	bool empty() const noexcept { return true; }

protected:
	Type *m_data;
};

template<typename Type>
class BlockAllocator {
public:
	union Block {
		Type value;
		Block *next;
	};

	BlockAllocator(size_t size) : 
		m_maxSize(size), m_allocated(0) {
		// allocates the block
		m_data = new Block[maxSize];
		// initializes the pointers to the first and last element
		m_first = m_data;
		Block *last = m_data + maxSize - 1;
		// all elements point to the next element
		for (Block *ptr = m_first; ptr != last; ptr++)
			ptr->next = ptr + 1;
		// last element points to null indicating the end
		last->next = nullptr;
	}

	~BlockAllocator() {
		delete[] m_data;
	}

	template<typename ...Args>
	Type* allocate(Args&&... args) {
		Type* ptr = &m_first->value;
		if (ptr == nullptr) {
			return nullptr; // TODO error handling
		} else {
			m_first = m_first->next();
			m_allocated++;
			new (ptr) Type(std::forward<Args>(args)...);
			return ptr;
		}
	}

	void deallocate(Type *ptr) {
		Block *original = reinterpret_cast<Block*>(ptr);
		if (original >= m_data && original < m_data + maxSize) {
			// check if the ptr actually belongs to this block of memory
			Block* tempFirst = m_first;
			m_first = original;
			m_first->next = tempFirst;
			original->~Type();
			m_allocated--;
		}
	}

	inline size_t allocated() { return m_allocated; }
	inline size_t maxSize() { return m_maxSize; }
protected:
	Block *m_data, *m_first;
	size_t m_maxSize, m_allocated;
};

NYREM_NAMESPACE_END

#endif
