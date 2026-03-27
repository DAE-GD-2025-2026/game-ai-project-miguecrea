#include "BFS.h"

#include <map>
#include <queue>
#include "Shared/Graph/Graph.h"

using namespace GameAI;

BFS::BFS(Graph* const pGraph)
	: pGraph(pGraph)
{
}

// TODO Breath First Search Algorithm searches for a path from the startNode to the destinationNode

//all edges have equal cost, BFS gives the optimal path
std::vector<Node*> BFS::FindPath(Node* const pStartNode, Node* const pDestinationNode) const
{
	std::vector<Node*> path;

	if (pStartNode == nullptr || pDestinationNode == nullptr) return path;
	if (pStartNode == pDestinationNode)
	{
		path.push_back(pStartNode);
		return path;
	}

	std::queue<Node*> openQueue;
	std::map<Node*, Node*> parentMap;  // child → parent

	openQueue.push(pStartNode);
	parentMap[pStartNode] = nullptr;  // start has no parent

	while (!openQueue.empty())
	{
		Node* currentNode = openQueue.front();
		openQueue.pop();

		if (currentNode == pDestinationNode)
		{
			// Backtrack from goal to start
			Node* current = currentNode;
			while (current != nullptr)
			{
				path.push_back(current);
				current = parentMap[current];
			}
			std::reverse(path.begin(), path.end());
			return path;
		}

		auto neighbors = pGraph->FindConnectionsFrom(currentNode->GetId());

		for (Connection * singleConnection : neighbors)
		{
			if (singleConnection == nullptr) continue;

			Node* neighborNode = pGraph->GetNode(singleConnection->GetToId()).get();

			if (neighborNode == nullptr) continue;
			if (parentMap.count(neighborNode) > 0) continue;  // already visited

			parentMap[neighborNode] = currentNode;
			openQueue.push(neighborNode);
		}
	}

	return path;
}
