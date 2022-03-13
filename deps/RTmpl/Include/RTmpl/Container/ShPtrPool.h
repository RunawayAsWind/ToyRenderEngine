#pragma once
#include <memory>
#include <queue>
#include <vector>

namespace RTmpl
{
	template<typename T>
	class ShPtrPool
	{
		public:
			ShPtrPool() { Extend(100); }
			
			virtual ~ShPtrPool()
			{
				for (auto& ptr : totalPtr){
					delete ptr;
				}
				totalPtr.clear();
			}

			std::shared_ptr<T> Get()
			{
				T* ptr = nullptr;
				if (freeQueue.empty()){
					Extend(50);
				}

				ptr = freeQueue.front();
				freeQueue.pop();
				return std::shared_ptr<T>(ptr,[&](T* p){freeQueue.push(p);});
			}

			size_t GetTotalNum() const noexcept
			{
				return totalPtr.size();
			}

			size_t GetFreeNum() const noexcept
			{
				return freeQueue.size();
			}

		private:
			std::queue<T*> freeQueue;
			std::vector<T*> totalPtr;
			void Extend(size_t num)
			{
				for (size_t i = 0; i < num; ++i)
				{
					T* ptr = new T();
					freeQueue.push(ptr);
					totalPtr.push_back(ptr);
				}
			}
	};

}