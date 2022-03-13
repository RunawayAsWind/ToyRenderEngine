#pragma once
#include <string>

class GobalPath
{
	public:
		std::string AssetsPath;
		std::string GraphVizGenPath;
		std::string ScreenGrapDir;
		std::string TexturePath;
		GobalPath()
		{
			AssetsPath = "Assets";
			GraphVizGenPath = AssetsPath + "/GenFile/GraphViz";
			ScreenGrapDir = AssetsPath + "/GenFile/ScreenGrap";
			TexturePath = AssetsPath + "/Textures";
		}
};