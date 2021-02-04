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
#include <pmast/mapworld.hpp>
#include <pmast/osm.hpp>
#include <pmast/osm_graph.hpp>
#include <pmast/osm_mesh.hpp>
#include <pmast/parser.hpp>
#include <pmast/render.hpp>
#include <pmast/agent.hpp>

#include <engine/window.hpp>

using namespace nyrem;
using namespace traffic;

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

	// Loads the default map
	bool loadDefault = true;
	if (loadDefault) {
		ParseTimings timings;
		ParseArguments args;
		args.file = "assets/warendorf.xmlmap";
		args.threads = 8;
		args.pool = manager.get();
		args.timings = &timings;

		auto newMap = std::make_shared<OSMSegment>(parseXMLMap(args));
		timings.summary();
		
		*newMap = newMap->findSquareNodes(initRect);
		world->loadMap(newMap);
	}
	auto m_canvas = std::make_shared<MapCanvas>(
		world->getMap(), world->getHighwayMap());
	auto map_world = std::make_shared<MapWorld>(
		world->getMap(), world->getHighwayMap());
	m_canvas->setAgentList(world->getAgents());

	spdlog::info(m_canvas->info());

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

	bool hasStart = false;
	bool hasEnd = false;
	bool mode = false; // true is 3D
	glm::dvec2 start{ 0.0 };
	glm::dvec2 end{ 0.0 };

	input.callbackKey(NYREM_KEY_G).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED) {
			if (mode) {
				eng.setPipeline(m_canvas.get());
			} else {
				eng.setPipeline(map_world.get());
			}
			mode = !mode;
		}
	});

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
		if (e.action == KEYSTATUS_PRESSED && hasStart && hasEnd) {
			auto &traffic = world->getTrafficGraph();
			TrafficGraphNodeIndex idStart = traffic->findClosestNodeIdx(Point(start.x, start.y));
			TrafficGraphNodeIndex idStop = traffic->findClosestNodeIdx(Point(end.x, end.y));
			std::cout << "Searching route from " << idStart << " " << idStop << "\n";

			Route r = traffic->findRoute(idStart, idStop);
			for (int64_t id : r.nodes) {
				std::cout << "Node: " << id << "\n";
			}
			m_canvas->loadRoute(r, world->getHighwayMap());
		}
	});
	input.callbackKey(NYREM_KEY_H).listen([&](KeyEvent e) {
		if (e.action == KEYSTATUS_PRESSED && hasStart && hasEnd) {
			auto &traffic = world->getTrafficGraph();
			TrafficGraphNodeIndex idStart = traffic->findClosestNodeIdx(Point(start.x, start.y));
			TrafficGraphNodeIndex idStop = traffic->findClosestNodeIdx(Point(end.x, end.y));
			std::cout << "Creating agent at " << idStart << " to " << idStop << "\n";
			world->createAgent(idStart, idStop);
		}
	});

	struct A {
		void a() { }
	};

	struct B {
		void a() { }
		void b() { }

	};

	struct C : public A, B {

	};
	C a;
	a.b();

	// 2, 2, 3, 3 1
	using VType = GenericVertex<
		TextureComponent2DFloat,
		VertexComponent3DFloat, NormalComponent3DFloat>;
	VType x;
	std::vector<VType> vec;
	using TrianlgeType = Triangle<float, TextureComponent2DFloat, VertexComponent2DFloat, VertexComponent3DFloat>;
	TrianlgeType tri;
	std::cout << "Area: " << tri.area() << " Use 3D " << TrianlgeType::use3D << " Use 2D " << TrianlgeType::use2D << " Loc V2 " <<
		TrianlgeType::VComponentType::locVertex2D << " LocV3 " << TrianlgeType::VComponentType::locVertex3D << std::endl;

	constexpr int loc = VType::componentIndex<VertexComponent3DFloat>();

	bool v = x.hasComponent<VertexComponent2DFloat>();
	x.component<VertexComponent3DFloat>() = vec3(2.0f);
	std::cout << sizeof(x) << " " << v << " " << loc << " " << "HELLO " << x.component<VertexComponent3DFloat>().get().x << std::endl;

    eng.setPipeline(m_canvas.get());
    eng.mainloop();
	eng.setPostRender([]() {
		
	});
    eng.exit();

    return 0;
}
