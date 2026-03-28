#include "NavGraphPathfinding.h"

#include "AStar.h"
#include "PathSmoothing.h"
#include "VectorTypes.h"
#include "Shared/Graph/NavGraph/NavGraph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"
#include"../../MACROS/MACRO.h"

using namespace GameAI;

std::vector<FVector2D> NavMeshPathfinding::FindPath(const FVector2D& startPos, const FVector2D& endPos,
	NavGraph * const pNavGraph, std::vector<FVector2D>& debugNodePositions, std::vector<NavLine> & debugPortals, UWorld * world)
{
	auto Graph = pNavGraph->Clone();
	std::vector<FVector2D> finalPath{};
	std::optional<int> StartNodeIndex = Graph->GetClosestNodeIndex(startPos,Graph.get());
	std::optional<int> EndNodeIndex = Graph->GetClosestNodeIndex(endPos,Graph.get());

	if (!StartNodeIndex.has_value() || !EndNodeIndex.has_value())
	{
		UE_LOG(LogTemp, Error, TEXT(" F closes to start or closest to End is null"));

		return finalPath;
	}


	FVector2D outPos1, outPos2;
	auto* startTri = Graph->GetNavPolygon()->GetClosestTriangleToPosition(startPos, outPos1);
	auto* endTri = Graph->GetNavPolygon()->GetClosestTriangleToPosition(endPos, outPos2);

	if (startTri == endTri && startTri != nullptr)
	{
		// Same triangle — no obstacles, go straight
		finalPath.push_back(startPos);
		finalPath.push_back(endPos);
		return finalPath;
	}




	AStar pathfinder = AStar(Graph.get(), GameAI::HeuristicFunctions::Euclidean);


	int StartIndex = StartNodeIndex.value();
	Node * StartPath = Graph->GetNode(StartIndex).get();


	int EndIndex = EndNodeIndex.value();

	Node* EndPath{ Graph->GetNode(EndIndex).get() };

	std::vector<GameAI::Node*> Path = pathfinder.FindPath(StartPath, EndPath);

	if (!Path.empty())
	{

		for (GameAI::Node * node : Path)
		{
			finalPath.push_back(node->GetPosition());
		}

		UE_LOG(LogTemp, Warning, TEXT(" Path is not Empty %d "), static_cast<int>(finalPath.size()));

		debugPortals = SSFA::FindPortals(Path, *Graph->GetNavPolygon(), Graph.get(),startPos);//is this the right graph 
		//finalPath = SSFA::OptimizePortals(debugPortals, *Graph->GetNavPolygon(),world);


	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT(" Path Not Found "));

	}
	return finalPath;
}

