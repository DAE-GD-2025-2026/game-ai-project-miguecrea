#include "SteeringBehaviors.h"
#include "GameAIProg/Movement/SteeringBehaviors/SteeringAgent.h"

#include "MACROS/MACRO.h"
SteeringOutput Seek::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput output{};

	UWorld * World = Agent.GetWorld();
	FVector2D Pos{ Agent.GetPosition()};

	FVector AgentPos = FVector{ Pos.X, Pos.Y, Agent.GetMeshZPosition() };
	FVector Target{ m_Target.Position.X, m_Target.Position.Y,AgentPos.Z };

	DRAW_CIRCLE(World, Target,10.f, FColor::Green, 7);
	DRAW_CIRCLE(World, Target,50.f, FColor::Green, 7);
	DRAW_VECTOR(World, AgentPos, AgentPos + Agent.GetActorForwardVector() * 100.f, FColor::Yellow);


	if (ArrivedToTarget(Agent))
	{
		return output;
	}

	output.LinearVelocity = AtoB(Pos, m_Target.Position).GetSafeNormal();
//	output.LinearVelocity = AtoB(Pos, m_Target.Position);
	return output;
}



Wander::Wander()
{
	m_AngleInRadians = FMath::RandRange(0.f, 2.f * PI);
}
SteeringOutput Wander::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{


	Agent.SetIsAutoOrienting(true);

	SteeringOutput Output;

	UWorld* World = Agent.GetWorld();


	FVector2D Pos = Agent.GetPosition();
	FVector AgentPos = FVector(Pos.X, Pos.Y, Agent.GetMeshZPosition());

	FVector Forward = Agent.GetActorForwardVector() * 300.f;
	FVector CircleCenter = AgentPos + Forward;


	constexpr float AngleChangePerSecond = 35.f; // degrees per second

	float RandomDeltaDegrees =
		FMath::RandRange(-AngleChangePerSecond, AngleChangePerSecond);

	float RandomDeltaRadians =
		FMath::DegreesToRadians(RandomDeltaDegrees);

	m_AngleInRadians += RandomDeltaRadians;


	FVector2D TargetPoint;
	TargetPoint.X = CircleCenter.X + FMath::Cos(m_AngleInRadians) * m_Radius;
	TargetPoint.Y = CircleCenter.Y + FMath::Sin(m_AngleInRadians) * m_Radius;

	FVector2D DesiredDirection = (TargetPoint - Pos);
	Output.LinearVelocity = DesiredDirection;

	FVector circleDebug{ TargetPoint.X,TargetPoint.Y,AgentPos.Z };


	//DRAW_VECTOR(World, AgentPos, CircleCenter, FColor::Yellow);
	//DRAW_CIRCLE(World, CircleCenter, m_Radius, FColor::Green, 7);
	//DRAW_CIRCLE(World, circleDebug, 20.f, FColor::Red, 7);

	return Output;
}

SteeringOutput Flee::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput output{};
	output.LinearVelocity = -AtoB(Agent.GetPosition(), m_Target.Position);
	return  output;
}

SteeringOutput Pursuit::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	return SteeringOutput();
}

SteeringOutput Evade::CalculateSteering(float DeltaT, ASteeringAgent & Agent)
{

	SteeringOutput steering{};

	UWorld * World = Agent.GetWorld();
	FVector2D agentPos = Agent.GetPosition();

	FVector AgentPosDebug = FVector(agentPos.X, agentPos.Y, Agent.GetMeshZPosition());

	
//	DRAW_CIRCLE(World, AgentPosDebug,m_EvadeRadius, FColor::Yellow,10);

	FVector2D targetPos = m_Target.Position;
	FVector2D toTarget = targetPos - agentPos;

	float distance = toTarget.Size();

	if (distance > m_EvadeRadius)
	{
		steering.IsValid = false;
	}
	else if (distance < m_EvadeRadius)
	{
		steering.IsValid = true;
	}

	FVector2D agentVel = Agent.GetLinearVelocity();
	FVector2D targetVel = m_Target.LinearVelocity;


	//how fast it is pulling away or how much faster it is compared to me 
	FVector2D relativeVel = targetVel - agentVel;
	float relativeSpeed = relativeVel.Size();

	//calculate prediction Time 

	float predictionTime = 0.f;

	if (relativeSpeed > 0.01f)
	{
		predictionTime = distance / relativeSpeed;
	}

	FVector2D futurePos = targetPos + targetVel * predictionTime;

	// Flee
	FVector2D fleeDir = agentPos - futurePos;

	steering.LinearVelocity = fleeDir.GetSafeNormal();

	return steering;
}

SteeringOutput Arrive::CalculateSteering(float DeltaT, ASteeringAgent& Agent)
{
	SteeringOutput output;

	FVector2D Pos = Agent.GetPosition();
	FVector2D ToTarget = m_Target.Position - Pos;

	float Distance = ToTarget.Size();

	if (Distance < m_StopRadius)
	{
		output.LinearVelocity = FVector2D::ZeroVector;
		return output;
	}

	float SpeedFactor = 1.f;

	if (Distance < m_SlowDownRadius)
	{
		SpeedFactor = Distance / m_SlowDownRadius; // 0–1
	}

	output.LinearVelocity = ToTarget.GetSafeNormal() * SpeedFactor;

	UWorld* World = Agent.GetWorld();
	FVector Target{ m_Target.Position.X,m_Target.Position.Y,Agent.GetMeshZPosition() };

	DRAW_CIRCLE(World, Target, m_SlowDownRadius, FColor::Black, 7);
	DRAW_CIRCLE(World, Target, m_StopRadius, FColor::Green, 7);


	return output;
}


bool ISteeringBehavior::ArrivedToTarget(ASteeringAgent & agent)
{
	if (AtoB(agent.GetPosition(), m_Target.Position).Size() <= 50.f)
	{
		return true;
	}
	return false;
}

// Helper Functions
FVector2D ISteeringBehavior::AtoB(FVector2D self, FVector2D target)
{
	return  target - self;
}
