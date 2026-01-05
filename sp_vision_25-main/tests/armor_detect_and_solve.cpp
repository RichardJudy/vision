#include <chrono>
#include <cmath>
#include <list>
#include <opencv2/opencv.hpp>

#include "io/camera.hpp"
#include "tasks/auto_aim/armor.hpp"
#include "tasks/auto_aim/detector.hpp"
#include "tasks/auto_aim/solver.hpp"
#include "tasks/auto_aim/tracker.hpp"
#include "tools/trajectory.hpp"

const std::string keys =
  "{help h usage ? | | 输出命令行参数说明}"
  "{@config-path   | | yaml配置文件路径 }";

int main(int argc, char * argv[])
{
  cv::CommandLineParser cli(argc, argv, keys);
  if (cli.has("help") || !cli.has("@config-path")) {
    cli.printMessage();
    return 0;
  }

  std::string config_path = cli.get<std::string>("@config-path");

  io::Camera camera(config_path);
  auto_aim::Detector detector(config_path);
  auto_aim::Solver solver(config_path);
  auto_aim::Tracker tracker(config_path, solver);

  double bullet_speed = 22.0;

  while (true) {
    cv::Mat img;
    std::chrono::steady_clock::time_point t;
    camera.read(img, t);

    if (img.empty()) {
      continue;
    }

    std::list<auto_aim::Armor> armors = detector.detect(img);

    for (auto_aim::Armor & armor : armors) {
      solver.solve(armor);
    }

    std::list<auto_aim::Target> targets = tracker.track(armors, t);

    for (const auto & armor : armors) {
      cv::rectangle(img, armor.box, cv::Scalar(0, 255, 0), 2);
      std::string name_text = auto_aim::ARMOR_NAMES[armor.name];
      cv::putText(
        img, name_text, cv::Point(armor.box.x, armor.box.y - 5),
        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    }

    if (!targets.empty()) {
      auto & target = targets.front();
      std::vector<Eigen::Vector4d> armor_xyza_list = target.armor_xyza_list();
      if (!armor_xyza_list.empty()) {
        Eigen::Vector4d xyza = armor_xyza_list.front();
        Eigen::Vector3d p = xyza.head(3);
        double d = std::sqrt(p[0] * p[0] + p[1] * p[1] + p[2] * p[2]);
        double h = p[2];

        tools::Trajectory traj(bullet_speed, d, h);
        if (!traj.unsolvable) {
          double pitch = traj.pitch;
          double fly_time = traj.fly_time;

          std::string info =
            "d=" + std::to_string(d) +
            "m pitch=" + std::to_string(pitch) +
            " rad t=" + std::to_string(fly_time) + "s";

          cv::putText(
            img, info, cv::Point(20, 40),
            cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 2);
        }
      }

      cv::putText(
        img, tracker.state(), cv::Point(20, 70),
        cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    }

    cv::imshow("armor_detect_and_solve", img);
    int key = cv::waitKey(1);
    if (key == 'q' || key == 27) {
      break;
    }
  }

  return 0;
}