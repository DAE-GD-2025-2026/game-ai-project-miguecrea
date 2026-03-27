#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"
#include"../../MACROS/MACRO.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph* const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine>& debugPortals)
{
	auto Graph = pNavGraph->Clone();
	std::vector<FVector2D> finalPath{};
	std::optional<int> ClosestNodeToStart = Graph->GetClosestNodeIndex(startPos);
	std::optional<int> ClosestNodeToEnd = Graph->GetClosestNodeIndex(endPos);

	if (!ClosestNodeToStart.has_value() || !ClosestNodeToEnd.has_value())
	{
		UE_LOG(LogTemp, Error, TEXT(" F closes to start or closest to End is null"));

		return finalPath;
	}

	int EndNodeIndex = Graph->AddNode(std::make_unique<Node>(endPos));

	Graph->AddConnection(EndNodeIndex, ClosestNodeToEnd.value());
	Graph->AddConnection(ClosestNodeToEnd.value(), EndNodeIndex);

	AStar pathfinder = AStar(Graph.get(), GameAI::HeuristicFunctions::Manhattan);
	Node * StartPath = Graph->GetNode(ClosestNodeToStart.value()).get();
	Node* EndPath{ Graph->GetNode(EndNodeIndex).get() };

	std::vector<GameAI::Node*> Path = pathfinder.FindPath(StartPath, EndPath);

	if (!Path.empty())
	{

		for (GameAI::Node* node : Path)
		{
			finalPath.push_back(node->GetPosition());
		}

		UE_LOG(LogTemp, Warning, TEXT(" Path is not Empty %d "), static_cast<int>(finalPath.size()));


		debugPortals = SSFA::FindPortals(Path, *Graph->GetNavPolygon(), Graph.get());//is this the right graph 
	//	finalPath = SSFA::OptimizePortals(debugPortals, *Graph->GetNavPolygon());


	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" Path Not Found "));

	}
	return finalPath;
}

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos, NavGraph* const pNavGraph)
{
	std::vector<FVector2D> debugNodePositions{};
	std::vector<NavLine> debugPortals{};

	return FindPath(startPos, endPos, pNavGraph, debugNodePositions, debugPortals);
}

