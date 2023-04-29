// OpenApplication.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include <iostream>
#include <Windows.h>
#include <cstdio>  
#include <tchar.h>

int main()
{
    WCHAR symbolicName[] = L"\\??\\OpenDriver";
    _tprintf(L"%s",symbolicName);
    HANDLE handle=NULL;
    BOOL bRet;
    DWORD dwRet;
    UCHAR buffer[100] = { 0, };
    handle=CreateFileW(symbolicName,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,0,NULL);

    bRet = ReadFile(handle, buffer, 5, &dwRet, NULL);
    printf("dwret ReadFile = %d\n", dwRet);

    bRet = WriteFile(handle, "Hello", 5, &dwRet, NULL);
    printf("dwret WriteFile = %d\n", dwRet);

    bRet = ReadFile(handle, buffer, sizeof(buffer), &dwRet, NULL);
    printf("dwret ReadFile second = %d\n", dwRet);

    CloseHandle(handle);

}

