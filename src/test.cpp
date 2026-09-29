// #include <iostream>
// #include <string>
// #include <opencv2/opencv.hpp>

// void color_filter(cv::Mat& frame)
// {
//     cv::Mat blur;
//     cv::GaussianBlur(
//         frame,
//         blur,
//         cv::Size(3,3),
//         0
//     );
//     cv::Mat hsv;
//     cv::cvtColor(blur,hsv,cv::COLOR_BGR2HSV);
//     cv::Mat mask;
//     int hmin=0,smin=0,vmin=0;
//     int hmax=179,smax=255,vmax=255;
//     namedWindow("Trackbars",cv::WINDOW_NORMAL);
//     cv::resizeWindow("Trackbars",800,600);
//     namedWindow("Mask",cv::WINDOW_NORMAL);
//     cv::resizeWindow("Mask",800,600);
//     cv::createTrackbar("Hue Min","Trackbars",&hmin,179);
//     cv::createTrackbar("Hue Max", "Trackbars", &hmax, 179);       
//     cv::createTrackbar("Sat Min", "Trackbars", &smin, 255);
//     cv::createTrackbar("Sat Max", "Trackbars", &smax, 255);
//     cv::createTrackbar("Val Min", "Trackbars", &vmin, 255);
//     cv::createTrackbar("Val Max", "Trackbars", &vmax, 255); 
//     while(true)
//     {
//         cv::imshow("output",blur);
//         cv::Scalar lower(hmin,smin,vmin);
//         cv::Scalar upper(hmax,smax,vmax);
//         cv::inRange(hsv,lower,upper,mask);
//         cv::imshow("Trackbars",hsv);
//         cv::imshow("Mask",mask);
//         if(cv::waitKey(1)=='q')
//         {
//             break;
//         }
//     } 
//     cv::destroyAllWindows();
//     return;
// }

// int main()
// {
//     std::string path = "/home/liqintao/图片/截图/截图 2026-09-28 16-09-01.png";
//     cv::Mat img = cv::imread(path);
//     color_filter(img);  
//     return 0;
// }