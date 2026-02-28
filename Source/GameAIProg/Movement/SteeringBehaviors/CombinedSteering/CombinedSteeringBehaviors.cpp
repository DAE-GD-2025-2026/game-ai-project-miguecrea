
#include "CombinedSteeringBehaviors.h"
#include <algorithm>
#include "../SteeringAgent.h"

BlendedSteering::BlendedSteering(const std::vector<WeightedBehavior> & WeightedBehaviors)
	:WeightedBehaviors(WeightedBehaviors)
{};

//****************
//BLENDED STEERING
SteeringOutput BlendedSteering::CalculateSteering(float deltaT, ASteeringAgent & Agent)
{
    SteeringOutput Result{};
    float TotalWeight = 0.f;

    for (const auto& Weighted : WeightedBehaviors)
    {
        if (!Weighted.pBehavior || Weighted.Weight <= 0.f)
            continue;

        SteeringOutput S = Weighted.pBehavior->CalculateSteering(deltaT, Agent);

        Result.LinearVelocity += S.LinearVelocity * Weighted.Weight;
        Result.AngularVelocity += S.AngularVelocity * Weighted.Weight;

        TotalWeight += Weighted.Weight;
    }

    if (TotalWeight > 0.f)
    {
       // Result.LinearVelocity /= TotalWeight;
        Result.AngularVelocity /= TotalWeight;
    }

    // Important for MovementComponent input
    Result.LinearVelocity = Result.LinearVelocity.GetClampedToMaxSize(1.f);

    return Result;
}

//*****************
//PRIORITY STEERING
SteeringOutput PrioritySteering::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput Steering = {};

	for (ISteeringBehavior * const pBehavior : m_PriorityBehaviors)
	{
		Steering = pBehavior->CalculateSteering(DeltaT, Agent);
		if (Steering.IsValid)
			break;
	}
	//If non of the behavior return a valid output, last behavior is returned
	return Steering;
}