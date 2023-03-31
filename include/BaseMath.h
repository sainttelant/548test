#ifndef BASE_MATH_H_HEADER_INCLUDED_B8E468C8
#define BASE_MATH_H_HEADER_INCLUDED_B8E468C8

#include "BaseDataDefine.h"
#include <vector>
#include <chrono>
#include <time.h>


using namespace std;

#ifndef WIN32
typedef long long INT64;
#endif


void getNumber(const char* pBuf, int len, INT64& number);

void getString(const char* pBuf, int len, string& str, bool bA = false);

void getString(const INT64& number, string& str);

void split(	const string& src, const string& flag, vector<string>& array);

void split1(const string& src, const string& flag, vector<string>& array, const int num = 1);

void getNumberString(const char* pBuf, int len, string& str);

void getHexSrcMsg(const char* pSrcMsg, int len, string& str);

void getPCString(const char* pBuf, int len, string& str);

void getBinString(const char* pBuf, int len, string& str);

void bitMoveTowardRight(u_char* buf, int len, int pos);

void bitCleanLeft(u_char* buf, int lenBitClean);

void getHexSrcMsg1(const char* pSrcMsg, int len, string& str);

u_int64 htonl64(u_int64 hostint64);

u_int64 ntohl64(u_int64 hostint64);

time_t now_milliseconds();

struct ip_address
{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;

	bool operator==(const ip_address& src) const
	{
		return (src.byte1 == byte1
			&& src.byte2 == byte2
			&& src.byte3 == byte3
			&& src.byte4 == byte4);
	}

	bool operator!=(const ip_address& src) const
	{
		bool bRet = operator==(src);
		if (bRet)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	bool operator>(const ip_address& src) const
	{
		return (memcmp(this, &src, sizeof(ip_address)) > 0);
	}

	bool operator<(const ip_address& src) const
	{
		return (memcmp(this, &src, sizeof(ip_address)) < 0);
	}

	void getString(string& str) const
	{
		char ip[16];
		sprintf_s(ip, "%u.%u.%u.%u", byte1, byte2, byte3, byte4);

		str = ip;

		return;
	}

	bool setAddress(const string& str)
	{
		vector<string> vct;
		split(str, ".", vct);

		if (vct.size() != 4 || vct[0] == "0")
		{
			return false;
		}
		else
		{
			byte1 = atoi(vct[0].c_str());
			byte2 = atoi(vct[1].c_str());
			byte3 = atoi(vct[2].c_str());
			byte4 = atoi(vct[3].c_str());

			return true;
		}
	}
};

#endif /* BASE_MATH_H_HEADER_INCLUDED_B8E468C8 */


