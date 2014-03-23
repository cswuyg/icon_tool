/**************************************************************************/
/** 
* @brief split *.ico
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/23
*****************************************************************************/ 
#include "stdafx.h"
#include "model/split_iconfile_impl.h"

void test(const std::wstring& input_icon)
{
	yg_icon::SplitIconFile split_file;
	yg_icon::T_map_ret ret_map;
	split_file.SplitIcon(input_icon, ret_map);
	for (yg_icon::T_map_ret::iterator it = ret_map.begin(); it != ret_map.end(); ++it)
	{
		std::wcout << it->first << L"  " << it->second << std::endl;
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 2)
	{
		std::cout << "input error" << std::endl;
		std::cout << "input format: (icon file)" << std::endl;
		system("pause");
		return -1;
	}
	std::wcout << L"input:" << argv[1] << std::endl;
	std::cout << "output:..." << std::endl;
	test(argv[1]);
	system("pause");
	return 0;
}

