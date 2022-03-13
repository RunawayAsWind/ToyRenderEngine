#include "../../Include/REcs/CmptMeta.h"

namespace REcs
{
	std::string GetUniqueName(std::vector<CmptMeta> cmptVec)
	{
		sort(cmptVec.begin(), cmptVec.end(), [](CmptMeta a, CmptMeta b) {return a.hash < b.hash; });
		std::string name = "<";
		for (int i = 0; i < cmptVec.size(); i++)
		{
			name += cmptVec[i].name;
			if (i != cmptVec.size() - 1)
				name += ",";
		}
		name += ">";
		return name;
	}
}