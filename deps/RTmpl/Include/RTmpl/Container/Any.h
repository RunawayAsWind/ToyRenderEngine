#pragma once
#include<string>
#include <memory>
#include "../../../3rdParty/nameof/nameof.hpp"
namespace RTmpl 
{
	class Any 
	{
	public:
		Any() :baseptr(nullptr) {};

		inline std::string GetName() const noexcept
		{
			return baseptr->GetName();
		}

		template<typename T>
		void SetValue(std::shared_ptr<T> t)
		{
			if (baseptr)
			{
				delete baseptr;
			}
			baseptr = new Data<T>(t);
		}

		template<typename T>
		inline std::shared_ptr<T> GetValue() const
		{
			Data<T>* pdata = dynamic_cast<Data<T>*>(baseptr);
			if (pdata)
			{
				return pdata->GetData();
			}
			else
			{
				return nullptr;
			}
		}

	private:
		struct Base
		{
			virtual std::string GetName() const = 0;
			virtual ~Base() {};
		};
		template<typename T>
		struct Data:public Base
		{
			public:
				Data(std::shared_ptr<T> t) : realdata(t) {};

				inline virtual std::string GetName() const noexcept
				{
					return std::string(NAMEOF_TYPE(T));
				}

				inline std::shared_ptr<T> GetData() noexcept
				{
					return realdata;
				}
			
			private:
				std::shared_ptr<T> realdata;
		};
		Base* baseptr;
	};

	template<typename MapType>
	void ShowAnyPairInfo(MapType map)
	{
		for (auto [k, v] : map)
		{
			std::cout << std::dec << k << " : " << v.GetName() << "  ";
		}
		std::cout << std::endl;
	}
}