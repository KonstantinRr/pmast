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

#include <engine/window.hpp>
#include <engine/shader.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <spdlog/spdlog.h>
#include <unordered_map>

using namespace nyrem;

std::unordered_map<GLFWwindow*, Engine*> windowMap;

KeyType
    keys::KEYSTATUS_PRESSED = (KeyType)GLFW_PRESS,
    keys::KEYSTATUS_RELEASED = (KeyType)GLFW_RELEASE,
    keys::KEYSTATUS_UNKNOWN = (KeyType)GLFW_KEY_UNKNOWN;
KeyType
    keys::NYREM_KEY_UNKNOWN = (KeyType)GLFW_KEY_UNKNOWN,
    keys::NYREM_KEY_SPACE = (KeyType)GLFW_KEY_SPACE,      // Printable keys
    keys::NYREM_KEY_APOSTROPHE = (KeyType)GLFW_KEY_APOSTROPHE, // '
    keys::NYREM_KEY_COMMA = (KeyType)GLFW_KEY_COMMA,      // ,
    keys::NYREM_KEY_MINUS = (KeyType)GLFW_KEY_MINUS,      // -
    keys::NYREM_KEY_PERIOD = (KeyType)GLFW_KEY_PERIOD,     // .
    keys::NYREM_KEY_SLASH = (KeyType)GLFW_KEY_SLASH,      // /
    keys::NYREM_KEY_0 = (KeyType)GLFW_KEY_0,
    keys::NYREM_KEY_1 = (KeyType)GLFW_KEY_1,
    keys::NYREM_KEY_2 = (KeyType)GLFW_KEY_2,
    keys::NYREM_KEY_3 = (KeyType)GLFW_KEY_3,
    keys::NYREM_KEY_4 = (KeyType)GLFW_KEY_4,
    keys::NYREM_KEY_5 = (KeyType)GLFW_KEY_5,
    keys::NYREM_KEY_6 = (KeyType)GLFW_KEY_6,
    keys::NYREM_KEY_7 = (KeyType)GLFW_KEY_7,
    keys::NYREM_KEY_8 = (KeyType)GLFW_KEY_8,
    keys::NYREM_KEY_9 = (KeyType)GLFW_KEY_9,
    keys::NYREM_KEY_SEMICOLON = (KeyType)GLFW_KEY_SEMICOLON,  // ,
    keys::NYREM_KEY_EQUAL = (KeyType)GLFW_KEY_EQUAL,      // =
    keys::NYREM_KEY_A = (KeyType)GLFW_KEY_A,
    keys::NYREM_KEY_B = (KeyType)GLFW_KEY_B,
    keys::NYREM_KEY_C = (KeyType)GLFW_KEY_C,
    keys::NYREM_KEY_D = (KeyType)GLFW_KEY_D,
    keys::NYREM_KEY_E = (KeyType)GLFW_KEY_E,
    keys::NYREM_KEY_F = (KeyType)GLFW_KEY_F,
    keys::NYREM_KEY_G = (KeyType)GLFW_KEY_G,
    keys::NYREM_KEY_H = (KeyType)GLFW_KEY_H,
    keys::NYREM_KEY_I = (KeyType)GLFW_KEY_I,
    keys::NYREM_KEY_J = (KeyType)GLFW_KEY_J,
    keys::NYREM_KEY_K = (KeyType)GLFW_KEY_K,
    keys::NYREM_KEY_L = (KeyType)GLFW_KEY_L,
    keys::NYREM_KEY_M = (KeyType)GLFW_KEY_M,
    keys::NYREM_KEY_N = (KeyType)GLFW_KEY_N,
    keys::NYREM_KEY_O = (KeyType)GLFW_KEY_O,
    keys::NYREM_KEY_P = (KeyType)GLFW_KEY_P,
    keys::NYREM_KEY_Q = (KeyType)GLFW_KEY_Q,
    keys::NYREM_KEY_R = (KeyType)GLFW_KEY_R,
    keys::NYREM_KEY_S = (KeyType)GLFW_KEY_S,
    keys::NYREM_KEY_T = (KeyType)GLFW_KEY_T,
    keys::NYREM_KEY_U = (KeyType)GLFW_KEY_U,
    keys::NYREM_KEY_V = (KeyType)GLFW_KEY_V,
    keys::NYREM_KEY_W = (KeyType)GLFW_KEY_W,
    keys::NYREM_KEY_X = (KeyType)GLFW_KEY_X,
    keys::NYREM_KEY_Y = (KeyType)GLFW_KEY_Y,
    keys::NYREM_KEY_Z = (KeyType)GLFW_KEY_Z,
    keys::NYREM_KEY_LEFT_BRACKET = (KeyType)GLFW_KEY_LEFT_BRACKET,  // [
    keys::NYREM_KEY_BACKSLASH = (KeyType)GLFW_KEY_BACKSLASH,     //
    keys::NYREM_KEY_RIGHT_BRACKET = (KeyType)GLFW_KEY_RIGHT_BRACKET, // ]
    keys::NYREM_KEY_GRAVE_ACCENT = (KeyType)GLFW_KEY_GRAVE_ACCENT,  // `
    keys::NYREM_KEY_WORLD_1 = (KeyType)GLFW_KEY_WORLD_1,       // non-US #1
    keys::NYREM_KEY_WORLD_2 = (KeyType)GLFW_KEY_WORLD_2,       // non-US #2
    keys::NYREM_KEY_ESCAPE = (KeyType)GLFW_KEY_ESCAPE,        // Function key
    keys::NYREM_KEY_ENTER = (KeyType)GLFW_KEY_ENTER,
    keys::NYREM_KEY_TAB = (KeyType)GLFW_KEY_TAB,
    keys::NYREM_KEY_BACKSPACE = (KeyType)GLFW_KEY_BACKSPACE,
    keys::NYREM_KEY_INSERT = (KeyType)GLFW_KEY_INSERT,
    keys::NYREM_KEY_DELETE = (KeyType)GLFW_KEY_DELETE,
    keys::NYREM_KEY_RIGHT = (KeyType)GLFW_KEY_RIGHT,
    keys::NYREM_KEY_LEFT = (KeyType)GLFW_KEY_LEFT,
    keys::NYREM_KEY_DOWN = (KeyType)GLFW_KEY_DOWN,
    keys::NYREM_KEY_UP = (KeyType)GLFW_KEY_UP,
    keys::NYREM_KEY_PAGE_UP = (KeyType)GLFW_KEY_PAGE_UP,
    keys::NYREM_KEY_PAGE_DOWN = (KeyType)GLFW_KEY_PAGE_DOWN,
    keys::NYREM_KEY_HOME = (KeyType)GLFW_KEY_HOME,
    keys::NYREM_KEY_END = (KeyType)GLFW_KEY_END,
    keys::NYREM_KEY_CAPS_LOCK = (KeyType)GLFW_KEY_CAPS_LOCK,
    keys::NYREM_KEY_SCROLL_LOCK = (KeyType)GLFW_KEY_SCROLL_LOCK,
    keys::NYREM_KEY_NUM_LOCK = (KeyType)GLFW_KEY_NUM_LOCK,
    keys::NYREM_KEY_PRINT_SCREEN = (KeyType)GLFW_KEY_PRINT_SCREEN,
    keys::NYREM_KEY_PAUSE = (KeyType)GLFW_KEY_PAUSE,
    keys::NYREM_KEY_F1 = (KeyType)GLFW_KEY_F1,
    keys::NYREM_KEY_F2 = (KeyType)GLFW_KEY_F2,
    keys::NYREM_KEY_F3 = (KeyType)GLFW_KEY_F3,
    keys::NYREM_KEY_F4 = (KeyType)GLFW_KEY_F4,
    keys::NYREM_KEY_F5 = (KeyType)GLFW_KEY_F5,
    keys::NYREM_KEY_F6 = (KeyType)GLFW_KEY_F6,
    keys::NYREM_KEY_F7 = (KeyType)GLFW_KEY_F7,
    keys::NYREM_KEY_F8 = (KeyType)GLFW_KEY_F8,
    keys::NYREM_KEY_F9 = (KeyType)GLFW_KEY_F9,
    keys::NYREM_KEY_F10 = (KeyType)GLFW_KEY_F10,
    keys::NYREM_KEY_F11 = (KeyType)GLFW_KEY_F11,
    keys::NYREM_KEY_F12 = (KeyType)GLFW_KEY_F12,
    keys::NYREM_KEY_F13 = (KeyType)GLFW_KEY_F13,
    keys::NYREM_KEY_F14 = (KeyType)GLFW_KEY_F14,
    keys::NYREM_KEY_F15 = (KeyType)GLFW_KEY_F15,
    keys::NYREM_KEY_F16 = (KeyType)GLFW_KEY_F16,
    keys::NYREM_KEY_F17 = (KeyType)GLFW_KEY_F17,
    keys::NYREM_KEY_F18 = (KeyType)GLFW_KEY_F18,
    keys::NYREM_KEY_F19 = (KeyType)GLFW_KEY_F19,
    keys::NYREM_KEY_F20 = (KeyType)GLFW_KEY_F20,
    keys::NYREM_KEY_F21 = (KeyType)GLFW_KEY_F21,
    keys::NYREM_KEY_F22 = (KeyType)GLFW_KEY_F22,
    keys::NYREM_KEY_F23 = (KeyType)GLFW_KEY_F23,
    keys::NYREM_KEY_F24 = (KeyType)GLFW_KEY_F24,
    keys::NYREM_KEY_F25 = (KeyType)GLFW_KEY_F25,
    keys::NYREM_KEY_KP_0 = (KeyType)GLFW_KEY_KP_0,
    keys::NYREM_KEY_KP_1 = (KeyType)GLFW_KEY_KP_1,
    keys::NYREM_KEY_KP_2 = (KeyType)GLFW_KEY_KP_2,
    keys::NYREM_KEY_KP_3 = (KeyType)GLFW_KEY_KP_3,
    keys::NYREM_KEY_KP_4 = (KeyType)GLFW_KEY_KP_4,
    keys::NYREM_KEY_KP_5 = (KeyType)GLFW_KEY_KP_5,
    keys::NYREM_KEY_KP_6 = (KeyType)GLFW_KEY_KP_6,
    keys::NYREM_KEY_KP_7 = (KeyType)GLFW_KEY_KP_7,
    keys::NYREM_KEY_KP_8 = (KeyType)GLFW_KEY_KP_8,
    keys::NYREM_KEY_KP_9 = (KeyType)GLFW_KEY_KP_9,
    keys::NYREM_KEY_KP_DECIMAL = (KeyType)GLFW_KEY_KP_DECIMAL,
    keys::NYREM_KEY_KP_DIVIDE = (KeyType)GLFW_KEY_KP_DIVIDE,
    keys::NYREM_KEY_KP_MULTIPLY = (KeyType)GLFW_KEY_KP_MULTIPLY,
    keys::NYREM_KEY_KP_SUBTRACT = (KeyType)GLFW_KEY_KP_SUBTRACT,
    keys::NYREM_KEY_KP_ADD = (KeyType)GLFW_KEY_KP_ADD,
    keys::NYREM_KEY_KP_ENTER = (KeyType)GLFW_KEY_KP_ENTER,
    keys::NYREM_KEY_KP_EQUAL = (KeyType)GLFW_KEY_KP_EQUAL,
    keys::NYREM_KEY_LEFT_SHIFT = (KeyType)GLFW_KEY_LEFT_SHIFT,
    keys::NYREM_KEY_LEFT_CONTROL = (KeyType)GLFW_KEY_LEFT_CONTROL,
    keys::NYREM_KEY_LEFT_ALT = (KeyType)GLFW_KEY_LEFT_ALT,
    keys::NYREM_KEY_LEFT_SUPER = (KeyType)GLFW_KEY_LEFT_SUPER,
    keys::NYREM_KEY_RIGHT_SHIFT = (KeyType)GLFW_KEY_RIGHT_SHIFT,
    keys::NYREM_KEY_RIGHT_CONTROL = (KeyType)GLFW_KEY_RIGHT_CONTROL,
    keys::NYREM_KEY_RIGHT_ALT = (KeyType)GLFW_KEY_RIGHT_ALT,
    keys::NYREM_KEY_RIGHT_SUPER = (KeyType)GLFW_KEY_RIGHT_SUPER,
    keys::NYREM_KEY_MENU = (KeyType)GLFW_KEY_MENU;

void error_callback(int error, const char* description) {
    spdlog::error("Captured GLFW Error: {}", description);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    windowMap[window]->keyCallback(key, scancode, action, mods);
    //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    //    glfwSetWindowShouldClose(window, GLFW_TRUE);
}
void character_callback(GLFWwindow* window, unsigned int codepoint)
{ windowMap[window]->characterCallback(codepoint); }
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{ windowMap[window]->cursorPositionCallback(xpos, ypos); }
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{ windowMap[window]->mouseButtonCallback(button, action, mods); }
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{ windowMap[window]->scrollCallback(xoffset, yoffset); }
void drop_callback(GLFWwindow* window, int count, const char** paths)
{ windowMap[window]->dropCallback(count, paths); }
void framebuffer_callback(GLFWwindow* window, int width, int height)
{ windowMap[window]->framebufferCallback(width, height); }

//// ---- WorldHandler ---- ////
void WorldHandler::update(Engine &engine, double dt) {
    for (auto &tickable : k_tickables)
        tickable->update(*this, dt);
}

void WorldHandler::addTickable(const std::shared_ptr<WorldTickable> &tickable) { k_tickables.push_back(tickable); }
void WorldHandler::removeTickable(const std::shared_ptr<WorldTickable> &tickable) {
    k_tickables.erase(std::remove_if(k_tickables.begin(), k_tickables.end(), [&](const auto &i) { return i == tickable; }));
}
void WorldHandler::clearTickables() { k_tickables.clear();}


//// ---- Engine ---- ////

struct Engine::EngineImpl {
    std::vector<std::shared_ptr<EngineTickable>> k_tickables;
    InputHandler k_input;
    std::function<void()> k_pre_render;
    std::function<void()> k_post_render;
    Renderable* k_pipeline = nullptr;
    GLFWwindow* k_window = nullptr;
};

nyrem::Engine::Engine()
    : k_impl(std::make_unique<EngineImpl>())
{
}

Engine::Engine(Engine &&engine)
    : k_impl(std::move(engine.k_impl)) { }

Engine& Engine::operator=(Engine &&engine) {
    k_impl = std::move(engine.k_impl);
    return *this;
}
        

Engine::~Engine() {
    if (k_impl->k_window) {
        auto it = windowMap.find(k_impl->k_window);
        if (it == windowMap.end())
            windowMap.erase(it);
    }
}

void Engine::init(const std::string &name, size_t width, size_t height) {
    spdlog::info("Initializing GLFW Environment");
    if (!glfwInit()) {
        // Initialization failed
        const char *errorMSG = "GLFW Initialization failed!";
        spdlog::error(errorMSG);
        throw std::runtime_error(errorMSG);
    }
    glfwSetErrorCallback(error_callback);
    
    spdlog::info("Creating GLFW Window");
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    k_impl->k_window = glfwCreateWindow(
        static_cast<int>(width), static_cast<int>(height),
        name.c_str(), NULL, NULL);
    if (!k_impl->k_window) {
        const char *errorMSG = "GLFW Window Initialization failed!";
        spdlog::error(errorMSG);
        throw std::runtime_error(errorMSG);
        // Window or OpenGL context creation failed
    }
    glfwSetInputMode(k_impl->k_window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(k_impl->k_window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    glfwSetKeyCallback(k_impl->k_window, key_callback);
    glfwSetCharCallback(k_impl->k_window, character_callback);
    glfwSetCursorPosCallback(k_impl->k_window, cursor_position_callback);
    glfwSetInputMode(k_impl->k_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetMouseButtonCallback(k_impl->k_window, mouse_button_callback);
    glfwSetScrollCallback(k_impl->k_window, scroll_callback);
    glfwSetDropCallback(k_impl->k_window, drop_callback);
    glfwSetWindowSizeCallback(k_impl->k_window, framebuffer_callback);

    windowMap[k_impl->k_window] = this;

    if (glfwRawMouseMotionSupported())
        glfwSetInputMode(k_impl->k_window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

    
    glfwMakeContextCurrent(k_impl->k_window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        const char *errorMSG = "GLAD Initialization failed!";
        spdlog::error(errorMSG);
        throw std::runtime_error(errorMSG);
    }

    // initializes the window sizes
    int w, h;
    glfwGetFramebufferSize(k_impl->k_window, &w, &h);
    glViewport(0, 0, w, h);
}

void Engine::mainloop() {
    glfwSwapInterval(1); // VSYNC
    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(k_impl->k_window)) {
        double nextTime = glfwGetTime();
        double dt = nextTime - lastTime;
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

        // Updates all child objects
        for (auto & tickable : k_impl->k_tickables)
            tickable->update(*this, dt);

        if (k_impl->k_pre_render) k_impl->k_pre_render();
        if (k_impl->k_pipeline) k_impl->k_pipeline->render();
        if (k_impl->k_post_render) k_impl->k_post_render();
        glfwSwapBuffers(k_impl->k_window);
        glfwPollEvents();
    }
}

void Engine::setPreRender(std::function<void()> &&func) { k_impl->k_pre_render = func; }
void Engine::setPostRender(std::function<void()> &&func) { k_impl->k_post_render = func; }
void Engine::setPipeline(Renderable *pipeline) { k_impl->k_pipeline = pipeline; }

void Engine::registerEngineUpdate(const std::shared_ptr<EngineTickable> &tickable) { k_impl->k_tickables.push_back(tickable); }
void Engine::removeTickable(const std::shared_ptr<EngineTickable> &tickable) {
    k_impl->k_tickables.erase(std::remove_if(k_impl->k_tickables.begin(), k_impl->k_tickables.end(), [&](const auto &i) { return i == tickable; }));
}
void Engine::clearTickables() { k_impl->k_tickables.clear(); }

void Engine::exit()
{
    spdlog::info("Terminating GLFW Environment");
    if (k_impl->k_window)
        glfwDestroyWindow(k_impl->k_window);
    glfwTerminate();
}

InputHandler& Engine::input() { return k_impl->k_input; }

struct InputHandler::InputHandlerImpl {
    Listener<CallbackKey> k_cb_key;
    Listener<CallbackChar> k_cb_character;
    Listener<CallbackCursorPos> k_cb_cursorpos;
    Listener<CallbackCursorButton> k_cb_mousebutton;
    Listener<CallbackCursorScroll> k_cb_scroll;
    Listener<CallbackDrop> k_cb_drop;
};

int nyrem::InputHandler::scanCode(KeyType key)
{
    return glfwGetKeyScancode(key);
}

InputHandler::InputHandler()
    : k_impl(std::make_unique< InputHandlerImpl>())
{

}

Listener<InputHandler::CallbackKey>& InputHandler::callbackKey() { return k_impl->k_cb_key; }
Listener<InputHandler::CallbackChar>& InputHandler::callbackCharacter() { return k_impl->k_cb_character; }
Listener<InputHandler::CallbackCursorPos>& InputHandler::callbackCursorPos() { return k_impl->k_cb_cursorpos; }
Listener<InputHandler::CallbackCursorButton>& InputHandler::callbackCursorButton() { return k_impl->k_cb_mousebutton; }
Listener<InputHandler::CallbackCursorScroll>& InputHandler::callbackCursorScroll() { return k_impl->k_cb_scroll; }
Listener<InputHandler::CallbackDrop>& InputHandler::callbackDrop() { return k_impl->k_cb_drop; }

int SizedObject::width() { return w; }
int SizedObject::height() { return h; }

void SizedObject::setWidth(int width) { w = width; }
void SizedObject::setHeight(int height) { h = height; }