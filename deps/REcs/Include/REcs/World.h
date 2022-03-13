#pragma once
#include <RTmpl/Container/ShPtrPool.h>
#include "EntityManager.h"
#include "SysManager.h"
#include "../../3rdParty/taskflow/taskflow.hpp"


namespace REcs {

	struct SysJob
	{
	public:
		std::string sysName;
		std::vector<Archetype*> archtypeVec;
		std::shared_ptr<tf::Taskflow> taskFlow;
		SysJob(std::string sysName, std::vector<Archetype*> archtypeVec, std::shared_ptr<tf::Taskflow> taskFlow) :sysName(sysName), archtypeVec(archtypeVec), taskFlow(taskFlow){}
	};
	
	void AddChunkUniFunc(std::shared_ptr<tf::Taskflow> job, std::vector<Archetype*> archtypeVec, std::function<void(Archetype*, size_t)>&& archUniFunc_Func);

	class World
	{
	public:
		EntityManager etm;
		SysManager sysm;
		void Update();
		bool IfHasSysNeededCmpts(std::vector<std::string> cmptNameVec, Archetype* archetype);
		void ShowGraph(std::string dirPath);
		World() {};
		~World() {};
	private:
		//lambda < ArchUnitFunc < ChunkJob < SysJob < mergeJob
		tf::Executor executor;
		tf::Taskflow mergeFlow;
		std::unordered_map<std::string, tf::Task> tasksDic;
		std::unordered_map<std::string, SysJob> sysJobs;
		RTmpl::ShPtrPool<tf::Taskflow> taskFlowPool;
	};
}