#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

//SEEK
//*******
// TODO: Do the Week01 assignment :^)

SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{

    SteeringOutput output{};


    output.LinearVelocity = AtoB(Agent.GetPosition(), Target.Position);

    return output;
}

SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
    return SteeringOutput();
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{

    SteeringOutput output{};
    output.LinearVelocity = -AtoB(Agent.GetPosition(), Target.Position);

    return  output;
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
    return SteeringOutput();
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
    return SteeringOutput();
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
    return SteeringOutput();
}



// Helper Functions
FVector2D ISteeringBehavior::AtoB(FVector2D self,FVector2D target)
{
    return  target - self;
}
