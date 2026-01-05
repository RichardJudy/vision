
# 运行指南 - armor_detect_and_solve

## 前置要求

### 1. 系统依赖
- **操作系统**: Linux (推荐 Ubuntu 20.04/22.04) 或 Windows
- **编译器**: GCC 7+ 或 Clang 10+ (支持 C++17)
- **CMake**: 3.16 或更高版本

### 2. 必需库
安装以下依赖库：

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libopencv-dev \
    libeigen3-dev \
    libyaml-cpp-dev \
    libspdlog-dev \
    libfmt-dev \
    libnlohmann-json-dev \
    libusb-1.0-0-dev

# OpenVINO (用于分类器)
# 下载并安装 OpenVINO: https://www.intel.com/content/www/us/en/developer/tools/openvino-toolkit/download.html
# 或使用包管理器安装
```

### 3. 相机驱动
根据使用的相机类型安装相应驱动：

- **海康相机 (hikrobot)**: 需要海康工业相机SDK
- **迈德威视相机 (mindvision)**: 需要迈德威视相机SDK

相机库文件已包含在 `io/hikrobot/lib/` 和 `io/mindvision/lib/` 目录中。

## 编译步骤

### 1. 创建构建目录
```bash
cd sp_vision_25-main
mkdir build
cd build
```

### 2. 配置 CMake
```bash
cmake ..
```

如果需要指定 OpenVINO 路径：
```bash
cmake -DOpenVINO_DIR=/opt/intel/openvino_2024.6.0/runtime/cmake ..
```

### 3. 编译
```bash
make -j$(nproc)  # Linux
# 或
cmake --build . --config Release  # Windows
```

编译成功后，可执行文件位于：`build/armor_detect_and_solve`

## 配置文件准备

### 1. 修改配置文件
复制并编辑配置文件：
```bash
cp configs/example.yaml configs/my_config.yaml
```

### 2. 必需配置项

**相机配置**（二选一）：
```yaml
# 工业相机（推荐）
camera_name: "mindvision"  # 或 "hikrobot"
exposure_ms: 2
gamma: 0.5
vid_pid: "f622:d13a"  # 根据实际相机修改

# 或 USB 相机
# camera_name: "usbcamera"
# image_width: 1920
# image_height: 1080
```

**相机标定参数**（必需）：
```yaml
camera_matrix: [fx, 0, cx, 0, fy, cy, 0, 0, 1]
distort_coeffs: [k1, k2, p1, p2, k3]
R_camera2gimbal: [旋转矩阵9个元素]
t_camera2gimbal: [平移向量3个元素]
R_gimbal2imubody: [旋转矩阵9个元素]
```

**检测参数**：
```yaml
enemy_color: "blue"  # 或 "red"
threshold: 150
min_confidence: 0.8
classify_model: assets/tiny_resnet.onnx
```

**跟踪器参数**：
```yaml
min_detect_count: 5
max_temp_lost_count: 15
outpost_max_temp_lost_count: 75
```

## 运行程序

### 基本运行
```bash
./armor_detect_and_solve configs/my_config.yaml
```

### 查看帮助
```bash
./armor_detect_and_solve --help
```

### 运行参数
- `@config-path`: YAML配置文件路径（必需）

## 程序功能

程序会执行以下操作：
1. **初始化**: 加载相机、检测器、求解器、跟踪器
2. **循环处理**:
   - 从相机读取图像
   - 检测装甲板
   - 求解3D坐标
   - 跟踪目标
   - 计算弹道
   - 在图像上绘制检测结果和信息
3. **显示**: 使用 OpenCV 窗口显示处理结果
4. **退出**: 按 `q` 或 `ESC` 键退出

## 显示内容

程序窗口会显示：
- **绿色矩形框**: 检测到的装甲板
- **装甲板名称**: 在框上方显示（one, two, three等）
- **距离信息**: 红色文本显示距离、俯仰角、飞行时间
- **跟踪状态**: 白色文本显示跟踪器状态（tracking/lost等）

## 常见问题

### 1. 相机无法打开
- 检查相机是否连接
- 检查 `vid_pid` 是否正确
- 检查相机驱动是否安装
- 检查相机权限（Linux下可能需要添加用户到相应组）

### 2. 编译错误：找不到 OpenVINO
- 安装 OpenVINO 或修改 CMakeLists.txt 中的 OpenVINO_DIR 路径
- 如果不需要分类器，可以注释掉相关代码

### 3. 运行时错误：找不到模型文件
- 确保 `assets/tiny_resnet.onnx` 文件存在
- 检查配置文件中的路径是否正确

### 4. 没有检测到装甲板
- 检查 `enemy_color` 设置是否正确
- 调整 `threshold` 参数
- 检查光照条件
- 检查相机标定参数是否正确

## 调试建议

1. **查看日志**: 程序使用 spdlog 输出日志，可以查看控制台输出
2. **调整参数**: 根据实际情况调整配置文件中的检测参数
3. **检查标定**: 确保相机标定参数准确，这对3D坐标解算很重要

## 注意事项

- 程序需要相机标定参数才能正确计算3D坐标
- 如果没有实际相机，可以修改代码使用视频文件或图像序列
- 弹道计算需要正确的子弹速度（代码中硬编码为 22.0 m/s，可根据需要修改）

