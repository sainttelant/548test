#include "PCL.h"

#include <iostream>
using namespace std;

#include <pcl/visualization/cloud_viewer.h>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>

#include <pcl/point_types.h>
#include <pcl/io/ply_io.h>
#include <pcl/visualization/point_cloud_color_handlers.h>



//pcl::visualization::CloudViewer vwr("PCV");
pcl::visualization::PCLVisualizer vwr("PCV");
bool show_scale_ = false;
void show_scale();


void show(const pcl::PointCloud<pcl::PointXYZI>::ConstPtr& cloud, pcl::visualization::PCLVisualizer& viz) {
  viz.removeAllShapes();

  pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> color_handler(cloud, "intensity");
  if (!viz.updatePointCloud<pcl::PointXYZI>(cloud, color_handler, "cloud")) {
    viz.addPointCloud<pcl::PointXYZI>(cloud, color_handler, "cloud");
    viz.resetCameraViewpoint("cloud");
  }
}

void keyboard_callback(const pcl::visualization::KeyboardEvent& event, void* viewer_void) {
  pcl::visualization::PCLVisualizer* viewer = static_cast<pcl::visualization::PCLVisualizer*> (viewer_void);
  if (event.keyDown()) {
    const string key = event.getKeySym();
    cout << key << endl;

    if (key == "Escape") {
      pcl::visualization::Camera camera;
      viewer->getCameraParameters(camera);
      printf("%.9lf,%.9lf,%.9lf,", camera.pos[0], camera.pos[1], camera.pos[2]);
      printf("%.9lf,%.9lf,%.9lf\n", camera.view[0], camera.view[1], camera.view[2]);
      /*
      printf("pos:  %lf,%lf,%lf\n", camera.pos[0], camera.pos[1], camera.pos[2]);
      printf("view: %lf,%lf,%lf\n", camera.view[0], camera.view[1], camera.view[2]);
      printf("focal:%lf,%lf,%lf\n", camera.focal[0], camera.focal[1], camera.focal[2]);
      printf("clip: %lf,%lf\n", camera.clip[0], camera.clip[1]);
      printf("window_size: %lf,%lf\n", camera.window_size[0], camera.window_size[1]);
      printf("window_pos:  %lf,%lf\n", camera.window_pos[0], camera.window_pos[1]);
      printf("fovy: %lf\n", camera.fovy);
      */

      exit(0);
    }
    else if (key == "a") {
      show_scale();
    }
  }
}

void viewerOneOff(pcl::visualization::PCLVisualizer& viewer)
{
  viewer.getRenderWindow()->GlobalWarningDisplayOff();
  viewer.registerKeyboardCallback(keyboard_callback, (void*)&viewer);
  viewer.addCoordinateSystem(10.0, 0, 0, 0);
  for (int i = 0; i <= 30; i++) {
    viewer.addCoordinateSystem(10.0, -30, i*10, 0);
    char txt[16];
    snprintf(txt, sizeof(txt), "%03d", i * 10);
    viewer.addText3D(string(txt), pcl::PointXYZ(-30, i * 10 + 1, 0), 2);;
  }
  /*
  viewer.initCameraParameters();
  pcl::visualization::Camera c;
  c.pos[0] = -285.160568;
  c.pos[1] = 177.278500;
  c.pos[2] = 374.454225;
  c.view[0] = -0.269006;
  c.view[1] = 0.956738;
  c.view[2] = 0.110849;
  c.focal[0] = 18.801006;
  c.focal[1] = 144.762203;
  c.focal[2] = 3.717977;
  c.clip[0] = 16.357253;
  c.clip[1] = 979.353723;
  c.window_size[0] = 1024;
  c.window_size[1] = 576;
  c.window_pos[0] = 0;
  c.window_pos[1] = 0;
  c.fovy = 0.8575;
  viewer.setCameraParameters(c);
  */
  viewer.setCameraPosition(-389.579354, 101.407535, 358.033719, 0.066328, 0.976741, -0.203905);
}

void show_scale() {
  if (!show_scale_) {
    for (int i = 0; i <= 30; i++) {
      vwr.addCoordinateSystem(10.0, -30, i * 10, 0);

      char txt[16];
      snprintf(txt, sizeof(txt), "%03d", i * 10);
      vwr.addText3D(string(txt), pcl::PointXYZ(-30, i * 10 + 1, 0), 2);;
    }
    show_scale_ = true;
  }
  else {
    //vwr.removeCoordinateSystem();
    vwr.removeAllCoordinateSystems();
    show_scale_ = false;
  }
}

void init_pcl() {
  //vwr.runOnVisualizationThreadOnce(viewerOneOff);
  vwr.getRenderWindow()->GlobalWarningDisplayOff();

  vwr.registerKeyboardCallback(keyboard_callback, (void*)&vwr);
  //vwr.setFullScreen(true);

  vwr.addCoordinateSystem(10.0, 0, 0, 0, "orientation");
  show_scale();
  
  //vwr.initCameraParameters();
  //vwr.setCameraPosition(43.385659897, 119.342598298, 391.567467075, -0.008925227, 0.997445438, 0.070872690);
}

void show(const vector<Point>& pts) {
  pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
  cloud->reserve(pts.size());
  for (auto p : pts) {
    cloud->emplace_back(pcl::PointXYZI(p.x, p.y, p.z, p.intensity));
  }

  //vwr.showCloud(cloud, "cloud");
  vwr.removePointCloud();
  pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> rgb(cloud, "intensity");
  vwr.addPointCloud(cloud, rgb);
  vwr.spinOnce();
}


