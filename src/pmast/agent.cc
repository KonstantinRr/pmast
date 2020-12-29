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

#include <pmast/engine.hpp>
#include <pmast/agent.hpp>
#include <pmast/parser.hpp>

#include <ctpl.h>
#include <thread>

using namespace traffic;
using namespace glm;
using namespace std;

// ---- Agent ---- //

void Agent::setGoal(int64_t newGoal) { goalID = newGoal; }
int64_t Agent::getGoal() const { return goalID; }

void Agent::update() {

}

void Agent::makeGreedyChoice() {
    auto &goal = world->getGraph()->findNodeByID(goalID);
    auto &node = world->getGraph()->findNodeByID(nextVisited);
    
    vec2 goalVec = glm::normalize(node.getPosition() - goal.getPosition());

    int bestFit = -1;
    float bestDotProduct = 0;
    for (size_t i = 0; i < node.connections.size(); i++) {
        // Creates the junction vector for each connection
        int64_t junctionCrossID = node.connections[i].goal;
        auto &junctionCrossNode = world->getGraph()->findNodeByID(junctionCrossID);
        vec2 junctionCross = glm::normalize(
            junctionCrossNode.getPosition() - node.getPosition());

        // checks if the junction vector is better than
        // the current best one. Updates the values if so.
        float dotProduct = glm::dot(junctionCross, goalVec);
        if (dotProduct > bestDotProduct) {
            bestDotProduct = dotProduct;
            bestFit = static_cast<int>(i);
        }
    }

    // There is no way left. The agent is stuck
    if (bestFit == -1) {
        // TODO
    } else {
        // TODO
    }
}

// ---- ConcurrencyManager ---- //

struct ConcurrencyManager::ThreadManagerImpl {
    ctpl::thread_pool pool;
    
    ThreadManagerImpl(size_t n) : pool(n) { }
};

ConcurrencyManager::~ConcurrencyManager() { }

ConcurrencyManager::ConcurrencyManager()
{
    size_t size = std::thread::hardware_concurrency();
    resize(size == 0 ? 8 : size);
}

ConcurrencyManager::ConcurrencyManager(size_t n)
{
    resize(n);
}

void ConcurrencyManager::resize(size_t size)
{
    m_pool = std::make_unique<ThreadManagerImpl>(size);
}

void ConcurrencyManager::addRaw(const std::function<void(int)> &exec) {
    m_pool->pool.push(exec);
}

// ---- WorldChunk ---- //

int eraseFast(std::vector<int64_t>& vector, int64_t val)
{
    int found = 0;
    for (size_t i = 0; i < vector.size(); i++) {
        while (vector[i] == val) {
            int64_t lastVal = vector.back();
            vector.pop_back();
            if (i < vector.size()) {
                vector[i] = lastVal;
            }
            found++;
        }
    }
    return found;
}

bool contains(const std::vector<int64_t>& vector, int64_t id)
{
    for (const int64_t check : vector) {
        if (check == id) return true;
    }
    return false;
}

// ---- Word ---- //


traffic::World::World(ConcurrencyManager* manager)
{
    m_manager = manager;
}

traffic::World::World(ConcurrencyManager* manager, const std::shared_ptr<OSMSegment>& map)
{
    m_manager = manager;
    loadMap(map);
}

void traffic::World::loadMap(const std::shared_ptr<OSMSegment>& map)
{
    m_map = make_shared<OSMSegment>(map->findNodes(
        OSMFinder()
            .setNodeAccept([](const OSMNode &node) { return !node.hasTag("highway"); })
            .setWayAccept([](const OSMWay& way) { return !way.hasTag("highway"); })
            .setRelationAccept([](const OSMRelation& rl) { return !rl.hasTag("highway"); })
    ));
    k_highway_map = make_shared<OSMSegment>(map->findNodes(
        OSMFinder()
            .setWayAccept([](const OSMWay& way) { return way.hasTag("highway"); })
            .setRelationAccept([](const OSMRelation&) { return false; }) // relations are not needed
    ));

    m_map->summary();
    k_highway_map->summary();

    m_graph = make_shared<Graph>(k_highway_map);
    m_graph->checkConsistency();
    m_graph->optimize();
}

void traffic::World::loadMap(const std::string& file)
{
    ParseTimings timings;
    ParseArguments args;
    args.file = file;
    args.threads = 8;
    args.pool = m_manager;
    args.timings = &timings;
    
    auto newMap = std::make_shared<OSMSegment>(parseXMLMap(args));
    timings.summary();

    loadMap(newMap);
}

bool traffic::World::hasMap() const noexcept { return m_map.get(); }
const std::shared_ptr<OSMSegment>& traffic::World::getMap() const { return m_map; }
const std::shared_ptr<OSMSegment>& traffic::World::getHighwayMap() const { return k_highway_map; }
const std::shared_ptr<Graph>& World::getGraph() const { return m_graph; }
const std::vector<Agent>& World::getAgents() const { return m_agents; }
