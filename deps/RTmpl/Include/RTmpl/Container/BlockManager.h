#pragma once
#include<vector>
#include<string>
#include<memory>

namespace RTmpl
{
	class BlockAllocation;

	class BlockManager
	{
	public:
		BlockManager(std::string name, int blockSize) :name(name), blockSize(blockSize) {}
		std::shared_ptr<BlockAllocation> Get(int num);//从池中获得区块
		void FreeBlock();							  //回收所有区块，会将waitFree中所有区块推入blockVec
		void ShowInfo();
		
		void Extend(int addBlockNum);                 //增加Block
		void AddBlock(std::pair<int, int> p);         //增加可用区块（回收用）
		void BlockWaitFree(std::pair<int, int> p);    //回收资源的时候先将回收区间push入待回收队列waitFree
		void BlockDirectFree(std::pair<int, int> p);  
	private:
		std::string name;
		int blockSize;
		int blockNum = 0;
		std::vector<std::pair<int, int>> blockVec;    //可用的区块
		std::vector<std::pair<int, int>> waitFree;    //等待释放的区块
		bool MergeBlock(std::pair<int, int> p);       //合并可用区块（配合增加可用区块使用）
	};

	class BlockAllocation
	{
	private:
		BlockManager* resFather;
	public:
		std::pair<int, int> range;
		BlockAllocation(BlockManager* resFather, std::pair<int, int>range) :resFather(resFather), range(range) {}
		~BlockAllocation();
	};
}