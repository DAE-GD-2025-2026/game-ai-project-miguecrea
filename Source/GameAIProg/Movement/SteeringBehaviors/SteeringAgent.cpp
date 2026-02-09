// Fill out your copyright notice in the Description page of Project Settings.

#include "SteeringAgent.h"


ASteeringAgent::ASteeringAgent()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASteeringAgent::BeginPlay()
{
	Super::BeginPlay();
}

void ASteeringAgent::BeginDestroy()
{
	Super::BeginDestroy();
}

void ASteeringAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SteeringBehavior)
	{
		SteeringOutput output = SteeringBehavior->CalculateSteering(DeltaTime, *this);
		AddMovementInput(FVector{output.LinearVelocity, 0.f});
	}
}

void ASteeringAgent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ASteeringAgent::SetSteeringBehavior(ISteeringBehavior * NewSteeringBehavior)
{
	SteeringBehavior = NewSteeringBehavior;
}

