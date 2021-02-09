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

#ifndef PARSER_HPP
#define PARSER_HPP

#include <pmast/internal.hpp>
#include <pmast/osm.hpp>

#include <engine/thread.hpp>

#include <string>
#include <chrono>

namespace traffic {

/// <summary>
/// Stores parser timings. Each call to 
/// </summary>
struct ParseTimings
{
	std::chrono::high_resolution_clock::time_point
		begin, endRead, endXMLParse, endDataParse, end;

	/// <summary>Prints a detailed summary on the timings</summary>
	void summary();
};

struct ParseArguments
{
	/// <summary>
	/// The concurrency manager used to generate the threads.
	/// This field might be changed in the future.
	/// </summary>
	nyrem::ConcurrencyManager *pool = nullptr;

	/// <summary>
	/// Defines the location where the parser should store additional
	/// timing information about the parsing process. Does not store
	/// any information if this value is nullptr.
	/// </summary>
	ParseTimings *timings = nullptr;

	/// <summary>
	/// The path to the file that the parser should parse.
	/// It should be relative to its current location to ensure
	/// compability with most systems.
	/// The default value for this field is "map.osm"
	/// </summary>
	std::string file = "map.osm";

	/// <summary>
	/// The amount of async threads that will be used to parse the map.
	/// This field might be changed in the future.
	/// </summary>
	int threads = 8;
};

/// <summary>
/// Writes an OSM map to disk. This function is currently under
/// development and not yet supported.
/// </summary>
std::vector<unsigned char> writeXOSMMap(const OSMSegment &map, const std::string &file);

/// <summary>
/// Parses an OSM map by using a map of arguments.
/// The ParseArguments for more information about the different arguments.
/// </summary>
OSMSegment parseXMLMap(const ParseArguments &args);

} // namespace traffic

#endif
