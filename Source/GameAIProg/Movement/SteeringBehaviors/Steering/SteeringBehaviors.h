#pragma once

#include <Movement/SteeringBehaviors/SteeringHelpers.h>
#include "Kismet/KismetMathLibrary.h"

class ASteeringAgent;

// SteeringBehavior base, all steering behaviors should derive from this.
class ISteeringBehavior
{
public:
	ISteeringBehavior() = default;
	virtual ~ISteeringBehavior() = default;

	// Override to implement your own behavior
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent) = 0;

	void SetTarget(const FTargetData & NewTarget) { m_Target = NewTarget; }
	
	FVector2D AtoB(FVector2D self,FVector2D target);
	template<class T, std::enable_if_t<std::is_base_of_v<ISteeringBehavior, T>>* = nullptr>
	T* As()
	{ return static_cast<T*>(this);}

	bool ArrivedToTarget(ASteeringAgent &  agent);

protected:
	FTargetData m_Target;
};


class Seek : public ISteeringBehavior
{
public:
	Seek() = default;
	virtual ~Seek() = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent)  override;
protected:
};


class Wander : public ISteeringBehavior
{
public:
	Wander();
	virtual ~Wander() = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent)  override;

	float m_Radius = 200.f;
	float m_Angle = 0.0f;

	float m_TotalTimeElapsed = 0.0f;
	float m_AngleUpdateTime = 0.4f;
	float m_AngleInRadians = 0.0f;

protected:
};

class Flee : public ISteeringBehavior
{
public:
	Flee() = default;
	virtual ~Flee() = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent & Agent)  override;
protected:
};

class Pursuit : public ISteeringBehavior
{
public:
	Pursuit() = default;
	virtual ~Pursuit() = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent)  override;
protected:
};


class Evade : public ISteeringBehavior
{
public:
	Evade() = default;
	virtual ~Evade() = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent)  override;
	float m_EvadeRadius = 400.f;
protected:
};


class Arrive : public ISteeringBehavior
{
public:
	Arrive() = default;
	virtual ~Arrive() = default;
	virtual SteeringOutput CalculateSteering(float DeltaT, ASteeringAgent& Agent)  override;
	
	void SetTargetRadius(float newstopRadius) { m_StopRadius = newstopRadius; }
	float  m_StopRadius{100.f};
	float m_SlowDownRadius{350.f};

protected:
};










//   Bigger Radius then Slow down 
// to a smaller one 
// Do Wold Hunting Pattern :







// Your own SteeringBehaviors should follow here...
