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

#ifndef NYREM_MODULE_HPP
#define NYREM_MODULE_HPP

// Enum containing platform types
#define LT_TARGET_LINUX 0 
#define LT_TARGET_WIN 1

// __unix__ is usually defined by compilers targeting Unix systems
#ifdef __unix__
    #define LT_PLATFORM_TARGET LT_TARGET_LINUX
    #include <unistd.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <string.h>
    #define EXPORT
// _Win32 is usually defined by compilers targeting 32 or 64 bit Windows systems
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)   
    #define LT_PLATFORM_TARGET LT_TARGET_WIN
    #define _CRT_SECURE_NO_WARNINGS 1
    #define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
    #define NOMINMAX // Exclude min max macros
    #if defined(APIENTRY)
        #undef APIENTRY
    #endif
    #include <stdio.h>
    #include <windows.h> // Windows Header Files

    #ifdef DLL_MAKE
        #define EXPORT __declspec(dllexport)
    #else
        #define EXPORT __declspec(dllimport)
    #endif

    #ifdef _WIN64
        // Specific Windows (64-bit only)
    #else
        // Specific something for Windows (32-bit only)
    #endif
#endif

#include <assert.h>
#include <exception>
#include <iostream>

bool lt_check_gl_error(const char* cmd, int line, const char* file);

#ifndef LT_DEFINE_GL_MACRO
    #define LT_DEFINE_GL_MACRO 1
#endif

#define NYREM_NAMESPACE

#ifdef LT_DEFINE_GL_MACRO
    #if defined(TARGET_RELEASE)
        #define CGL(cmd) cmd
    #else
        #define CGL(cmd)                          \
        do {                                    \
            cmd;                                \
            (void) lt_check_gl_error(#cmd, __LINE__, __FILE__); \
        } while (0)
    #endif
#else // defines empty macros
    #define CGL(cmd) cmd
#endif

#if !defined(GL_STACK_OVERFLOW)
    #define GL_STACK_OVERFLOW 0x0503
#endif

#if !defined(GL_STACK_UNDERFLOW)
    #define GL_STACK_UNDERFLOW 0x0504
#endif

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

#ifdef NYREM_NAMESPACE
    #define NYREM_NAMESPACE_BEGIN namespace nyrem {
    #define NYREM_NAMESPACE_END }
    #define NYREM_USE_NAMESPACE using namespace nyrem;
#else
    #define NYREM_NAMESPACE_BEGIN
    #define NYREM_NAMESPACE END
    #define NYREM_USE_NAMESPACE
#endif

NYREM_NAMESPACE_BEGIN

	// ---- Matrices ---- //
	using mat4x4 = glm::mat4x4;
	using mat3x3 = glm::mat3x3;
	using mat2x2 = glm::mat2x2;

	using mat4x4 = glm::mat4x4;
	using mat3x3 = glm::mat3x3;
	using mat2x2 = glm::mat2x2;	
	// ---- Vectors ---- //
	using vec4 = glm::vec4;
	using vec3 = glm::vec3;
	using vec2 = glm::vec2;
	
	using dvec4 = glm::dvec4;
	using dvec3 = glm::dvec3;
	using dvec2 = glm::dvec2;

NYREM_NAMESPACE_END

#endif // !NYREM_MODULE_HPP
