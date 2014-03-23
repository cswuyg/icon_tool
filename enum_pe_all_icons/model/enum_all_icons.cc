/**************************************************************************/
/** 
* @brief save icons in PE file to icon file
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/23
*****************************************************************************/ 
#include "stdafx.h"
#include "enum_all_icons.h"
#include "save_icon_file_by_handle.h"

namespace yg_icon
{
	BOOL CALLBACK EnumAllIconInPE::FindGroupIconProc( HANDLE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG lParam )
	{
		T_map_icons* all_icons = (T_map_icons*)lParam;
		HRSRC ico_group_rsrc = ::FindResource((HMODULE)hModule, lpszName, lpszType);
		if (ico_group_rsrc == NULL)
		{
			return TRUE;
		}
		HGLOBAL ico_group_handle = ::LoadResource((HMODULE)hModule, ico_group_rsrc);        
		if (ico_group_handle == NULL)
		{
			return TRUE;
		}
		GRPICONDIR* ico_group_data = (LPGRPICONDIR)::LockResource(ico_group_handle); 
		if (ico_group_data == NULL)
		{
			::FreeResource(ico_group_handle);
			return TRUE;
		}
		std::map< unsigned int, HICON > icons;
		for (int i = 0; i < ico_group_data->idCount; ++i)
		{              
			GRPICONDIRENTRY* ico_entry = &ico_group_data->idEntries[i];
			HRSRC ico_rsrc = ::FindResource((HMODULE) hModule, MAKEINTRESOURCE(ico_entry->nID), RT_ICON);
			if (ico_rsrc == NULL)
			{
				break;
			}
			HGLOBAL img_handle = ::LoadResource((HMODULE)hModule, ico_rsrc);
			if (img_handle == NULL)
			{
				break;
			}
			LPVOID icon_image = ::LockResource(img_handle);
			if (icon_image == NULL)
			{
				::FreeResource(img_handle);
				break;
			}
			HICON hIcon = ::CreateIconFromResourceEx((PBYTE)icon_image, ico_entry->dwBytesInRes, TRUE, 0x00030000, ico_entry->bWidth, ico_entry->bHeight, 0);               
			icons.insert(std::make_pair(icons.size(), hIcon));
		}; 
		all_icons->insert(std::make_pair(all_icons->size(), icons));
		::FreeResource(ico_group_handle);
		return TRUE;
	}

	bool EnumAllIconInPE::EnumAll( const std::wstring& pe_path, const std::wstring& save_dir )
	{
		HMODULE load_lib = ::LoadLibraryEx(pe_path.c_str(), NULL, LOAD_LIBRARY_AS_DATAFILE);
		if(load_lib == NULL)
		{
			return false;
		}

		T_map_icons all_icons;
		::EnumResourceNames(load_lib, RT_GROUP_ICON, (ENUMRESNAMEPROC)&FindGroupIconProc, (LONG)&all_icons);

		MyCreateDirectory(save_dir);
		SaveFileByHIcon save_by_handle;
		T_map_icons::iterator it = all_icons.begin();
		T_map_icons::iterator itEnd = all_icons.end();
		for (; it != itEnd; ++it)
		{
			std::wstring save_path = save_dir + L"\\" + int2wstr(it->first) + L".ico";
			save_by_handle.SaveIconFile(it->second, save_path);
		}


		::FreeLibrary(load_lib);
		return true;

	}

	std::wstring EnumAllIconInPE::int2wstr( int nData )
	{
		wchar_t wchBuf[32] = L"";
		_itow_s(nData, wchBuf, 32, 10);
		return wchBuf;
	}

	BOOL EnumAllIconInPE::MyCreateDirectory( const std::wstring& strDir )
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

}