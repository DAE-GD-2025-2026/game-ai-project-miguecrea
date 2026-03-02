// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameAIProg/Shared/BaseAgent.h"
#include "Steering/SteeringBehaviors.h"
#include "SteeringAgent.generated.h"

/*
 * Simple agent which will run a steering behavior and move according to its output
 *
 * ┌────────────────────┐                                     
 * │                    │                                     
 * │      ABaseAgent    │                                     
 * │                    │                                     
 * └─────────▲──────────┘                                     
 *           │                                                
 *           │inherits                                        
 * ┌─────────┼──────────┐            ┌───────────────────────┐
 * │                    │   uses     │                       │
 * │   ASteeringAgent   ├────────────►   ISteeringBehavior   │
 * │                    │            │                       │
 * └────────────────────┘            └─┬─────────────────────┘
 *                                     │                      
 *                                     │ inherits   ┌────────┐
 *                                     │            │        │
 *                                     ├───────────►│  Seek  │
 *                                     │            │        │
 *                                     │            └────────┘
 *                                     │                      
 *                                     │            ┌────────┐
 *                                     │            │        │
 *                                     ├───────────►│  Flee  │
 *                                     │            │        │
 *                                     │            └────────┘
 *                                     │                      
 *                                     │            ┌────────┐
 *                                     │            │        │
 *                                     └───────────►│  ....  │
 *                                                  │        │
 *                                                  └────────┘
 * 
 */


class UMeshComponent;

UCLASS()
class GAMEAIPROG_API ASteeringAgent : public ABaseAgent
{
	GENERATED_BODY()

public:
	ASteeringAgent();

protected:

	ISteeringBehavior * SteeringBehavior{nullptr}; // non-owning
	
	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	FVector2D m_PreviousFramePos{};
public:
	virtual void Tick(float DeltaTime) override;

	FVector2D GetPreviousFramePos() { return m_PreviousFramePos;}
	void  SetPreviousFramePos(FVector2D OldPos) { m_PreviousFramePos = OldPos;}



	USkeletalMeshComponent * m_MeshComponent;

	float GetMeshZPosition() const ;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void SetSteeringBehavior(ISteeringBehavior* NewSteeringBehavior);
};
