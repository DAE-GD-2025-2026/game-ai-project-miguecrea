/*=============================================================================*/
// SpacePartitioning.h: Contains Cell and Cellspace which are used to partition a space in segments.
// Cells contain pointers to all the agents within.
// These are used to avoid unnecessary distance comparisons to agents that are far away.

// Heavily based on chapter 3 of "Programming Game AI by Example" - Mat Buckland
/*=============================================================================*/

#pragma once
#include <list>
#include <vector>
#include <iterator>

#include "Debug/ReporterGraph.h"
#include "Movement/SteeringBehaviors/SteeringAgent.h"

// --- Cell ---
// ------------
struct Cell final
{
	Cell(float Left, float Bottom, float Width, float Height);

	// all the agents currently in this cell
	TArray<ASteeringAgent*> Agents;
	FVector2D m_CenterPos;
	float m_Width;
	float m_Height;
	float m_HalfWidth;
	float m_HalfHeight;

};

// --- Partitioned Space ---
// -------------------------
class CellSpace final
{
public:
	CellSpace(UWorld * pWorld, float Width, float Height, int Cols, int Rows, int MaxEntities,FVector2D Center,float MeshZPos,float Radius);

	void AddAgent(ASteeringAgent& Agent);
	void UpdateAgentCell(ASteeringAgent& Agent);

	void RegisterNeighbors(ASteeringAgent& Agent, float QueryRadius,bool IsFirstOne = false);
	const TArray<ASteeringAgent*> & GetNeighbors() const { return Neighbors; }
	int GetNrOfNeighbors() const { return NrOfNeighbors; }

	//empties the cells of entities
	void EmptyCells();
	void RenderCells()const;

	void RenderFirstCellAgent();

	int PositionToIndex(FVector2D const & Pos) const;
	int PositionToIndexRow(FVector2D const & Pos) const;
	int PositionToIndexColumn(FVector2D const & Pos) const;
private:
	UWorld * pWorld{};
	float m_MeshZPos;

	FVector m_StartPosOfTheSquare{};
	FVector m_CornerOfScreen{};

	

	FVector2D m_Center;
	// Cells and properties
	TArray<Cell> m_Cells;
	FVector2D CellOrigin{};
     

	int m_LastCellIndex{};


	float m_PrecomputedRadius{};
	
	float SpaceWidth;
	float SpaceHeight;

	int NrOfRows;
	int NrOfCols;

	float CellWidth;
	float CellHeight;

	// Members to avoid memory allocation on every frame
	TArray<ASteeringAgent*> Neighbors;
	int NrOfNeighbors;
};
