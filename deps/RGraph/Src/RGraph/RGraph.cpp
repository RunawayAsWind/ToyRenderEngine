#pragma once
#include "../Include/RGraph/RGraph.h"

namespace RGraph
{
	std::string Node::GetTxt() const noexcept
	{
		std::stringstream txt;
		txt << name;
		if (attr_list.size() > 0)
		{
			txt << "[";
			for (const auto& [k, v] : attr_list)
			{
				txt << k << "=" << v << ";";
			}
			txt << "]";
		}
		return txt.str();
	}
	
	std::string Edge::GetTxt() const noexcept
	{
		std::stringstream txt;
		txt << name;
		if (attr_list.size() > 0)
		{
			txt << "[";
			for (const auto& [k, v] : attr_list)
			{
				txt << k << "=" << v << ";";
			}
			txt << "]";
		}
		return txt.str();
	}

	std::string SubGraph::GetTxt() const noexcept
	{
		std::stringstream txt;
		if (nodevec.size() > 0 || edgevec.size() > 0)
		{
			txt << "{\n";
			for (auto node : nodevec)
			{
				txt << node.GetName() << ";\n";
			}
			for (auto edge : edgevec)
			{
				txt << edge.GetName() << ";\n";
			}
			txt << "}";
		}
		return txt.str();
	}

	void ShowGraph(Graph g)
	{
		char buffer[100];
		std::string nowurl(_getcwd(buffer, 100));
		std::string dirpath = nowurl + "\\GenFile\\RGraph";
		
		std::string command;
		command = "mkdir " + dirpath;
		system(command.c_str());


		std::string dotpath = nowurl +"\\GenFile\\RGraph\\" + g.name + ".dot";
		std::string imgpath = nowurl +"\\GenFile\\RGraph\\" + g.name + ".png";

		std::ofstream out(dotpath);
		out << g.GetTxt();
		out.close();

		std::string command_rd = "dot -Tpng " + dotpath + " -o " + imgpath;
		system(command_rd.c_str());
	}

	void ShowGraph(Graph g, std::string dirPath)
	{
		std::string command;
		command = "mkdir " + dirPath;
		system(command.c_str());

		std::string dotpath = dirPath +"\\"+ g.name + ".dot";
		std::string imgpath = dirPath +"\\"+ g.name + ".png";

		std::ofstream out(dotpath);
		out << g.GetTxt();
		out.close();

		std::string command_rd = "dot -Tpng " + dotpath + " -o " + imgpath;
		system(command_rd.c_str());
	}
}