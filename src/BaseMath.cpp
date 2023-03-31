#include "BaseMath.h"
#include <iostream>
using namespace std;
#include<WinSock2.h>

void getNumber(const char* pBuf, int len, INT64& number)
{
	if (len >= sizeof(INT64))
	{
		len = sizeof(INT64);
	}
	else if (len <= 0)
	{
		len = 0;
	}

	memset(&number, 0, sizeof(INT64));
	memcpy(&number, pBuf, len);

	return;
};

void getNumberString(const char* pBuf, int len, string& str)
{
	u_int num = 0;
	bool isNum = false;
	
	if (1 == len)
	{
		u_char c = *pBuf;
		num = (int)c;

		isNum = true;
	}
	else if (2 == len)
	{
		u_short* pUs = (u_short*)pBuf;
		num = ntohs(*pUs);

		isNum = true;
	}
	else if (4 == len)
	{
		num = ntohl(*((u_int*)pBuf));

		isNum = true;
	}
	else if (3 == len)
	{
		union U4{
			u_int	num;
			u_char	buf[4];
		}u4;

		u4.num = 0;
		
		u4.buf[0] = pBuf[2];
		u4.buf[1] = pBuf[1];
		u4.buf[2] = pBuf[0];

		num = u4.num;

		isNum = true;
	}
	else
	{
		isNum = false;
	}

	if (isNum)
	{
		char cBuf[12];
		memset(cBuf, 0, sizeof(cBuf));
		sprintf_s(cBuf, "%u", num);

		str = cBuf;
	}
	else
	{
		str = "";

		cout << "[debugInfo]:getNumberString" << endl;
	}

	return;
}

void getString(const char* pBuf, int len, string& str, bool bA/* = false*/)
{
	str = "";

	const unsigned char* pUBuf = (unsigned char*)pBuf;
	char dstContent[4];

	for(int i = 0; i < len; i++)
	{
		unsigned char c = pUBuf[i];
		if (bA)
		{
			char t1 = c, t2 = c;
			t1 = t1<<4;
			t2 = t2>>4;
			t2 = t2&0x0F;
			c = t2|t1;
		}

		sprintf_s(dstContent, "%02X", c);

		str += dstContent;
	}

	return;
}

void getString(const INT64& number, string& str)
{
	const int c_iMaxLen = sizeof(INT64);
	char cBuf[c_iMaxLen];
	memcpy(cBuf, &number, c_iMaxLen);

	int len = 0;
	for(int i = 0; i < c_iMaxLen; i++)
	{
		if (0 != cBuf[i])
		{
			len = i + 1;
		}
	}

	getString(cBuf, len, str, false);

	return;
}

void split1(const string& src, const string& flag, vector<string>& array, const int num/* = 1*/)
{
	int myNum = 0;

	string mystr;
	int begin=0;
	int end=src.length();
    
    mystr=src.substr(begin,end-begin+1);
    
	int prePos = 0;
	int nextPos = mystr.find(flag);
	while (nextPos != mystr.npos)
	{
		if ((mystr.substr(prePos,nextPos-prePos)).c_str()!="")
		{
			array.push_back(mystr.substr(prePos,nextPos-prePos));
		}
		
		prePos = nextPos + flag.length();
		if (num > ++myNum)
		{
			nextPos = mystr.find(flag,prePos);
		}
		else
		{
			nextPos = mystr.npos;
			break;
		}
	}
	array.push_back(mystr.substr(prePos, nextPos-prePos));
}

void split(const string& src, const string& flag, vector<string>& array)
{
	/*if (src.length()==0)
	{
		return;
	}
	if (src.find(flag)==src.npos) 
	{
		array.push_back(src);
		return;
	}*/
	string mystr;
	int begin=0;
	int end=src.length();
	
	/*for (int i=0;i<src.length();i++)//去掉前面的空格
	{
		if (src[i]!=' ')
		{
			begin=i;
			break;
		}
	}
	for (int j=src.length()-1;j>0;j--)//去掉后面的空格和flag
	{
		
		if (src[j]!=flag[0] &&src[j]!=' ')
		{
			end=j;
			break;
		}
	}*/
    
    mystr=src.substr(begin,end-begin+1);
    
	int prePos = 0;	
	int nextPos = mystr.find(flag);
	while (nextPos != mystr.npos)
	{
		if ((mystr.substr(prePos,nextPos-prePos)).c_str()!="")
		{
			array.push_back(mystr.substr(prePos,nextPos-prePos));
		//	cout<<(mystr.substr(prePos,nextPos-prePos)).c_str()<<endl;
		}
		
		prePos = nextPos + flag.length();
		nextPos = mystr.find(flag,prePos);
	}
	array.push_back(mystr.substr(prePos, nextPos-prePos));
	
	/*if ((src.substr(prePos,mystr.npos-prePos)).c_str()!="")
	{
		array.push_back(mystr.substr(prePos,mystr.npos-prePos));
	//	cout<<(mystr.substr(prePos,mystr.npos-prePos)).c_str()<<endl;
	}*/

}

void getHexSrcMsg(const char* pSrcMsg, int len, string& str)
{
	str = "";

	const unsigned char* pUBuf = (unsigned char*)pSrcMsg;
	char dstContent[4];

	for(int i = 0; i < len; i++)
	{
		unsigned char c = pUBuf[i];

		sprintf_s(dstContent, "%02X ", c);

		if (0 == i % 16)
		{
			str += "\r\n";
		}

		str += dstContent;
	}

	str += "\r\n";

	return;
}

void getPCString(const char* pBuf, int len, string& str)
{
	if (3 != len)
	{
		return;
	}

	char pc[16];
	sprintf_s(pc, "%u-%u-%u", 
		((u_char)(pBuf[2])), 
		((u_char)(pBuf[1])), 
		((u_char)(pBuf[0])));

	str = pc;

	return;
}

void getBinString(const char* pBuf, int len, string& str)
{
	return;
}

void bitMoveTowardRight(u_char* buf, int len, int pos)
{
	u_char cin = 0xFF >> (8-pos);

	u_char c = 0x00;
	for(int i=0; i < len; i++)
	{
		u_char c1 = buf[i]&cin;

		buf[i] = (buf[i]>>pos)|(c<<(8-pos));

		c = c1;
	}

	return;
}

void bitCleanLeft(u_char* buf, int lenBitClean)
{
	buf[0] = buf[0]&(0xFF>>lenBitClean);

	return;
}

void getHexSrcMsg1(const char* pSrcMsg, int len, string& str)
{
	str = "";

	const unsigned char* pUBuf = (unsigned char*)pSrcMsg;
	char dstContent[4];

	for(int i = 0; i < len; i++)
	{
		unsigned char c = pUBuf[i];

		sprintf_s(dstContent, "%02X ", c);

		str += dstContent;
	}

	str += "\r\n";

	return;
}

float my_ntohf(float a)
{
	union {
		u_char bytes[4];
		float f;
	} b;

	b.f = a;
	std::reverse(b.bytes, b.bytes + 4);

	return b.f;
}

u_int64 htonl64(u_int64 hostint64)
{
	u_int64 iLow = htonl(u_int(hostint64 >> 32));
	u_int64 iHigh= htonl(u_int(hostint64 & 0x00000000FFFFFFFF));

	return (iHigh << 32) | iLow;
}

u_int64 ntohl64(u_int64 hostint64)
{
	u_int64 iLow = htonl(u_int(hostint64 >> 32));
	u_int64 iHigh= htonl(u_int(hostint64 & 0x00000000FFFFFFFF));

	return (iHigh << 32) | iLow;
}

time_t now_milliseconds() {
	auto now = chrono::system_clock::now();
	auto ms = chrono::time_point_cast<chrono::milliseconds>(now);
	auto ms_duration = chrono::duration_cast<chrono::milliseconds>(ms.time_since_epoch());

	return ms_duration.count();
};


