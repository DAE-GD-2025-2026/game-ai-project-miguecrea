// Fill out your copyright notice in the Description page of Project Settings.

#include "SteeringAgent.h"

#include "Components/SkeletalMeshComponent.h"


ASteeringAgent::ASteeringAgent()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASteeringAgent::BeginPlay()
{
	Super::BeginPlay();

	m_MeshComponent = GetComponentByClass<USkeletalMeshComponent>();

	if (m_MeshComponent)
	{
		UE_LOG(LogTemp,Warning,TEXT("Succeded"))
	}
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

		// The angular velocity is not being usedddd
		SteeringOutput output = SteeringBehavior->CalculateSteering(DeltaTime, *this);
		AddMovementInput(FVector{output.LinearVelocity, 0.f});

		if (!IsAutoOrienting())
		{
			AddControllerYawInput(output.AngularVelocity);
		}
	}
}



float ASteeringAgent::GetMeshZPosition() const
{
	if (m_MeshComponent)
	{
	   return m_MeshComponent->GetComponentLocation().Z;
	}
	else
	{
		//I should Use Optional 
		return 0.0f;
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

