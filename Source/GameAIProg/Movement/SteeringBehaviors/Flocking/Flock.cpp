#include "Flock.h"
#include "FlockingSteeringBehaviors.h"
#include "Shared/ImGuiHelpers.h"
#include "Level_Flocking.h"
#include "Movement/SteeringBehaviors/CombinedSteering/CombinedSteeringBehaviors.h"


Flock::Flock(
	UWorld * pWorld,
	TSubclassOf<ASteeringAgent> AgentClass,
	ALevel_Flocking * CurrentLevel,
	int FlockSize,
	float WorldSize,
	ASteeringAgent * const pAgentToEvade,
	bool bTrimWorld)
	: m_pWorld{ pWorld }
	, m_FlockSize{ FlockSize }
	, pAgentToEvade{ pAgentToEvade}
	, m_CurrentLevel{ CurrentLevel }
	, m_TrimWorldSize{ WorldSize}
    ,m_AgentBlueprint{AgentClass}
{
	//AVOID UNNECESSARY VECTOR ALLOCATIONS 
	Agents.SetNum(FlockSize);
	if (!m_CurrentLevel)
	{
		UE_LOG(LogTemp, Error, TEXT(" Current Level Is Null"))
		return;
	}
	pSeekBehavior =  std::make_unique<Seek>();
	pWanderBehavior =  std::make_unique<Wander>();
	pEvadeBehavior =  std::make_unique<Evade>();
	//Flocking Behaviors
	pSeparationBehavior =  std::make_unique<Separation>(this);
	pCohesionBehavior =  std::make_unique<Cohesion>(this);
	pVelMatchBehavior =  std::make_unique<VelocityMatch>(this);
	
	std::vector<BlendedSteering::WeightedBehavior> m_WeightedBehaviors{};
	pBlendedSteering = std::make_unique<BlendedSteering>(m_WeightedBehaviors);
	
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{pSeekBehavior.get(),SeekWeight}));
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{pWanderBehavior.get(),WanderWeight}));
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{pSeparationBehavior.get(),SeparationWeight}));
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{pCohesionBehavior.get(),CohesionWeight}));
	pBlendedSteering->AddBehaviour((BlendedSteering::WeightedBehavior{pVelMatchBehavior.get(),AlignmentWeight}));
	
	std::vector<ISteeringBehavior*> m_SteeringBehaviors{};
	
	m_SteeringBehaviors.push_back(pBlendedSteering.get());
	m_SteeringBehaviors.push_back(pEvadeBehavior.get());
	
	pPrioritySteering = std::make_unique<PrioritySteering>(m_SteeringBehaviors);
	
	// SPAWN THE FLOCK 
	for (int i = 0; i < m_FlockSize; i++)
	{
		FVector RandomPose = m_CurrentLevel->GetRamdomPosInVolumeBox();
		ASteeringAgent * SteeringAgent = m_pWorld->SpawnActor<ASteeringAgent>(m_AgentBlueprint, FVector{RandomPose.X, RandomPose.Y, 90}, FRotator::ZeroRotator);
		if (SteeringAgent)
		{
			SteeringAgent->SetActorTickEnabled(false);
			//SteeringAgent->SetSteeringBehavior(pPrioritySteering.get());
			Agents.Add(SteeringAgent);
		}
		
	}
	
	
#ifdef GAMEAI_USE_SPACE_PARTITIONING

	pPartitionedSpace = std::make_unique<CellSpace>(m_pWorld,100.f,100.f,10,10,2000);

	
	
#endif





	// TODO: initialize the flock and the memory pool
}

Flock::~Flock()
{
	// TODO: Cleanup any additional data
}

void Flock::Tick(float DeltaTime)
{
	for ( ASteeringAgent * Agent : Agents)
	{
		//Agent->Tick(DeltaTime);	
	}

	
	
	
	
	
	
	
	
	
	
	if (m_CurrentLevel)
	{
		m_CurrentLevel->SetTrimWorldSize(m_TrimWorldSize);
	}
	// TODO: update the flock
	// TODO: for every agent:
	 // TODO: register the neighbors for this agent (-> fill the memory pool with the neighbors for the currently evaluated agent)
	 // TODO: update the agent (-> the steeringbehaviors use the neighbors in the memory pool)
	 // TODO: trim the agent to the world
}

void Flock::RenderDebug()
{
	// TODO: Render all the agents in the flock
}

void Flock::ImGuiRender(ImVec2 const& WindowPos, ImVec2 const& WindowSize)
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
		ImGui::SliderFloat("Seek", &WanderWeight, 0.0f, 1.0f);

		ImGui::Spacing();

		ImGui::Text("Trim WorldSize");
		ImGui::SliderFloat("Trim", &m_TrimWorldSize, 0.0f, 4000.f);


		ImGui::End();
	}
#pragma endregion
#endif
}

void Flock::RenderNeighborhood()
{
	
	// TODO: Debugrender the neighbors for the first agent in the flock
}

#ifndef GAMEAI_USE_SPACE_PARTITIONING
void Flock::RegisterNeighbors(ASteeringAgent* const pAgent)
{
	// TODO: Implement
}
#endif

FVector2D Flock::GetAverageNeighborPos() const
{
	FVector2D avgPosition = FVector2D::ZeroVector;

	// TODO: Implement

	return avgPosition;
}

FVector2D Flock::GetAverageNeighborVelocity() const
{
	FVector2D avgVelocity = FVector2D::ZeroVector;

	// TODO: Implement

	return avgVelocity;
}

void Flock::SetTarget_Seek(FSteeringParams const& Target)
{
	// TODO: Implement
}

