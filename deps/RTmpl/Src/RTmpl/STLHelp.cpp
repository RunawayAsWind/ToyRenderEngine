#include "../Include/RTmpl/STLHelp.h"

namespace RTmpl
{
    std::string GetFileName(std::string path)
    {
        auto vec = StrSplit<std::string >(path, "/");
        return vec[vec.size() - 1];
    }

	std::string GetFileNameWithOutEx(std::string path)
	{
		std::string filename = GetFileName(path);
		std::vector<std::string> splitstr = StrSplit<std::string>(filename, ".");
		return splitstr[0];
	}

    std::wstring GetFileName(std::wstring path)
    {
        auto vec = StrSplit<std::wstring >(path, L"/");
        return vec[vec.size() - 1];
    }
}
