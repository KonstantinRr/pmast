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
#include <pmast/osm_mesh.hpp>
#include <pmast/osm_graph.hpp>

#include <engine/util.hpp>

#include <chrono>
#include <limits>
#include <queue>

using namespace traffic;
using namespace glm;
using namespace std;

// ---- BufferedNode ---- //

/// <summary>
/// class BufferedNode
/// BufferedNodes are used in path finding algorithms to store additional information
/// about GraphNodes. This includes a visited flag and a previous pointer that marks
/// the location from which the node was discovered. It also holds a distance specifying
/// the total distance from the source.
/// </summary>
template<typename ParentType>
class BufferedNode
{
public:
	BufferedNode() = default;
	BufferedNode(ParentType* node, BufferedNode<ParentType>* previous, prec_t distance, bool visited);

	// ---- Member definitions ---- //
	ParentType* node;
	BufferedNode<ParentType>* previous;
	prec_t distance;
	prec_t heuristic;
	bool visited;
};

template<typename ParentType>
BufferedNode<ParentType>::BufferedNode(
	ParentType*pNode, BufferedNode *pPrevious, prec_t pDistance, bool pVisited)
{
	this->node = pNode;
	this->previous = pPrevious;
	this->distance = pDistance;
	this->visited = pVisited;
}

using BufferedGraphNode = BufferedNode<GraphNode>;
using BufferedFastNode = BufferedNode<TrafficGraphNode>;

// ---- TrafficGraphEdge ---- //

TrafficGraphEdge::TrafficGraphEdge(size_t goal, prec_t weight)
	: goal(goal), weight(weight)
{

}

// ---- TrafficGraphNode ---- //

TrafficGraphNode::TrafficGraphNode(int64_t nodeID, prec_t x, prec_t y)
	: nodeID(nodeID), x(x), y(y)
{

}

// ---- GraphEdge ---- //

GraphEdge::GraphEdge(int64_t pGoalID, prec_t pWeight) :
	goal(pGoalID),
	weight(pWeight)
{

}

// ---- GraphNode ---- //

GraphNode::GraphNode(const OSMNode &node) :
	lat(node.getLat()),
	lon(node.getLon()),
	nodeID(node.getID())
{

}

// ---- Graph ---- //

Graph::Graph(const shared_ptr<OSMSegment>& xmlmap)
{
	// Iterates throught the whole list of ways, and nodes for each way. The algorithm checks
	// constantly if the node already exists in the map. It connects the nodes by creating
	// a new edge.

	for (const OSMWay& way : *xmlmap->getWays()
	)
	{
		int64_t lastID = -1;

		for (int64_t currentID : way.getNodes())
		{
			// Checks whether the ID was found before
			auto indexIt = graphMap.find(currentID);
			if (indexIt == graphMap.end())
			{
				size_t currentIndex = graphBuffer.size();
				// (1) Inserts the value in the buffer
				// (2) Creates a new map entry
				graphBuffer.push_back(GraphNode(xmlmap->getNode(currentID)));
				graphMap[currentID] = currentIndex;

				if (lastID != -1)
				{
					size_t lastIndex = graphMap[lastID];
					prec_t distance = (prec_t)simpleDistance(
						xmlmap->getNode(lastID).asVector(),
						xmlmap->getNode(currentID).asVector());
					graphBuffer[currentIndex].connections.push_back(GraphEdge(lastID, distance));
					graphBuffer[lastIndex].connections.push_back(GraphEdge(currentID, distance));
				}
			}
			else
			{
				// Connect the points together if there is
				// a valid last point that can be connected.
				if (lastID != -1)
				{
					size_t currentIndex = graphMap[currentID];
					size_t lastIndex = graphMap[lastID];
					prec_t distance = (prec_t)simpleDistance(
						xmlmap->getNode(lastID).asVector(),
						xmlmap->getNode(currentID).asVector());
					graphBuffer[currentIndex].connections.push_back(GraphEdge(lastID, distance));
					graphBuffer[lastIndex].connections.push_back(GraphEdge(currentID, distance));
				}
			}
			lastID = currentID;
		}
	}
}

void Graph::optimize()
{
	fastGraph = std::make_unique<TrafficGraph>(*this);
}

Route Graph::findRoute(int64_t start, int64_t goal)
{
	int64_t startIndex = findNodeIndex(start);
	int64_t stopIndex = findNodeIndex(goal);
	if (startIndex == -1 || stopIndex == -1)
		throw std::runtime_error("Could not find start/goal indices\n");

	if (fastGraph)
		return fastGraph->findRoute(startIndex, stopIndex);
	return Route();
}

GraphNode& Graph::findNodeByIndex(size_t index) { return graphBuffer[index]; }
GraphNode& Graph::findNodeByID(int64_t id) {
	int64_t index = findNodeIndex(id);
	if (index == -1) throw std::runtime_error("Could not find node ID");
	return graphBuffer[static_cast<size_t>(index)];
}
const GraphNode& Graph::findNodeByIndex(size_t index) const { return graphBuffer[index]; }
const GraphNode& Graph::findNodeByID(int64_t id) const {
	int64_t index = findNodeIndex(id);
	if (index == -1) throw std::runtime_error("Could not find node ID");
	return graphBuffer[static_cast<size_t>(index)];
}

int64_t Graph::findNodeIndex(int64_t id) const {
	auto it = graphMap.find(id);
	return graphMap.end() == it ? -1 : it->second;
}

GraphNode& traffic::Graph::findClosestNode(const Point &p)
{
	if (graphBuffer.empty())
		throw std::runtime_error("Could not find Node; Graph is empty");

	size_t bestIndex = 0;
	double bestDistance = std::numeric_limits<double>::max();
	for (size_t i = 0; i < graphBuffer.size(); i++) {
		double newDistance = traffic::distance(
			graphBuffer[i].getPosition(), p.toVec());
		if (newDistance < bestDistance) {
			bestIndex = i;
			bestDistance = newDistance;
		}
	}
	return graphBuffer[bestIndex];
}

size_t Graph::countNodes() const {
	return graphBuffer.size();
}

size_t Graph::countEdges() const {
	size_t sum = 0;
	for (const auto &node : graphBuffer)
		sum += node.connections.size();
	return sum;
}

void Graph::clear() {
	graphBuffer.clear();
	graphMap.clear();
}

// checking buffer consistency
bool Graph::checkConsistency(const OSMSegment& seg) const {
	nyrem::FastSStream stream;

	stream.add(fmt::format(
		"Checking graph consistency: {} Nodes, {} Edges\n",
		countNodes(), countEdges()));

	bool check = true; // buffer is consistent
	for (size_t i = 0; i < graphBuffer.size(); i++) {
		auto checkIndex = graphMap.find(graphBuffer[i].nodeID);
		// checks whether the node is registered in the graph map
		if (checkIndex == graphMap.end()) {
			stream.add(fmt::format(
				"Could not find nodeID in map. INDEX: {} ID: {}\n",
				i, graphBuffer[i].nodeID));
			check = false;
			continue;
		}
		// checks whether both indices match
		if (checkIndex->second != i) {
			stream.add(fmt::format(
				"Map index does not match buffer index. Buffer: {} Map: {}\n",
				i, checkIndex->second));
			check = false;
			continue;
		}
		// checks whether the node is in the original map
		if (seg.hasNodeIndex(checkIndex->second)) {
			stream.add(fmt::format(
				"OSMNode does not exist in OSMSegment: {}\n",
				checkIndex->second));
			check = false;
			continue;
		}

		// Check if all the connections exists in the graph map
		for (size_t k = 0; k < graphBuffer[i].connections.size(); k++) {
			auto connCheck = graphMap.find(graphBuffer[i].connections[k].goal);
			if (connCheck == graphMap.end()) {
				stream.add(fmt::format(
					"Connection NodeID is not part of NodeMap. ID: {}\n",
					graphBuffer[i].connections[k].goal));
				check = false;
				continue;
			}
		}
	}

	// checks the map conistency range
	for (auto& it: graphMap) {
		if (it.second >= graphBuffer.size()) {
			stream.add(fmt::format("GraphMap value out of range {}\n", it.second));
			check = false;
			continue;
		}
	}

	stream.add(fmt::format("Graph consistency check computed {}\n", check));
	spdlog::info(stream.generate());
	return true;
}

size_t traffic::Graph::getManagedSize() const
{
	return getSizeOfObjects(graphBuffer);
}


TrafficGraph::TrafficGraph(const Graph& graph)
{
	const std::vector<GraphNode> &buf = graph.getBuffer();
	// reserves the needed capacity
	graphBuffer.resize(buf.size());
	
	for (size_t i = 0; i < buf.size(); i++) {
		TrafficGraphNode node(buf[i].nodeID, buf[i].lat, buf[i].lon);
		for (size_t k = 0; k < buf[i].connections.size(); k++) {
			node.connections.resize(buf[i].connections.size());

			int64_t goalIndex = graph.findNodeIndex(buf[i].connections[k].goal);
			if (goalIndex == -1) {
				spdlog::warn("FastGraph Creation: Could not find GoalIndex!");
				continue;
			}

			node.connections[k] = TrafficGraphEdge(
				static_cast<size_t>(goalIndex),
				buf[i].connections[k].weight
			);
		}
		graphBuffer[i] = std::move(node);
	}
}

Route TrafficGraph::findRoute(size_t start, size_t goal)
{
	prec_t maxDistanceScale = 3.0f;
	auto begin = std::chrono::steady_clock::now();

	if (start == goal)
		return Route();

	// Initializes the buffered data using an empty list
	size_t nodeCount = graphBuffer.size();
	vector<BufferedFastNode> nodes(nodeCount);
	for (size_t i = 0; i < nodeCount; i++) {
		nodes[i].distance = std::numeric_limits<double>::max();
		nodes[i].visited = false;
		nodes[i].previous = nullptr;
		nodes[i].node = &(graphBuffer[i]);
		nodes[i].heuristic = simpleDistance(
			glm::dvec2(graphBuffer[i].x, graphBuffer[i].y),
			glm::dvec2(graphBuffer[goal].x, graphBuffer[goal].y));
	}

	// Defines a min priority queue
	auto cmp = [](const BufferedFastNode* left, const BufferedFastNode* right)
	{ return left->distance + left->heuristic > right->distance + right->heuristic; };
	priority_queue<BufferedFastNode*, vector<BufferedFastNode*>,
		decltype(cmp)> queue(cmp);

	prec_t maxDistance = nodes[start].heuristic * maxDistanceScale;
	// Adds the starting node to the queue.
	nodes[start].distance = 0;
	queue.push(&(nodes[start]));

	while (true) {
		// All possible connections where searched and the goal was not found.
		// This means that there is not a possible way to reach the destination node.
		if (queue.empty())
			return Route();

		// Takes the element with the highest priority from the queue
		BufferedFastNode* currentNode = queue.top();
		if (currentNode->distance > maxDistance)
			return Route();
		queue.pop();

		// Checks the goal condition. Starts the backpropagation
		// algorithm if the goal was found to output the shortest route.
		if (currentNode->node->nodeID == graphBuffer[goal].nodeID) {
			Route route;
			do {
				route.addNode(currentNode->node->nodeID);
				currentNode = currentNode->previous;
			} while (currentNode->node->nodeID != graphBuffer[start].nodeID);

			auto end = std::chrono::steady_clock::now();
			spdlog::info("Found path in {}[us]", std::chrono::duration_cast<
				std::chrono::microseconds>(end - begin).count());
			return route;
		}

		auto& connections = currentNode->node->connections;
		for (size_t i = 0; i < connections.size(); i++) {
			// Checks if the node was already visited
			BufferedFastNode* nextNode = &(nodes[connections[i].goal]);
			if (!nextNode->visited) {
				// Calculates the total distance to this node
				prec_t newDistance = currentNode->distance + connections[i].weight;
				if (newDistance < nextNode->distance) {
					nextNode->distance = newDistance;
					nextNode->previous = currentNode;
				}
				// Adds the node to the list of nodes that need to be visited.
				// The node will be visited in one of the next iterations
				queue.push(nextNode);
			}
		}
		// marks the node finally as being visited
		currentNode->visited = true;
	}
}
