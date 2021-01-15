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

#include <engine/internal.hpp>

#include <vector>
#include <string>

#ifdef NYREM_NAMESPACE
namespace nyrem {
#endif

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

#ifdef NYREM_NAMESPACE
}
#endif