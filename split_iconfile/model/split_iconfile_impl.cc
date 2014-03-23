/**************************************************************************/
/** 
* @brief split *.ico
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/23
*****************************************************************************/ 
#include "stdafx.h"
#include "split_iconfile_impl.h"

namespace yg_icon
{


	bool SplitIconFile::SplitIcon( const std::wstring& icon_path, T_map_ret& map_icon )
	{
		if (!::PathFileExists(icon_path.c_str()) || ::PathIsDirectory(icon_path.c_str()))
		{
			return false;
		}
		FILE* fp_read = NULL;
		::_wfopen_s(&fp_read, icon_path.c_str(), L"rb");
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
			size_t read_size =  ::fread_s(&icon_file_header, sizeof(ICONDIR), sizeof(ICONDIR), 1, fp_read); 
			if (read_size != 1)
			{
				break;
			}
			if (icon_file_header.idReserved != 0 && icon_file_header.idType != 1 && icon_file_header.idCount <= 1)
			{
				break;
			}

			size_t entry_pos = ::ftell(fp_read);
			char* img_data = NULL;
			for (unsigned int i = 0; i != icon_file_header.idCount; ++i)
			{
				ICONDIRENTRY icon_entry;
				int seek_ret = ::fseek(fp_read, entry_pos, SEEK_SET);
				if (seek_ret != 0)
				{
					break;
				}
				size_t read_size = ::fread_s(&icon_entry, sizeof(ICONDIRENTRY), sizeof(ICONDIRENTRY), 1, fp_read); 
				if (read_size != 1)
				{
					break;
				}
				entry_pos = ::ftell(fp_read);
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
				//write new icon
				std::wstring new_icon_path = icon_path;
				new_icon_path += L"_";
				new_icon_path += int2wstr(i);
				new_icon_path += L".ico";
				icon_entry.dwImageOffset = sizeof(ICONDIR) + sizeof(ICONDIRENTRY);
				writeOneIconFile(new_icon_path, icon_entry, img_data);
				if (img_data != NULL)
				{
					::delete [] img_data;
					img_data = NULL;
				}
				std::wstring icon_size = int2wstr(icon_entry.bWidth);
				icon_size += L"*";
				icon_size += int2wstr(icon_entry.bHeight);
				map_icon.insert(std::make_pair(new_icon_path, icon_size));
			}
			if (img_data != NULL)
			{
				::delete [] img_data;
			}
		}while(false);
#pragma warning(pop)
		::fclose(fp_read);
		return true;
	}

	std::wstring SplitIconFile::int2wstr( int nData )
	{
		wchar_t wchBuf[32] = L"";
		_itow_s(nData, wchBuf, 32, 10);
		return wchBuf;
	}

	bool SplitIconFile::writeOneIconFile( const std::wstring& icon_path, const ICONDIRENTRY& icon_entry, const char* img_data )
	{
		FILE* fp_write;
		::_wfopen_s(&fp_write, icon_path.c_str(), L"wb");
		if (fp_write == NULL)
		{
			return false;
		}
		bool ret = false;
#pragma warning(push)
#pragma warning(disable:4127)
		do 
		{
			// icon file header
			ICONDIR new_dir = {0, 1, 1};
			size_t write_size = ::fwrite(&new_dir, sizeof(ICONDIR), 1, fp_write);
			if (write_size != 1)
			{
				break;
			}
			write_size = ::fwrite(&icon_entry, sizeof(ICONDIRENTRY), 1, fp_write);
			if (write_size != 1)
			{
				break;
			}
			write_size = ::fwrite(img_data, icon_entry.dwBytesInRes, 1, fp_write);
			if (write_size != 1)
			{
				break;
			}
			ret = true;
		}while(false);

#pragma warning(pop)
		::fclose(fp_write);
		return ret;
	}

}