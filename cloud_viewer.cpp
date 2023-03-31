#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#include "protocol.h"
#include "BaseMath.h"
#include "PCL.h"
#include "MyEcal.h"


#include <chrono>
#include <thread>
#define sleep_ms(ms) this_thread::sleep_for(chrono::milliseconds(ms))

#include <pcap.h>
bool g_run = false;


char g_buf[102400] = { 0 };
int g_len = 0;
float min_detection_velocity = 0;


float _ntohf(float a) {
	std::reverse((char*)&a, ((char*)&a) + 4);

	return a;
}

int gettimeofday(struct timeval* tp, void* tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}

long long get_now_ms()
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int decode(char* start, int len, long long time_ms) {
	SomeIPHeader* sh = (SomeIPHeader*)start;
	start += sizeof(SomeIPHeader);
	len -= sizeof(SomeIPHeader);
	sh->method_id = ntohs(sh->method_id);
	sh->length = ntohl(sh->length);
	if (sh->method_id != u_short(EMethodId::detection_list)
		&& sh->method_id != u_short(EMethodId::object_list)
		&& sh->method_id != 0) {

		return 0;
	}

	E2EP07Header* eh = (E2EP07Header*)start;
	start += sizeof(E2EP07Header);
	len -= sizeof(E2EP07Header);

	//printf("recv data:%d, %d, %d\n", len, sh->method_id, eh->data_id);

	switch (EMethodId(sh->method_id)) {
	case EMethodId::detection_list:
	{
		static long long cycle_from = time_ms;
		static int cycle_count = 0;
		cycle_count += 1;
		if (time_ms - cycle_from >= 1000) {
			printf("Detection FPS: %d\n", cycle_count);
			cycle_from = time_ms;
			cycle_count = 0;
		}

		DetectionListHead* dh = (DetectionListHead*)start;
		start += sizeof(DetectionListHead);
		len -= sizeof(DetectionListHead);

		dh->timestamp_seconds = ntohl(dh->timestamp_seconds);
		dh->origin_invalid_flags = ntohs(dh->origin_invalid_flags);
		dh->origin_x_pos = _ntohf(dh->origin_x_pos);
		dh->origin_y_pos = _ntohf(dh->origin_y_pos);
		dh->origin_z_pos = _ntohf(dh->origin_z_pos);
		dh->origin_roll = _ntohf(dh->origin_roll);
		dh->origin_pitch = _ntohf(dh->origin_pitch);
		dh->origin_yaw = _ntohf(dh->origin_yaw);

		DetectionListTail* dt = (DetectionListTail*)(start + 35200);
		dt->list_num_of_detections = ntohl(dt->list_num_of_detections);
		/*
		printf("DN:%3d, %10.4f,%10.4f,%10.4f,%10.4f,%10.4f,%10.4f\n", dt->list_num_of_detections,
			dh->origin_x_pos, dh->origin_y_pos, dh->origin_z_pos,
			dh->origin_roll, dh->origin_pitch, dh->origin_yaw);
		*/

		vector<Point> pts;
		int size = min(dt->list_num_of_detections, 800);
		pts.reserve(size);
		for (int i = 0; i < size; i++) {
			Detection* detection = (Detection*)start;
			start += sizeof(Detection);
			len -= sizeof(Detection);

			detection->azimuth_angle = _ntohf(detection->azimuth_angle);
			detection->elevation_angle = _ntohf(detection->elevation_angle);
			detection->range = _ntohf(detection->range);
			detection->range_rate = _ntohf(detection->range_rate);

			float z = sin(detection->elevation_angle) * detection->range;
			float xy = cos(detection->elevation_angle) * detection->range;
			float x = cos(detection->azimuth_angle) * xy;
			float y = sin(detection->azimuth_angle) * xy;
			//x and y used coordinates of the 408

			if (abs(detection->range_rate) >= min_detection_velocity) {
				pts.emplace_back(Point(y, x, z, float(int(detection->rcs))));  //x and y of PCL used standard coordinates default(x is horizon, y is vertical)
			}

			//printf("%3d: %10.4f, %10.4f, %10.4f\n", i, x, y, z);
		}

		show(pts);
	}
	break;

	case EMethodId::object_list:
	{
		static long long cycle_from = time_ms;
		static int cycle_count = 0;
		cycle_count += 1;
		if (time_ms - cycle_from >= 1000) {
			printf("Object FPS: %d\n", cycle_count);
			cycle_from = time_ms;
			cycle_count = 0;
		}

		ObjectListHead* oh = (ObjectListHead*)start;
		start += sizeof(ObjectListHead);
		len -= sizeof(ObjectListHead);

		//printf("ON:%d\n", u_int(oh->object_list_num_of_objects));

		string pub_msg = "";
		char ecal_line_buf[128] = { 0 };
		for (int i = 0; i < min(oh->object_list_num_of_objects, 50); i++) {
			Object* obj = (Object*)start;
			start += sizeof(Object);
			len -= sizeof(Object);

			obj->u_ID = ntohl(obj->u_ID);
			obj->u_Position_X = _ntohf(obj->u_Position_X);
			obj->u_Position_Y = _ntohf(obj->u_Position_Y);
			obj->u_Position_Z = _ntohf(obj->u_Position_Z);
			obj->f_Dynamics_RelVel_X = _ntohf(obj->f_Dynamics_RelVel_X);
			obj->f_Dynamics_RelVel_Y = _ntohf(obj->f_Dynamics_RelVel_Y);
			obj->u_Shape_Length_Edge_Mean = _ntohf(obj->u_Shape_Length_Edge_Mean);
			obj->u_Shape_Width_Edge_Mean = _ntohf(obj->u_Shape_Width_Edge_Mean);
			obj->u_Existence_Probability = _ntohf(obj->u_Existence_Probability);
			//obj-> = _ntohf(obj->);
			//obj-> = _ntohf(obj->);

			snprintf(ecal_line_buf, sizeof(ecal_line_buf), "%d,%.2f,%.2f,%.2f,%.2f,%.2f,%d,%d,%d,%d,%.2f,%.2f,%d\n",
				obj->u_ID, obj->u_Position_X, obj->u_Position_Y, obj->u_Position_Z, obj->f_Dynamics_RelVel_X, obj->f_Dynamics_RelVel_Y,
				0, 0, int(obj->u_StatusMeasurement), int(obj->u_Existence_Probability),
				obj->u_Shape_Length_Edge_Mean, obj->u_Shape_Width_Edge_Mean, 0);
			pub_msg += ecal_line_buf;

			//printf("%3d: %04d,   %10.4f, %10.4f, %10.4f, %d\n", i, obj->u_ID, obj->u_Position_X, obj->u_Position_Y, obj->u_Position_Z, int(obj->u_Position_Reference));
		}
		if (pub_msg != "") {
			pub_ecal_msg(pub_msg);
		}
	}
	break;

	default:
		return 0;
	}

	return 0;
}

void dispatcher_handler(u_char* temp1,
	const struct pcap_pkthdr* header,
	const u_char* pkt_data)
{
	static long long first_process_time = get_now_ms();
	static long long first_package_time = header->ts.tv_sec * 1000 + header->ts.tv_usec / 1000;
	long long current_process_time = get_now_ms();
	long long current_package_time = header->ts.tv_sec * 1000 + header->ts.tv_usec / 1000;
	while (current_process_time - first_process_time < current_package_time - first_package_time) {
		sleep_ms(1);
		current_process_time = get_now_ms();
	}

	ip_header* ih;
	udp_header* uh;
	u_int ip_len;

	bool last_fragment = false;
	ih = (ip_header*)(pkt_data + 14); //length of ethernet header
	if ((ih->flags0 & 0x20) == 0x00) {
		last_fragment = true;
		if (!g_run) {
			g_run = true;
			return;
		}
	}

	if (!g_run) {
		return;
	}

	/*
	u_short offset = 0;
	memcpy(&offset, &(ih->flags0), sizeof(offset));
	offset = ntohs(offset);
	offset = offset << 3;
	*/

	ip_len = (ih->ver_ihl & 0xf) * 4;
	ih->tlen = ntohs(ih->tlen);
	//memcpy(g_buf + offset, (char*)ih + ip_len, ih->tlen - ip_len);
	memcpy(g_buf + g_len, (char*)ih + ip_len, ih->tlen - ip_len);
	g_len += (ih->tlen - ip_len);
	if (!last_fragment) {
		return;
	}

	uh = (udp_header*)g_buf;
	char* start = g_buf + sizeof(udp_header);
	int len = g_len - sizeof(udp_header);
	if (len == 9393) {
		start = g_buf;
		len += 8;
	}
	
	decode(start, len, current_package_time);

	g_len = 0;
}

int main(int argc, char** argv)
{
	string file_name = "";
	for (int i = 0; i < argc; i++) {
		if (string(argv[i]) == "-velocity") {
			min_detection_velocity = atof(argv[i + 1]);
		}
		else if (string(argv[i]) == "-f") {
			file_name = argv[i + 1];
		}
	}

	init_pcl();
	init_ecal(argc, argv);

	if (file_name != "") {
		char packet_filter[] = "dst host 224.0.2.2";
		struct bpf_program fcode;
		pcap_t* fp;
		char errbuf[PCAP_ERRBUF_SIZE];
		u_int netmask = 0xffffff;

		if ((fp = pcap_open_offline(file_name.c_str(), errbuf)) == NULL)
		{
			fprintf(stderr, "\nUnable to open the file %s.\n", file_name.c_str());
			return -1;
		}

		if (pcap_compile(fp, &fcode, packet_filter, 1, netmask) < 0)
		{
			fprintf(stderr, "\nUnable to compile the packet filter. Check the syntax.\n");
			return -1;
		}

		if (pcap_setfilter(fp, &fcode) < 0)
		{
			fprintf(stderr, "\nError setting the filter.\n");
			return -1;
		}

		pcap_loop(fp, 0, dispatcher_handler, NULL);

		pcap_close(fp);

		return 0;
	}
	else {
		int iRet = 0;
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);

		SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		//addr.sin_addr.S_un.S_addr = inet_addr("10.13.1.113");
		addr.sin_port = htons(42102);

		bool bOptval = true;
		iRet = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&bOptval, sizeof(bOptval));
		if (iRet != 0) {
			printf("setsockopt fail:%d", WSAGetLastError());
			return -1;
		}

		iRet = bind(sock, (sockaddr*)&addr, sizeof(addr));
		if (iRet != 0) {
			printf("bind fail:%d", WSAGetLastError());
			return -1;
		}
		printf("socket:%d bind success\n", sock);

		// ¼ÓÈë×é²¥  
		ip_mreq multiCast;
		multiCast.imr_interface.S_un.S_addr = INADDR_ANY;
		//multiCast.imr_interface.S_un.S_addr = inet_addr("10.13.1.113");
		multiCast.imr_multiaddr.S_un.S_addr = inet_addr("224.0.2.2");
		iRet = setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&multiCast, sizeof(multiCast));
		if (iRet != 0) {
			printf("setsockopt fail:%d", WSAGetLastError());
			return -1;
		}

		printf("udp group start\n");

		int _len = sizeof(sockaddr);
		while (true)
		{
			memset(g_buf, 0, sizeof(g_buf));
			g_len = recvfrom(sock, g_buf, sizeof(g_buf) - 1, 0, (sockaddr*)&addr, &_len);
			if (g_len <= 0) {
				printf("recvfrom fail:%d", WSAGetLastError());
				return -1;
			}
			//printf("recv data:%d\n", len);

			decode(g_buf, g_len, get_now_ms());
		}

		closesocket(sock);
		WSACleanup();
	}

  return 0;
}


