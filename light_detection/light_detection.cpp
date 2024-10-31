#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// 全局变量用于滑动条
int threshold_value = 200;
int min_ratio = 10; // 最小长宽比 * 100
int max_ratio = 40; // 最大长宽比 * 100
const int max_value = 255;
const int max_ratio_value = 100;

// 回调函数
void on_threshold(int, void *) {
  // 空实现，仅用于创建滑动条
}

int main() {
  // 读取图像
  Mat src = imread("input.png");
  if (src.empty()) {
    cout << "Error: Could not read the image." << endl;
    return -1;
  }

  // 创建窗口和滑动条
  namedWindow("Controls", WINDOW_AUTOSIZE);
  createTrackbar("Threshold", "Controls", &threshold_value, max_value,
                 on_threshold);
  createTrackbar("Min Ratio*100", "Controls", &min_ratio, max_ratio_value,
                 on_threshold);
  createTrackbar("Max Ratio*100", "Controls", &max_ratio, max_ratio_value,
                 on_threshold);

  while (true) {
    Mat gray, binary;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;

    // 转换为灰度图
    cvtColor(src, gray, COLOR_BGR2GRAY);

    // 二值化
    threshold(gray, binary, threshold_value, 255, THRESH_BINARY);

    // 查找轮廓
    findContours(binary, contours, hierarchy, RETR_EXTERNAL,
                 CHAIN_APPROX_SIMPLE);

    // 创建用于显示结果的图像
    Mat result = src.clone();

    // 遍历所有轮廓
    for (size_t i = 0; i < contours.size(); i++) {
      // 获取最小外接矩形
      RotatedRect rect = minAreaRect(contours[i]);

      // 获取矩形的宽和高
      float width = rect.size.width;
      float height = rect.size.height;

      // 确保width是较短边
      if (width > height) {
        swap(width, height);
      }

      // 计算长宽比
      float ratio = width / height;

      // 获取矩形的角度
      float angle = abs(rect.angle);
      if (angle > 45.0) {
        angle = 90.0 - angle;
      }

      // 根据条件筛选灯条
      if (ratio >= min_ratio / 100.0 && ratio <= max_ratio / 100.0 &&
          angle <= 40.0) {
        // 绘制旋转矩形
        Point2f vertices[4];
        rect.points(vertices);
        for (int j = 0; j < 4; j++) {
          line(result, vertices[j], vertices[(j + 1) % 4], Scalar(0, 255, 0),
               2);
        }

        // 显示长宽比和角度
        string text = "Ratio: " + to_string(ratio).substr(0, 4) +
                      " Angle: " + to_string(angle).substr(0, 4);
        putText(result, text, Point(rect.center.x - 50, rect.center.y),
                FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 1);
      }
    }

    // 显示结果
    imshow("Original", src);
    imshow("Binary", binary);
    imshow("Result", result);

    // 按ESC退出
    char key = (char)waitKey(30);
    if (key == 27) {
      break;
    }
  }

  destroyAllWindows();
  return 0;
}