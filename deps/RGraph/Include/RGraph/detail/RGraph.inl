#pragma once

namespace RGraph 
{
	inline void Node::SetAttr(std::string key, std::string val) noexcept
	{
		attr_list[key] = val;
	}

	inline std::string Node::GetName() const noexcept
	{
		return name;
	}

	inline void Edge::SetAttr(std::string key, std::string val) noexcept
	{
		attr_list[key] = val;
	}
	inline std::string Edge::GetName() const noexcept
	{
		return name;
	}

	inline void SubGraph::AddNode(Node node) noexcept
	{
		nodevec.push_back(node);
	}
	inline void SubGraph::AddEdge(Edge edge) noexcept
	{
		edgevec.push_back(edge);
	}

	inline std::string Graph::GetTxt() const noexcept
	{
		std::stringstream txt;
		txt << "digraph G ";
		txt << SubGraph::GetTxt();
		return txt.str();
	}
}