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
			//1.����ÿ��sysFunc����ȡ��ִ�е�����Archetype����
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

				//2.��archUniFunc����ת����ChunkUniFunc,�ٰ���ִ�е�����Archetype�����ChunkUniFuncװ�һ��taskflow(AllChunkFuncs)��
				AddChunkUniFunc(taskFlow, archtypeVec, std::forward<std::function<void(Archetype*, size_t)>>(archUniFunc.GetFunc()));
				//3.sysName+archtypeVec+taskFlow(AllChunkFuncs) ����ʾһ��������sys�Ĳ���
				
				if (taskFlow->num_nodes() > 0)//�����Ȼ��ArchType���ڲ�û���κ����ݣ��򲻼���Sysִ�У����û����䣬����nodeNumΪ0��jobName�Ῠ�����ɿ��·�������е��ԣ�
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

		//4.��ÿ��sys��taskFlow(AllChunkFuncs)�ϲ���һ��mergeFlow�У���ʱ��ת���tf::Task��ʽ������tasksDic��¼ÿ��tf::Task(AllChunkFuncs)�����ں����˳�����
		for (auto [jobName, sysJob] : sysJobs)
		{
			tasksDic[jobName] = mergeFlow.composed_of(*(sysJob.taskFlow));
		}

		//5.��ÿ��sys(tf::Task(AllChunkFuncs))����˳�����
		for (auto [sys1, sys2] : sysm.GetFuncOrder())
		{
			if (tasksDic.find(sys1) != tasksDic.end() && tasksDic.find(sys2) != tasksDic.end())
			{
				tasksDic[sys1].precede(tasksDic[sys2]);
			}
		}

		//6.ִ����Flow
		executor.run(mergeFlow).wait();

		//7.�����ϴε����ݣ����������˲����ظ�����Update)
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