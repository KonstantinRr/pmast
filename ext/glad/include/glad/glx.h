/**
 * Loader generated by glad 2.0.0-beta on Sun Dec  6 21:56:48 2020
 *
 * Generator: C/C++
 * Specification: glx
 * Extensions: 2
 *
 * APIs:
 *  - glx=1.4
 *
 * Options:
 *  - MX_GLOBAL = False
 *  - ON_DEMAND = False
 *  - LOADER = True
 *  - ALIAS = False
 *  - HEADER_ONLY = False
 *  - DEBUG = False
 *  - MX = False
 *
 * Commandline:
 *    --api='glx=1.4' --extensions='GLX_ARB_create_context,GLX_ARB_create_context_profile' c --loader
 *
 * Online:
 *    http://glad.sh/#api=glx%3D1.4&extensions=GLX_ARB_create_context%2CGLX_ARB_create_context_profile&generator=c&options=LOADER
 *
 */

#ifndef GLAD_GLX_H_
#define GLAD_GLX_H_

#ifdef GLX_H
  #error GLX header already included (API: glx), remove previous include!
#endif
#define GLX_H 1


#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <glad/gl.h>

#define GLAD_GLX
#define GLAD_OPTION_GLX_LOADER

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLAD_PLATFORM_H_
#define GLAD_PLATFORM_H_

#ifndef GLAD_PLATFORM_WIN32
  #if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) || defined(__MINGW32__)
    #define GLAD_PLATFORM_WIN32 1
  #else
    #define GLAD_PLATFORM_WIN32 0
  #endif
#endif

#ifndef GLAD_PLATFORM_APPLE
  #ifdef __APPLE__
    #define GLAD_PLATFORM_APPLE 1
  #else
    #define GLAD_PLATFORM_APPLE 0
  #endif
#endif

#ifndef GLAD_PLATFORM_EMSCRIPTEN
  #ifdef __EMSCRIPTEN__
    #define GLAD_PLATFORM_EMSCRIPTEN 1
  #else
    #define GLAD_PLATFORM_EMSCRIPTEN 0
  #endif
#endif

#ifndef GLAD_PLATFORM_UWP
  #if defined(_MSC_VER) && !defined(GLAD_INTERNAL_HAVE_WINAPIFAMILY)
    #ifdef __has_include
      #if __has_include(<winapifamily.h>)
        #define GLAD_INTERNAL_HAVE_WINAPIFAMILY 1
      #endif
    #elif _MSC_VER >= 1700 && !_USING_V110_SDK71_
      #define GLAD_INTERNAL_HAVE_WINAPIFAMILY 1
    #endif
  #endif

  #ifdef GLAD_INTERNAL_HAVE_WINAPIFAMILY
    #include <winapifamily.h>
    #if !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP) && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
      #define GLAD_PLATFORM_UWP 1
    #endif
  #endif

  #ifndef GLAD_PLATFORM_UWP
    #define GLAD_PLATFORM_UWP 0
  #endif
#endif

#ifdef __GNUC__
  #define GLAD_GNUC_EXTENSION __extension__
#else
  #define GLAD_GNUC_EXTENSION
#endif

#ifndef GLAD_API_CALL
  #if defined(GLAD_API_CALL_EXPORT)
    #if GLAD_PLATFORM_WIN32 || defined(__CYGWIN__)
      #if defined(GLAD_API_CALL_EXPORT_BUILD)
        #if defined(__GNUC__)
          #define GLAD_API_CALL __attribute__ ((dllexport)) extern
        #else
          #define GLAD_API_CALL __declspec(dllexport) extern
        #endif
      #else
        #if defined(__GNUC__)
          #define GLAD_API_CALL __attribute__ ((dllimport)) extern
        #else
          #define GLAD_API_CALL __declspec(dllimport) extern
        #endif
      #endif
    #elif defined(__GNUC__) && defined(GLAD_API_CALL_EXPORT_BUILD)
      #define GLAD_API_CALL __attribute__ ((visibility ("default"))) extern
    #else
      #define GLAD_API_CALL extern
    #endif
  #else
    #define GLAD_API_CALL extern
  #endif
#endif

#ifdef APIENTRY
  #define GLAD_API_PTR APIENTRY
#elif GLAD_PLATFORM_WIN32
  #define GLAD_API_PTR __stdcall
#else
  #define GLAD_API_PTR
#endif

#ifndef GLAPI
#define GLAPI GLAD_API_CALL
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY GLAD_API_PTR
#endif

#define GLAD_MAKE_VERSION(major, minor) (major * 10000 + minor)
#define GLAD_VERSION_MAJOR(version) (version / 10000)
#define GLAD_VERSION_MINOR(version) (version % 10000)

#define GLAD_GENERATOR_VERSION "2.0.0-beta"

typedef void (*GLADapiproc)(void);

typedef GLADapiproc (*GLADloadfunc)(const char *name);
typedef GLADapiproc (*GLADuserptrloadfunc)(void *userptr, const char *name);

typedef void (*GLADprecallback)(const char *name, GLADapiproc apiproc, int len_args, ...);
typedef void (*GLADpostcallback)(void *ret, const char *name, GLADapiproc apiproc, int len_args, ...);

#endif /* GLAD_PLATFORM_H_ */

#define GLX_ACCUM_ALPHA_SIZE 17
#define GLX_ACCUM_BLUE_SIZE 16
#define GLX_ACCUM_BUFFER_BIT 0x00000080
#define GLX_ACCUM_GREEN_SIZE 15
#define GLX_ACCUM_RED_SIZE 14
#define GLX_ALPHA_SIZE 11
#define GLX_AUX_BUFFERS 7
#define GLX_AUX_BUFFERS_BIT 0x00000010
#define GLX_BACK_LEFT_BUFFER_BIT 0x00000004
#define GLX_BACK_RIGHT_BUFFER_BIT 0x00000008
#define GLX_BAD_ATTRIBUTE 2
#define GLX_BAD_CONTEXT 5
#define GLX_BAD_ENUM 7
#define GLX_BAD_SCREEN 1
#define GLX_BAD_VALUE 6
#define GLX_BAD_VISUAL 4
#define GLX_BLUE_SIZE 10
#define GLX_BUFFER_SIZE 2
#define GLX_BufferSwapComplete 1
#define GLX_COLOR_INDEX_BIT 0x00000002
#define GLX_COLOR_INDEX_TYPE 0x8015
#define GLX_CONFIG_CAVEAT 0x20
#define GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
#define GLX_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define GLX_CONTEXT_DEBUG_BIT_ARB 0x00000001
#define GLX_CONTEXT_FLAGS_ARB 0x2094
#define GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
#define GLX_CONTEXT_PROFILE_MASK_ARB 0x9126
#define GLX_DAMAGED 0x8020
#define GLX_DEPTH_BUFFER_BIT 0x00000020
#define GLX_DEPTH_SIZE 12
#define GLX_DIRECT_COLOR 0x8003
#define GLX_DONT_CARE 0xFFFFFFFF
#define GLX_DOUBLEBUFFER 5
#define GLX_DRAWABLE_TYPE 0x8010
#define GLX_EVENT_MASK 0x801F
#define GLX_EXTENSIONS 0x3
#define GLX_EXTENSION_NAME "GLX"
#define GLX_FBCONFIG_ID 0x8013
#define GLX_FRONT_LEFT_BUFFER_BIT 0x00000001
#define GLX_FRONT_RIGHT_BUFFER_BIT 0x00000002
#define GLX_GRAY_SCALE 0x8006
#define GLX_GREEN_SIZE 9
#define GLX_HEIGHT 0x801E
#define GLX_LARGEST_PBUFFER 0x801C
#define GLX_LEVEL 3
#define GLX_MAX_PBUFFER_HEIGHT 0x8017
#define GLX_MAX_PBUFFER_PIXELS 0x8018
#define GLX_MAX_PBUFFER_WIDTH 0x8016
#define GLX_NONE 0x8000
#define GLX_NON_CONFORMANT_CONFIG 0x800D
#define GLX_NO_EXTENSION 3
#define GLX_PBUFFER 0x8023
#define GLX_PBUFFER_BIT 0x00000004
#define GLX_PBUFFER_CLOBBER_MASK 0x08000000
#define GLX_PBUFFER_HEIGHT 0x8040
#define GLX_PBUFFER_WIDTH 0x8041
#define GLX_PIXMAP_BIT 0x00000002
#define GLX_PRESERVED_CONTENTS 0x801B
#define GLX_PSEUDO_COLOR 0x8004
#define GLX_PbufferClobber 0
#define GLX_RED_SIZE 8
#define GLX_RENDER_TYPE 0x8011
#define GLX_RGBA 4
#define GLX_RGBA_BIT 0x00000001
#define GLX_RGBA_TYPE 0x8014
#define GLX_SAMPLES 100001
#define GLX_SAMPLE_BUFFERS 100000
#define GLX_SAVED 0x8021
#define GLX_SCREEN 0x800C
#define GLX_SLOW_CONFIG 0x8001
#define GLX_STATIC_COLOR 0x8005
#define GLX_STATIC_GRAY 0x8007
#define GLX_STENCIL_BUFFER_BIT 0x00000040
#define GLX_STENCIL_SIZE 13
#define GLX_STEREO 6
#define GLX_TRANSPARENT_ALPHA_VALUE 0x28
#define GLX_TRANSPARENT_BLUE_VALUE 0x27
#define GLX_TRANSPARENT_GREEN_VALUE 0x26
#define GLX_TRANSPARENT_INDEX 0x8009
#define GLX_TRANSPARENT_INDEX_VALUE 0x24
#define GLX_TRANSPARENT_RED_VALUE 0x25
#define GLX_TRANSPARENT_RGB 0x8008
#define GLX_TRANSPARENT_TYPE 0x23
#define GLX_TRUE_COLOR 0x8002
#define GLX_USE_GL 1
#define GLX_VENDOR 0x1
#define GLX_VERSION 0x2
#define GLX_VISUAL_ID 0x800B
#define GLX_WIDTH 0x801D
#define GLX_WINDOW 0x8022
#define GLX_WINDOW_BIT 0x00000001
#define GLX_X_RENDERABLE 0x8012
#define GLX_X_VISUAL_TYPE 0x22
#define __GLX_NUMBER_EVENTS 17


#ifndef GLEXT_64_TYPES_DEFINED
/* This code block is duplicated in glext.h, so must be protected */
#define GLEXT_64_TYPES_DEFINED
/* Define int32_t, int64_t, and uint64_t types for UST/MSC */
/* (as used in the GLX_OML_sync_control extension). */
#if defined(__STDC_VERSION__) && __STDC_VERSION__ >= 199901L
#include <inttypes.h>
#elif defined(__sun__) || defined(__digital__)
#include <inttypes.h>
#if defined(__STDC__)
#if defined(__arch64__) || defined(_LP64)
typedef long int int64_t;
typedef unsigned long int uint64_t;
#else
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#endif /* __arch64__ */
#endif /* __STDC__ */
#elif defined( __VMS ) || defined(__sgi)
#include <inttypes.h>
#elif defined(__SCO__) || defined(__USLC__)
#include <stdint.h>
#elif defined(__UNIXOS2__) || defined(__SOL64__)
typedef long int int32_t;
typedef long long int int64_t;
typedef unsigned long long int uint64_t;
#elif defined(_WIN32) && defined(__GNUC__)
#include <stdint.h>
#elif defined(_WIN32)
typedef __int32 int32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#else
/* Fallback if nothing above works */
#include <inttypes.h>
#endif
#endif

















#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1060)

#else

#endif


#if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ > 1060)

#else

#endif








typedef XID GLXFBConfigID;

typedef struct __GLXFBConfigRec *GLXFBConfig;

typedef XID GLXContextID;

typedef struct __GLXcontextRec *GLXContext;

typedef XID GLXPixmap;

typedef XID GLXDrawable;

typedef XID GLXWindow;

typedef XID GLXPbuffer;

typedef void (GLAD_API_PTR *__GLXextFuncPtr)(void);

typedef XID GLXVideoCaptureDeviceNV;

typedef unsigned int GLXVideoDeviceNV;

typedef XID GLXVideoSourceSGIX;

typedef XID GLXFBConfigIDSGIX;

typedef struct __GLXFBConfigRec *GLXFBConfigSGIX;

typedef XID GLXPbufferSGIX;

typedef struct {
    int event_type;             /* GLX_DAMAGED or GLX_SAVED */
    int draw_type;              /* GLX_WINDOW or GLX_PBUFFER */
    unsigned long serial;       /* # of last request processed by server */
    Bool send_event;            /* true if this came for SendEvent request */
    Display *display;           /* display the event was read from */
    GLXDrawable drawable;       /* XID of Drawable */
    unsigned int buffer_mask;   /* mask indicating which buffers are affected */
    unsigned int aux_buffer;    /* which aux buffer was affected */
    int x, y;
    int width, height;
    int count;                  /* if nonzero, at least this many more */
} GLXPbufferClobberEvent;

typedef struct {
    int type;
    unsigned long serial;       /* # of last request processed by server */
    Bool send_event;            /* true if this came from a SendEvent request */
    Display *display;           /* Display the event was read from */
    GLXDrawable drawable;       /* drawable on which event was requested in event mask */
    int event_type;
    int64_t ust;
    int64_t msc;
    int64_t sbc;
} GLXBufferSwapComplete;

typedef union __GLXEvent {
    GLXPbufferClobberEvent glxpbufferclobber;
    GLXBufferSwapComplete glxbufferswapcomplete;
    long pad[24];
} GLXEvent;

typedef struct {
    int type;
    unsigned long serial;
    Bool send_event;
    Display *display;
    int extension;
    int evtype;
    GLXDrawable window;
    Bool stereo_tree;
} GLXStereoNotifyEventEXT;

typedef struct {
    int type;
    unsigned long serial;   /* # of last request processed by server */
    Bool send_event;        /* true if this came for SendEvent request */
    Display *display;       /* display the event was read from */
    GLXDrawable drawable;   /* i.d. of Drawable */
    int event_type;         /* GLX_DAMAGED_SGIX or GLX_SAVED_SGIX */
    int draw_type;          /* GLX_WINDOW_SGIX or GLX_PBUFFER_SGIX */
    unsigned int mask;      /* mask indicating which buffers are affected*/
    int x, y;
    int width, height;
    int count;              /* if nonzero, at least this many more */
} GLXBufferClobberEventSGIX;

typedef struct {
    char    pipeName[80]; /* Should be [GLX_HYPERPIPE_PIPE_NAME_LENGTH_SGIX] */
    int     networkId;
} GLXHyperpipeNetworkSGIX;

typedef struct {
    char    pipeName[80]; /* Should be [GLX_HYPERPIPE_PIPE_NAME_LENGTH_SGIX] */
    int     channel;
    unsigned int participationType;
    int     timeSlice;
} GLXHyperpipeConfigSGIX;

typedef struct {
    char pipeName[80]; /* Should be [GLX_HYPERPIPE_PIPE_NAME_LENGTH_SGIX] */
    int srcXOrigin, srcYOrigin, srcWidth, srcHeight;
    int destXOrigin, destYOrigin, destWidth, destHeight;
} GLXPipeRect;

typedef struct {
    char pipeName[80]; /* Should be [GLX_HYPERPIPE_PIPE_NAME_LENGTH_SGIX] */
    int XOrigin, YOrigin, maxHeight, maxWidth;
} GLXPipeRectLimits;



#define GLX_VERSION_1_0 1
GLAD_API_CALL int GLAD_GLX_VERSION_1_0;
#define GLX_VERSION_1_1 1
GLAD_API_CALL int GLAD_GLX_VERSION_1_1;
#define GLX_VERSION_1_2 1
GLAD_API_CALL int GLAD_GLX_VERSION_1_2;
#define GLX_VERSION_1_3 1
GLAD_API_CALL int GLAD_GLX_VERSION_1_3;
#define GLX_VERSION_1_4 1
GLAD_API_CALL int GLAD_GLX_VERSION_1_4;
#define GLX_ARB_create_context 1
GLAD_API_CALL int GLAD_GLX_ARB_create_context;
#define GLX_ARB_create_context_profile 1
GLAD_API_CALL int GLAD_GLX_ARB_create_context_profile;


typedef GLXFBConfig * (GLAD_API_PTR *PFNGLXCHOOSEFBCONFIGPROC)(Display * dpy, int screen, const int * attrib_list, int * nelements);
typedef XVisualInfo * (GLAD_API_PTR *PFNGLXCHOOSEVISUALPROC)(Display * dpy, int screen, int * attribList);
typedef void (GLAD_API_PTR *PFNGLXCOPYCONTEXTPROC)(Display * dpy, GLXContext src, GLXContext dst, unsigned long mask);
typedef GLXContext (GLAD_API_PTR *PFNGLXCREATECONTEXTPROC)(Display * dpy, XVisualInfo * vis, GLXContext shareList, Bool direct);
typedef GLXContext (GLAD_API_PTR *PFNGLXCREATECONTEXTATTRIBSARBPROC)(Display * dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int * attrib_list);
typedef GLXPixmap (GLAD_API_PTR *PFNGLXCREATEGLXPIXMAPPROC)(Display * dpy, XVisualInfo * visual, Pixmap pixmap);
typedef GLXContext (GLAD_API_PTR *PFNGLXCREATENEWCONTEXTPROC)(Display * dpy, GLXFBConfig config, int render_type, GLXContext share_list, Bool direct);
typedef GLXPbuffer (GLAD_API_PTR *PFNGLXCREATEPBUFFERPROC)(Display * dpy, GLXFBConfig config, const int * attrib_list);
typedef GLXPixmap (GLAD_API_PTR *PFNGLXCREATEPIXMAPPROC)(Display * dpy, GLXFBConfig config, Pixmap pixmap, const int * attrib_list);
typedef GLXWindow (GLAD_API_PTR *PFNGLXCREATEWINDOWPROC)(Display * dpy, GLXFBConfig config, Window win, const int * attrib_list);
typedef void (GLAD_API_PTR *PFNGLXDESTROYCONTEXTPROC)(Display * dpy, GLXContext ctx);
typedef void (GLAD_API_PTR *PFNGLXDESTROYGLXPIXMAPPROC)(Display * dpy, GLXPixmap pixmap);
typedef void (GLAD_API_PTR *PFNGLXDESTROYPBUFFERPROC)(Display * dpy, GLXPbuffer pbuf);
typedef void (GLAD_API_PTR *PFNGLXDESTROYPIXMAPPROC)(Display * dpy, GLXPixmap pixmap);
typedef void (GLAD_API_PTR *PFNGLXDESTROYWINDOWPROC)(Display * dpy, GLXWindow win);
typedef const char * (GLAD_API_PTR *PFNGLXGETCLIENTSTRINGPROC)(Display * dpy, int name);
typedef int (GLAD_API_PTR *PFNGLXGETCONFIGPROC)(Display * dpy, XVisualInfo * visual, int attrib, int * value);
typedef GLXContext (GLAD_API_PTR *PFNGLXGETCURRENTCONTEXTPROC)(void);
typedef Display * (GLAD_API_PTR *PFNGLXGETCURRENTDISPLAYPROC)(void);
typedef GLXDrawable (GLAD_API_PTR *PFNGLXGETCURRENTDRAWABLEPROC)(void);
typedef GLXDrawable (GLAD_API_PTR *PFNGLXGETCURRENTREADDRAWABLEPROC)(void);
typedef int (GLAD_API_PTR *PFNGLXGETFBCONFIGATTRIBPROC)(Display * dpy, GLXFBConfig config, int attribute, int * value);
typedef GLXFBConfig * (GLAD_API_PTR *PFNGLXGETFBCONFIGSPROC)(Display * dpy, int screen, int * nelements);
typedef __GLXextFuncPtr (GLAD_API_PTR *PFNGLXGETPROCADDRESSPROC)(const GLubyte * procName);
typedef void (GLAD_API_PTR *PFNGLXGETSELECTEDEVENTPROC)(Display * dpy, GLXDrawable draw, unsigned long * event_mask);
typedef XVisualInfo * (GLAD_API_PTR *PFNGLXGETVISUALFROMFBCONFIGPROC)(Display * dpy, GLXFBConfig config);
typedef Bool (GLAD_API_PTR *PFNGLXISDIRECTPROC)(Display * dpy, GLXContext ctx);
typedef Bool (GLAD_API_PTR *PFNGLXMAKECONTEXTCURRENTPROC)(Display * dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx);
typedef Bool (GLAD_API_PTR *PFNGLXMAKECURRENTPROC)(Display * dpy, GLXDrawable drawable, GLXContext ctx);
typedef int (GLAD_API_PTR *PFNGLXQUERYCONTEXTPROC)(Display * dpy, GLXContext ctx, int attribute, int * value);
typedef void (GLAD_API_PTR *PFNGLXQUERYDRAWABLEPROC)(Display * dpy, GLXDrawable draw, int attribute, unsigned int * value);
typedef Bool (GLAD_API_PTR *PFNGLXQUERYEXTENSIONPROC)(Display * dpy, int * errorb, int * event);
typedef const char * (GLAD_API_PTR *PFNGLXQUERYEXTENSIONSSTRINGPROC)(Display * dpy, int screen);
typedef const char * (GLAD_API_PTR *PFNGLXQUERYSERVERSTRINGPROC)(Display * dpy, int screen, int name);
typedef Bool (GLAD_API_PTR *PFNGLXQUERYVERSIONPROC)(Display * dpy, int * maj, int * min);
typedef void (GLAD_API_PTR *PFNGLXSELECTEVENTPROC)(Display * dpy, GLXDrawable draw, unsigned long event_mask);
typedef void (GLAD_API_PTR *PFNGLXSWAPBUFFERSPROC)(Display * dpy, GLXDrawable drawable);
typedef void (GLAD_API_PTR *PFNGLXUSEXFONTPROC)(Font font, int first, int count, int list);
typedef void (GLAD_API_PTR *PFNGLXWAITGLPROC)(void);
typedef void (GLAD_API_PTR *PFNGLXWAITXPROC)(void);

GLAD_API_CALL PFNGLXCHOOSEFBCONFIGPROC glad_glXChooseFBConfig;
#define glXChooseFBConfig glad_glXChooseFBConfig
GLAD_API_CALL PFNGLXCHOOSEVISUALPROC glad_glXChooseVisual;
#define glXChooseVisual glad_glXChooseVisual
GLAD_API_CALL PFNGLXCOPYCONTEXTPROC glad_glXCopyContext;
#define glXCopyContext glad_glXCopyContext
GLAD_API_CALL PFNGLXCREATECONTEXTPROC glad_glXCreateContext;
#define glXCreateContext glad_glXCreateContext
GLAD_API_CALL PFNGLXCREATECONTEXTATTRIBSARBPROC glad_glXCreateContextAttribsARB;
#define glXCreateContextAttribsARB glad_glXCreateContextAttribsARB
GLAD_API_CALL PFNGLXCREATEGLXPIXMAPPROC glad_glXCreateGLXPixmap;
#define glXCreateGLXPixmap glad_glXCreateGLXPixmap
GLAD_API_CALL PFNGLXCREATENEWCONTEXTPROC glad_glXCreateNewContext;
#define glXCreateNewContext glad_glXCreateNewContext
GLAD_API_CALL PFNGLXCREATEPBUFFERPROC glad_glXCreatePbuffer;
#define glXCreatePbuffer glad_glXCreatePbuffer
GLAD_API_CALL PFNGLXCREATEPIXMAPPROC glad_glXCreatePixmap;
#define glXCreatePixmap glad_glXCreatePixmap
GLAD_API_CALL PFNGLXCREATEWINDOWPROC glad_glXCreateWindow;
#define glXCreateWindow glad_glXCreateWindow
GLAD_API_CALL PFNGLXDESTROYCONTEXTPROC glad_glXDestroyContext;
#define glXDestroyContext glad_glXDestroyContext
GLAD_API_CALL PFNGLXDESTROYGLXPIXMAPPROC glad_glXDestroyGLXPixmap;
#define glXDestroyGLXPixmap glad_glXDestroyGLXPixmap
GLAD_API_CALL PFNGLXDESTROYPBUFFERPROC glad_glXDestroyPbuffer;
#define glXDestroyPbuffer glad_glXDestroyPbuffer
GLAD_API_CALL PFNGLXDESTROYPIXMAPPROC glad_glXDestroyPixmap;
#define glXDestroyPixmap glad_glXDestroyPixmap
GLAD_API_CALL PFNGLXDESTROYWINDOWPROC glad_glXDestroyWindow;
#define glXDestroyWindow glad_glXDestroyWindow
GLAD_API_CALL PFNGLXGETCLIENTSTRINGPROC glad_glXGetClientString;
#define glXGetClientString glad_glXGetClientString
GLAD_API_CALL PFNGLXGETCONFIGPROC glad_glXGetConfig;
#define glXGetConfig glad_glXGetConfig
GLAD_API_CALL PFNGLXGETCURRENTCONTEXTPROC glad_glXGetCurrentContext;
#define glXGetCurrentContext glad_glXGetCurrentContext
GLAD_API_CALL PFNGLXGETCURRENTDISPLAYPROC glad_glXGetCurrentDisplay;
#define glXGetCurrentDisplay glad_glXGetCurrentDisplay
GLAD_API_CALL PFNGLXGETCURRENTDRAWABLEPROC glad_glXGetCurrentDrawable;
#define glXGetCurrentDrawable glad_glXGetCurrentDrawable
GLAD_API_CALL PFNGLXGETCURRENTREADDRAWABLEPROC glad_glXGetCurrentReadDrawable;
#define glXGetCurrentReadDrawable glad_glXGetCurrentReadDrawable
GLAD_API_CALL PFNGLXGETFBCONFIGATTRIBPROC glad_glXGetFBConfigAttrib;
#define glXGetFBConfigAttrib glad_glXGetFBConfigAttrib
GLAD_API_CALL PFNGLXGETFBCONFIGSPROC glad_glXGetFBConfigs;
#define glXGetFBConfigs glad_glXGetFBConfigs
GLAD_API_CALL PFNGLXGETPROCADDRESSPROC glad_glXGetProcAddress;
#define glXGetProcAddress glad_glXGetProcAddress
GLAD_API_CALL PFNGLXGETSELECTEDEVENTPROC glad_glXGetSelectedEvent;
#define glXGetSelectedEvent glad_glXGetSelectedEvent
GLAD_API_CALL PFNGLXGETVISUALFROMFBCONFIGPROC glad_glXGetVisualFromFBConfig;
#define glXGetVisualFromFBConfig glad_glXGetVisualFromFBConfig
GLAD_API_CALL PFNGLXISDIRECTPROC glad_glXIsDirect;
#define glXIsDirect glad_glXIsDirect
GLAD_API_CALL PFNGLXMAKECONTEXTCURRENTPROC glad_glXMakeContextCurrent;
#define glXMakeContextCurrent glad_glXMakeContextCurrent
GLAD_API_CALL PFNGLXMAKECURRENTPROC glad_glXMakeCurrent;
#define glXMakeCurrent glad_glXMakeCurrent
GLAD_API_CALL PFNGLXQUERYCONTEXTPROC glad_glXQueryContext;
#define glXQueryContext glad_glXQueryContext
GLAD_API_CALL PFNGLXQUERYDRAWABLEPROC glad_glXQueryDrawable;
#define glXQueryDrawable glad_glXQueryDrawable
GLAD_API_CALL PFNGLXQUERYEXTENSIONPROC glad_glXQueryExtension;
#define glXQueryExtension glad_glXQueryExtension
GLAD_API_CALL PFNGLXQUERYEXTENSIONSSTRINGPROC glad_glXQueryExtensionsString;
#define glXQueryExtensionsString glad_glXQueryExtensionsString
GLAD_API_CALL PFNGLXQUERYSERVERSTRINGPROC glad_glXQueryServerString;
#define glXQueryServerString glad_glXQueryServerString
GLAD_API_CALL PFNGLXQUERYVERSIONPROC glad_glXQueryVersion;
#define glXQueryVersion glad_glXQueryVersion
GLAD_API_CALL PFNGLXSELECTEVENTPROC glad_glXSelectEvent;
#define glXSelectEvent glad_glXSelectEvent
GLAD_API_CALL PFNGLXSWAPBUFFERSPROC glad_glXSwapBuffers;
#define glXSwapBuffers glad_glXSwapBuffers
GLAD_API_CALL PFNGLXUSEXFONTPROC glad_glXUseXFont;
#define glXUseXFont glad_glXUseXFont
GLAD_API_CALL PFNGLXWAITGLPROC glad_glXWaitGL;
#define glXWaitGL glad_glXWaitGL
GLAD_API_CALL PFNGLXWAITXPROC glad_glXWaitX;
#define glXWaitX glad_glXWaitX





GLAD_API_CALL int gladLoadGLXUserPtr(Display *display, int screen, GLADuserptrloadfunc load, void *userptr);
GLAD_API_CALL int gladLoadGLX(Display *display, int screen, GLADloadfunc load);

#ifdef GLAD_GLX

GLAD_API_CALL int gladLoaderLoadGLX(Display *display, int screen);

GLAD_API_CALL void gladLoaderUnloadGLX(void);

#endif
#ifdef __cplusplus
}
#endif
#endif
