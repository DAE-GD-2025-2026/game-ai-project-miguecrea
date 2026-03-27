#pragma once

#include <vector>
#include "Shared/Graph/Graph.h"
#include "Heuristics.h"
#include <limits>



namespace GameAI
{
	class AStar
	{
	public:
		AStar(Graph * const pGraph, HeuristicFunctions::Heuristic hFunction);

		// stores the optimal connection to a node and its total costs related to the start and end node of the path
		struct NodeRecord final
		{
			Node * pNode = nullptr;
			Connection * pConnection = nullptr;

			float G_Cost = std::numeric_limits<float>::max();
			float H_Cost = std::numeric_limits<float>::max();
			float F_Cost = std::numeric_limits<float>::max();
		
			bool operator==(const NodeRecord& other) const
			{
				return pNode == other.pNode
					&& G_Cost == other.G_Cost
					&& H_Cost == other.H_Cost
					&& F_Cost == other.F_Cost;
			};

			bool operator<(const NodeRecord& other) const
			{
				return F_Cost < other.F_Cost;
			};
		};

		std::vector<Node*> FindPath(Node* const pStartNode, Node* const pDestinationNode);

	private:


		bool ContainsNode(const std::vector<NodeRecord> & list, Node * pNode)
		{
			auto it = std::find_if(list.begin(), list.end(),
				[pNode](const NodeRecord & r) {
					return r.pNode == pNode;
				});

			return it != list.end();
		}
		std::vector<Node*> ReconstructPath(Node* EndNode, std::vector<NodeRecord> closedList);
		float GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const;

		Graph * pGraph;
		HeuristicFunctions::Heuristic HeuristicFunction;
	};
}
