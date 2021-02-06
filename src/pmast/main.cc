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

//#include <engine/window.hpp>

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


    auto engine = std::make_shared<Engine>();
    engine->init("Window", 800, 600);

    auto manager = std::make_shared<ConcurrencyManager>();
	auto world = std::make_shared<World>(manager.get());

	// Loads the default map
	bool loadDefault = true;
	if (loadDefault) {
		ParseTimings timings;
		ParseArguments args;
		//args.file = "assets/warendorf.osm";
		args.file = "assets/map.osm";
		args.threads = 8;
		args.pool = manager.get();
		args.timings = &timings;

		auto newMap = std::make_shared<OSMSegment>(parseXMLMap(args));
		timings.summary();
		
		*newMap = newMap->findSquareNodes(initRect);
		world->loadMap(newMap);
	}
	auto m_canvas = std::make_shared<MapCanvas>(engine, world);
	auto map_world = std::make_shared<MapWorld>(engine, world);
	auto navigator = std::make_shared<Navigator>(
		[&](const RouteSettings &settings) -> std::shared_ptr<EngineStage> {
			if (settings.routeName == std::string("canvas"))
				return m_canvas;
			else if (settings.routeName == std::string("world"))
				return map_world;
			return nullptr;
		},
		[&](const RouteSettings &settings) {
			return nullptr;
		},
		std::string("canvas")
	);
	
	m_canvas->setAgentList(world->getAgents());

	spdlog::info(m_canvas->info());

	InputHandler &input = engine->input();

	using namespace nyrem::keys;
	using namespace nyrem::mouse;
	input.callbackKey(NYREM_KEY_ESCAPE).listen(false, [&](KeyEvent e) {
		if (e.action == KEYSTATUS_RELEASED)
			engine->shouldClose();
	});

    engine->setPipeline(navigator);
    engine->mainloop();
	engine->setPostRender([]() {
		
	});
    engine->exit();

    return 0;
}
