/**************************************************************************/
/** 
* @brief split *.ico
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/23
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

	typedef std::map<std::wstring, std::wstring> T_map_ret;
	class SplitIconFile
	{
	public:
		SplitIconFile(){}
		~SplitIconFile(){}

		bool SplitIcon(const std::wstring& icon_path, T_map_ret& map_icon);
	private:
		std::wstring int2wstr(int nData);
		bool writeOneIconFile(const std::wstring& icon_path, const ICONDIRENTRY& icon_entry, const char* img_data);

	};
}