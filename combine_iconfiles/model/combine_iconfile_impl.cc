/**************************************************************************/
/** 
* @brief combine *.ico
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/23
*****************************************************************************/ 
#include "stdafx.h"
#include "combine_iconfile_impl.h"

namespace yg_icon
{
	bool CombineIconFiles::CombineIcon( const T_map_icons& map_icons, const std::wstring& icon_path )
	{
		WORD all_img_count = 0;
		bool get_all_img_cout_ret = GetAllImgCount(map_icons, all_img_count);
		if (!get_all_img_cout_ret)
		{
			return false;
		}

		MyCreateDirectory(GetFilePath(icon_path));
		FILE* fp_write = NULL;
		::_wfopen_s(&fp_write, icon_path.c_str(), L"wb");
		if (fp_write == NULL)
		{
			return false;
		}

		ICONDIR icon_file_header = {0, 1, all_img_count};
		size_t write_ret = ::fwrite(&icon_file_header, sizeof(ICONDIR), 1, fp_write);
		if (write_ret != 1)
		{
			return false;
		}

		bool ret = true;
		T_map_icons::const_iterator it = map_icons.begin();
		T_map_icons::const_iterator itEnd = map_icons.end();
		unsigned long entry_pos = 6;
		unsigned long img_data = sizeof(ICONDIR) + all_img_count * sizeof(ICONDIRENTRY);
		for (; it != itEnd; ++it)
		{
			WORD img_numbers = 0;
			bool write_ret = WriteOneIconFile(fp_write, entry_pos, img_data, img_numbers, it->second);
			if (!write_ret)
			{
				ret = false;
				break;
			}
		}
		
		return ret;
	}

	bool CombineIconFiles::WriteOneIconFile( FILE* fp_write, unsigned long& entry_pos, unsigned long& img_pos, WORD& icon_num, const std::wstring& read_path )
	{
		FILE* fp_read = NULL;
		::_wfopen_s(&fp_read, read_path.c_str(), L"rb");  //note: rb..
		if (fp_read == NULL)
		{
			return false;
		}
#pragma warning(push)
#pragma warning(disable:4127)
		do 
		{
			// read icon file header get icons entry array length
			ICONDIR icon_file_header = { 0 };
			size_t read_size = ::fread_s(&icon_file_header, sizeof(ICONDIR), sizeof(ICONDIR), 1, fp_read); 
			if (read_size != 1)
			{
				break;
			}
			if (icon_file_header.idReserved != 0 && icon_file_header.idType != 1 && icon_file_header.idCount <= 0)
			{
				break;
			}
			icon_num = icon_file_header.idCount;

			size_t read_entry_pos = ::ftell(fp_read);
			char* img_data = NULL;
			for (unsigned int i = 0; i != icon_file_header.idCount; ++i)
			{
				ICONDIRENTRY icon_entry;
				int seek_ret = ::fseek(fp_read, read_entry_pos, SEEK_SET);
				if (seek_ret != 0)
				{
					break;
				}
				size_t read_size = ::fread_s(&icon_entry, sizeof(ICONDIRENTRY), sizeof(ICONDIRENTRY), 1, fp_read); 
				if (read_size != 1)
				{
					break;
				}
				read_entry_pos = ::ftell(fp_read);
				::fseek(fp_read, icon_entry.dwImageOffset, SEEK_SET);
				char* img_data = new(std::nothrow)char[icon_entry.dwBytesInRes];
				if (img_data == NULL)
				{
					break;
				}
				read_size = ::fread_s(img_data, icon_entry.dwBytesInRes, icon_entry.dwBytesInRes, 1, fp_read); 
				if (read_size != 1)
				{
					break;
				}
				icon_entry.dwImageOffset = img_pos;
				bool write_img_ret = WriteOneIconImg(fp_write, entry_pos, img_pos, icon_entry, img_data);
				if (!write_img_ret)
				{
					break;
				}
				img_pos += icon_entry.dwBytesInRes;
				entry_pos += sizeof(ICONDIRENTRY);
				if (img_data != NULL)
				{
					::delete [] img_data;
					img_data = NULL;
				}
			}
			if (img_data != NULL)
			{
				::delete [] img_data;
			}
		}while(false);

#pragma warning(pop)

		return true;
	}

	bool CombineIconFiles::WriteOneIconImg( FILE* fp_write, unsigned long& entry_pos, unsigned long& img_pos, const ICONDIRENTRY& icon_entry, const char* img_data )
	{
		bool ret = false;
#pragma warning(push)
#pragma warning(disable:4127)
		do 
		{
			::fseek(fp_write, entry_pos, SEEK_SET);
			size_t write_size = ::fwrite(&icon_entry, sizeof(ICONDIRENTRY), 1, fp_write);
			if (write_size != 1)
			{
				break;
			}
			::fseek(fp_write, img_pos, SEEK_SET);
			write_size = ::fwrite(img_data, icon_entry.dwBytesInRes, 1, fp_write);
			if (write_size != 1)
			{
				break;
			}
			ret = true;
		}while(false);

#pragma warning(pop)
		return ret;
	}

	bool CombineIconFiles::GetAllImgCount( const T_map_icons& map_icons, WORD& all_img_count )
	{
		bool ret = true;
		T_map_icons::const_iterator it = map_icons.begin();
		T_map_icons::const_iterator itEnd = map_icons.end();
		for (; it != itEnd; ++it)
		{
			FILE* fp_read = NULL;
			::_wfopen_s(&fp_read, it->second.c_str(), L"rb");
			if (fp_read == NULL)
			{
				ret = false;
				break;
			}

			ICONDIR icon_file_header = { 0 };
			size_t read_size = ::fread_s(&icon_file_header, sizeof(ICONDIR), sizeof(ICONDIR), 1, fp_read); 
			if (read_size != 1)
			{
				ret = false;
				break;
			}
			if (icon_file_header.idReserved != 0 && icon_file_header.idType != 1 && icon_file_header.idCount <= 0)
			{
				ret = false;
				break;
			}
			all_img_count += icon_file_header.idCount;
		}
		return ret;
	}

	BOOL CombineIconFiles::MyCreateDirectory( const std::wstring& strDir )
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

	std::wstring CombineIconFiles::GetFilePath( const std::wstring& wsFullName )
	{
		std::wstring::size_type nIndex1 = wsFullName.find_last_of(L"\\");
		std::wstring::size_type nIndex2 = wsFullName.find_last_of(L"/");
		if (std::wstring::npos == nIndex1)
		{
			nIndex1 = 0;
		}
		if (std::wstring::npos == nIndex2)
		{
			nIndex2 = 0;
		}
		std::wstring::size_type nIndex = max(nIndex1, nIndex2);
		return wsFullName.substr(0, nIndex);
	}




}