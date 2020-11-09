#include <iostream>
#include "CharactersetConvert.h"

using namespace std;

int main()
{
	std::string nihao = "ÄãºÃ";
	
	std::string nihaoUTF8 = CharactersetConvert::Std_AnsiToUTF8(nihao);

	std::wstring nihaoUnicode = CharactersetConvert::Std_UTF8ToUnicode(nihaoUTF8);

	std::wstring nihaoUnicode1 = CharactersetConvert::Std_AnsiToUnicode(nihao);

	std::string nihaoWin32UTF8 = CharactersetConvert::Win32_AnsiToUTF8(nihao);

	std::wstring nihaoWin32Unicode = CharactersetConvert::Win32_UTF8ToUnicode(nihaoWin32UTF8);

	std::wstring nihaoWin32Unicode1 = CharactersetConvert::Win32_AnsiToUnicode(nihao);

	getchar();
	return 0;
}