/**************************************************************************/
/** 
* @brief save icons in PE file to icon file
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/23
*****************************************************************************/ 

#include "stdafx.h"
#include "model/enum_all_icons.h"

void test(const std::wstring& pe_path, const std::wstring& save_dir)
{
	yg_icon::EnumAllIconInPE enum_all;
	enum_all.EnumAll(pe_path, save_dir);
}


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc != 3)
	{
		std::cout << "input error" << std::endl;
		std::cout << "input format: (pe file) (save folder)" << std::endl;
		system("pause");
		return -1;
	}

	test(argv[1], argv[2]);
	system("pause");
	return 0;
}

