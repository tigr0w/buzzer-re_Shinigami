#include "pch.h"
#include "Utils.h"
#include "Logger.h"

BOOL Utils::SaveToFile(const wchar_t* filename, Memory* data, BOOL Paginate)
{
    HANDLE hFile = CreateFileW(filename, FILE_APPEND_DATA, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    BOOL success = TRUE;
    
    if (hFile == INVALID_HANDLE_VALUE) {
        return false;
    }

    DWORD BytesWritten = 0;

    if (Paginate)
    {
        // Write based on the VirtualQuery output
        MEMORY_BASIC_INFORMATION mbi;
        DWORD Written = 0;
        while (Written < data->Size)
        {
            VirtualQuery(data->Addr + Written, &mbi, sizeof(mbi));
            WriteFile(hFile, data->Addr + Written, mbi.RegionSize, &BytesWritten, NULL);
            Written += BytesWritten;
        }
    }
    else
    {
        success = WriteFile(hFile, data->Addr, data->Size, &BytesWritten, NULL) && (BytesWritten == data->Size);
    }

    
    CloseHandle(hFile);

    return success;
}



std::wstring Utils::BuildFilenameFromProcessName(const wchar_t* suffix)
{
    //
    // Get the name of the process executing 
    //
    wchar_t exePath[MAX_PATH];
    GetModuleFileName(nullptr, exePath, MAX_PATH);
    //
    // Get filename
    //
    wchar_t* exeName = PathFindFileNameW(exePath);
    
    // 
    // Get the . pos
    //
    wchar_t* dot = wcsrchr(exeName, L'.');

    //
    // Quick hack
    //
    if (dot != nullptr && _wcsicmp(dot, L".exe") == 0) {
        *dot = L'\0'; // Replace the dot with a null terminator
    }


    return std::wstring(exeName) + suffix;
}

MEM_ERROR Utils::IsReadWritable(ULONG_PTR* Address)
{
    MEMORY_BASIC_INFORMATION mbi = { 0 };

    if (!VirtualQuery(Address, &mbi, sizeof(mbi)))
    {
        return INVALID_MEMORY_AREA;
    }

    return (MEM_ERROR) (mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_READWRITE);
}

