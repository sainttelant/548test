#ifndef _A_B_C_CRecvBuf_1_2_3_
#define _A_B_C_CRecvBuf_1_2_3_

#define SOCKET_BUFSIZE 1024 * 1024 * 10
using namespace std;


class CRecvBuf
{
public:
	CRecvBuf()
	{
		pEnd = buf;
		len = 0;
	}

	void Recv(const char* _buf, size_t _len) {
		if (_len > 0)
		{
			memcpy(pEnd, _buf, _len);
			len += _len;
			pEnd = buf + len;
		}

		return;
	}

	size_t Del(int lenNow)
	{
		len -= lenNow;
		memmove(buf, buf + lenNow, len);
		pEnd = buf + len;
		return len;
	}

	void Reset()
	{
		memset(buf, 0, SOCKET_BUFSIZE);
		pEnd = buf;
		len = 0;
	}

	size_t	len;
	char*		pEnd;
	char		buf[SOCKET_BUFSIZE];
};

#endif

