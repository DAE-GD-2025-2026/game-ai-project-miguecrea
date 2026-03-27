#include "AStar.h"

using namespace GameAI;

AStar::AStar(Graph* const pGraph, HeuristicFunctions::Heuristic hFunction)
	: pGraph(pGraph)
	, HeuristicFunction(hFunction)
{

}

std::vector<Node*>AStar::FindPath(Node * const pStartNode, Node * const pGoalNode)
{
	std::vector<Node*> path{};

	if (!pStartNode || !pGoalNode)
	{

		UE_LOG(LogTemp, Error, TEXT("Start Node or End Node is null "));

		return path;
	}



	std::vector<NodeRecord> OpenList{};
	std::vector<NodeRecord> ClosedList{};

	NodeRecord StartNodeRecord;
	StartNodeRecord.pNode = pStartNode;
	StartNodeRecord.G_Cost = 0.0f;
	StartNodeRecord.H_Cost = GetHeuristicCost(pStartNode, pGoalNode);
	StartNodeRecord.F_Cost = StartNodeRecord.H_Cost;
	StartNodeRecord.pConnection = nullptr;
	OpenList.push_back(StartNodeRecord);

	while (!OpenList.empty())
	{
		auto currentIt = std::min_element(OpenList.begin(), OpenList.end());
		NodeRecord currentRecord = *currentIt;

		if (currentRecord.pNode == nullptr)
		{
			UE_LOG(LogTemp, Error, TEXT("CurrentNode Pointer is null"));
			break;
		}

		if (currentRecord.pNode == pGoalNode)
		{
			ClosedList.push_back(currentRecord);
			path = ReconstructPath(currentRecord.pNode, ClosedList);
			return path;
		}

		OpenList.erase(currentIt);
		ClosedList.push_back(currentRecord);

		auto Neighbors = pGraph->FindConnectionsFrom(currentRecord.pNode->GetId());

		for (Connection * singleConnection : Neighbors)
		{
			if (singleConnection == nullptr) continue;

			Node * neighborNode = pGraph->GetNode(singleConnection->GetToId()).get();

			if (neighborNode == nullptr) continue;
			if (ContainsNode(ClosedList, neighborNode)) continue;

			float tentativeG = currentRecord.G_Cost + singleConnection->GetWeight();

			auto openIt = std::find_if(OpenList.begin(), OpenList.end(),
				[neighborNode](const NodeRecord& r) {
					return r.pNode == neighborNode;
				});

			if (openIt != OpenList.end())
			{
				if (tentativeG < openIt->G_Cost)
				{
					openIt->pConnection = singleConnection;
					openIt->G_Cost = tentativeG;
					openIt->F_Cost = tentativeG + openIt->H_Cost;
				}
			}
			else
			{
				NodeRecord neighborRecord;
				neighborRecord.pNode = neighborNode;
				neighborRecord.pConnection = singleConnection;
				neighborRecord.G_Cost = tentativeG;
				neighborRecord.H_Cost = GetHeuristicCost(neighborNode, pGoalNode);
				neighborRecord.F_Cost = tentativeG + neighborRecord.H_Cost;
				OpenList.push_back(neighborRecord);
			}
		}
	}

	return path;
}

std::vector<Node*> GameAI::AStar::ReconstructPath(Node* EndNode, std::vector<NodeRecord> closedList)
{

	
	std::vector<Node*> path;

	auto it = std::find_if(closedList.begin(), closedList.end(),
		[EndNode](const NodeRecord& r) {
			return r.pNode == EndNode;
		});

	if (it == closedList.end())
	{
		UE_LOG(LogTemp, Warning, TEXT("End node not found in closed list"));
		return path;
	}

	NodeRecord current = *it;

	while (current.pNode != nullptr)
	{
		path.push_back(current.pNode);

		if (current.pConnection == nullptr) break;

		Node * parentNode = pGraph->GetNode(current.pConnection->GetFromId()).get();

		if (parentNode == nullptr) break;

		auto parentIt = std::find_if(closedList.begin(), closedList.end(),
			[parentNode](const NodeRecord& r) {
				return r.pNode == parentNode;
			});

		if (parentIt == closedList.end()) break;

		current = *parentIt;
	}

	std::reverse(path.begin(), path.end());
	return path;

}

float AStar::GetHeuristicCost(Node* const pStartNode, Node* const pEndNode) const
{
	FVector2D toDestination = pGraph->GetNode(pEndNode->GetId())->GetPosition() - pGraph->GetNode(pStartNode->GetId())->GetPosition();
	return HeuristicFunction(abs(toDestination.X), abs(toDestination.Y));
}
//