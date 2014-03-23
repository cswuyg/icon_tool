/**************************************************************************/
/** 
* @brief save icons in PE file to icon file
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
		BYTE        bWidth;          // Width, in pixels, of the image
		BYTE        bHeight;         // Height, in pixels, of the image
		BYTE        bColorCount;     // Number of colors in image (0 if >=8bpp)
		BYTE        bReserved;       // Reserved ( must be 0)
		WORD        wPlanes;         // Color Planes
		WORD        wBitCount;       // Bits per pixel
		DWORD       dwBytesInRes;    // How many bytes in this resource?
		WORD        nID;             // resource id
	} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;
	typedef struct 
	{
		WORD            idReserved;   // Reserved (must be 0)
		WORD            idType;       // Resource type (1 for icons)
		WORD            idCount;      // How many images?
		GRPICONDIRENTRY   idEntries[1]; // The entries for each image
	} GRPICONDIR, *LPGRPICONDIR;

#pragma pack(pop)



	typedef std::map< unsigned int, std::map<unsigned int, HICON> > T_map_icons;
	class EnumAllIconInPE 
	{
	public:
		EnumAllIconInPE()
		{}
		~EnumAllIconInPE(){}

		bool EnumAll(const std::wstring& pe_path, const std::wstring& save_dir);

	private:
		BOOL static CALLBACK FindGroupIconProc(HANDLE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG lParam);
		std::wstring int2wstr(int nData);
		BOOL MyCreateDirectory(const std::wstring& strDir);
	};
}