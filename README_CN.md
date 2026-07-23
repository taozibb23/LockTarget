\# AimLab Auto-Aim (视觉辅助瞄准)



基于 OpenCV + 颜色检测 + 卡尔曼滤波 + PID 控制的 AimLabs 训练辅助工具。  

项目目标：通过屏幕捕获（OBS）实时检测蓝色靶心，自动平滑移动鼠标并点击，实现视觉-控制闭环。



本项目的长期目标是作为 \*\*RoboMaster 自动瞄准系统\*\* 的雏形练习，后续将移植到 Ubuntu + ROS 并集成 PnP 解算和云台控制。



\---



\## 功能



\- 实时屏幕捕获（通过 OBS 虚拟摄像头）

\- 蓝色目标颜色检测 + 轮廓提取

\- 卡尔曼滤波平滑目标位置（一维两个独立滤波器）

\- 目标锁定机制（基于鼠标位置的最远距离 / 锁定阈值）

\- PID 控制器（支持 P / I / D 参数调节）

\- 鼠标自动移动至目标（相对移动 + 边缘防卡）

\- 自动点击（连续稳定瞄准 5 帧后触发左键）

\- 轨迹显示、FPS 显示、中心十字等调试工具

\- 模块化代码结构（Armor / Locktarget / Display 分离）



\---



\## 环境要求



\- \*\*操作系统\*\*：Windows 10/11（需要 OBS 虚拟摄像头）

\- \*\*工具库\*\*：

&#x20; - OpenCV 4.x（推荐 4.5+，VC14/VC15 预编译版）

&#x20; - Visual Studio 2022（或其他支持 C++17 的编译器）

&#x20; - OBS Studio（>=28，自带虚拟摄像头）

&#x20; - Windows SDK（`GetCursorPos`, `mouse\_event` 等 API）

\- \*\*硬件\*\*：普通 PC，建议主频 2.5GHz+，内存 8GB+



\---



\## 快速开始



\### 1. 安装 OBS 并配置虚拟摄像头

\- 下载 OBS Studio 并安装。

\- 打开 OBS → 添加「显示器采集」或「窗口采集」。

\- 在菜单「工具」→「虚拟摄像头」→ 启动。



\### 2. 配置 OBS 虚拟摄像头

\- 确保虚拟摄像头输出分辨率固定（例如 1920x1080 或 640x480），代码会自动缩放。



\### 3. 编译运行

```bash

git clone https://github.com/taozibb23/LockTarget.git

cd LockTarget

\# 用 Visual Studio 打开 .sln 或 .vcxproj

\# 配置 OpenCV 属性表（include 和 lib 路径）

\# 生成解决方案，运行


├── Armor_tz.cpp          # Armor 类实现（颜色检测、轮廓、卡尔曼、画框）
├── lock_tz.cpp           # Locktarget 类实现（目标选择锁定、鼠标控制、点击）
├── display_tz.cpp        # Display 类实现（轨迹、十字、FPS）
├── main_tz.cpp           # 主循环（调度以上模块）
├── tzbb.h                # 全局头文件（类声明、Kalman1D、PID、ColorValue）
├── display.h             # Display 头文件
├── test_coord.cpp        # 坐标映射测试（已排除编译）
└── README.md             # 本文件
