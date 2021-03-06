#pragma once

#include <string>
#include "windows.h"
#include <locale>
#include <codecvt>

/*
	General utility methods that don't have a better place to go.

	As better places for these items become available, it may be better to move them.
*/

#define __USE_2017__

inline std::wstring s2ws(const std::string& str)
{
#ifdef __USE_2017__
	std::wstring result;

	for (auto x : str)
		result += (char)x;

	return result;
#else
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
#endif
}

inline std::string ws2s(const std::wstring& wstr)
{
#ifdef __USE_2017__
	std::string result;

	for (auto x : wstr)
		result += (char)x;

	return result;
#else
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
#endif
}

inline std::string HrToString(HRESULT hr)
{
	char s_str[64] = {};
	sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	return std::string(s_str);
}

class MCException 
{
public:
	MCException() = default;
	MCException(HRESULT hr, const std::string& functionName, const std::string& filename, int lineNumber)
		: ErrorCode(hr), FunctionName(functionName), Filename(filename), LineNumber(lineNumber) {}
	MCException(const std::string& info, const std::string& filename, int lineNumber)
		: Info(info), Filename(filename), LineNumber(lineNumber) {}
	MCException(HRESULT hr)
		: ErrorCode(hr) {}

	std::string what() {
		char lineNumberBuffer[20];
		_itoa_s(LineNumber, lineNumberBuffer, 10);
		return
			std::string("HRESULT (s_ok when not used): ") + HrToString(ErrorCode) + std::string("\n") +
			std::string("FunctionName: ") + FunctionName + std::string("\n") +
			std::string("Filename: ") + Filename + std::string("\n") +
			std::string("Line Number: ") + std::string(lineNumberBuffer) + std::string("\n") +
			std::string("Extra info: ") + Info + std::string("\n");
	}

	HRESULT      ErrorCode = S_OK;
	std::string  FunctionName;
	std::string  Filename;
	std::string  Info;
	int          LineNumber = -1;
};

#define MCSAFE_RELEASE(p) if (p) (p)->Release()

#define MCThrowIfFailed(x) \
{ \
	HRESULT hr__ = (x); \
	if(FAILED(hr__)) { throw MCException(hr__, #x, __FILE__, __LINE__); } \
}

#define MCTHROW(x) throw MCException(x, __FILE__, __LINE__)

#define MCASSERT(x) if(x) { } else { MCTHROW (#x); }
