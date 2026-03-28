#include "NavGraph.h"

#include "NavGraphNode.h"
#include<algorithm>

GameAI::NavGraph::NavGraph(std::unique_ptr<TriPolygon>&& NavPoly)
	: Graph{ false }
	, pNavPoly{ std::move(NavPoly) }
{
	CreateNavigationGraph();
}

GameAI::NavGraph::NavGraph(const NavGraph& Other)
	: Graph(false)
{
	m_Dictionary = Other.m_Dictionary;

	edgeToNode = Other.edgeToNode;

	nodeToEdgeEndpoints = Other.nodeToEdgeEndpoints;


	pNavPoly = std::make_unique<TriPolygon>(*Other.pNavPoly);

	Nodes.reserve(Other.Nodes.size());

	for (std::unique_ptr<Node> const& OtherNode : Other.Nodes)
	{
		Node* node = OtherNode.get();
		NavGraphNode* navNode = dynamic_cast<NavGraphNode*>(node);

		if (navNode != nullptr)
		{
			Nodes.push_back(std::make_unique<NavGraphNode>(*navNode));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Node at index is not a NavGraphNode!"));
		}

	}

	Connections.reserve(Other.Connections.size());

	for (std::unique_ptr<Connection> const& OtherConnection : Other.Connections)
	{
		Connections.push_back(std::make_unique<Connection>(*OtherConnection.get()));
	}
}

std::unique_ptr<GameAI::NavGraph> GameAI::NavGraph::Clone() const
{
	return std::make_unique<NavGraph>(*this);
}

int GameAI::NavGraph::GetNodeIdFromEdgeIndex(int EdgeIdx) const
{
	//from Parent class Nodes
	if (EdgeIdx >= 0)
	{
		for (auto const& pNode : Nodes)
		{
			if (reinterpret_cast<NavGraphNode*>(pNode.get())->GetEdgeIdx() == EdgeIdx)
			{
				return pNode->GetId();
			}
		}
	}

	return Graphs::InvalidNodeId;
}

std::optional<int> GameAI::NavGraph::GetClosestNodeIndex(FVector2D const & DesiredPosition, NavGraph* const pNavGraph)
{
	FVector2D OutPosition;
	const TriPolygon::Triangle* closestTriangle = pNavPoly->GetClosestTriangleToPosition(DesiredPosition, OutPosition);

	if (closestTriangle == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT(" Triangle Is  Null"))
			return std::nullopt;
	}


	std::vector<Node*> triangleNodes;
	for (const TriPolygon::Edge & edge : closestTriangle->GetEdges())
	{
		uint64 key = MakeEdgeKey(edge.EdgeIndices[0], edge.EdgeIndices[1]);
		auto it = edgeToNode.find(key);
		if (it != edgeToNode.end())
		{
			triangleNodes.push_back(it->second);
		}
	}

	// ClosestTriangle Has No Nodes 
	if (triangleNodes.empty()) return std::nullopt;

	int NodeIndex = pNavGraph->AddNode(std::make_unique<Node>(DesiredPosition));

	
		for (Node* triNode : triangleNodes)
		{
			float dist = FVector2D::Distance(DesiredPosition, triNode->GetPosition());

			pNavGraph->AddConnection(NodeIndex, triNode->GetId());
			if (auto * conn = pNavGraph->FindConnection(NodeIndex, triNode->GetId()))
			{
				conn->SetWeight(dist);
			}

			pNavGraph->AddConnection(triNode->GetId(), NodeIndex);
			if (auto* conn = pNavGraph->FindConnection(triNode->GetId(), NodeIndex))
			{
				conn->SetWeight(dist);
			}
		}

	


	return NodeIndex;


}

void GameAI::NavGraph::CreateNavigationGraph()
{


	for (const TriPolygon::Triangle& triangle : pNavPoly->GetTriangles())
	{
		for (const TriPolygon::Edge& edge : triangle.GetEdges())
		{
			uint64 key = MakeEdgeKey(edge.EdgeIndices[0], edge.EdgeIndices[1]);

			auto it = m_Dictionary.find(key);

			if (it == m_Dictionary.end())
			{
				//edge to traingle 
				m_Dictionary[key] = triangle;
			}
			else
			{
				m_Dictionary.erase(it);



				FVector vertex1 = edge.GetP1(*pNavPoly.get());
				FVector vertex2 = edge.GetP2(*pNavPoly.get());
				FVector midpoint = (vertex1 + vertex2) * 0.5f;

				std::optional<int> edgeIndex = pNavPoly->FindEdgeIndex(edge);

				if (edgeIndex.has_value())
				{
					int nodeIndex = AddNode(std::make_unique<NavGraphNode>(FVector2D{ midpoint.X,midpoint.Y }, edgeIndex.value()));

					edgeToNode[key] = GetNode(nodeIndex).get(); //edge to node 

					nodeToEdgeEndpoints[nodeIndex] = { FVector2D{vertex1},FVector2D{vertex2}};

				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Edge has no Value"))
				}

			}
		}
	}

	// Second pass: connect nodes within each triangle
	for (const TriPolygon::Triangle& triangle : pNavPoly->GetTriangles())
	{
		std::vector<Node*> triangleNodes;

		for (const TriPolygon::Edge& edge : triangle.GetEdges())
		{
			uint64 key = MakeEdgeKey(edge.EdgeIndices[0], edge.EdgeIndices[1]);
			auto it = edgeToNode.find(key);

			if (it != edgeToNode.end())
			{
				triangleNodes.push_back(it->second);
			}
		}


		// we have all the nodes  of a triangle connect them to other nodes 
		for (size_t i = 0; i < triangleNodes.size(); ++i)
		{
			for (size_t j = i + 1; j < triangleNodes.size(); ++j)
			{
				float distance = FVector2D::Distance(
					triangleNodes[i]->GetPosition(),
					triangleNodes[j]->GetPosition()
				);

				std::unique_ptr<Connection> conection = std::make_unique<Connection>(triangleNodes[i]->GetId(), triangleNodes[j]->GetId());
				conection->SetWeight(distance);
				AddConnection(std::move(conection));

				std::unique_ptr< Connection> conection2 = std::make_unique<Connection>(triangleNodes[j]->GetId(), triangleNodes[i]->GetId());
				conection2->SetWeight(distance);
				AddConnection(std::move(conection2));

			}
		}
	}


}
