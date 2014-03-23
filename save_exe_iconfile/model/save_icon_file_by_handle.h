/**************************************************************************/
/** 
* @brief save icon[s] handle to icon file
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/22
*****************************************************************************/ 
#pragma once

namespace yg_icon
{
#pragma pack(push)
#pragma pack(2)
	typedef struct
	{
		WORD           idReserved;   // Reserved (must be 0)
		WORD           idType;       // Resource Type (1 for icons)
		WORD           idCount;      // How many images?
	} ICONDIR, *LPICONDIR;

	typedef struct
	{
		BYTE        bWidth;          // Width, in pixels, of the image
		BYTE        bHeight;         // Height, in pixels, of the image
		BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
		BYTE        bReserved;       // Reserved ( must be 0)
		WORD        wPlanes;         // Color Planes
		WORD        wBitCount;       // Bits per pixel
		DWORD       dwBytesInRes;    // How many bytes in this resource?
		DWORD       dwImageOffset;   // Where in the file is this image?
	} ICONDIRENTRY, *LPICONDIRENTRY;
#pragma pack(pop)

	class AutoHDC
	{
	public:
		AutoHDC(HDC handle)
			: handle_(handle)
		{
		};
		~AutoHDC()
		{
			::ReleaseDC(NULL, handle_);
		}
		operator HDC() const
		{
			return handle_;
		}
	private:
		HDC handle_;
	};

	class AutoBITMAPHandle
	{
	public:
		AutoBITMAPHandle(HBITMAP handle)
			: handle_(handle)
		{
		};
		~AutoBITMAPHandle()
		{
			::DeleteObject(handle_);
		}
		operator HBITMAP() const
		{
			return handle_;
		}
	private:
		HBITMAP handle_;
	};

	class SaveFileByHIcon
	{
	public:
		SaveFileByHIcon()
		{}
		~SaveFileByHIcon(){}

		bool SaveIconFile(std::map<unsigned int, HICON> map_icon, const std::wstring& icon_path);

	private:
		bool WriteData(FILE* fp, char* data, LONG pos, unsigned long length);
		bool WriteData(FILE* fp, char* data, unsigned long length);

		bool WriteFileHeader(FILE* fp, WORD ico_num);
		bool WriteIconData(FILE* fp, HICON hIcon, unsigned long pos, ICONDIRENTRY& icon_entry);
	};
}