#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput output;
	if (pFlock->GetNrOfNeighbors() > 0)
	{
		m_Target.Position = pFlock->GetAverageNeighborPos();
	}
    else
    {
        output.LinearVelocity = FVector2D::ZeroVector;
        return output;
    }

	FVector2D ToAveragePos = m_Target.Position - pAgent.GetPosition();
	output.LinearVelocity = ToAveragePos.GetSafeNormal();

	return output;
}



SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent & pAgent)
{
    SteeringOutput output;

    FVector2D result{};
    int nrNeighbors = pFlock->GetNrOfNeighbors();

    if (nrNeighbors > 0)
    {
        for (int i = 0; i < nrNeighbors; ++i)
        {
            FVector2D offset = pAgent.GetPosition() - pFlock->GetNeighbors()[i]->GetPosition();

            float dist = offset.Size();

            if (dist > 0.001f)
            {
                result += offset / dist;   // 1/d weighting
            }
        }

      //  result /= nrNeighbors;  // average
        output.LinearVelocity = result.GetSafeNormal();
    }

    return output;

}

SteeringOutput VelocityMatch::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	SteeringOutput output;

    FVector2D avgVel = pFlock->GetAverageNeighborVelocity();
    FVector2D desired = avgVel - pAgent.GetLinearVelocity();
    output.LinearVelocity = desired.GetSafeNormal();

    return output;
}
