/**************************************************************************/
/** 
* @brief combine *.ico
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/23
*****************************************************************************/ 
#include "stdafx.h"
#include "model/combine_iconfile_impl.h"

void test(const yg_icon::T_map_icons& icons, const std::wstring& dest_path)
{
	yg_icon::CombineIconFiles combine_files;
	combine_files.CombineIcon(icons, dest_path);
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		std::cout << "input error" << std::endl;
		std::cout << "input format: (dest_path) (iconpath_1) (iconpath_2) ..." << std::endl;
		::system("pause");
		return -1;
	}
	yg_icon::T_map_icons icons;
	for(unsigned int i = 2; i != (unsigned int)argc; ++i)
	{
		icons.insert(std::make_pair(i, argv[i]));
	}
	test(icons, argv[1]);
	return 0;
}

