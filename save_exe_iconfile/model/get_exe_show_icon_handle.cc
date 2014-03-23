/**************************************************************************/
/** 
* @brief get exe's icon handle
* @author cswuyg@gmail.com
* @version 1.0.0.1 
* @date 2014/3/22
*****************************************************************************/ 
#include "stdafx.h"
#include "get_exe_show_icon_handle.h"


namespace yg_icon
{

	GetEXEShowIconHandle::GetEXEShowIconHandle() 
		: com_init_(0)
	{
		com_init_ = ::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}

	GetEXEShowIconHandle::~GetEXEShowIconHandle()
	{
		if (com_init_ == S_OK || com_init_ == S_FALSE)
		{
			::CoUninitialize();
		}
	}

	std::wstring GetEXEShowIconHandle::GetOSVersionNumber()
	{
		OSVERSIONINFOEX osvi = { 0 };
		osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
		BOOL get_ver_ret = ::GetVersionEx((OSVERSIONINFO*)&osvi);
		if (!get_ver_ret)
		{
			return L"0.0";
		}
		wchar_t buffer[64];

		::_snwprintf_s(buffer, _countof(buffer), 63, L"%d.%d", osvi.dwMajorVersion, osvi.dwMinorVersion);
		return buffer;
	}

	HICON GetEXEShowIconHandle::GetIconHandle( const std::wstring& pe_path, E_EXEShowIcon_Type icon_type )
	{
		if (IsPEFile(pe_path) != e_pe_EXE)
		{
			return NULL;
		}
		if (icon_type == e_icon_type_jumbo)
		{
			std::wstring os_ver = GetOSVersionNumber();
			if (os_ver < L"6.0")
			{
				return NULL;
			}
		}

		SHFILEINFO sfi = { 0 };
		::SHGetFileInfo(pe_path.c_str(), FILE_ATTRIBUTE_NORMAL, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX);
		HIMAGELIST* imageList = NULL;
		HRESULT hResult = ::SHGetImageList(icon_type, IID_IImageList, (void**)&imageList);  
		HICON icon_handle = NULL;
		if (hResult == S_OK)
		{
			hResult = ((IImageList*)imageList)->GetIcon(sfi.iIcon, ILD_NORMAL, &icon_handle);
		}

		return icon_handle;
	}

	BOOL GetEXEShowIconHandle::DestroyIconHandle( HICON hIcon )
	{
		if (hIcon != NULL)
		{
			return ::DestroyIcon(hIcon);
		}
		else
		{
			return  FALSE;
		}
	}

	GetEXEShowIconHandle::E_PE_Type GetEXEShowIconHandle::IsPEFile( const std::wstring& file_path )
	{
		HANDLE hFile = ::CreateFile(file_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			return e_pe_none;
		}
		E_PE_Type e_ret = e_pe_other;
#pragma warning(push)
#pragma warning(disable:4127)
		do
		{
			DWORD red = 0;
			DWORD file_size = ::GetFileSize(hFile, NULL); 
			if (file_size <= (sizeof(IMAGE_DOS_HEADER) + sizeof(IMAGE_NT_HEADERS)))  //PE file's size > DOS header + NT header
			{
				e_ret = e_pe_other;
				break;
			}
			unsigned short magic; 
			::ReadFile(hFile, &magic, sizeof(magic), &red, NULL);
			if (magic != IMAGE_DOS_SIGNATURE) //DOS head
			{
				e_ret = e_pe_other;
				break;
			}
			::SetFilePointer(hFile, (DWORD)&((IMAGE_DOS_HEADER*)0)->e_lfanew, NULL, FILE_BEGIN);   //0x3C point out NT HEADER's offset
			unsigned long nt_head_pos;  
			::ReadFile(hFile, &nt_head_pos, sizeof(nt_head_pos), &red, NULL);
			if (file_size <= nt_head_pos + sizeof(IMAGE_NT_HEADERS))
			{
				e_ret = e_pe_other;
				break;
			}
			::SetFilePointer(hFile, nt_head_pos, NULL, FILE_BEGIN); 
			IMAGE_NT_HEADERS nt_header = { 0 }; 
			::ReadFile(hFile, &nt_header, sizeof(IMAGE_NT_HEADERS), &red, NULL);
			if (nt_header.Signature == IMAGE_NT_SIGNATURE)  //has "PE"
			{
				if (nt_header.FileHeader.Characteristics & IMAGE_FILE_DLL)
				{
					e_ret = e_pe_DLL;
					break;
				}
				else if (nt_header.FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE )
				{
					e_ret = e_pe_EXE;
					break;
				}
			}

		}while(false);
#pragma warning(pop)
		if (hFile != INVALID_HANDLE_VALUE)
		{
			::CloseHandle(hFile);
		}
		return e_ret;
	}

}