#pragma once

#include <string>
#include "windows.h"
#include <locale>
#include <codecvt>

/*
	General utility methods that don't have a better place to go.

	As better places for these items become available, it may be better to move them.
*/

inline std::wstring s2ws(const std::string& str)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(str);
}

inline std::string ws2s(const std::wstring& wstr)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(wstr);
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
	MCException(HRESULT hr, const std::string& functionName, const std::string& filename, int lineNunber)
		: ErrorCode(hr), FunctionName(functionName), Filename(filename), LineNumber(LineNumber) {}
	MCException(HRESULT hr)
		: ErrorCode(hr) {}

	std::string what() {
		char lineNumberBuffer[20];
		_itoa_s(LineNumber, lineNumberBuffer, 10);
		return
			std::string("ErrorCode: ") + HrToString(ErrorCode) + std::string("\n") +
			std::string("FunctionName: ") + FunctionName + std::string("\n") +
			std::string("Filename: ") + Filename + std::string("\n") +
			std::string("Line Number: ") + std::string(lineNumberBuffer);
	}

	HRESULT      ErrorCode = S_OK;
	std::string  FunctionName;
	std::string  Filename;
	int          LineNumber = -1;
};

#define MCSAFE_RELEASE(p) if (p) (p)->Release()

#define MCThrowIfFailed(x) \
{ \
	HRESULT hr__ = (x); \
	std::string wfn = __FILE__; \
	if(FAILED(hr__)) { throw MCException(hr__, #x, wfn, __LINE__); } \
}