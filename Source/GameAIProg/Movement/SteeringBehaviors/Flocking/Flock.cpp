#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"
#include "Level_Flocking.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"
#include "../SpacePartitioning/SpacePartitioning.h"
#include"../../../MACROS/MACRO.h"

#include"../SteeringAgent.h"
Flock::Flock(
	UWorld* pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	float MeshZPos,
	ALevel_Flocking* CurrentLevel,
	int FlockSize,
	float WorldSize,
	ASteeringAgent * const pAgentToEvade,
	bool bTrimWorld,const FVector & CenterBox)
	: m_pWorld{ pWorld }
	, m_FlockSize{ FlockSize }
	, m_MeshZPos{MeshZPos}
	, pAgentToEvade{ pAgentToEvade }
	, m_CurrentLevel{ CurrentLevel }
	, m_TrimWorldSize{ WorldSize }
	, m_AgentBlueprint{ AgentClass }
	,m_VolumeBoxCenter{CenterBox}
{
	Agents.Reserve(FlockSize);
	m_Neighbors.SetNum(FlockSize);

	if (!m_CurrentLevel)
	{
		UE_LOG(LogTemp, Error, TEXT(" Current Level Is Null"))
	   return;
	}

	pSeekBehavior = std::make_unique<Seek>();
	//pSeekBehavior->SetTarget(m_SeekTarget);
	pWanderBehavior = std::make_unique<Wander>();


	///EVADE  I snot added in blended 
	pEvadeBehavior = std::make_unique<Evade>();

	pSeparationBehavior = std::make_unique<Separation>(this);
	pCohesionBehavior = std::make_unique<Cohesion>(this);
	pVelMatchBehavior = std::make_unique<VelocityMatch>(this);


	std::vector<BlendedSteering::WeightedBehavior> m_WeightedBehaviors{};
	pBlendedSteering = std::make_unique<BlendedSteering>(m_WeightedBehaviors);
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{ pSeekBehavior.get(),SeekWeight}));
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{ pWanderBehavior.get(),WanderWeight }));
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{ pSeparationBehavior.get(),SeparationWeight }));
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{ pCohesionBehavior.get(),CohesionWeight }));
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{ pVelMatchBehavior.get(),AlignmentWeight }));

	std::vector<ISteeringBehavior*> m_SteeringBehaviors{};

	m_SteeringBehaviors.push_back(pBlendedSteering.get());
	m_SteeringBehaviors.push_back(pEvadeBehavior.get());

	pPrioritySteering = std::make_unique<PrioritySteering>(m_SteeringBehaviors);

	for (int i = 0; i < m_FlockSize; i++)
	{
		FVector RandomPose = m_CurrentLevel->GetRamdomPosInVolumeBox();
		FActorSpawnParameters SpawnParameters{};
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ASteeringAgent * SteeringAgent = m_pWorld->SpawnActor<ASteeringAgent>(m_AgentBlueprint, FVector{ RandomPose.X, RandomPose.Y, 90 }, FRotator::ZeroRotator, SpawnParameters);
		if (SteeringAgent)
		{
			SteeringAgent->SetIsAutoOrienting(true);
			SteeringAgent->SetActorTickEnabled(false);
			SteeringAgent->SetSteeringBehavior(pBlendedSteering.get());

		//	SteeringAgent->SetSteeringBehavior(pPrioritySteering.get());
			Agents.Add(SteeringAgent);
		}


	}

	pPartitionedSpace = std::make_unique<CellSpace>(m_pWorld,m_TrimWorldSize, m_TrimWorldSize,11,11, m_FlockSize, FVector2D{ m_VolumeBoxCenter.X,m_VolumeBoxCenter.Y },m_MeshZPos);

}

Flock::~Flock()
{
	// TODO: Cleanup any additional data
}

void Flock::Tick(float DeltaTime)
{



	float PosOgMovingAgent = pAgentToEvade->GetPosition().X;

	//UE_LOG(LogTemp,Warning,TEXT("%f"), PosOgMovingAgent)

	for (ASteeringAgent * Agent : Agents)
	{
	
		RegisterNeighbors(Agent);
		Agent->Tick(DeltaTime);
	}

	

	if (pPartitionedSpace)
	{
		FVector2D Pos = pAgentToEvade->GetPosition();
		int Index = pPartitionedSpace->PositionToIndex(Pos);
		//UE_LOG(LogTemp,Warning,TEXT("%d"),Index)

	}




	UpdateIamguiTrimVariable();
	
}

void Flock::UpdateIamguiTrimVariable()
{
	if (m_CurrentLevel)
	{
		m_CurrentLevel->SetTrimWorldSize(m_TrimWorldSize);
	}
}

void Flock::RenderDebug()
{
	pPartitionedSpace->RenderCells();

}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const & WindowSize)
{
#ifdef PLATFORM_WINDOWS
#pragma region UI
	{
		bool bWindowActive = true;
		ImGui::SetNextWindowPos(WindowPos);
		ImGui::SetNextWindowSize(WindowSize);
		ImGui::Begin("Gameplay Programming", &bWindowActive, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		//Elements
		ImGui::Text("CONTROLS");
		ImGui::Indent();
		ImGui::Text("LMB: place target");
		ImGui::Text("RMB: move cam.");
		ImGui::Text("Scrollwheel: zoom cam.");
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();
		ImGui::Spacing();

		ImGui::Text("STATS");
		ImGui::Indent();
		ImGui::Text("%.3f ms/frame", 1000.0f / ImGui::GetIO().Framerate);
		ImGui::Text("%.1f FPS", ImGui::GetIO().Framerate);
		ImGui::Unindent();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Flocking");
		ImGui::Spacing();

		ImGui::Checkbox("Render Steering", &DebugRenderSteering);
		ImGui::Checkbox("DebugRender Neighborh", &DebugRenderNeighborhood);
		ImGui::Checkbox("Debug Render Partition", &DebugRenderPartitions);

		ImGui::Text("Behavior Weights");
		ImGui::Spacing();

		ImGui::SliderFloat("Separation", &SeparationWeight, 0.0f, 1.0f);
		ImGui::SliderFloat("Cohesion", &CohesionWeight, 0.0f, 1.0f);
		ImGui::SliderFloat("Alignment", &AlignmentWeight, 0.0f, 1.0f);
		ImGui::SliderFloat("Seek", &SeekWeight, 0.0f, 1.0f);
		ImGui::SliderFloat("Wander", &WanderWeight, 0.0f, 1.0f);

		ImGui::Spacing();

		ImGui::Text("Trim WorldSize");
		ImGui::SliderFloat("Trim", &m_TrimWorldSize, 0.0f, 4000.f);


		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RegisterNeighbors(const ASteeringAgent * CurrentAgent)
{
	m_NumberOfNeighbors = 0;

	for (ASteeringAgent * Agent : Agents)
	{
		if (Agent == CurrentAgent || !IsValid(Agent) || !IsValid(CurrentAgent))
		{
			continue;
		}
		FVector2D ToAgent = Agent->GetPosition() - CurrentAgent->GetPosition();

		float distanceToNeigbord = ToAgent.Size();
		if (distanceToNeigbord < m_NeighborhoodRadius)
		{
			//MemoryPool
			m_Neighbors[m_NumberOfNeighbors] = Agent;
			m_NumberOfNeighbors++;
		}
	}

	if (!DebugRenderNeighborhood) return;
	if (CurrentAgent == Agents[0])
	{
		for (int i = 0; i < m_NumberOfNeighbors; i++)
		{
			FVector2D Pos = { m_Neighbors[i]->GetPosition() };
			FVector RenderPos{ Pos.X,Pos.Y,m_Neighbors[i]->GetMeshZPosition() };
			DRAW_CIRCLE(m_pWorld, RenderPos, 60.f, FColor::Blue, 10);
		}
		FVector2D Pos = { CurrentAgent->GetPosition()};
		FVector RenderPos{ Pos.X,Pos.Y,m_MeshZPos};
		DRAW_CIRCLE(m_pWorld, RenderPos, m_NeighborhoodRadius, FColor::Green, 10);
		DRAW_CIRCLE(m_pWorld, RenderPos, 70.f, FColor::Red, 10);

	}
}

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = { FVector2D::ZeroVector};
	
	for (int i{ 0 }; i < m_NumberOfNeighbors; ++i)
	{
		avgPosition += m_Neighbors[i]->GetPosition();
	}
	if (m_NumberOfNeighbors > 0)
	{
		avgPosition /= m_NumberOfNeighbors;
	}
	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;
	for (int i{ 0 }; i < m_NumberOfNeighbors; ++i)
	{
		avgVelocity += m_Neighbors[i]->GetLinearVelocity();
	}
	if (m_NumberOfNeighbors > 0)
	{
		avgVelocity /= m_NumberOfNeighbors;
		avgVelocity.Normalize();
	}
	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const & Target)
{
	m_SeekTarget = Target;
}



