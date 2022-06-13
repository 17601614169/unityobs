
#include <opencv2/opencv.hpp>
class _declspec(dllexport) BackGroundMatting{
    public:
        cv::Mat pha;
        void init(int thread_num);
        void detect(const cv::Mat& mat, float downsample_ratio = 0.25f);
};