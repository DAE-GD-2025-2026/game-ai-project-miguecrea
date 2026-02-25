#include "FlockingSteeringBehaviors.h"
#include "Flock.h"
#include "../SteeringAgent.h"
#include "../SteeringHelpers.h"


//*******************
//COHESION (FLOCKING)
SteeringOutput Cohesion::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	return SteeringOutput{};
}



SteeringOutput Separation::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	return SteeringOutput();
}

SteeringOutput VelocityMatch::CalculateSteering(float deltaT, ASteeringAgent& pAgent)
{
	return SteeringOutput();
}
