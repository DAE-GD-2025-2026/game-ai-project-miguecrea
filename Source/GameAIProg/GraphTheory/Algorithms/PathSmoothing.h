#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"
#include "Shared/Graph/NavGraph/NavGraph.h"

namespace GameAI
{

    static float TriArea2(const FVector2D& a, const FVector2D& b, const FVector2D& c)
    {
       
        const float ax = b.X - a.X;
        const float ay = b.Y - a.Y;
        const float bx = c.X - a.X;
        const float by = c.Y - a.Y;
        return ax * by - bx * ay;  // flipped from bx*ay - ax*by
    }

	class SSFA final
	{
	public:
		static std::vector<NavLine> FindPortals(std::vector<Node*> const& Path, TriPolygon const& NavPoly, NavGraph* const pNavGraph,const FVector2D & startPos)
		{
            std::vector<NavLine> Portals;

            // Degenerate portal at start (first node is startPos, not on any edge)
            Portals.push_back(NavLine{ Path.front()->GetPosition(), Path.front()->GetPosition() });

            // Real portals for middle nodes (they sit on edges)
            for (size_t i = 1; i < Path.size() - 1; ++i)
            {
                auto it = pNavGraph->nodeToEdgeEndpoints.find(Path[i]->GetId());
                if (it != pNavGraph->nodeToEdgeEndpoints.end())
                {
                    auto [Vertex1, Vertex2] = it->second;

                    FVector2D prevPos = Path[i - 1]->GetPosition();
                    FVector2D nextPos = Path[i + 1]->GetPosition();

                    FVector2D pathDir = nextPos - prevPos;
                    FVector2D edgeDir = Vertex2 - Vertex1;
                    float cross = pathDir.X * edgeDir.Y - pathDir.Y * edgeDir.X;

                    NavLine Portal;
               
                    if (cross > 0)
                    {
                        Portal.P1 = Vertex1;
                        Portal.P2 = Vertex2;
                    }
                    else
                    {
                        Portal.P1 = Vertex2;
                        Portal.P2 = Vertex1;
                    }
                    Portals.push_back(Portal);
                }
            }

            // Degenerate portal at end (last node is endPos, not on any edge)
            Portals.push_back(NavLine{ Path.back()->GetPosition(), Path.back()->GetPosition() });

            return Portals;

		}

		static std::vector<FVector2D> OptimizePortals(std::vector<NavLine> const & Portals, TriPolygon const & NavPoly,UWorld * world)
		{
            std::vector<FVector2D> Path{};
            if (Portals.size() < 3)
            {
                Path.push_back(Portals.front().P1);
                Path.push_back(Portals.back().P1);
                return Path;
            }

            FVector2D portalApex = Portals[0].P1;
            FVector2D portalLeft = Portals[0].P2;
            FVector2D portalRight = Portals[0].P1;
            int apexIndex = 0;
            int leftIndex = 0;
            int rightIndex = 0;

            Path.push_back(portalApex);

            int safetyCounter = 0;
            const int maxIterations = static_cast<int>(Portals.size()) * 10;

            for (int i = 1; i < static_cast<int>(Portals.size()); ++i)
            {
                if (++safetyCounter > maxIterations)
                {
                    UE_LOG(LogTemp, Error, TEXT("Funnel infinite loop"));
                    break;
                }

                FVector2D left = Portals[i].P1;
                FVector2D right = Portals[i].P2;


                // Debug
              /*  DrawDebugLine(world,
                    FVector{ portalApex.X, portalApex.Y, 20.f },
                    FVector{ portalRight.X, portalRight.Y, 20.f },
                    FColor::Red, false, 5.f);
                DrawDebugLine(world,
                    FVector{ portalApex.X, portalApex.Y, 20.f },
                    FVector{ portalLeft.X, portalLeft.Y, 20.f },
                    FColor::Blue, false, 5.f);*/

                UE_LOG(LogTemp, Warning, TEXT("i=%d apex=(%f,%f) rLeg=(%f,%f) lLeg=(%f,%f)"),
                    i, portalApex.X, portalApex.Y,
                    portalRight.X, portalRight.Y,
                    portalLeft.X, portalLeft.Y);

                // --- RIGHT ---
                if (TriArea2(portalApex, portalRight, right) <= 0.0f)
                {
                    if (portalApex == portalRight || TriArea2(portalApex, portalLeft, right) > 0.0f)
                    {
                        portalRight = right;
                        rightIndex = i;
                    }
                    else
                    {
                        Path.push_back(portalLeft);
                        portalApex = portalLeft;
                        apexIndex = leftIndex;

                        portalLeft = portalApex;
                        portalRight = portalApex;
                        leftIndex = apexIndex;
                        rightIndex = apexIndex;

                        i = apexIndex;
                        continue;
                    }
                }

                // --- LEFT ---
                if (TriArea2(portalApex, portalLeft, left) >= 0.0f)
                {
                    if (portalApex == portalLeft || TriArea2(portalApex, portalRight, left) < 0.0f)
                    {
                        portalLeft = left;
                        leftIndex = i;
                    }
                    else
                    {
                        Path.push_back(portalRight);
                        portalApex = portalRight;
                        apexIndex = rightIndex;

                        portalLeft = portalApex;
                        portalRight = portalApex;
                        leftIndex = apexIndex;
                        rightIndex = apexIndex;

                        i = apexIndex;
                        continue;
                    }
                }
            }

            FVector2D endPoint = Portals.back().P1;
            if (Path.empty() || FVector2D::Distance(Path.back(), endPoint) > 1.f)
            {
                Path.push_back(endPoint);
            }

            return Path;
		}

	private:
		SSFA() {};
		~SSFA() {};
	};
}