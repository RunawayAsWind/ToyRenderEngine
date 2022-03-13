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
		std::shared_ptr<BlockAllocation> Get(int num);//�ӳ��л������
		void FreeBlock();							  //�����������飬�ὫwaitFree��������������blockVec
		void ShowInfo();
		
		void Extend(int addBlockNum);                 //����Block
		void AddBlock(std::pair<int, int> p);         //���ӿ������飨�����ã�
		void BlockWaitFree(std::pair<int, int> p);    //������Դ��ʱ���Ƚ���������push������ն���waitFree
		void BlockDirectFree(std::pair<int, int> p);  
	private:
		std::string name;
		int blockSize;
		int blockNum = 0;
		std::vector<std::pair<int, int>> blockVec;    //���õ�����
		std::vector<std::pair<int, int>> waitFree;    //�ȴ��ͷŵ�����
		bool MergeBlock(std::pair<int, int> p);       //�ϲ��������飨������ӿ�������ʹ�ã�
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