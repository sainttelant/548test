#ifndef _ABC_BASE_DATA_DEFIND_243GAL_GAPO_
#define _ABC_BASE_DATA_DEFIND_243GAL_GAPO_

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
using namespace std;

#ifdef WIN32
#include <windows.h>
#pragma warning(disable : 4786)
#else
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/ipc.h>
//#include <sys/msg.h>
//#include <netinet/in.h>
#endif

typedef unsigned short u_short;
typedef unsigned char u_char;
typedef unsigned int u_int;
#include <string>
using namespace std;

#pragma pack(1)

#ifndef WIN32
typedef long long INT64;
typedef unsigned long long u_int64;
typedef int SOCKET;
#else
typedef __int64 INT64;
typedef unsigned __int64 u_int64;
#endif

#pragma pack()


#endif //_ABC_BASE_DATA_DEFIND_243GAL_GAPO_


