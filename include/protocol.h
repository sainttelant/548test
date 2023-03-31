#pragma once


enum class EMethodId {
  detection_list = 336,
  object_list = 329,
};

#pragma pack(1)

struct SomeIPHeader {
  u_short service_id;
  u_short method_id;
  u_long length;
  u_short client_id;
  u_short session_id;
  u_char someip_version;
  u_char interface_version;
  u_char message_type;
  u_char return_code;
};

struct E2EP07Header {
  u_int64 crc;
  u_int length;
  u_int sqc;
  u_int data_id;
};

struct DetectionListHead {
  u_int timestamp_nanoseconds;
  u_int timestamp_seconds;
  u_char timestamp_sync_status;
  u_int event_data_qualifier;
  u_char extend_qualifier;
  u_short origin_invalid_flags;
  float origin_x_pos;
  float origin_x_std;
  float origin_y_pos;
  float origin_y_std;
  float origin_z_pos;
  float origin_z_std;
  float origin_roll;
  float origin_roll_std;
  float origin_pitch;
  float origin_pitch_std;
  float origin_yaw;
  float origin_yaw_std;
  u_char list_invalid_flags;
};

struct DetectionListTail {
  float list_rad_vel_domain_min;
  float list_rad_vel_domain_max;
  u_int list_num_of_detections;
  float ain_azimuth_correction;
  float ain_elevation_correction;
};

struct Detection {
  float azimuth_angle;
  float azimuth_angle_std;
  u_char invalid_flags;
  float elevation_angle;
  float elevation_angle_std;
  float range;
  float range_std;
  float range_rate;
  float range_rate_std;
  char rcs;
  u_short measurement_id;
  u_char postive_predictive_value;
  u_char classfication;
  u_char multi_targe_probability;
  u_short object_id;
  u_char ambiguity_flag;
  u_short sort_index;
};

struct ObjectListHead {
  u_int timestamp_nanoseconds;
  u_int timestamp_seconds;
  u_char timestamp_sync_status;
  u_int event_data_qualifier;
  u_char extend_qualifier;
  u_char object_list_num_of_objects;
};

struct Object {
	unsigned short u_StatusSensor;
	unsigned int u_ID;
	unsigned short u_Age;
	unsigned char u_StatusMeasurement;
	unsigned char u_StatusMovement;
	unsigned short u_Position_InvalidFlags;
	unsigned char u_Position_Reference;
	float u_Position_X;
	float u_Position_X_STD;
	float u_Position_Y;
	float u_Position_Y_STD;
	float u_Position_Z;
	float u_Position_Z_STD;
	float u_Position_ConvarianceXY;
	float u_Position_Orientation;
	float u_Position_Orientation_STD;
	unsigned char u_Existence_InvalidFlags;
	float u_Existence_Probability;
	float u_Existence_PPV;
	unsigned char u_Classification_Car;
	unsigned char u_Classification_Truck;
	unsigned char u_Classification_Motorcycle;
	unsigned char u_Classification_Bicycle;
	unsigned char u_Classification_Pedestrian;
	unsigned char u_Classification_Animal;
	unsigned char u_Classification_Hazard;
	unsigned char u_Classification_Unknown;
	unsigned char u_Classification_Overdrivable;
	unsigned char u_Classification_Underdrivable;
	unsigned char u_Dynamics_AbsVel_InvalidFlags;
	float f_Dynamics_AbsVel_X;
	float f_Dynamics_AbsVel_X_STD;
	float f_Dynamics_AbsVel_Y;
	float f_Dynamics_AbsVel_Y_STD;
	float f_Dynamics_AbsVel_ConvarianceXY;
	unsigned char u_Dynamics_RelVel_InvalidFlags;
	float f_Dynamics_RelVel_X;
	float f_Dynamics_RelVel_X_STD;
	float f_Dynamics_RelVel_Y;
	float f_Dynamics_RelVel_Y_STD;
	float f_Dynamics_RelVel_ConvarianceXY;
	unsigned char u_Dynamics_AbsAccel_InvalidFlags;
	float f_Dynamics_AbsAccel_X;
	float f_Dynamics_AbsAccel_X_STD;
	float f_Dynamics_AbsAccel_Y;
	float f_Dynamics_AbsAccel_Y_STD;
	float f_Dynamics_AbsAccel_ConvarianceXY;
	unsigned char u_Dynamics_RelAccel_InvalidFlags;
	float f_Dynamics_RelAccel_X;
	float f_Dynamics_RelAccel_X_STD;
	float f_Dynamics_RelAccel_Y;
	float f_Dynamics_RelAccel_Y_STD;
	float f_Dynamics_RelAccel_ConvarianceXY;
	unsigned char u_Dynamics_Orientation_InvalidFlags;
	float u_Dynamics_Orientation__Rate_Mean;
	float u_Dynamics_Orientation__Rate_STD;
	unsigned int  u_Shape_Length_Status;
	unsigned char u_Shape_Length_Edge_InvalidFlags;
	float u_Shape_Length_Edge_Mean;
	float u_Shape_Length_Edge_STD;
	unsigned int u_Shape_Width_Status;
	unsigned char u_Shape_Width_Edge_InvalidFlags;
	float u_Shape_Width_Edge_Mean;
	float u_Shape_Width_Edge_STD;
};



typedef struct ip_address_t
{
	u_char byte1;
	u_char byte2;
	u_char byte3;
	u_char byte4;
}ip_address_t;

typedef struct ip_header
{
	u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
	u_char	tos;			// Type of service 
	u_short tlen;			// Total length 
	u_short identification; // Identification
	u_char flags0;
	u_char flags1_fo;		// Flags (3 bits) + Fragment offset (13 bits)
	u_char	ttl;			// Time to live
	u_char	proto;			// Protocol
	u_short crc;			// Header checksum
	ip_address_t	saddr;		// Source address
	ip_address_t	daddr;		// Destination address
	u_int	op_pad;			// Option + Padding
}ip_header;

typedef struct udp_header
{
	u_short sport;			// Source port
	u_short dport;			// Destination port
	u_short len;			// Datagram length
	u_short crc;			// Checksum
}udp_header;

#pragma pack()


