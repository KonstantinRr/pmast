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
	// Groningen coordinates
    // tl,tr [53.265301,6.465842][53.265301,6.675939]
    // br,bl [53.144829,6.675939][53.144829, 6.465842]	
    //Rect initRect = Rect::fromBorders(53.144829, 53.265301, 6.465842, 6.675939);

    // Warendorf coordinates
    // tl,tr [51.9362,7.9553][51.9362,8.0259]
    // br,bl [51.9782,8.0259][51.9362,7.9553]
    Rect initRect = Rect::fromBorders(51.9362, 51.9782, 7.9553, 8.0259);


    Engine eng;
    eng.init("Window", 800, 600);

    auto manager = std::make_shared<ConcurrencyManager>();
	auto world = std::make_shared<World>(manager.get());

	auto m_canvas = std::make_shared<MapCanvas>(world->getMap());

	// Loads the default map
	bool loadDefault = true;
	if (loadDefault) {
		ParseTimings timings;
		ParseArguments args;
		args.file = "maps/warendorf.xmlmap";
		args.threads = 8;
		args.pool = manager.get();
		args.timings = &timings;

		auto newMap = std::make_shared<OSMSegment>(parseXMLMap(args));
		timings.summary();
		
		*newMap = newMap->findSquareNodes(initRect);

		world->loadMap(newMap);
		
		m_canvas->loadMap(world->getMap());
		m_canvas->loadHighwayMap(world->getHighwayMap());
		spdlog::info(m_canvas->info());
	}

	InputHandler &input = eng.input();

	using namespace nyrem::keys;
	using namespace nyrem::mouse;
	input.callbackKey(NYREM_KEY_ESCAPE).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_RELEASED)
			eng.shouldClose();
	});


	double rotateSpeed = 0.01;
	double translateSpeed = 0.015;
	double zoomSpeed = 1.5;
	input.loopKey(NYREM_KEY_P).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED)
			m_canvas->applyZoom(zoomSpeed);
	});
	input.loopKey(NYREM_KEY_O).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED)
			m_canvas->applyZoom(-zoomSpeed);
	});

	input.loopKey(NYREM_KEY_K).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED)
			m_canvas->applyRotation(rotateSpeed);
	});
	input.loopKey(NYREM_KEY_L).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED)
			m_canvas->applyRotation(-rotateSpeed);
	});


	input.loopKey(NYREM_KEY_LEFT).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED)
			m_canvas->applyTranslation({translateSpeed, 0.0});
	});
	input.loopKey(NYREM_KEY_RIGHT).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED)
			m_canvas->applyTranslation({-translateSpeed, 0.0});
	});
	input.loopKey(NYREM_KEY_UP).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED)
			m_canvas->applyTranslation({0.0, -translateSpeed});
	});
	input.loopKey(NYREM_KEY_DOWN).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED)
			m_canvas->applyTranslation({0.0, translateSpeed});
	});

	bool hasStart;
	bool hasEnd;
	glm::dvec2 start;
	glm::dvec2 end;

	input.callbackKey(NYREM_KEY_R).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED) {
			start = m_canvas->windowToPosition(
				{input.cursorX(), input.cursorY()});
			hasStart = true;
			spdlog::info("Set Start {} {}", start.x, start.y);
		}
	});
	input.callbackKey(NYREM_KEY_T).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED) {
			end = m_canvas->windowToPosition(
				{input.cursorX(), input.cursorY()});
			hasEnd = true;
			spdlog::info("Set End {} {}", end.x, end.y);
		}
	});
	input.callbackKey(NYREM_KEY_ENTER).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED) {
			auto& graph = world->getGraph();
			GraphNode& idStart = graph->findClosestNode(Point(start.x, start.y));
			GraphNode& idStop = graph->findClosestNode(Point(end.x, end.y));
			std::cout << "Searching route from " << idStart.nodeID << " " << idStop.nodeID << "\n";
			Route r = graph->findRoute(idStart.nodeID, idStop.nodeID);
			for (int64_t id : r.nodes) {
				std::cout << "Node: " << id << "\n";
			}
			m_canvas->loadRoute(r, world->getHighwayMap());
		}
	});


    eng.setPipeline(m_canvas.get());
    eng.mainloop();
    eng.exit();

    return 0;
}
