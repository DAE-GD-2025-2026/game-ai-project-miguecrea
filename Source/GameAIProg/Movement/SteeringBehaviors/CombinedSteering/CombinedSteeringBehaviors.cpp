
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
	SteeringOutput BlendedSteering = {};



	auto totalWeight = 0.f;

	/*for (auto weightedBehavior : m_WeightedBehaviors)
	{
		auto steering = weightedBehavior.pBehavior->CalculateSteering(deltaT, pAgent);
		blendedSteering.LinearVelocity += weightedBehavior.weight * steering.LinearVelocity;
		blendedSteering.AngularVelocity += weightedBehavior.weight * steering.AngularVelocity;

		totalWeight += weightedBehavior.weight;
	}

	if (totalWeight > 0.f)
	{
		auto scale = 1.f / totalWeight;
		blendedSteering *= scale;
	}*/


	/*for (const WeightedBehavior & weightedBehavior : WeightedBehaviors)
	{
		auto BlendedSteering = weightedBehavior.pBehavior->CalculateSteering(deltaT, Agent);
		BlendedSteering.LinearVelocity += weightedBehavior.Weight;

	}*/




	//TODO: Calculate the weighted average steeringbehavior

	if (Agent.GetDebugRenderingEnabled())DrawDebugDirectionalArrow(
		Agent.GetWorld(),Agent.GetActorLocation(),Agent.GetActorLocation()
		+ FVector{BlendedSteering.LinearVelocity, 0} *
		(Agent.GetMaxLinearSpeed() * deltaT),30.f, FColor::Red
			);

      return BlendedSteering;
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