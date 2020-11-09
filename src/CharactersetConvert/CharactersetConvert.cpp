#include "CharactersetConvert.h"
#include <locale>
#include <codecvt>
#include <Windows.h>

static std::wstring string_to_wstring(const std::string& s)
{
	using default_convert = std::codecvt<wchar_t, char, std::mbstate_t>;
	static std::wstring_convert<default_convert>conv(new default_convert("CHS"));
	return conv.from_bytes(s);
}
static std::string wstring_to_string(const std::wstring& s)
{
	using default_convert = std::codecvt<wchar_t, char, std::mbstate_t>;
	static std::wstring_convert<default_convert>conv(new default_convert("CHS"));
	return conv.to_bytes(s);
}


std::string CharactersetConvert::Std_AnsiToUTF8(const std::string& ansiStr)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;

	return conv.to_bytes(string_to_wstring(ansiStr));
}

std::string CharactersetConvert::Std_UTF8ToAnsi(const std::string& utf8Str)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t> > conv;
	return wstring_to_string(conv.from_bytes(utf8Str));
}

std::string CharactersetConvert::Std_UnicodeToUTF8(const std::wstring& unicodeStr)
{
	std::string ret;
	try 
	{
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.to_bytes(unicodeStr);
	}
	catch (const std::exception& e) 
	{

	}
	return ret;
}

std::wstring CharactersetConvert::Std_UTF8ToUnicode(const std::string& utf8Str)
{
	std::wstring ret;
	try 
	{
		std::wstring_convert< std::codecvt_utf8<wchar_t> > wcv;
		ret = wcv.from_bytes(utf8Str);
	}
	catch (const std::exception& e) 
	{

	}
	return ret;
}

std::wstring CharactersetConvert::Std_AnsiToUnicode(const std::string& ansiStr)
{
	return Std_UTF8ToUnicode(Std_AnsiToUTF8(ansiStr));
}

std::string CharactersetConvert::Std_UnicodeToAnsi(const std::wstring& unicodeStr)
{
	return Std_UTF8ToAnsi(Std_UnicodeToUTF8(unicodeStr));
}

std::string CharactersetConvert::Win32_AnsiToUTF8(const std::string& ansiStr)
{
	if (ansiStr.empty())
	{
		return "";
	}

	int num = MultiByteToWideChar(CP_ACP, NULL, ansiStr.c_str(), -1, NULL, NULL);
	wchar_t* buffw = new wchar_t[(unsigned int)num];
	MultiByteToWideChar(CP_ACP, NULL, ansiStr.c_str(), -1, buffw, num);

	int len = WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	char* lpsz = new char[(unsigned int)len + 1];
	WideCharToMultiByte(CP_UTF8, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	delete[] buffw;

	std::string rtn(lpsz);
	delete[] lpsz;
	return rtn;
}

std::string CharactersetConvert::Win32_UTF8ToAnsi(const std::string& utf8Str)
{
	if (utf8Str.empty())
	{
		return "";
	}
	int num = MultiByteToWideChar(CP_UTF8, NULL, utf8Str.c_str(), -1, NULL, NULL);
	wchar_t* buffw = new wchar_t[(unsigned int)num];
	MultiByteToWideChar(CP_UTF8, NULL, utf8Str.c_str(), -1, buffw, num);
	int len = WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, NULL, NULL, NULL, NULL);
	char* lpsz = new char[(unsigned int)len + 1];
	WideCharToMultiByte(CP_ACP, 0, buffw, num - 1, lpsz, len, NULL, NULL);
	lpsz[len] = '\0';
	delete[] buffw;
	std::string rtn(lpsz);
	delete[] lpsz;
	return rtn;
}

std::string CharactersetConvert::Win32_UnicodeToUTF8(const std::wstring& unicodeStr)
{
	if (unicodeStr.empty())
	{
		return "";
	}


	int textlen;
	textlen = WideCharToMultiByte(CP_UTF8, 0, unicodeStr.c_str(), -1, NULL, 0, NULL, NULL);
	char* result = new char[(unsigned int)textlen + 1];
	memset(result, 0, sizeof(char) * (textlen + 1));
	WideCharToMultiByte(CP_UTF8, 0, unicodeStr.c_str(), -1, result, textlen, NULL, NULL);
	result[textlen] = '\0';
	std::string utf8Str(result);
	delete[] result;
	return utf8Str;

}

std::wstring CharactersetConvert::Win32_UTF8ToUnicode(const std::string& utf8Str)
{
	if (utf8Str.empty())
	{
		return L"";
	}

	int textlen;
	textlen = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, NULL, 0);
	wchar_t* result = new wchar_t[(unsigned int)textlen + 1];
	memset(result, 0, (textlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, (LPWSTR)result, textlen);
	result[textlen] = '\0';

	std::wstring unicodeStr(result);
	delete[] result;

	return unicodeStr;
}

std::wstring CharactersetConvert::Win32_AnsiToUnicode(const std::string& ansiStr)
{
	if (ansiStr.empty())
	{
		return L"";
	}

	int textlen = -1;
	textlen = MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, NULL, 0);
	wchar_t* result = new wchar_t[(unsigned int)textlen + 1];
	memset(result, 0, (textlen + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, ansiStr.c_str(), -1, (LPWSTR)result, textlen);
	result[textlen] = '\0';

	std::wstring unicodeStr(result);
	delete[] result;

	return unicodeStr;
}

std::string CharactersetConvert::Win32_UnicodeToAnsi(const std::wstring& unicodeStr)
{
	if (unicodeStr.empty())
	{
		return "";
	}

	int textlen;
	textlen = WideCharToMultiByte(CP_ACP, 0, unicodeStr.c_str(), -1, NULL, 0, NULL, NULL);
	char* result = new char[(unsigned int)textlen + 1];
	memset(result, 0, sizeof(char) * (textlen + 1));
	WideCharToMultiByte(CP_ACP, 0, unicodeStr.c_str(), -1, result, textlen, NULL, NULL);
	result[textlen] = '\0';

	std::string ansiStr(result);
	delete[] result;

	return ansiStr;
}
