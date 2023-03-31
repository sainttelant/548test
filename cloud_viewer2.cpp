#include <pcl/visualization/cloud_viewer.h>
#include <iostream>
#include <fstream>
#include <pcl/io/io.h>
#include <pcl/io/pcd_io.h>
#include <Winsock2.h>
using namespace std;
#include "readers.h"
#include "BaseMath.h"

#include <ecal/ecal.h>
#include <ecal/msg/string/subscriber.h>
eCAL::string::CSubscriber<string>* ecal_sub = nullptr;


void show(const pcl::PointCloud<pcl::PointXYZI>::ConstPtr& cloud, const vector<Label3D>& objs, const vector<Label3D>& predicted, pcl::visualization::PCLVisualizer& viz) {
    viz.removeAllShapes();

    pcl::visualization::PointCloudColorHandlerGenericField<pcl::PointXYZI> color_handler(cloud, "intensity");
    if (!viz.updatePointCloud<pcl::PointXYZI>(cloud, color_handler, "cloud")) {
        viz.addPointCloud<pcl::PointXYZI>(cloud, color_handler, "cloud");
        viz.resetCameraViewpoint("cloud");
    }

    vector<Label3D> bboxes;
    bboxes.insert(bboxes.end(), objs.begin(), objs.end());
    bboxes.insert(bboxes.end(), predicted.begin(), predicted.end());
    int count = 0;
    static char buf[64];
    for (auto& obj : bboxes) {
        count++;
        snprintf(buf, sizeof(buf), "bbox:%d", count);
        const string id = buf;

        Eigen::Vector3f centroid(obj.centroid.x, obj.centroid.y, obj.centroid.z);

        Eigen::Matrix3f z_rotation_matrix = Eigen::Matrix3f::Identity();
        z_rotation_matrix << cos(obj.yaw), sin(obj.yaw), 0,
                            -sin(obj.yaw), cos(obj.yaw), 0,
                                        0,            0, 1;
        Eigen::Quaternionf bboxQuaternion(z_rotation_matrix);

        viz.addCube(centroid, bboxQuaternion, obj.dimension.l, obj.dimension.w, obj.dimension.h, id);
        viz.setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_REPRESENTATION, pcl::visualization::PCL_VISUALIZER_REPRESENTATION_WIREFRAME, id);
        if (!obj.predicted) {
            viz.setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 1.0, 0.0, 1.0, id);
        }
        else {
            viz.setShapeRenderingProperties(pcl::visualization::PCL_VISUALIZER_COLOR, 0.0, 1.0, 1.0, id);
        }
    }

    snprintf(buf, sizeof(buf), "( %d, %d )", int(objs.size()), int(predicted.size()));
    if (!viz.updateText(string(buf), 0, 36, 15, 0, 1.0, 1.0, "count")) {
        viz.addText(string(buf), 0, 36, 15, 0, 1.0, 1.0, "count");
    }
}

void read_and_show(pcl::visualization::PCLVisualizer& viz, const string& ecal_info) {
    vector<string> infos;
	split(ecal_info, ":", infos);
    const string c_no = infos[0];

    vector<Label3D> predicted;
    vector<string> bboxes;
    if (infos.size() > 1 && infos[1] != "") {
        split(infos[1], ";", bboxes);
        for (auto box_str : bboxes) {
            vector<string> items;
            split(box_str, ",", items);
            if (items.size() == 9) {
                Label3D label3d;
                label3d.classification = items[0]; //number str
                label3d.centroid = { float(atof(items[2].c_str())), float(atof(items[3].c_str())), float(atof(items[4].c_str())) };
                label3d.dimension = { float(atof(items[5].c_str())), float(atof(items[6].c_str())), float(atof(items[7].c_str())) };
                label3d.yaw = float(atof(items[8].c_str()));
                label3d.predicted = true;

                predicted.push_back(label3d);
            }
        }
    }

    string file_path = R"(D:\Data\GitClone\pointcloud\PointPillars\dataset\training\label_2\)" + c_no + ".txt";
    vector<Label3D> elements;
    if (!read_label(file_path.c_str(), elements)) {
        cout << "FFFFFFFFF" << endl;

        return;
    }

    file_path = R"(D:\Data\GitClone\pointcloud\PointPillars\dataset\training\calib\)" + c_no + ".txt";
    Eigen::Matrix3f rotation;
    Eigen::Vector3f translation;
    if (!read_calibration(file_path, rotation, translation)) {
        cout << "WWWWWWWWWW" << endl;

        return;
    }

    vector<Label3D> transformed;
    transform_labels_2_lidar_coordinates(elements, rotation, translation, transformed);

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
    file_path = R"(D:\Data\GitClone\pointcloud\PointPillars\dataset\training\velodyne\)" + c_no + ".bin";
    if (!read_point_cloud(file_path, cloud)) {
        cout << "EEEEEEEEE" << endl;

        return;
    }

    show(cloud, transformed, predicted, viz);

    if (!viz.updateText(c_no, 0, 18, 15, 1.0, 1.0, 1.0, "nnoo")) {
        viz.addText(c_no, 0, 18, 15, 1.0, 1.0, 1.0, "nnoo");
    }
}

void read_and_show(pcl::visualization::PCLVisualizer& viz, bool next=true) {
    static int no = -1;

    if (next) {
        no++;
    }
    else {
        if (no > 0) {
            no--;
        }
    }

    char buf[10];
    snprintf(buf, sizeof(buf), "%06d", no);
    const string c_no = buf;

    string file_path = R"(D:\Data\GitClone\pointcloud\PointPillars\dataset\training\label_2\)" + c_no + ".txt";
    vector<Label3D> elements;
    if (!read_label(file_path.c_str(), elements)) {
        cout << "FFFFFFFFF" << endl;

        return;
    }

    file_path = R"(D:\Data\GitClone\pointcloud\PointPillars\dataset\training\calib\)" + c_no + ".txt";
    Eigen::Matrix3f rotation;
    Eigen::Vector3f translation;
    if (!read_calibration(file_path, rotation, translation)) {
        cout << "WWWWWWWWWW" << endl;

        return;
    }

    vector<Label3D> transformed;
    transform_labels_2_lidar_coordinates(elements, rotation, translation, transformed);

    pcl::PointCloud<pcl::PointXYZI>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZI>);
    file_path = R"(D:\Data\GitClone\pointcloud\PointPillars\dataset\training\velodyne\)" + c_no +".bin";
    if (!read_point_cloud(file_path, cloud)) {
        cout << "EEEEEEEEE" << endl;

        return;
    }


    file_path = R"(D:\Data\GitClone\pointcloud\PointPillars\dataset\test2\predicted\)" + c_no + ".txt";
    vector<Label3D> predicted;
    read_predicted(file_path, predicted);


    show(cloud, transformed, predicted, viz);

    if (!viz.updateText(c_no, 0, 18, 15, 1.0, 1.0, 1.0, "nnoo")) {
        viz.addText(c_no, 0, 18, 15, 1.0, 1.0, 1.0, "nnoo");
    }
}

void keyboard_callback(const pcl::visualization::KeyboardEvent& event, void* viewer_void) {
    pcl::visualization::PCLVisualizer* viewer = static_cast<pcl::visualization::PCLVisualizer*> (viewer_void);
    if (event.keyDown()) {
        const string key = event.getKeySym();
        cout << key << endl;

        if (key == "space" || key == "Next" || key == "Return") {
            read_and_show(*viewer, true);
        }
        else if (key == "Prior") {
            read_and_show(*viewer, false);
        }
        else if (key == "Escape") {
            exit(0);
        }
    }
}

void viewerOneOff(pcl::visualization::PCLVisualizer& viewer)
{
    viewer.setBackgroundColor(0, 0, 0);
    viewer.addCoordinateSystem(4.0);
    //viewer.setFullScreen(true);

    viewer.registerKeyboardCallback(keyboard_callback, (void*)&viewer);

    pcl::PointXYZ o{ 0, 0, 0 };
    viewer.addSphere(o, 0.2, 1, 1, 1, "sphere", 0);
    // std::cout << "i only run once" << std::endl;

    read_and_show(viewer, true);
}

void viewerPsycho(pcl::visualization::PCLVisualizer& viewer)
{
    //read_and_show(viewer, true);
    string message;
    if (ecal_sub->Receive(message, nullptr, 5)) {
        cout << message << endl;
        read_and_show(viewer, message);
    }
}


int main_01(int argc, char** argv)
{
    eCAL::Initialize(argc, argv, "cloud_viewer");
    ecal_sub = new eCAL::string::CSubscriber<string>("pub_point_pillar_infer");

    pcl::visualization::CloudViewer viewer("Cloud Viewer");
    
    //blocks until the cloud is actually rendered
    //viewer.showCloud(cloud, "cloud");
    
    //use the following functions to get access to the underlying more advanced/powerful
    //PCLVisualizer
    
    //This will only get called once
    viewer.runOnVisualizationThreadOnce (viewerOneOff);
    
    //This will get called once per visualization iteration
    viewer.runOnVisualizationThread (viewerPsycho);
    while (!viewer.wasStopped ())
    {
    //you can also do cool processing here
    //FIXME: Note that this is running in a separate thread from viewerPsycho
    //and you should guard against race conditions yourself...
    }

    return 0;
}


