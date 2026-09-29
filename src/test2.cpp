// #include <iostream>
// #include <vector>
// #include <string>
// #include <opencv2/opencv.hpp>

// cv::Mat pre_process(const cv::Mat& frame)
// {
//     cv::Mat blur;
//     cv::GaussianBlur(
//         frame,
//         blur,
//         cv::Size(3,3),
//         0
//     );
//     std::vector<cv::Mat> channels;
//     cv::split(blur, channels);
//     cv::Mat blue_ch = channels[0];
//     cv::Mat red_ch  = channels[2];

//     cv::Mat diff_blue,binary_blue;
//     cv::subtract(blue_ch,red_ch,diff_blue);
//     double ostu_blue=cv::threshold(
//         diff_blue,
//         binary_blue,
//         0,
//         255,
//         cv::THRESH_BINARY|cv::THRESH_OTSU
//     );
//     if(ostu_blue<80)
//     {
//         cv::threshold(
//             diff_blue,
//             binary_blue,
//             100,
//             255,
//             cv::THRESH_BINARY
//         );
//     }

//     cv::Mat diff_red,binary_red;
//     cv::subtract(red_ch,blue_ch,diff_red);
//     double ostu_red = cv::threshold(
//         diff_red,
//         binary_red,
//         0,
//         255,
//         cv::THRESH_BINARY|cv::THRESH_OTSU
//     );
//     if(ostu_red<80)
//     {
//         cv::threshold(
//             diff_red,
//             binary_red,
//             100,
//             255,
//             cv::THRESH_BINARY
//         );
//     }
//     cv::Mat binary = binary_blue|binary_red;
    
//     cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT,cv::Size(3,3));
//     cv::morphologyEx(
//         binary,
//         binary,
//         cv::MORPH_CLOSE,
//         kernel
//     );
//     return binary;
// }

// void light_filter(const cv::Mat binary,cv::Mat& frame)
// {
//     std::vector<std::vector<cv::Point>>contours;
//     std::vector<cv::Vec4i> hierarchy;
//     cv::findContours(binary,contours,hierarchy,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);

//     std::vector<std::vector<cv::Point>>conPoly(contours.size());
//     std::vector<cv::Rect>boundRect(contours.size());
//     for(int i=0;i<contours.size();i++)
//     {
//         double area = cv::contourArea(contours[i]);
//         std::cout<<area<<std::endl;
//         // if (area < 10.0 || area > 2000.0)
//         // {./myapp
//         //     continue; 
//         // }
//         // float peri = cv::arcLength(contours[i],true);
//         // cv::approxPolyDP(contours[i],conPoly[i],0.02*peri,true);
//         // boundRect[i] = cv::boundingRect(conPoly[i]);
//         // cv::rectangle(frame,boundRect[i].tl(),boundRect[i].br(),cv::Scalar(0,255,0),1);
//     }
// }

// int main()
// {
//     std::string path = "/home/liqintao/图片/截图/截图 2026-09-25 20-28-39.png";
//     cv::Mat img=cv::imread(path);
//     cv::Mat binary = pre_process(img);
//     light_filter(binary,img);
//     cv::imshow("Video_player",binary);      
//     cv::imshow("real",img);
//     cv::destroyAllWindows();
//     return 0;
// }