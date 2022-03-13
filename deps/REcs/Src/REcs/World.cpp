#include<RGraph/RGraph.h>
#include "../Include/REcs/World.h"

namespace REcs {

	void AddChunkUniFunc(std::shared_ptr<tf::Taskflow> taskFlow, std::vector<Archetype*> archtypeVec, std::function<void(Archetype*, size_t)>&& archUniFunc_Func)
	{
		for (auto archtypePtr : archtypeVec)
		{
			int totalnum = 0;
			
			while (totalnum != archtypePtr->archTotalNum)
			{
				int lastnum = totalnum;
				totalnum += archtypePtr->archNumPerChunk;
				
				if (totalnum > archtypePtr->archTotalNum)
				{
					totalnum = archtypePtr->archTotalNum;
				}

				std::function<void()> chunkUniFunc = std::function
				(
					[archtypePtr = archtypePtr, lastnum = lastnum, totalnum = totalnum, archUniFunc_Func = archUniFunc_Func]()
					{
						for (int i = lastnum; i < totalnum; i++)
						{
							archUniFunc_Func(archtypePtr, i);
						}
					}
				);

				taskFlow->emplace(chunkUniFunc);
			}
		}
	}

	void World::Update()
	{
		for (auto [sysName, archUniFunc] : sysm.GetFuncMap())
		{
			//1.对于每个sysFunc，获取能执行的所有Archetype对象
			std::vector<Archetype*> archtypeVec;
			for (auto [archtypeName, archetypePtr] : etm.h2a)
			{
				if (IfHasSysNeededCmpts(archUniFunc.GetCmptMetaVec(), archetypePtr))
				{
					archtypeVec.push_back(archetypePtr);
				}
			}


			if (archtypeVec.size() != 0)
			{
				auto taskFlow = taskFlowPool.Get();
				taskFlow->clear();

				//2.将archUniFunc函数转化成ChunkUniFunc,再把能执行的所有Archetype对象的ChunkUniFunc装填到一个taskflow(AllChunkFuncs)中
				AddChunkUniFunc(taskFlow, archtypeVec, std::forward<std::function<void(Archetype*, size_t)>>(archUniFunc.GetFunc()));
				//3.sysName+archtypeVec+taskFlow(AllChunkFuncs) 来表示一个完整的sys的操作
				
				if (taskFlow->num_nodes() > 0)//如果虽然有ArchType但内部没有任何数据，则不加入Sys执行，如果没有这句，出现nodeNum为0的jobName会卡死（可看下方输出进行调试）
				{
					sysJobs.emplace(sysName, SysJob(sysName, archtypeVec, taskFlow));
				}
			}
		}

#ifdef REcs_World_Update_Debug
		std::cout << std::endl;
		std::cout << "----------REcs: World Update(Start)---------" << std::endl;
		std::cout << std::endl;
		for (auto [jobName, sysJob] : sysJobs)
		{
			std::cout << jobName << " ";
			for (auto archtypePtr : sysJob.archtypeVec)
			{
				std::cout << archtypePtr->name << " ";
			}
			std::cout <<"    nodeNum:"<< sysJob.taskFlow->num_nodes() << " ";
			std::cout << std::endl;
		}
		std::cout << std::endl;
		std::cout << "----------REcs: World Update(End)---------" << std::endl;
		std::cout << std::endl;
#endif

		//4.把每个sys的taskFlow(AllChunkFuncs)合并到一个mergeFlow中（此时会转变成tf::Task格式），用tasksDic记录每个tf::Task(AllChunkFuncs)，用于后面的顺序设计
		for (auto [jobName, sysJob] : sysJobs)
		{
			tasksDic[jobName] = mergeFlow.composed_of(*(sysJob.taskFlow));
		}

		//5.对每个sys(tf::Task(AllChunkFuncs))进行顺序设计
		for (auto [sys1, sys2] : sysm.GetFuncOrder())
		{
			if (tasksDic.find(sys1) != tasksDic.end() && tasksDic.find(sys2) != tasksDic.end())
			{
				tasksDic[sys1].precede(tasksDic[sys2]);
			}
		}

		//6.执行总Flow
		executor.run(mergeFlow).wait();

		//7.清理上次的数据（必须清理了才能重复调用Update)
		tasksDic.clear();
		mergeFlow.clear();
		sysJobs.clear();

	}

	bool World::IfHasSysNeededCmpts(std::vector<std::string> cmptNameVec, Archetype* archetype)
	{

		for (auto name : cmptNameVec)
		{
			if (archetype->cmptsRef.cmptDic.find(name) == archetype->cmptsRef.cmptDic.end())
			{
				return false;
			}
		}
		return true;
	}

	void World::ShowGraph(std::string dirPath)
	{
		RGraph::Graph g("WorldGraph");
		for (auto [sys1, sys2] : sysm.GetFuncOrder())
		{
			std::cout << sys1 << "->" << sys2 << std::endl;
			RGraph::Edge e(sys1 +"->"+ sys2);
			g.AddEdge(e);
		}
		std::cout << std::endl;
		RGraph::ShowGraph(g, dirPath);
	}
}