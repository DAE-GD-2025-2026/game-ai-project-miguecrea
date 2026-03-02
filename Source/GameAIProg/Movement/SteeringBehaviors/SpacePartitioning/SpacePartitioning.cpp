#include "SpacePartitioning.h"

#include"../../../MACROS/MACRO.h"
// --- Cell ---
// ------------
Cell::Cell(float CenterX, float CenterY, float Width, float Height)
{
	//Half Width 
	m_Width = Width;
	//Half Height
	m_Height = Height;

	m_CenterPos.X = CenterX;
	m_CenterPos.Y = CenterY;

}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Cols, int Rows, int MaxEntities, FVector2D Center, float MeshZPos,float Radius)
	: pWorld{ pWorld }
	, SpaceWidth{ Width }  // hafl
	, SpaceHeight{ Height }
	, NrOfRows{ Rows }
	, NrOfCols{ Cols }
	, NrOfNeighbors{ 0 }
	, m_Center{ Center }
	, m_MeshZPos{ MeshZPos }
	,m_PrecomputedRadius{Radius}
{
	Neighbors.SetNum(MaxEntities);

	//Half Cell Width 
	CellWidth = Width / Cols;
	//Half Cell Height 
	CellHeight = Height / Rows;

	m_Cells.Reserve(NrOfRows * NrOfCols);
	m_StartPosOfTheSquare = { m_Center.X - (SpaceWidth - CellWidth) ,m_Center.Y - (SpaceHeight - CellHeight) ,m_MeshZPos };

	m_CornerOfScreen = { m_Center.X - (SpaceWidth - CellWidth) - (CellHeight),m_Center.Y - (SpaceHeight - CellHeight) - (CellWidth) ,m_MeshZPos };

	for (size_t row = 0; row < NrOfRows; row++)
	{
		for (int column = 0; column < NrOfCols; column++)
		{
			float X = m_StartPosOfTheSquare.X + column * (2 * CellWidth);
			float Y = m_StartPosOfTheSquare.Y + row * (2 * CellHeight);
			m_Cells.Add(Cell{ Y,X, CellHeight, CellWidth });
		}
	}

	m_LastCellIndex = m_Cells.Num() - 1;
}


void CellSpace::AddAgent(ASteeringAgent& Agent)
{
	int CellIndex = PositionToIndex(Agent.GetPosition());
	m_Cells[CellIndex].Agents.Add(&Agent);
}

void CellSpace::UpdateAgentCell(ASteeringAgent& Agent)
{

	int NewIndex = PositionToIndex(Agent.GetPosition());
	int OldIndex = PositionToIndex(Agent.GetPreviousFramePos());

	if (NewIndex == OldIndex) return;

	m_Cells[OldIndex].Agents.RemoveSingleSwap(&Agent);
	m_Cells[NewIndex].Agents.Add(&Agent);

}

void CellSpace::RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius, bool IsFirstOne)
{
	FVector2D  centerPos = Agent.GetPosition();

	FVector2D bottomLeft{ centerPos.X - QueryRadius,centerPos.Y - QueryRadius };
	FVector2D topRight{ centerPos.X + QueryRadius, centerPos.Y + QueryRadius };

	if (IsFirstOne)
	{
		FVector Pos{ centerPos.X, centerPos.Y,m_MeshZPos };
		DRAW_CIRCLE(pWorld, Pos, QueryRadius, FColor::Black, 20);
	}


	int bottomLeftRow = PositionToIndexRow(bottomLeft);
	int bottomLeftCol = PositionToIndexColumn(bottomLeft);

	int topRightRow = PositionToIndexRow(topRight);
	int topRightCol = PositionToIndexColumn(topRight);

	TArray<int32> Indices;


	for (int32 row = bottomLeftRow; row <= topRightRow; ++row)
	{
		for (int32 col = bottomLeftCol; col <= topRightCol; ++col)
		{
			int32 WrappedRow = (row + NrOfRows) % NrOfRows;
			int32 WrappedCol = (col + NrOfCols) % NrOfCols;

			int32 Index = WrappedRow * NrOfCols + WrappedCol;
			Indices.Add(Index);
		}
	}

	NrOfNeighbors = 0;

	for (int i = 0; i < Indices.Num(); i++)
	{
		//dont copy the cell 
		const Cell & cell = m_Cells[Indices[i]];
		for (ASteeringAgent * cellAgent : cell.Agents)
		{
			FVector2D ToAgent = cellAgent->GetPosition() - Agent.GetPosition();

			float distanceToNeigbord = ToAgent.SizeSquared();
			if (distanceToNeigbord < m_PrecomputedRadius)
			{
				Neighbors[NrOfNeighbors] = cellAgent;
				NrOfNeighbors++;
				if (IsFirstOne)
				{
					FVector2D PosOfCellAgent = { cellAgent->GetPosition() };
					FVector RenderPos{ PosOfCellAgent.X,PosOfCellAgent.Y,cellAgent->GetMeshZPosition() };
					DRAW_CIRCLE(pWorld, RenderPos, 60.f, FColor::Blue, 10);

				}

			}

		}


		if (IsFirstOne)
		{
			FVector CellPos{ cell.m_CenterPos.X, cell.m_CenterPos.Y,m_MeshZPos };
			FVector Extent = FVector(cell.m_Width, cell.m_Width, 1.f);
			auto color = FColor::Yellow;
			color.A = 40.f;
			DrawDebugSolidBox(pWorld, CellPos, Extent, color, false, 0.0f, 39);
		}







	}




}




void CellSpace::EmptyCells()
{
//	for (Cell& c : m_Cells)
//		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	FVector BottomLeftCorner{ m_CornerOfScreen.X,m_CornerOfScreen.Y ,m_MeshZPos };

	DRAW_CIRCLE(pWorld, BottomLeftCorner, 40.f, FColor::Black, 3);

	for (Cell singlecell : m_Cells)
	{
		FVector2D CenterPos = { singlecell.m_CenterPos };
		FVector singlecellPos{ CenterPos.X,CenterPos.Y,m_MeshZPos };
		DRAW_BOX(pWorld, singlecellPos, singlecell.m_Width, singlecell.m_Height, FColor::Emerald);

	}
}

void CellSpace::RenderFirstCellAgent()
{
	auto Agents = m_Cells[0].Agents;

	for (const ASteeringAgent* agent : Agents)
	{
		FVector Pos{ agent->GetPosition().X,agent->GetPosition().Y,m_MeshZPos };
		DRAW_CIRCLE(pWorld, Pos, 60.f, FColor::Red, 20);
	}
}

int  CellSpace::PositionToIndex(FVector2D const& Pos) const
{
	double YPos{ FMath::Abs(m_CornerOfScreen.Y - Pos.Y) };
	double XPos{ FMath::Abs(m_CornerOfScreen.X - Pos.X) };
	int Column = YPos / (CellHeight * 2);
	int row = XPos / (CellWidth * 2);
	int index{ Column + row * NrOfCols };
	return FMath::Clamp(index, 0, m_LastCellIndex);
}

int CellSpace::PositionToIndexRow(FVector2D const& Pos) const
{
	double XPos{ FMath::Abs(m_CornerOfScreen.X - Pos.X) };
	int row = XPos / (CellWidth * 2);
	return FMath::Clamp(row, 0, NrOfRows - 1);
}

int CellSpace::PositionToIndexColumn(FVector2D const& Pos) const
{

	double YPos{ FMath::Abs(m_CornerOfScreen.Y - Pos.Y) };
	int Column = YPos / (CellHeight * 2);
	return FMath::Clamp(Column, 0, NrOfCols - 1 );
}
