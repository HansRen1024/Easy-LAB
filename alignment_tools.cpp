#include <string>
#include <vector>
#include "opencv2/opencv.hpp"
#include "caffe/caffe.hpp"
using namespace std;
using namespace cv;
using namespace caffe;
inline void Copy(const cv::Mat& cv_mat, caffe::Blob<float>& blob) {
	blob.Reshape(1, cv_mat.channels(), cv_mat.rows, cv_mat.cols);
	if(cv_mat.channels()==1) {
		memcpy(blob.mutable_cpu_data(), cv_mat.data, blob.count()*sizeof(float));
	}
	else {
		std::vector<cv::Mat> splited_img;
		cv::split(cv_mat, splited_img);
		for(size_t i=0; i<splited_img.size(); i++)
			memcpy(blob.mutable_cpu_data()+blob.count(2)*i, splited_img[i].data, blob.count(2)*sizeof(float));
	}
}
template <typename T>
inline std::vector<T> ToVector(const caffe::Blob<T>& blob) {
	std::vector<T> vec(blob.count());
	memcpy(vec.data(), blob.cpu_data(), vec.size()*sizeof(T));
	return vec;
}

inline std::vector<cv::Point2f> ToPoints(const std::vector<float>& labels) {
	std::vector<cv::Point2f> points(labels.size()/2);
	for(size_t i=0; i<points.size(); i++) {
		points[i].x=labels[2*i];
		points[i].y=labels[2*i+1];
	}
	return points;
}
void ConvertImageToGray(Mat &image) {
	if(image.channels()==1)
		return;
	else if(image.channels()==3) {
		cvtColor(image, image, CV_BGR2GRAY);
		return;
	}
}
void NormalizeImage(Mat &img) {
	Mat mean;
	Mat std;
	meanStdDev(img, mean, std);
	for(int i=0;i<img.channels();i++) {
		if(std.at<double>(i, 0)<1E-6)
			std.at<double>(i, 0)=1;
	}
	vector<Mat> split_img(img.channels());
	split(img, &(split_img[0]));
	for(int i=0;i<img.channels();i++)
		split_img[i].convertTo(split_img[i], CV_32F, 1.0/std.at<double>(i, 0), -1*mean.at<double>(i, 0)/std.at<double>(i, 0));
	merge(&(split_img[0]), img.channels(), img);
}
int main(void) {
    string FLAGS_model_path="./models/";
    caffe::Caffe::set_mode(caffe::Caffe::CPU);
    string imgPath = "./faceImg.jpg";
    caffe::shared_ptr<Net<float>> deep_align;
    caffe::shared_ptr<Blob<float>> input_blob;
    caffe::shared_ptr<Blob<float>> output_blob;
    deep_align.reset(new Net<float>(FLAGS_model_path+"rel.prototxt", TEST));
    deep_align->CopyTrainedLayersFrom(FLAGS_model_path+"model.bin");
    input_blob=deep_align->blob_by_name("data");
    output_blob=deep_align->blob_by_name("result");
    Mat image=imread(imgPath);
    cv::resize(image,image,cv::Size(256,256));
    Mat show = image.clone();
    ConvertImageToGray(image);
    image.convertTo(image, CV_32F);
    NormalizeImage(image);
    Copy(image, *(input_blob));
    deep_align->Forward();
    vector<Point2f> landmark_98pt=ToPoints(ToVector(*(output_blob)));
    for(uint i=0; i<landmark_98pt.size(); i++){
        cv::circle(show,cv::Point(int(landmark_98pt[i].x),int(landmark_98pt[i].y)),0.1,cv::Scalar(0,255,0), 4, 8, 0);
    }
    imshow("tracking", show);
    waitKey(0);
    return 0;
}
