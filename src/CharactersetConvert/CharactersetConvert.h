#ifndef CHARACTERSET_CONVERT_H
#define CHARACTERSET_CONVERT_H

#include <string>

class CharactersetConvert
{
public:
	static std::wstring string_to_wstring(const std::string& s);
	static std::string wstring_to_string(const std::wstring& s);

	static std::string Std_AnsiToUTF8(const std::string& ansiStr);
	static std::string Std_UTF8ToAnsi(const std::string& utf8Str);
	static std::string Std_UnicodeToUTF8(const std::wstring& unicodeStr);
	static std::wstring Std_UTF8ToUnicode(const std::string& utf8Str);
	static std::wstring Std_AnsiToUnicode(const std::string& ansiStr);
	static std::string Std_UnicodeToAnsi(const std::wstring& unicodeStr);


	static std::string Win32_AnsiToUTF8(const std::string& ansiStr);
	static std::string Win32_UTF8ToAnsi(const std::string& utf8Str);
	static std::string Win32_UnicodeToUTF8(const std::wstring& unicodeStr);
	static std::wstring Win32_UTF8ToUnicode(const std::string& utf8Str);
	static std::wstring Win32_AnsiToUnicode(const std::string& ansiStr);
	static std::string Win32_UnicodeToAnsi(const std::wstring& unicodeStr);

};

#endif // !CHARACTERSET_CONVERT_H
