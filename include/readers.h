#pragma once
#include <iostream>
#include <vector>
using namespace std;
#include <Eigen/Eigen>
#include <pcl/io/pcd_io.h>
#include "BaseDataDefine.h"


struct Label3D {
	string classification;
	struct Centroid {
		float x, y, z;
	}centroid;
	struct Dimension {
		float l, w, h;
	}dimension;
	float yaw;
	bool predicted = false;
};



