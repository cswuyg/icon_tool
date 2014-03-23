/**************************************************************************/
/** 
* @brief save icon[s] handle to icon file
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/22
*****************************************************************************/ 
#include "stdafx.h"
#include "save_icon_file_by_handle.h"
#include <assert.h>

namespace yg_icon
{
	bool SaveFileByHIcon::SaveIconFile(std::map<unsigned int, HICON> map_icon, const std::wstring& icon_path)
	{
		AutoHDC hDC = ::GetDC(NULL);
		FILE* fp = NULL;
		::_wfopen_s(&fp, icon_path.c_str(), L"wb");
		if (fp == NULL)
		{
			return false;
		}
		bool ret = WriteFileHeader(fp, (WORD)map_icon.size());
		if (!ret)
		{
			return false;
		}
		unsigned long file_header_len = ::ftell(fp);
		unsigned long img_data_pos = file_header_len + map_icon.size() * sizeof(ICONDIRENTRY);
		std::map<unsigned int, HICON>::iterator it = map_icon.begin();
		std::map<unsigned int, HICON>::iterator it_end = map_icon.end();
		for (unsigned int index = 0; it != it_end; ++it, ++index)
		{
			ICONDIRENTRY icon_entry = { 0 };
			bool write_ret = WriteIconData(fp, it->second, img_data_pos, icon_entry);
			if (!write_ret)
			{
				ret = false;
				break;
			}
			img_data_pos = ::ftell(fp);
			::fseek(fp, file_header_len + index * sizeof(ICONDIRENTRY), SEEK_SET);
			write_ret = WriteData(fp, (char*)&icon_entry, sizeof(icon_entry));
			if (!write_ret)
			{
				ret = false;
				break;
			}
		}

		::fclose(fp);
		return ret;
	}

	bool SaveFileByHIcon::WriteData( FILE* fp, char* data, LONG pos, unsigned long length )
	{
		if (!::fseek(fp, pos, SEEK_SET))
		{
			if (::fwrite(data, length, 1, fp) == 1)
			{
				return true;
			}
		}
		return false;
	}

	bool SaveFileByHIcon::WriteData( FILE* fp, char* data, unsigned long length )
	{
		if (::fwrite(data, length, 1, fp) == 1)
		{
			return true;
		}
		return false;
	}

	bool SaveFileByHIcon::WriteFileHeader( FILE* fp, WORD ico_num )
	{
		ICONDIR ico_header = {0, 1, ico_num};
		return WriteData(fp, (char*)&ico_header, sizeof(ICONDIR));
	}

	bool SaveFileByHIcon::WriteIconData( FILE* fp, HICON hIcon, unsigned long pos, ICONDIRENTRY& icon_entry )
	{
		AutoHDC hDC = ::GetDC(NULL);
		ICONINFO icon_info;
		::GetIconInfo(hIcon, &icon_info);
		AutoBITMAPHandle hbmColor(icon_info.hbmColor);
		AutoBITMAPHandle hbmMask(icon_info.hbmMask); // free bitmaps when function ends
		BITMAPINFO color_header = { 0 };
		color_header.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		if (!::GetDIBits(hDC, hbmColor, 0, 0, NULL, &color_header, DIB_RGB_COLORS))
		{
			return false;
		}

		BITMAPINFO* color_header_all = NULL;
		char* bm_img_data = NULL;
		char* mask_data = NULL;
		BITMAPINFO* mask_header_all = NULL;
#pragma warning(push)
#pragma warning(disable:4127)
		do
		{
			//bitmap info header and bitmap image data
			int bminfo_header_size = sizeof(BITMAPINFOHEADER);
			if (color_header.bmiHeader.biBitCount <= 8)
			{
				bminfo_header_size += (1 << color_header.bmiHeader.biBitCount) * sizeof(RGBQUAD);
			}
			color_header_all = (BITMAPINFO*)new(std::nothrow)char[bminfo_header_size];
			if (color_header_all == NULL)
			{
				break;
			}
			::memcpy(color_header_all, &color_header, sizeof(BITMAPINFOHEADER));

			bm_img_data = new(std::nothrow)char[color_header.bmiHeader.biSizeImage];
			if (bm_img_data == NULL)
			{
				break;
			}
			color_header_all->bmiHeader.biCompression = BI_RGB;
			if (!::GetDIBits(hDC, hbmColor, 0, color_header.bmiHeader.biHeight, bm_img_data, color_header_all, DIB_RGB_COLORS))
			{
				break;
			}

			//mask data:
			BITMAPINFO mask_header = { 0 };
			mask_header.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			if (!::GetDIBits(hDC, hbmMask, 0, 0, NULL, &mask_header, DIB_RGB_COLORS))
			{
				break;
			}
			mask_data = new(std::nothrow)char[mask_header.bmiHeader.biSizeImage];
			mask_header_all = (BITMAPINFO*)new(std::nothrow)char[sizeof(BITMAPINFO) + (mask_header.bmiHeader.biBitCount < 16 ? mask_header.bmiHeader.biClrUsed : 0) * sizeof(RGBQUAD)];
			::memcpy(mask_header_all, &mask_header, sizeof(mask_header));
			color_header_all->bmiHeader.biCompression = BI_RGB;
			if (!::GetDIBits(hDC, hbmMask, 0, mask_header.bmiHeader.biHeight, mask_data, mask_header_all, DIB_RGB_COLORS))
			{
				break;
			}

			//directory entry
			icon_entry.bWidth = (BYTE)color_header_all->bmiHeader.biWidth;
			icon_entry.bHeight = (BYTE)color_header_all->bmiHeader.biHeight;
			icon_entry.bColorCount = (color_header.bmiHeader.biBitCount > 8 ? 0 : 1 << color_header.bmiHeader.biBitCount);
			icon_entry.bReserved = 0;
			icon_entry.wPlanes = 0;
			icon_entry.wBitCount = color_header_all->bmiHeader.biBitCount;
			//bitmap info header + image data + mask data
			icon_entry.dwBytesInRes = bminfo_header_size + color_header_all->bmiHeader.biSizeImage + mask_header_all->bmiHeader.biSizeImage; 
			icon_entry.dwImageOffset = pos;

			// bitmap info header
			int nBitsSize = color_header_all->bmiHeader.biSizeImage;
			color_header_all->bmiHeader.biHeight *= 2; //note:*2
			color_header_all->bmiHeader.biCompression = 0;
			color_header_all->bmiHeader.biSizeImage += mask_header_all->bmiHeader.biSizeImage;  //note: add mask info
			WriteData(fp, (char*)color_header_all, pos, bminfo_header_size);
			// Write image data:
			WriteData(fp, (char*)bm_img_data, nBitsSize);
			// Write mask data:
			WriteData(fp, (char*)mask_data, mask_header_all->bmiHeader.biSizeImage);
		}while(false);
#pragma warning(pop)

		if (color_header_all != NULL)
		{
			delete [] color_header_all;
		}
		if (bm_img_data != NULL)
		{
			delete [] bm_img_data;
		}
		if (mask_data != NULL)
		{
			delete [] mask_data;
		}
		if (mask_header_all != NULL)
		{
			delete[] mask_header_all;
		}

		return true;
	}

}