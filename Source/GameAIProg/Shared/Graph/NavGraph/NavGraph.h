#pragma once
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include <unordered_map>
#include<vector>
#include <utility> // Required for std::pair
namespace GameAI
{
	class NavGraph : public Graph
	{
	public:
		explicit NavGraph(std::unique_ptr<TriPolygon> && NavPoly);
		NavGraph(const NavGraph & Other);
		
		std::unique_ptr<NavGraph> Clone() const;
		TriPolygon const * GetNavPolygon() const {return pNavPoly.get();}
		int GetNodeIdFromEdgeIndex(int EdgeIdx) const;
		std::optional<int> GetClosestNodeIndex(FVector2D const& DesiredPosition);

		std::unordered_map<uint64,TriPolygon::Triangle> m_Dictionary{};
		std::unordered_map<uint64, Node*> edgeToNode;

		std::unordered_map<int, std::pair<FVector2D,FVector2D>> nodeToEdgeEndpoints;

	private:
		std::unique_ptr<TriPolygon> pNavPoly;
		void CreateNavigationGraph();

	
		// Compiler cantr hash to a std::pair to we turn to ints into one 
		uint64 MakeEdgeKey(int a, int b)
		{
			int lo = FMath::Min(a, b);
			int hi = FMath::Max(a, b);
			return (static_cast<uint64>(lo) << 32) | static_cast<uint64>(hi);
		}
	};
}
