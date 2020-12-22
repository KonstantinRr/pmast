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

#include <spdlog/spdlog.h>

#include <memory>
#include <vector>

#include <pmast/mapcanvas.hpp>
#include <pmast/osm.hpp>
#include <pmast/osm_graph.hpp>
#include <pmast/osm_mesh.hpp>
#include <pmast/parser.hpp>
#include <pmast/render.hpp>
#include <pmast/agent.hpp>

#include "engine/window.hpp"

using namespace nyrem;
using namespace traffic;

/*
class EngineService {
	bool mouse_button_event(
		const Vector2i& p, int button, bool down, int modifiers) {
		Canvas::mouse_button_event(p, button, down, modifiers);
		Vector2d position = toView(
			k_canvas.viewToPlane(k_canvas.windowToView(toGLM(p))));
		if (button == GLFW_MOUSE_BUTTON_1 && down) {
			k_canvas.cb_leftclick().trigger(k_canvas.getCursor());
			return true;
		}
		else if (button == GLFW_MOUSE_BUTTON_2 && down) {
			k_canvas.cb_rightclick().trigger(k_canvas.getCursor());
			return true;
		}
		return false;
	}
};

bool MapCanvasNano::mouse_drag_event(
	const Vector2i& p, const Vector2i& rel, int button, int modifiers) {
	Canvas::mouse_drag_event(p, rel, button, modifiers);
	if (button == 0b01)
		k_canvas.applyTranslation(
            k_canvas.scaleWindowDistance(toGLM(rel)));
	else if (button == 0b10)
		k_canvas.applyZoom(rel.y());
	else if (button == 0b11)
		k_canvas.applyRotation(rel.y() * 0.01);
	return true;
}

bool MapCanvasNano::mouse_motion_event(
	const Vector2i& p, const Vector2i& rel, int button, int modifiers)
{
	Canvas::mouse_motion_event(p, rel, button, modifiers);
	glm::dvec2 cursor = k_canvas.viewToPlane(k_canvas.windowToView(toGLM(p))); // TODO change cursor
	k_canvas.cb_cursor_moved().trigger(cursor);
	return true;
}

bool MapCanvasNano::scroll_event(const Vector2i& p, const Vector2f& rel)
{
	Canvas::scroll_event(p, rel);
	k_canvas.setZoom(std::clamp(
		k_canvas.getZoom() * pow(0.94, -rel.y()), 2.0, 1000.0));
	return true;
}


void MapCanvasNano::updateKeys(double dt)
{
	GLFWwindow* window = screen()->glfw_window();
	double arrowSpeed = dt * 0.8;

	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		k_canvas.applyZoom(4.0);
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		k_canvas.applyZoom(-4.0);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		k_canvas.applyTranslation(glm::dvec2(arrowSpeed, 0.0) / k_canvas.getZoom());
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		k_canvas.applyTranslation(glm::dvec2(-arrowSpeed, 0.0) / k_canvas.getZoom());

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		k_canvas.applyTranslation(glm::dvec2(0.0, -arrowSpeed) / k_canvas.getZoom());
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		k_canvas.applyTranslation(glm::dvec2(0.0, arrowSpeed) / k_canvas.getZoom());
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
		k_canvas.setRotation(k_canvas.getRotation() + 0.01);
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
		k_canvas.setRotation(k_canvas.getRotation() - 0.01);
}
*/
int main(int argc, char** argv)
{
    spdlog::info("Starting Engine Backend");
    Engine eng;
    eng.init("Window", 800, 600);

    auto manager = std::make_shared<ConcurrencyManager>();
	auto world = std::make_shared<World>(manager.get());

    SizedObject size;
	size.setWidth(800);
	size.setHeight(600);

	auto m_canvas = std::make_shared<MapCanvas>(world->getMap(), &size);
	m_canvas->setActive(true);

	// Loads the default map
	bool loadDefault = true;
	if (loadDefault) {
		world->loadMap("maps/warendorf.xmlmap");
		m_canvas->loadMap(world->getMap());
		m_canvas->loadHighwayMap(world->getHighwayMap());
		m_canvas->setActive(true);
	}

    eng.setPipeline(m_canvas.get());
    eng.mainloop();
    eng.exit();

    return 0;
}
