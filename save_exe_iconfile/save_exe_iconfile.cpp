/**************************************************************************/
/** 
* @brief save PE file's icon image to a *.ico file
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/22 & 23
*****************************************************************************/ 
#include "stdafx.h"
#include "model/save_icon_file_by_handle.h"
#include "model/get_exe_show_icon_handle.h"

BOOL MyCreateDirectory(const std::wstring& strDir)
{
	if (strDir.empty() || strDir.length() < 3 || strDir[1] != L':')
	{
		return FALSE;
	}
	std::wstring sub_dir = strDir;
	wchar_t last_char = sub_dir[sub_dir.length()-1];
	if ( last_char != L'\\' && last_char != L'/')
	{
		sub_dir += L'\\';
	}
	std::wstring::size_type gap_pos_1 = sub_dir.find_first_of(L'\\', 0);
	std::wstring::size_type gap_pos_2 = sub_dir.find_first_of(L'/', 0);
	if (gap_pos_1 == std::wstring::npos)
	{
		gap_pos_1 = 0xFFFFFFFF;
	}
	if (gap_pos_2 == std::wstring::npos)
	{
		gap_pos_2 = 0xFFFFFFFF;
	}
	std::wstring::size_type gap_pos = min(gap_pos_1, gap_pos_2);
	if (gap_pos == 0xFFFFFFFF)
	{
		return FALSE;
	}

	BOOL bRet = TRUE;
	std::wstring::size_type gap_pos_bak = gap_pos;
	while(gap_pos != std::wstring::npos)
	{
		std::wstring pre_dir = strDir.substr(0, gap_pos);
		if(!::PathIsDirectory(pre_dir.c_str()))
		{
			BOOL bCreate =  ::CreateDirectory(pre_dir.c_str(), NULL);
			if (bCreate == 0)
			{
				DWORD dwError = ::GetLastError();
				if (dwError != ERROR_ALREADY_EXISTS)
				{
					bRet = FALSE;
					break;
				}
			}
		}

		gap_pos_bak = gap_pos;
		std::wstring::size_type gap_pos_1 = sub_dir.find_first_of(L'\\', gap_pos_bak+1);
		std::wstring::size_type gap_pos_2 = sub_dir.find_first_of(L'/', gap_pos_bak+1);
		if (gap_pos_1 == std::wstring::npos)
		{
			gap_pos_1 = 0xFFFFFFFF;
		}
		if (gap_pos_2 == std::wstring::npos)
		{
			gap_pos_2 = 0xFFFFFFFF;
		}
		gap_pos = min(gap_pos_1, gap_pos_2);
		if (gap_pos == 0xFFFFFFFF)
		{
			break;
		}
	}
	return bRet;
}
void test(const std::wstring& pe_path, const std::wstring& save_folder)
{
	typedef std::map<unsigned int, HICON> T_map;
	T_map map_icons;
	yg_icon::GetEXEShowIconHandle get_exe_h;
	HICON hIcon = get_exe_h.GetIconHandle(pe_path, yg_icon::e_icon_type_jumbo);
	map_icons.insert(std::make_pair(0, hIcon));
	hIcon = get_exe_h.GetIconHandle(pe_path, yg_icon::e_icon_type_extralarge);
	map_icons.insert(std::make_pair(1, hIcon));
	hIcon = get_exe_h.GetIconHandle(pe_path, yg_icon::e_icon_type_large);
	map_icons.insert(std::make_pair(2, hIcon));
	hIcon = get_exe_h.GetIconHandle(pe_path, yg_icon::e_icon_type_small);
	map_icons.insert(std::make_pair(3, hIcon));

	MyCreateDirectory(save_folder);
	std::wstring save_path = save_folder + L"\\0.ico";
	yg_icon::SaveFileByHIcon save_by_hicon;
	save_by_hicon.SaveIconFile(map_icons, save_path);  //save four icon image to one *.ico file.
	for (T_map::iterator it = map_icons.begin(); it != map_icons.end(); ++it)
	{
		get_exe_h.DestroyIconHandle(it->second);
	}
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

