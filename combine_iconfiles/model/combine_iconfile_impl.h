/**************************************************************************/
/** 
* @brief combine *.ico
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/23
*****************************************************************************/ 
#pragma once

namespace yg_icon
{
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


	typedef std::map<unsigned int, std::wstring> T_map_icons;
	class CombineIconFiles
	{
	public:
		CombineIconFiles(){}
		~CombineIconFiles(){}

		bool CombineIcon(const T_map_icons& map_icons, const std::wstring& icon_path);
	private:
		bool WriteOneIconFile(FILE* fp_write, unsigned long& entry_pos, unsigned long& img_pos, WORD& icon_num, const std::wstring& read_path);
		bool WriteOneIconImg(FILE* fp_write, unsigned long& entry_pos, unsigned long& img_pos, const ICONDIRENTRY& icon_entry, const char* img_data);
		bool GetAllImgCount(const T_map_icons& map_icons, WORD& all_img_count);


	private:
		BOOL MyCreateDirectory(const std::wstring& strDir);
	std::wstring GetFilePath(const std::wstring& wsFullName);
	};
}