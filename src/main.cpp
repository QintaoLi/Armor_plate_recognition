#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>
#include <opencv2/opencv.hpp>

struct Light_Candidate
{
    cv::RotatedRect rect;
    float length;
    float width;
    float angle;
    cv::Point2f direction;
};

cv::Mat pre_process(const cv::Mat& frame)
{
    cv::Mat blur;
    cv::GaussianBlur(
        frame,
        blur,
        cv::Size(3,3),
        0
    );

    // 拆分 BGR 通道
    std::vector<cv::Mat> channels;
    cv::split(blur, channels);
    cv::Mat blue_ch = channels[0];
    cv::Mat red_ch  = channels[2];

    // 提取蓝色灯条 (B - R)
    cv::Mat diff_blue, binary_blue;
    cv::subtract(blue_ch, red_ch, diff_blue);
    double otsu_blue = cv::threshold(diff_blue, binary_blue, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    if (otsu_blue < 80) {
        cv::threshold(diff_blue, binary_blue, 100, 255, cv::THRESH_BINARY);
    }

    // 提取红色灯条 (R - B)
    cv::Mat diff_red, binary_red;
    cv::subtract(red_ch, blue_ch, diff_red);
    double otsu_red = cv::threshold(diff_red, binary_red, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    if (otsu_red < 80) {
        cv::threshold(diff_red, binary_red, 100, 255, cv::THRESH_BINARY);
    }

    // 将红、蓝灯条的二值掩码通过按位或合并在一起
    cv::Mat binary = binary_blue | binary_red;

    // 闭运算
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(binary, binary, cv::MORPH_CLOSE, kernel);

    return binary;
}

std::vector<Light_Candidate> light_filter(const cv::Mat& binary,cv::Mat& frame)
{
    std::vector<std::vector<cv::Point>>contours;
    cv::findContours(binary,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    std::vector<Light_Candidate>light;
    for(int i=0;i<contours.size();i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area<10.0 || area > 180.0)
        {
            continue; 
        }
        cv::RotatedRect box = cv::minAreaRect(contours[i]);
        
        cv::Point2f p[4];
        box.points(p);
        cv::Point2f edge1 = p[1] - p[0];
        cv::Point2f edge2 = p[2] - p[1];

        float len1 = cv::norm(edge1);
        float len2 = cv::norm(edge2);
        float length = std::max(len1, len2);
        float width  = std::min(len1, len2);
        if(width <= 0 || length <= 0) continue;
        if(length/width>5.0) continue;

        // 长轴单位向量，并统一为朝向图像上方
        cv::Point2f direction = (len1 >= len2 ? edge1 : edge2) / length;
        if (direction.y > 0) direction *= -1.f;

        // 相对图像竖直方向的倾角
        float angle = std::atan2(direction.x, -direction.y)
                    * 180.0f / CV_PI;
        if(angle>20.0f)
        {
            continue;
        }

        light.push_back({box,length,width,angle,direction});
    }
    return light;
}


void light_match(cv::Mat& frame,const std::vector<Light_Candidate>&light)
{
    for(int i=0;i<light.size();i++){
        for(int j=i+1;j<light.size();j++){
            float length_ratio = light[i].length/light[j].length;
            if(length_ratio>1.5||length_ratio<0.6)continue;

            float width_ratio = light[i].width/light[j].width;
            if(width_ratio>1.5||width_ratio<0.6)continue;

            float avg_length = (light[i].length+light[j].length)*0.5;
            cv::Point2f p1 = light[i].rect.center;
            cv::Point2f p2 = light[j].rect.center;
            cv::Point2f delta = p1 - p2;
            cv::Point2f axis = light[i].direction;
            cv::Point2f normal(-axis.y, axis.x);

            float parallel_offset = std::abs(delta.dot(axis));
            float perpendicular_offset = std::abs(delta.dot(normal));

            if (parallel_offset > avg_length * 0.3f)
                continue;

            if (perpendicular_offset < avg_length ||
                perpendicular_offset > avg_length * 4.0)
                continue;

            float angle_diff = abs(light[i].angle - light[j].angle);
            angle_diff = std::min(angle_diff, 180.0f - angle_diff);
            if(angle_diff > 10)continue;

            const Light_Candidate* left  = &light[i];
            const Light_Candidate* right = &light[j];

            if (light[i].rect.center.x > light[j].rect.center.x)
                std::swap(left,right);

            cv::Point2f left_dir = light[i].direction;
            cv::Point2f right_dir = light[j].direction;

            // 让两根灯条的方向一致，避免四边形交叉
            if (left_dir.dot(right_dir) < 0)
                right_dir *= -1.0f;

            // 图像坐标中，y 增大的方向向下
            if (left_dir.y < 0)
            {
                left_dir  *= -1.0f;
                right_dir *= -1.0f;
            }

            // 计算上下端点
            cv::Point2f leftTop =
                left->rect.center - left_dir * (left->length * 0.5f);

            cv::Point2f leftBottom =
                left->rect.center + left_dir * (left->length * 0.5f);

            cv::Point2f rightTop =
                right->rect.center - right_dir * (right->length * 0.5f);

            cv::Point2f rightBottom =
                right->rect.center + right_dir * (right->length * 0.5f);

            // 按顺时针顺序连接
            auto toPoint = [](const cv::Point2f& p)
            {
                return cv::Point(cvRound(p.x), cvRound(p.y));
            };

            std::vector<cv::Point> poly_rect = {
                toPoint(leftTop),
                toPoint(rightTop),
                toPoint(rightBottom),
                toPoint(leftBottom)
            };

            cv::polylines(
                frame,
                poly_rect,
                true,              
                cv::Scalar(0, 255, 0), 
                1,
                cv::LINE_AA
            );
        }
    }
}

void middle_draw(cv::Mat& middle,const std::vector<Light_Candidate>& light)
{
    for(int i=0;i<light.size();i++){
        cv::Point2f p[4];
        light[i].rect.points(p);
        std::vector<cv::Point> contours(4);
        for(int i=0;i<4;i++){
            contours[i] = {cvRound(p[i].x),cvRound(p[i].y)};
        }
        cv::polylines(
            middle,
            contours,
            true,
            cv::Scalar(0,255,0),
            1,
            cv::LINE_AA
        );
    }
}


int main()
{
    std::string video_path = "/home/liqintao/视频/drone_blue3.avi";
    cv::VideoCapture cap(video_path);
    if(!cap.isOpened())
    {
        std::cout<<"视频文件打开失败！"<<std::endl;
        return -1;
    }
    double fps = cap.get(cv::CAP_PROP_FPS);
    int delay = (fps > 0) ? static_cast<int>(1000.0 / fps) : 30;
    cv::Mat frame;
    while(true)
    {
        bool is_success = cap.read(frame);
        if(!is_success||frame.empty())
        {
            std::cout<<"视频播放失败！"<<std::endl;
            break;
        }
        cv::Mat binary = pre_process(frame);
        std::vector<Light_Candidate> light = light_filter(binary,frame); 
        cv::Mat middle = frame.clone();
        middle_draw(middle,light);
        cv::imshow("middle",middle);
        light_match(frame,light);   
        cv::imshow("final",frame);
        char key = cv::waitKey(delay);
        if (key == 'q') break;
        if (key == 's') cv::waitKey(0);
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}