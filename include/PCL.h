#pragma once
#include <vector>
using namespace std;

struct Point {
  Point(float _x, float _y, float _z, float _intensity) {
    x = _x;
    y = _y;
    z = _z;
    intensity = _intensity;
  }
  float x, y, z, intensity;
};

void init_pcl();
void show(const vector<Point>& pts);


