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
	//=== SSFA Functions ===
	//--- References ---
	//http://digestingduck.blogspot.be/2010/03/simple-stupid-funnel-algorithm.html
	//https://gamedev.stackexchange.com/questions/68302/how-does-the-simple-stupid-funnel-algorithm-work
	static std::vector<NavLine> FindPortals(std::vector<Node*> const & Path, TriPolygon const & NavPoly, NavGraph * const pNavGraph)
	{
	
		std::vector<NavLine> Portals;

		// Degenerate portal at start
		FVector2D startPos = Path.front()->GetPosition();
		Portals.push_back(NavLine{startPos,startPos });

		for (size_t i = 1; i < Path.size() - 1; ++i)
		{
			//find vertices of that edge where node sits
			auto it = pNavGraph->nodeToEdgeEndpoints.find(Path[i]->GetId());
			if (it != pNavGraph->nodeToEdgeEndpoints.end())
			{
				auto [Vertex1, Vertex2] = it->second;

				// Determine left/right orientation based on path direction
				FVector2D pathDir = Path[i + 1]->GetPosition() - Path[i - 1]->GetPosition();

				FVector2D edgeDir = Vertex2 - Vertex1;
				float cross = pathDir.X * edgeDir.Y - pathDir.Y * edgeDir.X;


				NavLine Portal;
				if (cross > 0)
				{
					Portal.P1 = Vertex1;  // right
					Portal.P2 = Vertex2;  // left
	
				}
				else
				{
					Portal.P1 = Vertex2;  // right
					Portal.P2 = Vertex1;  // left
				}

				Portals.push_back(Portal);  // p1 right, p2 left


			}
			else
			{
				UE_LOG(LogTemp,Error,TEXT("Node not found  on Map"))
			}
		}

		// Degenerate portal at end
		FVector2D endPos = Path.back()->GetPosition();
		Portals.push_back(NavLine{ endPos, endPos });

		return Portals;
	}

	static std::vector<FVector2D> OptimizePortals( std::vector<NavLine> const & Portals, TriPolygon const & NavPoly)
	{
        
        std::vector<FVector2D> Path{};
        if (Portals.size() < 3)
        {
            // Only start and end, just go straight
            Path.push_back(Portals.front().P1);
            Path.push_back(Portals.back().P1);
            return Path;
        }

        // Start from the actual start position
        FVector2D apex = Portals[0].P1;  // start position
        int apexIndex = 0;

        // First real portal legs
        FVector2D rightLeg = Portals[1].P1;
        FVector2D leftLeg = Portals[1].P2;
        int rightIndex = 1;
        int leftIndex = 1;

        Path.push_back(apex);

        int safetyCounter = 0;
        const int maxIterations = static_cast<int>(Portals.size()) * 10;

        // Only iterate through real portals (skip first and last degenerate ones)
        for (int i = 2; i <= static_cast<int>(Portals.size()) - 1; ++i)
        {
            if (++safetyCounter > maxIterations)
            {
                UE_LOG(LogTemp, Error, TEXT("Funnel infinite loop"));
                break;
            }

            FVector2D newRight;
            FVector2D newLeft;

            if (i == static_cast<int>(Portals.size()) - 1)
            {
                // Last portal is degenerate (end point)
                // Both sides converge to the end position
                newRight = Portals[i].P1;
                newLeft = Portals[i].P1;
            }
            else
            {
                newRight = Portals[i].P1;
                newLeft = Portals[i].P2;
            }

            // --- RIGHT CHECK ---
            FVector2D apexToRight = rightLeg - apex;
            FVector2D apexToNewRight = newRight - apex;
            float crossRight = apexToRight.X * apexToNewRight.Y - apexToRight.Y * apexToNewRight.X;

            if (crossRight >= 0.f)
            {
                FVector2D apexToLeft = leftLeg - apex;
                float crossCheck = apexToLeft.X * apexToNewRight.Y - apexToLeft.Y * apexToNewRight.X;

                if (crossCheck < 0.f)
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

                    i = apexIndex + 1;
                    continue;
                }
            }

            // --- LEFT CHECK ---
            FVector2D apexToLeftCurrent = leftLeg - apex;
            FVector2D apexToNewLeft = newLeft - apex;
            float crossLeft = apexToLeftCurrent.X * apexToNewLeft.Y - apexToLeftCurrent.Y * apexToNewLeft.X;

            if (crossLeft <= 0.f)
            {
                FVector2D apexToRightCurrent = rightLeg - apex;
                float crossCheck = apexToRightCurrent.X * apexToNewLeft.Y - apexToRightCurrent.Y * apexToNewLeft.X;

                if (crossCheck > 0.f)
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

                    i = apexIndex + 1;
                    continue;
                }
            }
        }

        // Add end point
        FVector2D endPoint = Portals.back().P1;
        if (Path.back() != endPoint)
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
