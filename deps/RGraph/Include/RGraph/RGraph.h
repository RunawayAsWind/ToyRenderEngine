#pragma once
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>
#include <direct.h>

namespace RGraph {

	class Node{
	private:
		std::string name;
		std::map<std::string, std::string> attr_list;
	public:
		Node(std::string name) :name(name) {}
		void SetAttr(std::string key, std::string val) noexcept;
		std::string GetTxt() const noexcept;
		std::string GetName() const noexcept;
	};


	class Edge{
	private:
		std::string name;
		std::map<std::string, std::string> attr_list;
	public:
		Edge(std::string name) :name(name){}
		void SetAttr(std::string key, std::string val) noexcept;
		std::string GetTxt() const noexcept;
		std::string GetName() const noexcept;
	};



	class SubGraph
	{
	private:
		std::vector<Node> nodevec;
		std::vector<Edge> edgevec;
	public:
		void AddNode(Node node) noexcept;
		void AddEdge(Edge edge) noexcept;
		std::string GetTxt() const noexcept;
	};

	class Graph :public SubGraph
	{
	public:
		std::string name;
		Graph(std::string name) :name(name){}
		std::string GetTxt() const noexcept;
	};

	void ShowGraph(Graph g);
	void ShowGraph(Graph g, std::string dirPath);
}

#include "./detail/RGraph.inl"