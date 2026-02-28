#include "SpacePartitioning.h"

#include"../../../MACROS/MACRO.h"
// --- Cell ---
// ------------
Cell::Cell(float CenterX,float CenterY,float Width, float Height)
{
	m_Width = Width;
	m_Height = Height;

	m_CenterPos.X = CenterX;
	m_CenterPos.Y = CenterY;

	m_HalfWidth = m_Width * 0.5f;
	m_HalfHeight = m_Height * 0.5f;

}

std::vector<FVector2D> Cell::GetRectPoints() const
{

	const float left = m_CenterPos.X -  m_Width;
	const float bottom = m_CenterPos.Y - m_HalfHeight;
	const float width =  m_Width;
	const float height = m_Height;



	std::vector<FVector2D> rectPoints =
	{
		{ left , bottom }
	/*	{ left , bottom + height  },
		{ left + width , bottom + height },
		{ left + width , bottom  },*/
	};

	return rectPoints;
}

// --- Partitioned Space ---
// -------------------------
CellSpace::CellSpace(UWorld* pWorld, float Width, float Height, int Cols, int Rows, int MaxEntities, FVector2D Center,float MeshZPos)
	: pWorld{pWorld}
	, SpaceWidth{Width}
	, SpaceHeight{Height}
	, NrOfRows{Rows}
	, NrOfCols{Cols}
	, NrOfNeighbors{0}
	,m_Center{Center}
	,m_MeshZPos{MeshZPos}
{
	Neighbors.SetNum(MaxEntities);
	
	CellWidth = Width / Cols;
	CellHeight = Height / Rows;

	m_Cells.Reserve(NrOfRows * NrOfCols);
	m_StartPosOfTheSquare = { m_Center.X - (SpaceWidth - CellWidth) ,m_Center.Y - (SpaceHeight - CellHeight) ,m_MeshZPos};


	m_CornerOfScreen = { m_Center.X - (SpaceWidth - CellWidth) - (CellHeight),m_Center.Y - (SpaceHeight - CellHeight) - (CellWidth) ,m_MeshZPos};



	for (size_t row = 0; row < NrOfRows; row++)
	{
		for (int column = 0; column < NrOfCols; column++)
		{
			float X = m_StartPosOfTheSquare.X + column * (2 * CellWidth);
			float Y = m_StartPosOfTheSquare.Y + row *  (2 * CellHeight);

			m_Cells.Add(Cell{Y,X, CellHeight, CellWidth});
		}
	}
}


void CellSpace::AddAgent(ASteeringAgent & Agent)
{
	// get actor - Bottom Left Corner


	//give us the corect pos based on the 
	//center point - Half 
	FVector BottonLeft{};

	
//	BottonLeft


	// use PosTo Index if is same as cell index 
	// add 
	// 
	// TODO Add the agent to the correct cell
}

void CellSpace::UpdateAgentCell(ASteeringAgent & Agent,const FVector2D & OldPos)
{
	//TODO Check if the agent needs to be moved to another cell.
	//TODO Use the calculated index for oldPos and currentPos for this
}

void CellSpace::RegisterNeighbors(ASteeringAgent & Agent,float QueryRadius)
{


	//if rect overlap 

	//GetCells();
	// 
	// 
	// 
	// 
	// TODO Register the neighbors for the provided agent
	// TODO Only check the cells that are within the radius of the neighborhood
}

void CellSpace::EmptyCells()
{
	for (Cell& c : m_Cells)
		c.Agents.clear();
}

void CellSpace::RenderCells() const
{
	FVector BottomLeftCorner{ m_CornerOfScreen.X,m_CornerOfScreen.Y ,m_MeshZPos};

	DRAW_CIRCLE(pWorld, BottomLeftCorner, 40.f, FColor::Black, 3);



	for (Cell singlecell : m_Cells)
	{
		FVector2D CenterPos = { singlecell.m_CenterPos };
		FVector singlecellPos{ CenterPos.X,CenterPos.Y,m_MeshZPos};
	    DRAW_BOX(pWorld, singlecellPos,singlecell.m_Width,singlecell.m_Height,FColor::Emerald);
		DRAW_CIRCLE(pWorld, singlecellPos, 20.f, FColor::Red, 3);
		
		//Render rect Poits 
		auto vectorOfPoints = singlecell.GetRectPoints();
		for (size_t i = 0; i < vectorOfPoints.size(); i++)
		{
			FVector2D point = vectorOfPoints[i];
			FVector Corners{ point.X,point.Y,m_MeshZPos };

		   DRAW_CIRCLE(pWorld, Corners, 20.f, FColor::Yellow, 3);

		}

	}
}

int CellSpace::PositionToIndex(FVector2D const & Pos) const
{



	int Column{ FMath::FloorToInt(FMath::Abs(m_CornerOfScreen.Y - Pos.Y))};
	int Row{ FMath::FloorToInt(FMath::Abs(m_CornerOfScreen.X - Pos.X))};
	
	UE_LOG(LogTemp,Warning,TEXT("%f"), Column)

	float XPos{};
	float YPos{};

	// TODO Calculate the index of the cell based on the position
	return 0;
}

bool CellSpace::DoRectsOverlap(FRect const & RectA, FRect const & RectB)
{
	// Check if the rectangles are separated on either axis
	if (RectA.Max.X < RectB.Min.X || RectA.Min.X > RectB.Max.X) return false;
	if (RectA.Max.Y < RectB.Min.Y || RectA.Min.Y > RectB.Max.Y) return false;
    
	// If they are not separated, they must overlap
	return true;
}