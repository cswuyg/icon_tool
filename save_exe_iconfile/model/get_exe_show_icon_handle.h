/**************************************************************************/
/** 
* @brief get exe's icon handle
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/22
*****************************************************************************/ 
/**
http://msdn.microsoft.com/en-us/library/windows/desktop/bb762179(v=vs.85).aspx
You must initialize Component Object Model (COM) with CoInitialize or OleInitialize prior to calling SHGetFileInfo.

http://msdn.microsoft.com/en-us/library/windows/desktop/ms688715(v=vs.85).aspx
A thread must call CoUninitialize once for each successful call it has made to the CoInitialize or CoInitializeEx function, including any call that returns S_FALSE
*/
#pragma once

namespace yg_icon
{

	enum E_EXEShowIcon_Type
	{
		e_icon_type_small = SHIL_SMALL,             //< 16*16
		e_icon_type_large = SHIL_LARGE,             //< 32 * 32
		e_icon_type_extralarge = SHIL_EXTRALARGE,   //< 48 * 48
		e_icon_type_jumbo = SHIL_JUMBO,             // 256 * 256  //vista system + 
	};


	class GetEXEShowIconHandle
	{
		enum E_PE_Type
		{
			e_pe_none,
			e_pe_EXE,
			e_pe_DLL,
			e_pe_other,
		};

	public:
		GetEXEShowIconHandle();
		~GetEXEShowIconHandle();

		HICON GetIconHandle(const std::wstring& pe_path, E_EXEShowIcon_Type icon_type);
		BOOL DestroyIconHandle(HICON hIcon);

	private:
		std::wstring GetOSVersionNumber();
		E_PE_Type IsPEFile(const std::wstring& file_path);

	private:
		HRESULT com_init_;
	};
}