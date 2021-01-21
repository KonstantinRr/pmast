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

#ifndef NYREM_WINDOW_HPP
#define NYREM_WINDOW_HPP

#include <engine/internal.hpp>
#include <engine/listener.hpp>

#include <memory>     // uses std::unique_ptr
#include <functional>

NYREM_NAMESPACE_BEGIN

class SizedObject;
class Tickable;
class Engine;
class WorldHandler;
class InputHandler;
class Renderable;

//class SizedObject {
//public:
//	virtual int width();
//	virtual int height();
//
//	void setWidth(int width);
//	void setHeight(int height);
//protected:
//	int w, h;
//};

using MouseType = uint8_t;
namespace mouse
{
	const extern MouseType
		NYREM_BUTTON_1, NYREM_BUTTON_2, NYREM_BUTTON_3, NYREM_BUTTON_4,
		NYREM_BUTTON_5, NYREM_BUTTON_6, NYREM_BUTTON_7, NYREM_BUTTON_8,
		NYREM_BUTTON_LAST, NYREM_BUTTON_LEFT, NYREM_BUTTON_RIGHT, NYREM_BUTTON_MIDDLE;
}


using KeyType = uint16_t;
namespace keys
{
	/// <summary>
	/// Stores all key states that a key event can have
	/// </summary>
	const extern KeyType
		KEYSTATUS_PRESSED, KEYSTATUS_RELEASED, KEYSTATUS_UNKNOWN;
	/// <summary>
	/// Stores most keys on a regular keyboard. Every other key is interpreted as
	/// NYREM_KEY_UNKNOWN
	/// </summary>
	const extern KeyType
		NYREM_KEY_UNKNOWN, NYREM_KEY_SPACE, NYREM_KEY_APOSTROPHE, NYREM_KEY_COMMA,
		NYREM_KEY_MINUS, NYREM_KEY_PERIOD, NYREM_KEY_SLASH, NYREM_KEY_0, NYREM_KEY_1,
		NYREM_KEY_2, NYREM_KEY_3, NYREM_KEY_4, NYREM_KEY_5, NYREM_KEY_6, NYREM_KEY_7,
		NYREM_KEY_8, NYREM_KEY_9, NYREM_KEY_SEMICOLON, NYREM_KEY_EQUAL, NYREM_KEY_A,
		NYREM_KEY_B, NYREM_KEY_C, NYREM_KEY_D, NYREM_KEY_E, NYREM_KEY_F, NYREM_KEY_G,
		NYREM_KEY_H, NYREM_KEY_I, NYREM_KEY_J, NYREM_KEY_K, NYREM_KEY_L, NYREM_KEY_M,
		NYREM_KEY_N, NYREM_KEY_O, NYREM_KEY_P, NYREM_KEY_Q, NYREM_KEY_R, NYREM_KEY_S,
		NYREM_KEY_T, NYREM_KEY_U, NYREM_KEY_V, NYREM_KEY_W, NYREM_KEY_X, NYREM_KEY_Y,
		NYREM_KEY_Z, NYREM_KEY_LEFT_BRACKET, NYREM_KEY_BACKSLASH, NYREM_KEY_RIGHT_BRACKET,
		NYREM_KEY_GRAVE_ACCENT, NYREM_KEY_WORLD_1, NYREM_KEY_WORLD_2, NYREM_KEY_ESCAPE,
		NYREM_KEY_ENTER, NYREM_KEY_TAB, NYREM_KEY_BACKSPACE, NYREM_KEY_INSERT, NYREM_KEY_DELETE,
		NYREM_KEY_RIGHT, NYREM_KEY_LEFT, NYREM_KEY_DOWN, NYREM_KEY_UP, NYREM_KEY_PAGE_UP,
		NYREM_KEY_PAGE_DOWN, NYREM_KEY_HOME, NYREM_KEY_END, NYREM_KEY_CAPS_LOCK,
		NYREM_KEY_SCROLL_LOCK, NYREM_KEY_NUM_LOCK, NYREM_KEY_PRINT_SCREEN, NYREM_KEY_PAUSE,
		NYREM_KEY_F1, NYREM_KEY_F2, NYREM_KEY_F3, NYREM_KEY_F4, NYREM_KEY_F5, NYREM_KEY_F6,
		NYREM_KEY_F7, NYREM_KEY_F8, NYREM_KEY_F9, NYREM_KEY_F10, NYREM_KEY_F11, NYREM_KEY_F12,
		NYREM_KEY_F13, NYREM_KEY_F14, NYREM_KEY_F15, NYREM_KEY_F16, NYREM_KEY_F17, NYREM_KEY_F18,
		NYREM_KEY_F19, NYREM_KEY_F20, NYREM_KEY_F21, NYREM_KEY_F22, NYREM_KEY_F23, NYREM_KEY_F24,
		NYREM_KEY_F25, NYREM_KEY_KP_0, NYREM_KEY_KP_1, NYREM_KEY_KP_2, NYREM_KEY_KP_3,
		NYREM_KEY_KP_4, NYREM_KEY_KP_5, NYREM_KEY_KP_6, NYREM_KEY_KP_7, NYREM_KEY_KP_8,
		NYREM_KEY_KP_9, NYREM_KEY_KP_DECIMAL, NYREM_KEY_KP_DIVIDE, NYREM_KEY_KP_MULTIPLY,
		NYREM_KEY_KP_SUBTRACT, NYREM_KEY_KP_ADD, NYREM_KEY_KP_ENTER, NYREM_KEY_KP_EQUAL,
		NYREM_KEY_LEFT_SHIFT, NYREM_KEY_LEFT_CONTROL, NYREM_KEY_LEFT_ALT, NYREM_KEY_LEFT_SUPER,
		NYREM_KEY_RIGHT_SHIFT, NYREM_KEY_RIGHT_CONTROL, NYREM_KEY_RIGHT_ALT,
		NYREM_KEY_RIGHT_SUPER, NYREM_KEY_MENU, NYREM_KEY_LAST;
};

struct KeyEvent {
	int key, scancode, action, mods;
};
struct CharEvent {
	unsigned int codepoint;
};
struct CursorPosEvent {
	double xpos, ypos;
};
struct CursorButtonEvent {
	int button, action, mods;
	double xpos, ypos;
};
struct CursorScrollEvent {
	double xoffset, yoffset;
};
struct DropEvent {
	int count;
	const char **paths;
};

// This class is responsible for mapping 
class InputHandler {
public:
	using CallbackLoopKey = void(KeyEvent);
	using CallbackSKey = void(KeyEvent);
	using CallbackKey = void(KeyEvent);
	using CallbackChar = void(CharEvent);
	using CallbackCursorPos = void(CursorPosEvent);
	using CallbackCursorButton = void(CursorButtonEvent);
	using CallbackCursorScroll = void(CursorScrollEvent);
	using CallbackDrop = void(DropEvent);

	// <summary>Creates an input handler without any event handlers</summary>
	InputHandler();
	// <summary>Creates a copy of the given input handler</summary>
	InputHandler(const InputHandler &handler);
	// <summary>Moves the given input handler to *this</summary>
	InputHandler(InputHandler &&) = default;

	InputHandler& operator=(const InputHandler &handler);
	InputHandler& operator=(InputHandler &&) = default;
	~InputHandler() = default;

	int scanCode(KeyType key) const;

	/// <summary>
	/// Gets a callback reference whenever a specific key is pressed.
	/// Callback arguments: void(int scancode, int action, int mods)
	/// </summary>
	/// <returns>Listener object that listens to key presses</returns>
	Listener<CallbackSKey>& callbackKey(KeyType key);
	
	/// <summary>
	/// Gets a callback reference whenever a specific key is pressed.
	/// Callback arguments: void(int key, int scancode, int mods)
	/// </summary>
	/// <returns>Listener object that listens to key presses</returns>
	Listener<CallbackLoopKey>& loopKey(KeyType key, bool enable=true);
	
	void setLoopEnabled(KeyType key, bool val=true);
	bool isLoopEnabled(KeyType key) const;

	/// <summary>Gets a callback reference whenever a key is pressed.
	/// Callback arguments: void(int key, int scancode, int action, int mods)</summary>
	/// <returns>Listener object that listens to key presses</returns>
	Listener<CallbackKey>& callbackKey();


	/// <summary>Gets a callback reference whenever a character is written.
	/// Callback arguments: void(unsigned int codepoint)/summary>
	/// <returns>Listener object that listens to character callbacks</returns>
	Listener<CallbackChar>& callbackCharacter();

	/// <summary>Gets a callback reference whenever the cursor position changes
	/// Callback arguments: void(double xpos, double ypos)</summary>
	/// <returns>Listener that listens to cursor position updates</returns>
	Listener<CallbackCursorPos>& callbackCursorPos();
	Listener<CallbackCursorPos>& loopCursorPos();

	/// <summary>Gets a callback reference whenever a cursor button is pressed.
	/// Callback arguments: void(int button, int action, int mods)</summary>
	/// <returns>Listener that listens to cursor button updates</returns>
	Listener<CallbackCursorButton>& callbackCursorButton();
	Listener<CallbackCursorButton>& loopCursorButton();

	/// <summary>Gets a callback reference whenever the cursor scrolls.
	/// Callback arguments: void(double xoffset, double yoffset)</summary>
	/// <returns>Listener that listens to cursor scroll events</returns>
	Listener<CallbackCursorScroll>& callbackCursorScroll();
	/// <summary>Gets a callback reference whenever files are drag and dropped.
	/// Callback arguments: void(int count, const char** paths)</summary>
	/// <returns>Listener that listens to drag and drop events</returns>
	Listener<CallbackDrop>& callbackDrop();

	double cursorX() const;
	double cursorY() const;
	
protected:
	friend class Engine;

	struct InputHandlerImpl;
	std::unique_ptr<InputHandlerImpl> k_impl;
};

class RegisterableEventUpdate {
public:
	virtual void registerEventUpdate(InputHandler& handler) = 0;
	virtual void removeEventUpdate(InputHandler& handler);
};
class RegisterableWorldUpdate {
public:
	virtual void registerWorldUpdate(WorldHandler& handler) = 0;
	virtual void removeWorldUpdate(WorldHandler& handler) = 0;
};
class RegisterableEngineUpdate {
public:
	virtual void registerEngineUpdate(Engine& handler) = 0;
	virtual void removeEngineUpdate(Engine& handler) = 0;
};

class EngineTickable { public: virtual void update(Engine& engine, double dt) = 0; };
class WorldTickable { public: virtual void update(WorldHandler& world, double dt) = 0; };

class WorldHandler : public EngineTickable {
public:
	virtual void update(Engine& engine, double dt) override;

	void addTickable(const std::shared_ptr<WorldTickable>& tickable);
	void removeTickable(const std::shared_ptr<WorldTickable>& tickable);
	void clearTickables();

protected:
	std::vector<std::shared_ptr<WorldTickable>> k_tickables;
};

class Engine {
public:
	Engine();
	Engine(const Engine&) = delete;
	Engine(Engine&& engine);

	Engine& operator=(const Engine&) = delete;
	Engine& operator=(Engine&& engine);

	void swap(Engine &&engine);

	virtual ~Engine();

	void shouldClose();

	void init(const std::string& name, size_t width, size_t height);
	void mainloop();
	void exit();

	void setPipeline(Renderable* pipeline);
	void setPreRender(std::function<void()>&& func);
	void setPostRender(std::function<void()>&& func);

	void registerEngineUpdate(const std::shared_ptr<EngineTickable>& tickable);
	void removeTickable(const std::shared_ptr<EngineTickable>& tickable);
	void clearTickables();

	InputHandler& input();

public:
	virtual void keyCallback(int key, int scancode, int action, int mods);
	virtual void characterCallback(unsigned int codepoint);
	virtual void cursorPositionCallback(double xpos, double ypos);
	virtual void mouseButtonCallback(int button, int action, int mods);
	virtual void scrollCallback(double xoffset, double yoffset);
	virtual void dropCallback(int count, const char** paths);
	virtual void framebufferCallback(int width, int height);

protected:
	struct EngineImpl; // Engine implementation type
	std::unique_ptr<EngineImpl> k_impl;
};

NYREM_NAMESPACE_END

#endif // !NYREM_WINDOW_HPP
