// MIT License
// 
// Copyright (c) 2020 Konstantin Rolf
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <engine/engine.h>

#if defined(__linux__) || defined(__unix__)

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <glad/gl.h>
#include <glad/glx.h>


typedef struct {
    const unsigned char *image;
    int length;
} AssetBuffer;


int env(int window_width, int window_height, AssetBuffer *buffer)
{
    // opens a connection to the X server and starts the display creation process
    Display *display = XOpenDisplay(NULL);
    if (!display) {
        fprintf(stderr, "Cannot connect to X server\n");
        return 1;
    }

    // finds the default screen connected to the display
    int screen = DefaultScreen(display);
    Window root = RootWindow(display, screen);
    Visual *visual = DefaultVisual(display, screen);

    Colormap colormap = XCreateColormap(display, root, visual, AllocNone);

    XSetWindowAttributes attributes;
    attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask;
    attributes.colormap = colormap;

    Window window = XCreateWindow(
        display, root, 0, 0, window_width, window_height, 0,
        DefaultDepth(display, screen), InputOutput, visual,
        CWColormap | CWEventMask, &attributes);

    XMapWindow(display, window);
    XStoreName(display, window, "[glad] Modern GLX with X11");

    if (!window) {
        fprintf(stderr, "Unable to create window.\n");
        return 1;
    }

    int glx_version = gladLoaderLoadGLX(display, screen);
    if (!glx_version) {
        fprintf(stderr, "Unable to load GLX.\n");
        return 1;
    }

    printf("Loaded GLX %d.%d\n",
        GLAD_VERSION_MAJOR(glx_version),
        GLAD_VERSION_MINOR(glx_version));

    GLint visual_attributes[] = {
        GLX_RENDER_TYPE, GLX_RGBA_BIT,
        GLX_DOUBLEBUFFER, 1, None
    };

    int num_fbc = 0;
    GLXFBConfig *fbc = glXChooseFBConfig(
        display, screen, visual_attributes, &num_fbc);

    GLint context_attributes[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
        GLX_CONTEXT_MINOR_VERSION_ARB, 3,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None
    };

    GLXContext context =
        glXCreateContextAttribsARB(display, fbc[0], NULL, 1, context_attributes);
    if (!context) {
        fprintf(stderr, "Unable to create OpenGL context.\n");
        return 1;
    }

    glXMakeCurrent(display, window, context);

    int gl_version = gladLoaderLoadGL();
    if (!gl_version) {
        fprintf(stderr, "Unable to load GL.\n");
        return 1;
    }
    printf("Loaded GL %d.%d\n",
        GLAD_VERSION_MAJOR(gl_version),
        GLAD_VERSION_MINOR(gl_version));

    XWindowAttributes gwa;
    XGetWindowAttributes(display, window, &gwa);
    glViewport(0, 0, gwa.width, gwa.height);

    Shader2D shader;
    Texture2D texture;
    EntityQueue queue;
    TileAtlas atlas;
    
    Create2DShader(&shader);
    Create2DTextureFromPNG(&texture, buffer->image, buffer->length);
    TileAtlasCreate(&atlas, &texture, 16);
    QueueCreate(&queue);
    EntityCreateTiling(QueuePointerNext(&queue), 5, &atlas, 0, 0, 0, 0.5);

    bool quit = false;
    while (!quit) {
        while (XPending(display)) {
            XEvent xev;
            XNextEvent(display, &xev);

            if (xev.type == KeyPress) {
                quit = true;
            }
        }

        glClearColor(0.8, 0.6, 0.7, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        // Rendering process
        Render2DShader(&shader, &texture, &queue);
        
        glXSwapBuffers(display, window);

        usleep(1000 * 10);
    }
    Delete2DShader(&shader);

    glXMakeCurrent(display, 0, 0);
    glXDestroyContext(display, context);

    XDestroyWindow(display, window);
    XFreeColormap(display, colormap);
    XCloseDisplay(display);

    gladLoaderUnloadGLX();
    return 0;
}

const int window_width = 800, window_height = 480;

int main() {
    // evaluate all the assets that are stored in the executable
    extern const unsigned char __start_foo_image[];
    extern const unsigned char __stop_foo_image[];

    __asm__("\n\
    .pushsection foo_image, \"a\", @progbits\n\
    .incbin \"map.png\"\n\
    .popsection\n");

    printf("Image size: %ld\n", __stop_foo_image - __start_foo_image);
    AssetBuffer buffer;
    buffer.image = __start_foo_image;
    buffer.length = __stop_foo_image - __start_foo_image;

	return env(window_width, window_height, &buffer);
}

#endif
