#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>


using namespace std;

inline void validateArgs(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Wrong number of arguments, usage " << argv[0] << " data_path" << endl;
        exit(1);
    }
}

struct Data_t{
    Data_t(double t, string str){
        timestamp = t;
        img_path = str;
    }

    double timestamp;
    string img_path;
};

struct RGBD{
    double timestamp;
    cv::Mat rgb;
    cv::Mat depth;
};

using FileList = std::vector<Data_t>;

struct Setting{
    float fx;// = fSettings["Camera.fx"];
    float fy;// = fSettings["Camera.fy"];
    float cx;// = fSettings["Camera.cx"];
    float cy;// = fSettings["Camera.cy"];
    cv::Mat K;// = cv::Mat::eye(3,3,CV_32F);
//    K.at<float>(0,0) = fx;
//    K.at<float>(1,1) = fy;
//    K.at<float>(0,2) = cx;
//    K.at<float>(1,2) = cy;
//    cv::Mat DistCoef(4,1,CV_32F);
//    DistCoef.at<float>(0) = fSettings["Camera.k1"];
//    DistCoef.at<float>(1) = fSettings["Camera.k2"];
//    DistCoef.at<float>(2) = fSettings["Camera.p1"];
//    DistCoef.at<float>(3) = fSettings["Camera.p2"];

};


inline void skip_file_comments(std::ifstream& in){
    while(!in.eof()){
        string str;
        getline(in, str);
        if(str.empty())
            break;
        if(str[0] == '#')
            continue;
        else
            break;
    }
}

inline bool load_filelist_data(const string& path, std::vector<Data_t>& datafile_list, int max_read = INT_MAX){
    ifstream in(path);
    datafile_list.clear();

    if(in.good()){

        // skip comments
        skip_file_comments(in);

        // read content
        for(int i=0; i<max_read; i++){
            if(in.eof()){
                cout << "end of file." <<endl;
                break;
            }

            string str;
            getline(in, str);
            if(str.empty()){
                cout << "read empty line." <<endl;
                break;
            }

            stringstream ss;
            ss << str;
            double t;
            string str_file;
            ss >> t;
            ss >> str_file;
            datafile_list.push_back(Data_t(t, str_file));
        }
        return true;
    }else{
        cerr << "Fail to open file: " << path <<endl;
        return false;
    }
}

inline void rgbd_to_point_cloud(){
//    {
//        // Without LUT:
//        const float r_cx =  src_obs.cameraParams.cx();
//        const float r_cy = src_obs.cameraParams.cy();
//        const float r_fx_inv = 1.0f/src_obs.cameraParams.fx();
//        const float r_fy_inv = 1.0f/src_obs.cameraParams.fy();
//        size_t idx=0;
//        for (int r=0;r<H;r++)
//            for (int c=0;c<W;c++)
//            {
//                const float D = src_obs.rangeImage.coeff(r,c);
//                if (D!=.0f) {
//                    const float Kz = (r_cy - r) * r_fy_inv;
//                    const float Ky = (r_cx - c) * r_fx_inv;
//                    pca.setPointXYZ(idx,
//                        D,        // x
//                        Ky * D,   // y
//                        Kz * D    // z
//                        );
//                    src_obs.points3D_idxs_x[idx]=c;
//                    src_obs.points3D_idxs_y[idx]=r;
//                    ++idx;
//                }
//            }
//        pca.resize(idx); // Actual number of valid pts
//    }
}

int main(int argc, char** argv){
    const int NUM_IMG = 100;
    validateArgs(argc, argv);
    string data_path = argv[1];
    cout << "load rgbd data from path: " << data_path <<endl;

    string rgb_file_name = data_path + "/rgb.txt";
    string depth_file_name = data_path + "/depth.txt";

    FileList rgb_data_list;
    FileList depth_data_list;
    if(load_filelist_data(rgb_file_name, rgb_data_list, NUM_IMG)){
        cout << "load rgb file list done, total lines: " << rgb_data_list.size() <<endl;
    }
    if(load_filelist_data(depth_file_name, depth_data_list, NUM_IMG)){
        cout << "load depth file list done, total lines: " << depth_data_list.size() <<endl;
    }

    const double depth_factor = 1.0/5000.;
    cv::Mat img_rgb, img_depth;
    for(int i=50; i<NUM_IMG; i++){
        img_rgb = cv::imread(data_path + "/" + rgb_data_list[i].img_path, cv::IMREAD_UNCHANGED);
        img_depth = cv::imread(data_path + "/" + depth_data_list[i].img_path, cv::IMREAD_UNCHANGED);
        cv::imshow("rgb", img_rgb);
        cv::imshow("depth", img_depth);

        if((fabs(depth_factor-1.0f)>1e-5) || img_depth.type()!=CV_32F)
            img_depth.convertTo(img_depth,CV_32F, depth_factor);

        std::printf("center z: %f \n", img_depth.at<float>(img_depth.rows/2, img_depth.cols/2));

        if(cv::waitKey()==27) break;
    }
    return 0;
}
