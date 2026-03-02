

#include "Level_Flocking.h"


// Sets default values
ALevel_Flocking::ALevel_Flocking()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALevel_Flocking::SetTrimWorldSize(float newSize)
{
	TrimWorld->SetTrimWorldSize(newSize);
}

// Called when the game starts or when spawned
void ALevel_Flocking::BeginPlay()
{
	Super::BeginPlay();

	TrimWorld->bShouldTrimWorld = true;
	pAgentToEvade = GetWorld()->SpawnActor<ASteeringAgent>(m_FollowAgentBlueprint, FVector{ 0,0,90 }, FRotator::ZeroRotator);

	m_SeekBehavior = MakeUnique<Seek>();
	if (pAgentToEvade)
	{
		pAgentToEvade->SetSteeringBehavior(m_SeekBehavior.Get());
	}

	TrimWorld->SetTrimWorldSize(1800);
	pFlock = TUniquePtr<Flock>(new Flock(GetWorld(),SteeringAgentClass,pAgentToEvade->GetMeshZPosition(), this, FlockSize, TrimWorld->GetTrimWorldSize(), pAgentToEvade, true, TrimWorld->GetCenterPoint()));
}

// Called every frame
void ALevel_Flocking::Tick(float DeltaTime)
{

	pFlock->ImGuiRender(WindowPos, WindowSize);
	pFlock->Tick(DeltaTime);
	pFlock->RenderDebug();

	//target would be better off Set OnMouse Click
	if (m_SeekBehavior)
	{
		m_SeekBehavior->SetTarget(MouseTarget);
	}

	if (bUseMouseTarget)
    pFlock->SetTarget_Seek(MouseTarget);
}

