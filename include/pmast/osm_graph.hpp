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

namespace traffic {

// Forward Declarations //
class OSMViewTransformer;
// ==== //

class Agent;
class World;
class Scheduler;

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
class RouteGeneric {
public:
	using ThisType = RouteGeneric<Type>;
	using RouteGenericType = ThisType;

public:
	// ---- Member definitions ---- //
	std::vector<Type> nodes;

public:
	/// <summary>
	/// Creates a new empty route without any nodes.
	/// </summary>
	RouteGeneric() = default;

	/// <summary>
	/// Creates a route containing the given nodes.
	/// The given input vector is copied to this object.
	/// </summary>
	RouteGeneric(std::vector<Type> const& nodes) : nodes(nodes) { }
	
	/// <summary>
	/// Creates a route containing the given nodes.
	/// The given input vector is moved to this object.
	/// </summary>
	RouteGeneric(std::vector<Type>&& nodes) : nodes(std::move(nodes)) { }

	/// <summary>
	/// Returns the amount of nodes contained in this route.
	/// </summary>
	size_t size() const noexcept { return nodes.size(); }

	/// <summary>
	/// Checks whether the route exists. A route exists if it has
	/// at least one node.
	/// </summary>
	bool exists() const noexcept { return !nodes.empty(); }

	/// <summary>
	/// Adds an additional node at the end of the route.
	/// </summary>
	void addBack(Type nodeID) { nodes.push_back(nodeID); }

	/// <summary>
	/// Adds an additional node at the beginning of the route.
	/// </summary>
	void addFront(Type nodeID) { nodes.insert(nodes.begin(), nodeID); }


	/// <summary>
	/// Returns the iterator at the beginning of the route.
	/// </summary>
	auto begin() const { return nodes.begin(); }

	/// <summary>
	/// Returns the iterator at the end of the route.
	/// </summary>
	auto end() const { return nodes.end(); }

	/// <summary>
	/// Reverses the current route.
	/// </summary>
	void reverse() { std::reverse(nodes.begin(), nodes.end()); }

	/// <summary>
	/// Returns the node at the given index.
	/// </summary>
	const Type& operator[](size_t idx) const { return nodes[idx]; }
};

/// <summary>
/// A route consisting of IDs.
/// </summary>
class Route : public RouteGeneric<int64_t> {
public:
	using ThisType = Route;
	using RouteType = ThisType;
	using RouteGenericType = RouteGeneric<int64_t>;
	
public:
	Route() = default;
	Route(std::vector<int64_t> const& nodes);
	Route(std::vector<int64_t>&& nodes);
};

/// <summary>
/// A route consisting of indices.
/// </summary>
class IndexRoute : public RouteGeneric<TrafficGraphNodeIndex> {
public:
	using ThisType = IndexRoute;
	using IndexRouteType = ThisType;
	using RouteGenericType = RouteGeneric<TrafficGraphNodeIndex>;

public:
	IndexRoute() = default;
	IndexRoute(std::vector<TrafficGraphNodeIndex> const& nodes);
	IndexRoute(std::vector<TrafficGraphNodeIndex>&& nodes);
};


/// <summary>
/// A traffic graph edge represents a part of a street in the context of
/// city traffic simulation. Each edge connects two nodes with each other in
/// a straight line. It is therefore the optimal path between those points.
/// A whole street with turns and corners is therefore implemented using many
/// nodes and edges connecting them together.
/// Each edge contains 
/// </summary>
class TrafficGraphEdge {
public:
	using ThisType = TrafficGraphEdge;
	using TrafficGraphEdgeType = ThisType;

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
	/// Stores tRouteGeneric<TrafficGraphNodeIndex>he maximum speed that is allowd on this route. This speed
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

/// <summary>
/// Represents a node in the traffic network. Nodes contain a list of
/// outgoing and incoming connections to other nodes, have a fixed
/// position in plane space and are connected to a scheduler that
// is controlling the gates of the nodes.
/// </summary>
class TrafficGraphNode {
public:
	using ThisType = TrafficGraphNode;
	using TrafficGraphNodeType = ThisType;

	/// <summary>
	/// Stores connections to other nodes that are reachable
	/// from this node by using a single edge (street).
	/// </summary>
	std::vector<TrafficGraphEdge> connections;

	/// <summary>
	/// Stores the incoming connections to this node as a list.
	/// of pointers. The actual edges are always stored as outgoing edges.
	/// </summary>
	std::vector<TrafficGraphEdge*> incoming;

	/// <summary>
	/// Stores the linked OSM Graph node. This node is used to gain
	/// information about the OSM attributes of this node. May also be null
	/// if the graph node is unlinked. 
	/// </summary>
	GraphNode *linked;

	/// <summary>
	/// Stores whether the given gates at an intersection are open. The
	/// vector is actually a two dimensional array of size incoming.size() *
	/// connections.size() that is projected on a one dimensional array.
	/// The value at x * incoming.size() + y with x as index of the incoming
	/// array, and y as index of the outgoing index stores whether the connection
	/// of x to y is open. Agents traversing the network are required to wait
	/// before this node until the node / traffic light becomes unblocked.
	/// NOTE: Don't use std::vector<bool> as it does not fullfill the STL container
	/// requirements because of its special dense memory layout.
	/// </summary>
	std::vector<uint8_t> m_gates;


	std::shared_ptr<Scheduler> m_scheduler;
	
	/// <summary>
	/// Stores the coordinates of this graph node in plane space.
	/// </summary>
	nyrem::vec2 m_plane;

public:
	/// <summary>
	/// Sets the plane coordinates of this node.
	/// </summary>
	void setPlane(nyrem::vec2 plane) noexcept;

	/// <summary>
	/// Sets the scheduler for this node.
	/// </summary>
	void setScheduler(const std::shared_ptr<Scheduler> &sched) noexcept;

	/// <summary>
	/// Returns the plane coordinates of this node.
	/// </summary>
	nyrem::vec2 plane() const noexcept;

	/// <summary>
	/// Returns the scheduler responsible for this node.
	/// </summary>
	std::shared_ptr<Scheduler> scheduler() const noexcept;

	// Link back functions //
	
	prec_t lat() const noexcept;
	prec_t lon() const noexcept;
	int64_t nodeID() const noexcept;

	void resizeGates() noexcept;

	void setAllGates(bool value) noexcept;
	void openAllGates() noexcept;
	void closeAllGates() noexcept;

public:
	TrafficGraphNode() = default;
	TrafficGraphNode(GraphNode *linked, nyrem::vec2 plane);

	/// <summary>
	/// Unlinks the TrafficGraphNode from its GraphNode.
	/// </summary>
	void unlink() noexcept;

	/// <summary>
	/// Links the TrafficGraphNode with the given GraphNode.
	/// </summary>
	void link(GraphNode *nd) noexcept;

protected:
	void linkBack() noexcept;
};

class TrafficGraph {
public:
	using ThisType = TrafficGraph;
	using TrafficGraphType = ThisType;

public:
	/// <summary>
	/// Creates a new TrafficGraph from an OSMGraph.
	/// </summary>
	/// <param name="graph">The graph from which it is generated</param>
	/// <param name="trans">The view transformer used to translate coordinates</param>
	explicit TrafficGraph(Graph& graph,
		const OSMViewTransformer &trans);

	/// <summary>
	/// Applies the AStar (A*) path finding algorithm on the graph and returns
	/// Returns a route list containing all node IDs that create the shortest path.
	/// See findIndexRoute for more information.
	/// </summary>
	/// <param name="start">The starting node ID</param>
	/// <param name="goal">The destination node ID</param>
	/// <returns>The shortest route between start and goal</returns>	
	Route findRoute(TrafficGraphNodeIndex start, TrafficGraphNodeIndex goal);

	/// <summary>
	/// Applies the AStar (A*) path finding algorithm on the graph and returns
	/// Returns a route list containing all the indices that create the shortest path.
	/// See findRoute for more information.
	/// </summary>
	/// <param name="start">The starting node ID</param>
	/// <param name="goal">The destination node ID</param>
	/// <returns>The shortest route between start and goal</returns>		
	IndexRoute findIndexRoute(TrafficGraphNodeIndex start, TrafficGraphNodeIndex goal);
	
	/// <summary>
	/// Converts a route consisting of indices to IDs.
	/// <param name="idxRoute">The index route</param>
	/// </summary>
	Route toIDRoute(const IndexRoute &idxRoute) const noexcept;

	
	TrafficGraphNode& findNodeByIndex(TrafficGraphNodeIndex nodeIdx) noexcept;
	TrafficGraphEdge& findEdgeByIndex(TrafficGraphNodeIndex nodeIdx, TrafficGraphEdgeIndex edgeIdx) noexcept;

	size_t nodeCount() const noexcept;
	std::vector<TrafficGraphNode>& nodes() noexcept;
	const std::vector< TrafficGraphNode>& nodes() const noexcept;

	template<typename Func>
	TrafficGraphNodeIndex closestIdx(Func &&functor) const noexcept
	{
		size_t bestIndex = std::numeric_limits<size_t>::max();
		double bestDistance = std::numeric_limits<double>::max();
		for (size_t i = 0; i < graphBuffer.size(); i++) {
			// checks if the node has a link back to a GraphNode
			double newDistance = functor(graphBuffer[i]);
			if (newDistance < bestDistance) {
				bestIndex = i;
				bestDistance = newDistance;
			}
		}
		return static_cast<TrafficGraphNodeIndex>(bestIndex);
	}

	TrafficGraphNodeIndex findClosestNodeIdx(const Point &p) const noexcept;
	TrafficGraphNodeIndex findClosestNodeIdxPlane(nyrem::vec2 vec) const noexcept;


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
class GraphEdge { 
public:
	using ThisType = GraphEdge;
	using GraphEdgeType = ThisType;

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
struct GraphNode {
public:
	using ThisType = GraphNode;
	using GraphNodeType = ThisType;

public:
	// ---- Member definitions ---- //
	prec_t lat, lon;
	int64_t nodeID;
	std::vector<GraphEdge> connections;

	TrafficGraphNode *m_linked;

public:
	/// <summary>
	/// Creates a GraphNode from a regular OSM node
	/// </summary>
	/// <param name="node">A regular OSM node</param>
	/// <returns></returns>
	GraphNode(const OSMNode &node);

	/// <summary>
	/// Checks whether this object has a managed size
	/// </summary>
	inline bool hasManagedSize() const noexcept { return true; }

	/// <summary>
	/// Returns the managed size by this object.
	/// </summary>
	inline size_t getManagedSize() const noexcept { return getSizeOfObjects(connections); }
	
	/// <summary>
	/// Returns the size of this object with its additional managed size.
	/// </summary>
	inline size_t getSize() const noexcept { return sizeof(*this) + getManagedSize();}

	/// <summary>
	/// Returns the position of this node as a vector.
	/// </summary>
	inline glm::vec2 getPosition() const noexcept { return glm::vec2(lat, lon); }
	
	/// <summary>
	/// Returns the latitude of 
	inline prec_t getLatitude() const noexcept { return lat; }
	inline prec_t getLongitude() const noexcept { return lon; }

	inline void link(TrafficGraphNode &node) { m_linked = &node; }
};

/// <summary>
/// This class defines a directed graph that is essential a network of GraphNodes
/// which is connected by GraphEdges. A graph may be created from an OSM map by
/// converting the node, way, relation structure to a directed graph. The map
/// owns an additional 
/// </summary>
class Graph {
public:
	using ThisType = Graph;
	using GraphType = ThisType;

protected:
	/// <summary>
	/// List of all nodes stored in this graph.
	/// </summary>
	std::vector<GraphNode> graphBuffer;

	/// <summary>
	/// Map that translates IDs to indices
	/// </summary>
	graphmap_t graphMap;

public:
	/// <summary>
	/// Creates a Graph from an OSM map object. The node, way, relation
	/// structure is converted to a directional graph. Each node that is enclosed
	/// by at least one way is appended as GraphNode to this graph.
	/// </summary>
	/// <param name="xmlmap">The OSM map that is converted</param>
	Graph(const std::shared_ptr<OSMSegment> &xmlmap);

	/// <summary>
	/// Finds a node by its index in the sequential node array.
	/// </summary>
	/// <param name="index">The node's index</param>
	/// <returns>The node at the given index</returns>
	GraphNode& findNodeByIndex(size_t index);
	const GraphNode& findNodeByIndex(size_t index) const;

	/// <summary>
	/// Finds a node by its ID. The ID is converted to an actual
	/// array position which is used to return the node.
	/// </summary>
	/// <param name="id">The node's ID</param>
	/// <returns>The node that is stored by the given ID</returns>
	GraphNode& findNodeByID(int64_t id);
	const GraphNode& findNodeByID(int64_t id) const;

	/// <summary>
	/// Converts a GraphNode ID to the index in the node array.
	/// </summary>
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

	/// <summary>
	/// Checks whether this graph is consistend
	/// </summary
	/// <returns>True iff this grpah is consistent</returns>
	bool checkConsistency(const OSMSegment& seg) const;

	/// <summary>
	/// Checks whether this object has managed size.
	/// </summary>
	inline bool hasManagedSize() const { return true; }

	/// <summary>
	/// Returns the managed size of this object.
	/// </summary>
	/// <returns>The managed size of this object</returns>
	size_t getManagedSize() const;

	/// <summary>
	/// Returns the size of this object with its managed size.
	/// </summary>
	/// <returns>The size of this object with its managed size</summary>
	inline size_t getSize() const { return sizeof(*this) + getManagedSize(); }
};

} // traffic

#endif
