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

namespace traffic
{
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
        /// <summary>
        /// Stores the global friction coefficient
        /// </summary>
        prec_t friction;
    };

    /// <summary>
    /// Torque Graph
    ///          { 
    /// T(rpm) = {
    ///.          {
    /// </summary>
    struct CarEngine {
        
    };

    class PhysicalEntity {
    public:
        static constexpr float g = (float)9.81;
        
        PhysicalEntity() noexcept;
        PhysicalEntity(
            float maxAcceleration, float maxDeceleration,
            float mass, float tireFric) noexcept;

        float downforce() const noexcept;
        float speed() const noexcept;
        float energy() const noexcept;

        float accelerationTime(float newSpeed) const noexcept;
        float accelerationDistance(float newSpeed) const noexcept;

        float tireFriction() const noexcept;
        float mass() const noexcept;
        
        nyrem::vec2& position() noexcept;
        const nyrem::vec2& velocity() const noexcept;
        const nyrem::vec2& position() const noexcept;

        void setPosition(nyrem::vec2 pos) noexcept;
        void setSpeed(float speed) noexcept; 

    protected:
        float d_speed;

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
        /// The mass of this car. Used for future calculation.
        /// </summary>
        float d_mass;

        /// <summary>
        /// The car's current position in plane coordinates
        /// </summary>
        nyrem::vec2 d_position;
    };

    class Scheduler {
    public:
        using ThisType = Scheduler;
        using SchedulerType = ThisType;

    protected:
        const TrafficGraphNode &m_node;

    public:
        Scheduler(const TrafficGraphNode &node) noexcept;
        virtual ~Scheduler() = default;

        virtual void update(float dt);
    };

    class SchedulerAll : public Scheduler {
    public:
        using ThisType = SchedulerAll;
        using SchedulerAllType = ThisType;

    public:
        SchedulerAll(const TrafficGraphNode &node) noexcept;
        virtual ~SchedulerAll() = default;

        virtual void update(float dt) override;
    };

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
        Agent(World &world, TrafficGraph &graph,
            TrafficGraphNodeIndex begin, TrafficGraphNodeIndex end) noexcept;
        virtual ~Agent() = default;

        TrafficGraphNodeIndex m_goal() const noexcept;

        void determinePath() noexcept;

        AgentState update(double dt);
        void makeGreedyChoice();

        PhysicalEntity& physical() noexcept;
        const PhysicalEntity& physical() const noexcept;

    protected:
        // ---- Member definitions ---- //
        
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


    class World {
    public:
        // ---- Contstructors ---- //
        World(nyrem::ConcurrencyManager *manager);
        World(nyrem::ConcurrencyManager *manager, const std::shared_ptr<OSMSegment> &map);

        virtual ~World() = default;

        // ---- Functions ---- //
        bool hasMap() const noexcept;
        
        void loadMap(const std::shared_ptr<OSMSegment>& map);
        void loadMap(const std::string &file);
        
        void update(double dt);

        void createAgent(TrafficGraphNodeIndex start, TrafficGraphNodeIndex end);

        const std::shared_ptr<OSMSegment>& getMap() const;
        const std::shared_ptr<OSMSegment>& getHighwayMap() const;
        const std::shared_ptr<Graph>& getGraph() const;
        const std::shared_ptr<OSMViewTransformer>& transformer() const;
        const std::shared_ptr<TrafficGraph>& getTrafficGraph() const;

        const std::vector<Agent>& getAgents() const;

    protected:
        // ---- Member definitions ---- //
        nyrem::ConcurrencyManager *m_manager;
        std::shared_ptr<OSMSegment> m_map;
        std::shared_ptr<OSMSegment> k_highway_map;
        std::shared_ptr<OSMViewTransformer> m_transformer;

        std::shared_ptr<Graph> m_graph;
        std::shared_ptr<TrafficGraph> m_traffic_graph;
        std::vector<Agent> m_agents;
    }; 
} // namespace traffic

#endif
