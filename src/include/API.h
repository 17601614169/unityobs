#pragma once
#if defined(SAPI_LIBRARY_STATIC)
#define SAPI_EXPORT
#elif defined(SAPI_LIBRARY)
#define DLL_EXPORT __declspec(dllexport)
#else
#define DLL_EXPORT __declspec(dllimport)
#endif

#include <opencv2/opencv.hpp>

DLL_EXPORT long SAPI_init();

//setting
DLL_EXPORT int SAPI_enable_human_seg(long ptr, bool cmd);
DLL_EXPORT int SAPI_enable_face_function(long ptr, bool cmd);
DLL_EXPORT int SAPI_set_face_function_shink_size(long ptr, int w, int h);

DLL_EXPORT int SAPI_face_point478_parallel(long ptr, cv::Mat &rgb, bool debug);

DLL_EXPORT int SAPI_get_landmarks(long ptr, float p[468 * 2]);
DLL_EXPORT int SAPI_get_translation_vector(long ptr, float p[3]);
DLL_EXPORT int SAPI_get_rotation_quaternion(long ptr, float p[4]);
DLL_EXPORT int SAPI_human_seg(long ptr, const cv::Mat &rgb, cv::Mat &alpha);
