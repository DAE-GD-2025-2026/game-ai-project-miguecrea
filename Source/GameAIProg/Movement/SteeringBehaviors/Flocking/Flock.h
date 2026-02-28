#pragma once

// Toggle this define to enable/disable spatial partitioning
 //#define GAMEAI_USE_SPACE_PARTITIONING

#include "FlockingSteeringBehaviors.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"
#include "Movement/SteeringBehaviors/SteeringHelpers.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include <memory>
#include "imgui.h"


class CellSpace;

class Flock final
{
public:
	Flock(UWorld * pWorld,TSubclassOf<ASteeringAgent> AgentClass, float  MeshZPos,class ALevel_Flocking* m_CurrentLevel,int FlockSize = 10,float WorldSize = 100.f,ASteeringAgent* const pAgentToEvade = nullptr, bool bTrimWorld = false, const FVector & CenterBox  = {});

	~Flock();

	void Tick(float DeltaTime);
	void UpdateIamguiTrimVariable();
	void RenderDebug();
	void ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize);



	//NORMAL



	// SPACE_PARTITION

	std::unique_ptr<CellSpace> pPartitionedSpace{};
	int NrOfCellsX{ 10 };


	//QUAD_TREE

	//const TArray<ASteeringAgent*> & GetNeighbors() const { return pPartitionedSpace->GetNeighbors(); }
	//int GetNrOfNeighbors() const { return pPartitionedSpace->GetNrOfNeighbors(); }


	void RegisterNeighbors(const ASteeringAgent * Agent);

	int GetNrOfNeighbors() const { return m_NumberOfNeighbors; }
	const TArray<ASteeringAgent*> & GetNeighbors() const { return m_Neighbors; }

	FVector2D GetAverageNeighborPos() const;
	FVector2D GetAverageNeighborVelocity() const;
	void SetTarget_Seek(FSteeringParams const & Target);


protected:



private:

	FSteeringParams m_SeekTarget;
	UWorld * m_pWorld{nullptr};
	int m_FlockSize{};
	TArray<ASteeringAgent*> Agents{};

	TArray<FVector2D> OldPositions{};

	TArray<ASteeringAgent*> m_Neighbors{};
	float m_NeighborhoodRadius{280.f};
	FVector m_VolumeBoxCenter{};
	int m_NumberOfNeighbors{0};

	ASteeringAgent * pAgentToEvade{nullptr};
	std::unique_ptr<Separation> pSeparationBehavior{};
	std::unique_ptr<Cohesion> pCohesionBehavior{};
    std::unique_ptr<VelocityMatch> pVelMatchBehavior{};
	std::unique_ptr<Seek> pSeekBehavior{};
	std::unique_ptr<Wander> pWanderBehavior{};
	std::unique_ptr<Evade> pEvadeBehavior{};
	std::unique_ptr<BlendedSteering> pBlendedSteering{};
	std::unique_ptr<PrioritySteering> pPrioritySteering{};
	
	float SeparationWeight = 0.4f;
	float CohesionWeight = 0.3f;
	float AlignmentWeight = 0.6f;
	float SeekWeight = 0.f;
	float WanderWeight = 0.7f;

	// UI and rendering
	bool DebugRenderSteering{false};
	bool DebugRenderNeighborhood{true};
	bool DebugRenderPartitions{true};

	enum class Type
	{
		NO_OPTIMIZATION,
		SPACE_PARTITIONING,
		QUAD_TREE
	};

	Type m_Type{ Type::NO_OPTIMIZATION };


	float m_MeshZPos;

    ALevel_Flocking * m_CurrentLevel;
	TSubclassOf<ASteeringAgent> m_AgentBlueprint;
	float m_TrimWorldSize = 0.f;
	bool m_TrimWorld  = false;



};
