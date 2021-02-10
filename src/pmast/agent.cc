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

#include <glm/glm.hpp>

#include <pmast/agent.hpp>
#include <pmast/parser.hpp>
#include <pmast/osm.hpp>
#include <pmast/osm_graph.hpp>
#include <pmast/geom.hpp>
#include <pmast/osm_mesh.hpp>

#include <thread>

using namespace traffic;
using namespace glm;
using namespace std;

// ---- PhysicalEntity ---- //

PhysicalEntity::PhysicalEntity() noexcept :
    d_mass(float(1000)), d_speed(float(0)),
    d_maxDecelleration(float(0.8) * g),
    d_maxAcceleration(float(0.3) * g),
    d_tireFriction(float(1.0)) { }

PhysicalEntity::PhysicalEntity(
    float maxAcceleration, float maxDeceleration,
    float mass, float tireFric) noexcept :
    d_mass(mass), d_speed(float(0)),
    d_maxDecelleration(maxDeceleration),
    d_maxAcceleration(maxAcceleration),
    d_tireFriction(tireFric)
{

}

float PhysicalEntity::downforce() const noexcept { return g * d_mass; }
float PhysicalEntity::speed() const noexcept { return d_speed; }
float PhysicalEntity::energy() const noexcept {
    prec_t currentSpeed = speed();
    return d_mass * currentSpeed * currentSpeed;
}

float PhysicalEntity::accelerationTime(float newSpeed) const noexcept {
    prec_t currentSpeed = speed();
    if (newSpeed > currentSpeed) {
        // we want to accelerate
        return (newSpeed - currentSpeed) / d_maxAcceleration;
    }
    if (newSpeed < currentSpeed) {
        return (currentSpeed - newSpeed) / d_maxDecelleration;
    }
    return prec_t(0.0); // we are already at the given distance
}

float PhysicalEntity::accelerationDistance(float newSpeed) const noexcept {
    float currentSpeed = speed();
    if (newSpeed > currentSpeed) { // we want to accelerate
        float time = (newSpeed - currentSpeed) / d_maxAcceleration;
        return currentSpeed * time + 0.5 * d_maxAcceleration * time * time;
    }
    if (newSpeed < currentSpeed) { // we want to decellerate
        float time = (currentSpeed - newSpeed) / d_maxDecelleration;
        return currentSpeed * time - 0.5 * d_maxDecelleration * time * time; 
    }
    return float(0.0); // we are already at the given distance
}

float PhysicalEntity::tireFriction() const noexcept { return d_tireFriction; }
float PhysicalEntity::mass() const noexcept { return d_mass; }
const nyrem::vec2& PhysicalEntity::position() const noexcept { return d_position; }

void PhysicalEntity::setPosition(nyrem::vec2 pos) noexcept {
    d_position = pos;
}
void PhysicalEntity::setSpeed(float speed) noexcept {
    d_speed = speed;
}
// ---- Scheduler ---- //
Scheduler::Scheduler(const TrafficGraphNode &node) noexcept
    : m_node(node)
{

}

void Scheduler::update(float dt)
{

}

// ---- SchedulerAll ---- //

SchedulerAll::SchedulerAll(const TrafficGraphNode &node) noexcept
    : Scheduler(node) { }

void SchedulerAll::update(float dt)
{

}

// ---- SchedulerRightBeforeLeft ---- //

SchedulerRightBeforeLeft::SchedulerRightBeforeLeft(
    const TrafficGraphNode &node) noexcept : Scheduler(node) { }

void SchedulerRightBeforeLeft::update(float dt)
{

}

// ---- Agent ---- //
Agent::Agent(World& world, TrafficGraph& graph,
    TrafficGraphNodeIndex begin, TrafficGraphNodeIndex end) noexcept :
    m_world(&world), m_graph(&graph), m_begin(begin), m_end(end),
    m_node(begin), m_edge(nullIndex),  m_edgePosition(prec_t(0)) {
}

PhysicalEntity& Agent::physical() noexcept { return m_physicalEntity; }
const PhysicalEntity& Agent::physical() const noexcept { return m_physicalEntity; }

TrafficGraphNodeIndex Agent::goal() const noexcept { return m_end; }
TrafficGraphNodeIndex Agent::start() const noexcept { return m_begin; }

AgentState Agent::update(double dt)
{
    if (m_node == m_end)
        // we reached our destination, give back controll to the WorldHandler
        return DEAD; 

    // check if we have a route to our destination.
    if (!m_route.exists()) {
        m_route = m_graph->findIndexRoute(m_node, m_end);
        m_route_point = 0; // we start at the beginning
        if (!m_route.exists())
            // there is no possible way to reach the goal
            return DEAD;
    }

    // we have made sure that a route exists
    if (m_edge != nullIndex) {
        TrafficGraphNode *node = &m_graph->findNodeByIndex(m_node);
        TrafficGraphEdge *edge = &m_graph->findEdgeByIndex(m_node, m_edge);
        // we are currently driving on an edge
        float nextPosition = m_edgePosition + m_physicalEntity.speed();
        while (nextPosition > edge->distance) { // check for overshoot
            // subtracts the distance to the next node
            nextPosition -= edge->distance - m_edgePosition;
            // we reached a new new node and need to navigate
            node = &m_graph->findNodeByIndex(edge->goal);
            bool navigated = false;
            // go over all possible edges and check which matches the route
            for (TrafficGraphEdge& checkEdge : node->connections) {
                if (checkEdge.goal == m_route[m_route_point]) {
                    edge = &checkEdge;
                    navigated = true;
                }
            }
            if (!navigated) {
                return DEAD;
            }
        }
        m_edgePosition = nextPosition;
    }
    // assign the final position to the agent
    //TrafficGraphNode &nd = m_graph->findNodeByIndex(m_node);
    //vec2 pos = nd.plane();
    //if (m_edge )
    //m_physicalEntity.position()
    return ALIVE;
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


traffic::World::World(nyrem::ConcurrencyManager* manager)
{
    m_manager = manager;
}

traffic::World::World(nyrem::ConcurrencyManager* manager, const std::shared_ptr<OSMSegment>& map)
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
    m_transformer = std::make_shared<OSMViewTransformer>(*map);

    m_map->summary();
    k_highway_map->summary();

    m_graph = make_shared<Graph>(k_highway_map);
    m_graph->checkConsistency(*k_highway_map);
    // creates the optimized Traffic Graph
    m_traffic_graph = make_shared<TrafficGraph>(*m_graph, *m_transformer);
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

void World::update(double dt) {
    for (size_t i = 0; i < m_agents.size(); ) {
        AgentState state = m_agents[i].update(dt);
        if (state == DEAD) {
            m_agents.erase(m_agents.begin() + i);
        } else {
            i++;
        }
    }
}

void World::createAgent(TrafficGraphNodeIndex start, TrafficGraphNodeIndex end)
{
    spdlog::info("Creating agent at {} to {}", start, end);
    Agent agent(*this, *m_traffic_graph, start, end);
    auto &trafficNode = m_traffic_graph->buffer(start);
    agent.physical().setPosition(trafficNode.plane());
    m_agents.push_back(std::move(agent));
}


bool traffic::World::hasMap() const noexcept { return m_map.get(); }
const std::shared_ptr<OSMSegment>& traffic::World::getMap() const { return m_map; }
const std::shared_ptr<OSMSegment>& traffic::World::getHighwayMap() const { return k_highway_map; }
const std::shared_ptr<Graph>& World::getGraph() const { return m_graph; }
const std::shared_ptr<TrafficGraph>& World::getTrafficGraph() const { return m_traffic_graph; }
const std::vector<Agent>& World::getAgents() const { return m_agents; }
const std::shared_ptr<OSMViewTransformer>& World::transformer() const { return m_transformer; }