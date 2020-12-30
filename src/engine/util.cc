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

#include <engine/util.hpp>

using namespace nyrem;

//// ---- FastSStream ---- //// 

void FastSStream::add(const char *string) {
	size_t size = std::strlen(string);
	segments.push_back(DataSegment{
		{.u_char = {string, size}}, TYPE_CHAR});
}
void FastSStream::add(const char *string, size_t size) {
	segments.push_back(DataSegment{
		{.u_char = {string, size}}, TYPE_CHAR});
}
void FastSStream::add(const std::string &string, bool lives) {
	const std::string *ptr;
	if (!lives) {
    	strings.push_back(string);
		segments.push_back(DataSegment{
			{.u_index = strings.size()-1}, TYPE_STRING_INDEX});
	} else {
		segments.push_back(DataSegment{
			{.u_string = &string}, TYPE_STRING});
	}
}
void FastSStream::add(std::string &&string) {
    strings.push_back(string);
    segments.push_back(DataSegment{
		{.u_index = strings.size()-1}, TYPE_STRING_INDEX});
}

void FastSStream::add(char fill, size_t size) {
	segments.push_back(DataSegment{
		{.u_fill = {size, fill}}, TYPE_FILL});
}

void FastSStream::clear() {
	segments.clear();
	strings.clear();
}

std::string FastSStream::generate() const {
	// calculates the needed size
	size_t size = 0;
	for (const DataSegment &seg : segments) {
		switch (seg.type) {
			case TYPE_STRING:
				size += seg.u_string->size();
				break;
			case TYPE_CHAR:
				size += seg.u_char.size;
				break;
			case TYPE_FILL:
				size += seg.u_fill.size;
				break;
			case TYPE_STRING_INDEX:
				size += strings[seg.u_index].size();
				break;
		}
	}
	
	// copies the data segments
	size_t index = 0;
	std::string data(size, '0');
	for (const DataSegment &seg : segments) {
		switch (seg.type) {
			case TYPE_STRING:
				std::memcpy(&data[index], seg.u_string->data(),
					seg.u_string->size());
				index += seg.u_string->size();
				break;
			case TYPE_CHAR:
				std::memcpy(&data[index], seg.u_char.data,
					seg.u_char.size);
				index += seg.u_char.size;
				break;
			case TYPE_FILL:
				std::memset(&data[index], seg.u_fill.fill,
					seg.u_fill.size);
				index += seg.u_fill.size;
			case TYPE_STRING_INDEX:
				std::memcpy(&data[index], strings[seg.u_index].data(),
					strings[seg.u_index].size());
				index += strings[seg.u_index].size();
		}
	}
	return data;
}