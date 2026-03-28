#pragma once
#include <vector>

#include "NavGraphPathfinding.h"
#include "Movement/Pathfinding/Navmesh/TriPolygon.h"
#include "Shared/Graph/Graph.h"
#include "Shared/Graph/NavGraph/NavGraphNode.h"
#include "Shared/Graph/NavGraph/NavGraph.h"

namespace GameAI
{
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
                    if (cross < 0)
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

            FVector2D apex = Portals[0].P1;
            int apexIndex = 0;

            FVector2D rightLeg = Portals[1].P1;
            FVector2D leftLeg = Portals[1].P2;
            int rightIndex = 1;
            int leftIndex = 1;

            Path.push_back(apex);

            int safetyCounter = 0;
            const int maxIterations = static_cast<int>(Portals.size()) * 10;

            for (int i = 2; i <= static_cast<int>(Portals.size()) - 1; ++i)
            {
                if (++safetyCounter > maxIterations)
                {
                    UE_LOG(LogTemp, Error, TEXT("Funnel infinite loop"));
                    break;
                }

                // Debug at top
                DrawDebugLine(world,
                    FVector{ apex.X, apex.Y, 20.f },
                    FVector{ rightLeg.X, rightLeg.Y, 20.f },
                    FColor::Red, false, 5.f);
                DrawDebugLine(world,
                    FVector{ apex.X, apex.Y, 20.f },
                    FVector{ leftLeg.X, leftLeg.Y, 20.f },
                    FColor::Blue, false, 5.f);

                FVector2D newRight;
                FVector2D newLeft;

                if (i == static_cast<int>(Portals.size()) - 1)
                {
                    newRight = Portals[i].P1;
                    newLeft = Portals[i].P1;
                }
                else
                {
                    newRight = Portals[i].P1;
                    newLeft = Portals[i].P2;
                }

                // --- RIGHT CHECK --- (flipped signs)
                FVector2D apexToRight = rightLeg - apex;
                FVector2D apexToNewRight = newRight - apex;
                float crossRight = apexToRight.X * apexToNewRight.Y - apexToRight.Y * apexToNewRight.X;

                if (crossRight <= 0.f)
                {
                    FVector2D apexToLeft = leftLeg - apex;
                    float crossCheck = apexToLeft.X * apexToNewRight.Y - apexToLeft.Y * apexToNewRight.X;

                    if (crossCheck > 0.f)
                    {
                        rightLeg = newRight;
                        rightIndex = i;
                    }
                    else
                    {
                        Path.push_back(leftLeg);
                        apex = leftLeg;
                        apexIndex = leftIndex;

                        rightLeg = apex;
                        leftLeg = apex;
                        rightIndex = apexIndex;
                        leftIndex = apexIndex;

                        i = apexIndex;
                        continue;
                    }
                }

                // --- LEFT CHECK --- (flipped signs)
                FVector2D apexToLeftCurrent = leftLeg - apex;
                FVector2D apexToNewLeft = newLeft - apex;
                float crossLeft = apexToLeftCurrent.X * apexToNewLeft.Y - apexToLeftCurrent.Y * apexToNewLeft.X;

                UE_LOG(LogTemp, Warning, TEXT("i=%d crossRight=%f crossLeft=%f apex=(%f,%f) rLeg=(%f,%f) lLeg=(%f,%f)"),
                    i, crossRight, crossLeft,
                    apex.X, apex.Y,
                    rightLeg.X, rightLeg.Y,
                    leftLeg.X, leftLeg.Y);

                if (crossLeft >= 0.f)
                {
                    FVector2D apexToRightCurrent = rightLeg - apex;
                    float crossCheck = apexToRightCurrent.X * apexToNewLeft.Y - apexToRightCurrent.Y * apexToNewLeft.X;

                    if (crossCheck < 0.f)
                    {
                        leftLeg = newLeft;
                        leftIndex = i;
                    }
                    else
                    {
                        Path.push_back(rightLeg);
                        apex = rightLeg;
                        apexIndex = rightIndex;

                        rightLeg = apex;
                        leftLeg = apex;
                        rightIndex = apexIndex;
                        leftIndex = apexIndex;

                        i = apexIndex;
                        continue;
                    }
                }
            }

            // Always add end point
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