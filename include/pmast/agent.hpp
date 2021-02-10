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

#ifndef AGENT_H
#define AGENT_H

#include <pmast/internal.hpp>
#include <pmast/osm_graph.hpp>
#include <engine/thread.hpp>

#include <string>
#include <memory>

namespace traffic {

class Agent; // A single agent that takes part in the world
class World; // The world the agent takes part of

class OSMSegment;   // externally defined
class Graph;        // externally defined
class OSMViewTransformer;
class TrafficGraphEdge; // externally defined
class TrafficGraphNode; // externally defined
class TrafficGraph;     // externally defined

using AgentRef = Agent*;
using WorldRef = World*;


struct EnvironmentalInfo {
    using ThisType = EnvironmentalInfo;
    using EnvironmentalInfoType = ThisType;

    /// <summary>
    /// Stores the global friction coefficient
    /// </summary>
    prec_t friction;
};

/// <summary>
/// A physical car entity in the world. Each physical object has
/// some physical properties like a speed, tire friction, max acceleration,
/// max deceleration, the mass of the object and the position.
/// </summary>
class PhysicalEntity {
public:
    using ThisType = PhysicalEntity;
    using PhysicalEntityType = ThisType;

public:
    static constexpr float g = (float)9.81;
    
    /// <summary>
    /// Creates a new PhysicalEntity with the following values:
    /// mass = 1000.0, speed = 0.0,
    /// maxDecelleration = 0.8g,
    /// maxAcceleration = 0.3g,
    /// tireFriction = 1.0,
    /// </summary>
    PhysicalEntity() noexcept;

    /// <summary>
    /// Creates a new PhysicalEntity with the given values.
    /// </summary>
    /// <param name="maxAcceleration">Maximum acceleration the object can achieve</param>
    /// <param name="maxDeceleration">Maximum deceleration the object can achieve</param>
    /// <param name="mass">The mass this object has</param>
    /// <param name="tireFric">The tire friction coefficient of this object</param>
    PhysicalEntity(
        float maxAcceleration, float maxDeceleration,
        float mass, float tireFric) noexcept;

    /// <summary>
    /// Calculates the downforce in Newton that this object experiences F = g * m
    /// </summary>
    float downforce() const noexcept;

    /// <summary>
    /// Calculates the speed in m/s that this object has.
    /// </summary>
    float speed() const noexcept;

    /// <summary>
    /// Calculates the kinetic energy that this object has. E = m * v^2
    /// </summary>
    float energy() const noexcept;

    /// <summary>
    /// Calculates the time that this object needs to accelerate or decellerate
    /// from its current speed to the new speed. Returns the time in seconds.
    /// </summary>
    /// <param name="newSpeed">The desired speed this object should aquire.</param>
    float accelerationTime(float newSpeed) const noexcept;
    
    /// <summary>
    /// Calculates the distance that this object needs to accelerate or decellerate
    /// from its current speed to the new speed. Returns the distance in meters.
    /// </summary>
    /// <param name="newSpeed">The desired speed this object should aquire.</param>
    float accelerationDistance(float newSpeed) const noexcept;

    /// <summary>
    /// Returns the tire friction coefficient of this object.
    /// </summary>
    float tireFriction() const noexcept;

    /// <summary>
    /// Returns the mass of this object in kg.
    /// </summary>
    float mass() const noexcept;

    /// <summary>
    /// Returns the position of this object in relation to the origin of
    /// the coordinate system. Distances are given in meters.
    /// </summary>
    const nyrem::vec2& position() const noexcept;

    /// <summary>
    /// Sets the position of this object.
    /// </summary>
    /// <param name="pos">The new position in meters</param>
    void setPosition(nyrem::vec2 pos) noexcept;

    /// <summary>
    /// Sets the speed of this object.
    /// </summary>
    /// <param name="speed">The new speed in m/s.</param>
    void setSpeed(float speed) noexcept; 

protected:
    /// <summary>
    /// The current speed of the car in meters per second.
    /// </summary>
    float d_speed;

    /// <summary>
    /// The tire friction coefficient.
    /// </summary
    float d_tireFriction;

    /// <summary>
    /// The maximum acceleration that the car's engine can deliver in m/s^2.
    /// </summary>
    float d_maxAcceleration;

    /// <summary>
    /// The maximum decelleration that the car's brake can deliver in m/s^2
    /// </summary>
    float d_maxDecelleration;

    /// <summary>
    /// The mass of this car in kg. Used for future calculation.
    /// </summary>
    float d_mass;

    /// <summary>
    /// The car's current position in plane coordinates.
    /// The position is given as distance from the origin of the map.
    /// </summary>
    nyrem::vec2 d_position;
};

/// <summary>
/// The basic interface for all schedulers. Schedulers are used to
/// select which agents are allowed to pass through a given node.
/// They implement a basic update function that should be called by
/// the simulation manager.
/// </summary>
class Scheduler {
public:
    using ThisType = Scheduler;
    using SchedulerType = ThisType;

protected:
    const TrafficGraphNode &m_node;

public:
    /// <summary>
    /// Creates a new generic Scheduler that controls the given node
    /// </summary>
    /// <param name="node">The traffic node controlled by this scheduler.</param>
    Scheduler(const TrafficGraphNode &node) noexcept;
    virtual ~Scheduler() = default;

    /// <summary>
    /// Updates the given scheduler and applies all functions.
    /// The default implementation does not perform any actions.
    /// </summary>
    /// <param name="dt">The time that has passed since the last update.</param>
    virtual void update(float dt);
};

/// <summary>
/// This class implements the Scheduler interface. It opens all
/// gates in a TrafficGraphNode allowing the traffic to flow freely.
/// </summary>
class SchedulerAll : public Scheduler {
public:
    using ThisType = SchedulerAll;
    using SchedulerAllType = ThisType;

public:
    /// <summary>
    /// Creates a new open Scheduler that controls the given node
    /// </summary>
    /// <param name="node">The traffic node controlled by this scheduler.</param>
    SchedulerAll(const TrafficGraphNode &node) noexcept;
    virtual ~SchedulerAll() = default;

    /// <summary>
    /// Updates the Scheduler by opening all gates.
    /// </summary>
    /// <param name="dt">The time that has passed since the last update.</param>
    virtual void update(float dt) override;
};

/// <summary>
/// This class implements the Scheduler interface. It allows the traffic
/// from a given lane if there is not traffic at the lane right to it.
/// </summary>
class SchedulerRightBeforeLeft : public Scheduler {
public:
    using ThisType = SchedulerRightBeforeLeft;
    using SchedulerRightBeforeLeftType = ThisType;

public:
    /// <summary>
    /// Creates a new right before left scheduler that controls the given node
    /// </summary>
    /// <param name="node">The traffic node controlled by this scheduler.</param>
    SchedulerRightBeforeLeft(const TrafficGraphNode &node) noexcept;
    virtual ~SchedulerRightBeforeLeft() = default;

    /// <summary>
    /// Updates the scheduler and checks which lanes are allowed to drive
    /// </summary>
    /// <param name="dt">The time that has passed since the last update.</param>
    virtual void update(float dt) override;
};

/// <summary>
/// Determines the state of agent. Agents may either be ALIVE
/// or DEAD. Potential other states may be added in the future.
/// </summary>
enum AgentState {
    ALIVE, DEAD
};

/// <summary>
/// Agents are entities that act in the world to achieve a certain goal. Each agent
/// has its own set of believes desires and goals that it tries to achieve. Agents
/// are generally selfish meaning they always want the best outcome for themselves.
/// </summary>
class Agent : public nyrem::IDObject {
public:
    // ---- Constructors ---- //

    /// <summary>
    /// Creates a new agent with the following parameters
    /// <param name="world">The world in which the agent lives</param>
    /// <param name="graph">The traffic graph in which the agent navigates</param>
    /// <param name="begin">The position where the agent starts</param>
    /// <param name="end">The position where the agent should end</param>
    /// </summary
    Agent(World &world, TrafficGraph &graph,
        TrafficGraphNodeIndex begin, TrafficGraphNodeIndex end) noexcept;
    
    /// <summary>
    /// The class offers a virtual destructor so it can be destroyed by a
    /// reference to the base class.
    /// </summary>
    virtual ~Agent() = default;


    /// <summary>
    /// The index of the position where the agent wants to navigate to
    /// </summary>
    TrafficGraphNodeIndex goal() const noexcept;

    /// <summary>
    /// The index of the position where the agent started
    /// </summary> 
    TrafficGraphNodeIndex start() const noexcept;

    /// <summary>
    /// Updates the agent by using the following policies
    /// 1. Return DEAD if the current position is the end position
    /// 2. Generate a route from the current position to the goal if there is no route
    /// 3. Drive on the current edge
    /// 4. Waits at each node until the gate to its target is open
    /// </summary>
    AgentState update(double dt);

    /// <summary>
    /// Returns a modifiable reference to the physcial entitiy.
    /// </summary>
    PhysicalEntity& physical() noexcept;

    /// <summary>
    /// Returns a const reference to the physical entity.
    /// </summary>
    const PhysicalEntity& physical() const noexcept;

protected:
    // ---- Member definitions ---- //
    
    /// <summary>
    /// The physical entity that this entity represents in the world.
    /// </summary>
    PhysicalEntity m_physicalEntity;

    /// <summary>
    /// The route that the agent calculated to reach its goal. This route may
    /// be changed during navigating to the goal by the result of environmental
    /// changes. The route represents the shortes route to the destination in the
    /// agent's perception.
    /// </summary>
    IndexRoute m_route;

    size_t m_route_point;

    /// <summary>
    /// Stores a reference to the world the agent is living in. This variable
    /// should stay constant during runs to ensure the same behaviour. Changing
    /// the world during simulation runs may lead to unexpected behaviour.
    /// The world variable is mainly used to give controll back to the WorldHandler
    /// in case of important events. The agent will use the TrafficGraph for all
    /// navigation tasks.
    /// </summary>
    World* m_world;

    /// <summary>
    /// Allows the agent to access the traffic network to gain access to all
    /// navigational tasks.
    /// </summary>
    TrafficGraph *m_graph;

    /// <summary>
    /// Stores the street element that the agent is currently at
    /// </summary>
    TrafficGraphEdgeIndex m_edge;

    prec_t m_edgePosition;

    /// <summary>
    /// Stores the last node that the agent visited.
    /// </summary>
    TrafficGraphNodeIndex m_node;

    /// <summary>
    /// Stores the agent's starting position as well as its destination in the
    /// traffic graph. The agent's goal is satisfied as soon as it reaches the
    /// goal node. It will be destroyed by the World handler in that case. 
    /// </summary>
    TrafficGraphNodeIndex m_begin, m_end;
};

/// <summary>
/// The world class is responsible for handling all data and state
/// management for the simulation. It represents the environment in which
/// the agents are capable of acting.
/// </summary>
class World {
public:
    // ---- Contstructors ---- //
    World(nyrem::ConcurrencyManager *manager);
    World(nyrem::ConcurrencyManager *manager, const std::shared_ptr<OSMSegment> &map);

    // ---- Functions ---- //

    /// <summary>
    /// Checks whether this world currently has a map loaded.
    /// </summary>
    bool hasMap() const noexcept;
    
    /// <summary>
    /// Loads a map from an OSM file into this world.
    /// </summary>
    void loadMap(const std::shared_ptr<OSMSegment>& map);

    /// <summary>
    /// Loads a map from an OSM file stored at the host system,
    /// </summary>
    void loadMap(const std::string &file);
    

    /// <summary>
    /// Updates the world handler by updating all agents and schedulers
    /// </summary>
    /// <param name="dt">The time that has passed since the last update.</param>
    void update(double dt);

    /// <summary>
    /// Creates a new agent that starts at the given start index and
    /// navigates to the end index.
    /// </summary>
    void createAgent(TrafficGraphNodeIndex start, TrafficGraphNodeIndex end);

    // ---- Access modifiers ---- //

    /// <summary>
    /// Returns a reference to the map that defines this world.
    /// </summary>
    const std::shared_ptr<OSMSegment>& getMap() const;

    /// <summary>
    /// Returns a reference to the highway map generated from the map.
    /// </summary>
    const std::shared_ptr<OSMSegment>& getHighwayMap() const;

    /// <summary>
    /// Returns the graph generated from the highway map.
    /// </summary>
    const std::shared_ptr<Graph>& getGraph() const;

    /// <summary>
    /// Returns a reference to the transformer used to translate coordinates.
    /// </summary>
    const std::shared_ptr<OSMViewTransformer>& transformer() const;

    /// <summary>
    /// Returns a reference to the traffic graph generated from the highway graph.
    /// </summary>
    const std::shared_ptr<TrafficGraph>& getTrafficGraph() const;

    /// <summary>
    /// Returns a reference to the list of agents.
    /// </summary>
    const std::vector<Agent>& getAgents() const;

protected:
    // ---- Member definitions ---- //
    nyrem::ConcurrencyManager *m_manager;

    /// <summary>
    /// The general map that contains all data loaded from the OSM file
    /// </summary>
    std::shared_ptr<OSMSegment> m_map;

    /// <summary>
    /// The highway map represents a subset of the the general map and
    /// only includes the data that is used to represent the networs of streets.
    /// </summary>
    std::shared_ptr<OSMSegment> k_highway_map;

    /// <summary>
    /// The view transformer is used to transform coordinates given in latitude
    /// and longititude to local plane coordinates. Latitude and longitude coordinate
    /// pairs must be stored as double floating point values to give enough precision
    /// to determine small scale changes. The OSMViewTransformer transforms the coordinates
    /// into local space where it is possible to use default floating point scale.
    /// </summary>
    std::shared_ptr<OSMViewTransformer> m_transformer;

    /// <summary>
    /// A graph representation of the highway map.
    /// </summary>
    std::shared_ptr<Graph> m_graph;

    /// <summary>
    /// An optimized traffic graph that is overlayed on the OSMGraph to simulate the
    /// agents on this networl
    /// </summary>
    std::shared_ptr<TrafficGraph> m_traffic_graph;

    /// <summary>
    /// A list of all agents that are living at the current time stamp
    /// </summary>
    std::vector<Agent> m_agents;
};

} // namespace traffic

#endif
