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

#ifndef GRAHP_HPP
#define GRAHP_HPP

#include <pmast/internal.hpp>
#include <pmast/osm.hpp>

#include <engine/internal.hpp>

#include <vector>
#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>


using graphmap_t = robin_hood::unordered_node_map<int64_t, size_t>;

namespace traffic
{
	class Agent;
	class World;

	using AgentRef = Agent*;
	using WorldRef = World*;

	struct Route; 			// Defines a route between two graph nodes

    // ==== OSM Graph Representation ==== //
	struct GraphEdge; 		// A single edge that connects two GraphNode(s)
	struct GraphNode; 		// A node that is part of a larger graph
	class Graph; 			// Combines a list of GraphNodes in a network by GraphEdges
	
	// ==== Traffic Graph Representation ==== //
	class TrafficGraphEdge;	// An edge that connects two TrafficGraphNode(s) together
	class TrafficGraphNode;	// A node in the TrafficGraph network
	class TrafficGraph;		// A collection of traffic graph nodes

	using TrafficGraphEdgeIndex = size_t;
	using TrafficGraphNodeIndex = size_t;
	using TrafficGraphEdgeRef = TrafficGraphEdge*;
	using TrafficGraphNodeRef = TrafficGraphNode*;

	extern size_t nullIndex;

	/// <summary>
	/// A route defines a way to navigate inside a Graph. It stores a sequential
	/// array of node IDs defining a way to navigate. Routes are produced by path
	/// finding algorithms.
	/// </summary>
	template<typename Type>
	class RouteGeneric
	{
	public:
		// ---- Member definitions ---- //
		std::vector<Type> nodes;

		RouteGeneric() = default;
		RouteGeneric(std::vector<Type> const& nodes) : nodes(nodes) { }
		RouteGeneric(std::vector<Type>&& nodes) : nodes(std::move(nodes)) { }

		size_t size() const { return nodes.size(); }
		bool exists() const { return !nodes.empty(); }
		void addBack(Type nodeID) { nodes.push_back(nodeID); }
		void addFront(Type nodeID) { nodes.insert(nodes.begin(), nodeID); }

		auto begin() const { return nodes.begin(); }
		auto end() const { return nodes.end(); }

		void reverse() { std::reverse(nodes.begin(), nodes.end()); }

		const Type& operator[](size_t idx) const { return nodes[idx]; }
	};

	class Route : public RouteGeneric<int64_t> {
	public:
		Route() = default;
		Route(std::vector<int64_t> const& nodes);
		Route(std::vector<int64_t>&& nodes);
	};

	class IndexRoute : public RouteGeneric<TrafficGraphNodeIndex> {
	public:
		IndexRoute() = default;
		IndexRoute(std::vector<TrafficGraphNodeIndex> const& nodes);
		IndexRoute(std::vector<TrafficGraphNodeIndex>&& nodes);
	};


	/// <summary> class TrafficGraphEdge
	/// A traffic graph edge represents a part of a street in the context of
	/// city traffic simulation. Each edge connects two nodes with each other in
	/// a straight line. It is therefore the optimal path between those points.
	/// A whole street with turns and corners is therefore implemented using many
	/// nodes and edges connecting them together.
	/// Each edge contains 
	/// </summary>
	class TrafficGraphEdge {
	public:
		/// <summary>
		/// Stores a list of all agents that are currently on this route.
		/// We currently only support single lane traffic. So no overtaking yet.
		/// </summary>
		std::vector<AgentRef> agents;

		/// <summary>
		/// Stores the destination index of this connection. This is the
		/// index in the TrafficGraph buffer object that will be reached
		/// when the end of this edge is reached.
		/// </summary>
		size_t goal;

		/// <summary>
		/// Stores the weight/cost of this node. This value does not have
		/// any meaning on its own but is just used as measurement on how
		/// fast this route may be travelled.
		/// </summary>
		prec_t weight;

		/// <summary>
		/// Stores the distance between the start of this edge and the end.
		/// </summary>
		prec_t distance;

		/// <summary>
		/// Stores the maximum speed that is allowd on this route. This speed
		/// is usually defined by country laws.
		/// </summary>
		prec_t maxAllowedSpeed;

		/// <summary>
		/// Stores the maximum speed that is physcially possible on this route.
		/// This speed can not be topped without causing an accident. The difference
		/// between maxSpeed and maxAllowedSpeed is therefore the 'safe speeding' limit.
		/// It is physically safe to drive 50 in most 30er zones, even though it is not
		/// allowed. This does not take into consideration external events like children
		/// running on the street.
		/// </summary>
		prec_t maxSpeed;

		/// <summary>
		/// Stores the amount of lanes that this street. Later usage.
		/// </summary>
		uint8_t lanes;

	public:
		TrafficGraphEdge() = default;
		TrafficGraphEdge(size_t goal, prec_t weight, prec_t distance);
	};

	class TrafficGraphNode {
	public:
		/// <summary>
		/// Stores connections to other nodes using all viable streets from this node.
		/// </summary>
		std::vector<TrafficGraphEdge> connections;

		/// <summary>
		/// Stores the OSM ID for this graph node
		/// </summary>
		GraphNode *linked;
		
		/// <summary>
		/// Stores the coordinates of this graph node. 
		/// </summary>
		prec_t x, y;

		// Link back functions //
		prec_t lat() const noexcept;
		prec_t lon() const noexcept;
		int64_t nodeID() const noexcept;


	public:
		TrafficGraphNode() = default;
		TrafficGraphNode(GraphNode *linked, prec_t x, prec_t y);

		void linkBack() noexcept;
	};

	class TrafficGraph {
	public:
		explicit TrafficGraph(Graph& graph);

		/// <summary>Applies the AStar (A*) path finding algorithm on the graph</summary>
		/// <param name="start">The starting node ID</param>
		/// <param name="goal">The destination node ID</param>
		/// <returns>The shortest route between start and goal</returns>
		Route findRoute(TrafficGraphNodeIndex start, TrafficGraphNodeIndex goal);
		IndexRoute findIndexRoute(TrafficGraphNodeIndex start, TrafficGraphNodeIndex goal);
		
		
		Route toIDRoute(const IndexRoute &idxRoute) const noexcept;

		TrafficGraphNode& findNodeByIndex(TrafficGraphNodeIndex nodeIdx) noexcept;
		TrafficGraphEdge& findEdgeByIndex(TrafficGraphNodeIndex nodeIdx, TrafficGraphEdgeIndex edgeIdx) noexcept;

		size_t nodeCount() const noexcept;
		std::vector<TrafficGraphNode>& nodes() noexcept;
		const std::vector< TrafficGraphNode>& nodes() const noexcept;

		TrafficGraphNodeIndex findClosestNodeIdx(const Point &p) const noexcept;
		const TrafficGraphNode& findClosestNode(const Point &p) const;
		TrafficGraphNode& findClosestNode(const Point &p);

		inline TrafficGraphNode& buffer(size_t size) noexcept {return graphBuffer[size]; }
		inline const TrafficGraphNode& buffer(size_t size) const noexcept { return graphBuffer[size]; }
	
	protected:
		std::vector<TrafficGraphNode> graphBuffer;
	};

	// ==== Default OSM Graph ==== //

	/// <summary>
	/// Graph edges are used to connect GraphNodes with each other. The implemented
	/// graph is directed which means that connections are uni-directional. Each
	/// edge contains a weight value specifying the cost of taking this path. The
	/// cost may be calculated by different method but may never be negative. This
	/// may lead to unexpected consequences.
	/// </summary>
	class GraphEdge
	{ 
	public:
		/// <summary>Creates an edge of a graph</summary>
		/// <param name="goalID">The destination where this edge leads</param>
		/// <param name="weight">The weight that is associated with this edge</param>
		/// <returns></returns>
		GraphEdge(int64_t goalID, prec_t weight, prec_t distance);

		// ---- Size access members ---- //
		inline bool hasManagedSize() const { return false; }
		inline size_t getManagedSize() const { return 0; }
		inline size_t getSize() const { return sizeof(*this); }
	public:
		// ---- Member definitions ---- //

		// stores the goal ID
		int64_t goal;
		// stores the weight
		prec_t weight;

		prec_t distance;
	};

	// ==== OSM Classes

	/// <summary>
	/// Graph nodes are used in a graph representation to perform route optimization.
	/// Each node has a certain set of connections that lead to other nodes creating
	/// a connected graph.
	/// </summary>
	struct GraphNode
	{
	public:
		// ---- Member definitions ---- //
		prec_t lat, lon;
		nyrem::dvec2 plane;
		int64_t nodeID;
		std::vector<GraphEdge> connections;

		TrafficGraphNode *m_linked;
	public:
		/// <summary> Creates a GraphNode from a regular OSM node</summary>
		/// <param name="node">A regular OSM node</param>
		/// <returns></returns>
		GraphNode(const OSMNode &node);

		inline bool hasManagedSize() const { return true; }
		inline size_t getManagedSize() const { return getSizeOfObjects(connections); }
		inline size_t getSize() const { return sizeof(*this) + getManagedSize();}

		inline glm::vec2 getPosition() const { return glm::vec2(lat, lon); }
		inline glm::dvec2 getPlanePosition() const { return plane; }
		inline prec_t getLatitude() const { return lat; }
		inline prec_t getLongitude() const { return lon; }

		inline void link(TrafficGraphNode &node) { m_linked = &node; }
	};

	/// <summary>
	/// This class defines a directed graph that is essential a network of GraphNodes
	/// which is connected by GraphEdges. A graph may be created from an OSM map by
	/// converting the node, way, relation structure to a directed graph. The map
	/// owns an additional 
	/// </summary>
	class Graph
	{
	public:
		/// <summary>Creates a Graph from an OSM map object. The node, way, relation
		/// structure is converted to a directional graph. Each node that is enclosed
		/// by at least one way is appended as GraphNode to this graph</summary>
		/// <param name="xmlmap">The OSM map that is converted</param>
		/// <returns></returns>
		Graph(const std::shared_ptr<OSMSegment> &xmlmap);

		/// <summary>Finds a node by its index in the sequential node array</summary>
		/// <param name="index">The node's index</param>
		/// <returns>The node at the given index</returns>
		GraphNode& findNodeByIndex(size_t index);
		const GraphNode& findNodeByIndex(size_t index) const;

		/// <summary>Finds a node by its ID. The ID is converted to an actual
		/// array position which is used to return the node.</summary>
		/// <param name="id">The node's ID</param>
		/// <returns>The node that is stored by the given ID</returns>
		GraphNode& findNodeByID(int64_t id);
		const GraphNode& findNodeByID(int64_t id) const;

		/// <summary>Converts a GraphNode ID to the index in the node array</summary>
		/// <param name="id">The GraphNode ID that is searched</param>
		/// <returns>The corresponding index or -1 if that node does not exist</returns>
		int64_t findNodeIndex(int64_t id) const;

		GraphNode& findClosestNode(const Point &p);

		// ---- Getter functions ---- //

		graphmap_t& getMap() { return graphMap; }
		const graphmap_t& getMap() const { return graphMap; }

		std::vector<GraphNode>& getBuffer() { return graphBuffer; }
		const std::vector<GraphNode>& getBuffer() const { return graphBuffer; }

		size_t countNodes() const;
		size_t countEdges() const;
		void clear();

		bool checkConsistency(const OSMSegment& seg) const;

		inline bool hasManagedSize() const { return true; }
		size_t getManagedSize() const;
		inline size_t getSize() const { return sizeof(*this) + getManagedSize(); }

	protected:
		std::vector<GraphNode> graphBuffer;
		graphmap_t graphMap;
	};
}

#endif
