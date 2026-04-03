# 基于NB-IoT的智能路灯系统

<div align="center">
  
![STM32](https://img.shields.io/badge/MCU-STM32F103C8T6-Cortex--M3-blue)
![NB-IoT](https://img.shields.io/badge/Communication-NB-IoT--M5310A-green)
![RTOS](https://img.shields.io/badge/RTOS-FreeRTOS--UCOS-orange)
![License](https://img.shields.io/badge/License-MIT-yellow)

</div>

---

## 📋 项目简介

本项目设计并实现了一套基于NB-IoT技术的智能路灯控制系统，采用STM32F103C8T6作为主控芯片，集成多种传感器和执行机构，实现路灯的智能化管理。系统具备远程监控、本地显示、自动调光、定时控制、人体感应等核心功能，有效提升路灯系统的能效管理水平。

**项目特点**：
- ✅ **低功耗设计**：采用智能调光和感应控制，大幅降低能耗
- ✅ **远程管理**：通过NB-IoT实现远程监控和控制
- ✅ **精准监测**：INA226实时监测电压、电流、功率
- ✅ **稳定可靠**：独立时钟芯片，断电不丢失时间
- ✅ **易于维护**：OLED本地显示，方便调试和维护

---

## 🎯 核心功能

### 1. 智能调光控制
- **光敏检测**：实时检测环境光照强度（0-100%）
- **多级亮度调节**：支持4级亮度调节（关闭/一档/二档/三档）
- **自适应调光**：根据环境光自动调整路灯亮度

### 2. 人体感应
- **红外检测**：采用HC-SR501人体红外传感器
- **智能感应**：有人时自动开启或提升亮度，无人时自动关闭

### 3. 电力监测
- **INA226传感器**：实时监测电压、电流、功率
- **能耗统计**：精确测量路灯功耗（精度可达毫瓦级）

### 4. NB-IoT远程通信
- **M5310A模块**：支持中国移动OneNet平台
- **远程控制**：通过APP远程开关灯、调节亮度
- **数据上传**：实时上传路灯状态和环境参数

### 5. 实时时间管理
- **DS1302时钟芯片**：独立实时时钟，断电不丢失
- **定时控制**：支持设置开关灯时间（时:分格式）
- **时间校准**：支持本地时间校准

### 6. 本地显示系统
- **OLED显示屏**：0.96寸SSD1306驱动，I2C接口
- **信息展示**：时间、亮度、功率、状态等实时显示

### 7. RGB LED状态指示
- **WS2812B灯带**：可编程RGB LED
- **状态反馈**：不同颜色指示不同工作状态

---

## 📦 硬件组成

### 主控系统
| 组件 | 型号/规格 | 说明 |
|------|-----------|------|
| 主控芯片 | STM32F103C8T6 | Cortex-M3内核，72MHz，64KB Flash |
| 时钟芯片 | DS1302 | 实时时钟，支持年月日时分秒 |
| NB-IoT模块 | M5310A | 中国移动NB-IoT通信模块 |
| 显示屏 | 0.96寸OLED | SSD1306驱动，128×64分辨率 |

### 传感器模块
| 组件 | 类型 | 功能 | 引脚 |
|------|------|------|------|
| 光敏电阻 | 模拟量 | 环境光照强度检测 | PA0, PA1 |
| 人体红外 | 数字量 | 人体存在检测 | PB5 (可调整) |
| INA226 | I2C | 电压/电流/功率监测 | PB8, PB9 |

### 执行机构
| 组件 | 类型 | 控制方式 | 引脚 |
|------|------|----------|------|
| PWM调光 | 模拟量 | TIM3/PWM控制 | PA6 |
| RGB LED | 数字量 | WS2812B单线协议 | PA1 |
| 继电器 | 数字量 | 开关控制 | 可选 |

### 按键输入
| 按键 | 功能 | 引脚 |
|------|------|------|
| KEY_SET | 进入设置模式 | PB12 |
| KEY_ADD | 参数增加 | PB13 |
| KEY_DEL | 参数减少 | PB14 |
| KEY_TIME | 时间校准确认 | PB15 |

---

## 🔧 硬件接口定义

### GPIO引脚分配

```c
// OLED显示屏 (I2C)
OLED_SCL      -> PB6 (具体引脚，根据实际设计调整)
OLED_SDA      -> PB7 (具体引脚，根据实际设计调整)

// 光敏电阻 (ADC)
Light_Sensor1 -> PA0 (通道0) - 环境光检测
Light_Sensor2 -> PA1 (通道1) - 路灯光检测

// 人体红外
HC-SR501      -> PB5 (具体引脚，根据实际设计调整)

// INA226 (I2C)
SCL           -> PB8 (I2C1_SCL)
SDA           -> PB9 (I2C1_SDA)

// PWM输出
PWM           -> PA6 (TIM3_CH1) - 路灯调光

// WS2812B数据线
RGB_DATA      -> PA1 (TIM2_CH1) - 注意：与Light_Sensor2复用，需根据实际设计调整

// DS1302时钟芯片
SCLK          -> PB13
IO            -> PB14
RST           -> PB15

// 按键输入
KEY_SET       -> PB12
KEY_ADD       -> PB13
KEY_DEL       -> PB14
KEY_TIME      -> PB15

// 状态指示
LED_RUN       -> PC13 (板载LED)
```

**⚠️ 注意事项**：
- PA0和PA1同时用于光敏电阻和WS2812B，实际设计中需要根据具体需求进行权衡
- 建议使用不同的ADC通道进行光敏检测，避免与WS2812B冲突
- I2C引脚建议使用PB6/PB7或PB8/PB9（标准I2C引脚）

### 通信接口
- **USART1**：NB-IoT模块通信（9600bps）
- **I2C**：OLED、INA226、DS1302
- **ADC**：光敏传感器模拟量采集
- **PWM**：TIM3通道1输出

---

## 💻 软件架构

### 系统架构图

```
┌─────────────────────────────────────────────────────────┐
│                    应用层 (Application)                 │
│  ├─ 主循环处理 (main.c)                                 │
│  ├─ 按键处理 (Key.c)                                    │
│  ├─ 时间管理 (DS1302.c)                                 │
│  └─ 显示管理 (OLED.c)                                   │
├─────────────────────────────────────────────────────────┤
│                    驱动层 (Driver)                      │
│  ├─ 通信驱动                                            │
│  │  ├─ USART (USART.c, usart2.c)                        │
│  │  ├─ I2C (myiic.c)                                    │
│  │  └─ NB-IoT (M5310A)                                  │
│  ├─ 传感器驱动                                          │
│  │  ├─ ADC (adc.c)                                      │
│  │  ├─ INA226 (INA226.c)                                │
│  │  └─ DS1302 (DS1302.c)                                │
│  └─ 执行机构驱动                                        │
│     ├─ PWM (pwm.c, timer.c)                             │
│     └─ WS2812B (WS2812B.c)                              │
├─────────────────────────────────────────────────────────┤
│                    HAL层 (STM32F10x StdPeriph)          │
│  ├─ GPIO, RCC, EXTI, TIM, USART, I2C, ADC等            │
└─────────────────────────────────────────────────────────┘
```

### 主要源文件说明

#### 核心应用文件
| 文件 | 功能说明 | 说明 |
|------|----------|------|
| `main.c` | 主程序入口，系统初始化，主循环逻辑 | 包含所有业务逻辑 |
| `stm32f10x_it.c` | 中断服务程序 | 包含USART1中断等 |
| `delay.c` | 毫秒/微秒延时函数 | 支持OS和非OS模式 |
| `sys.c` | 系统基础配置 | GPIO、NVIC等基础配置 |

#### 驱动模块文件
| 模块 | 文件 | 说明 | 引脚/接口 |
|------|------|------|----------|
| **ADC** | `adc.c/h` | 模拟量采集，光敏传感器读取 | PA0, PA1 |
| **I2C** | `myiic.c/h` | 软件I2C协议实现 | PB6/PB7 或 PB8/PB9 |
| **INA226** | `INA226.c/h` | 电力监测传感器驱动 | I2C1 (PB8/PB9) |
| **OLED** | `oled_iic.c/h` | SSD1306 OLED显示驱动 | I2C (PB6/PB7) |
| **PWM** | `pwm.c/h` | PWM波形生成 | TIM4/PWM |
| **TIMER** | `timer.c/h` | 定时器配置 | TIM3/PWM |
| **DS1302** | `DS1302.c/h` | 实时时钟驱动 | PB13, PB14, PB15 |
| **USART** | `USART.c/h` | 串口通信驱动 | USART1 (PA9/PA10) |
| **USART2** | `usart2.c/h` | 串口2驱动 | USART2 (PA2/PA3) |
| **WS2812B** | `WS2812B.c/h` | RGB LED驱动 | TIM2_CH1 (PA1) |
| **Key** | `Key.c/h` | 按键扫描处理 | PB12, PB13, PB14, PB15 |

---

## 📊 系统工作原理

### 1. 系统初始化流程

```
1. 系统时钟配置 (72MHz)
2. 延时函数初始化
3. OLED显示屏初始化
4. PWM模块初始化 (用于亮度控制)
5. 串口1初始化 (NB-IoT通信)
6. NB-IoT模块初始化与网络注册
7. 网络连接配置 (TCP连接到服务器)
8. NVIC中断优先级分组
9. 按键初始化
10. ADC初始化 (光敏传感器)
11. DS1302时钟初始化
12. INA226电力监测初始化
```

### 2. 主循环工作流程

```
每秒执行一次：
├─ 读取DS1302当前时间
├─ 获取INA226电力数据
├─ 读取环境光强度
├─ 读取路灯光强度
├─ 判断路灯状态（开/关）
├─ 显示时间、亮度、功率等信息
├─ 判断是否在定时范围内
├─ 根据模式执行调光逻辑：
│  ├─ 感应模式：根据人体感应和环境光调节亮度
│  └─ 定时模式：根据时间设定调节亮度
└─ 每3秒上传一次数据到服务器
```

### 3. 亮度调节策略

#### 感应模式（非定时范围）
```
环境光 > 85%  → 亮度 = 0 (关闭)
环境光 ≤ 80% 且 > 60% → 亮度 = 1 (一档)
环境光 ≤ 60% 且 > 30% → 亮度 = 2 (二档)
环境光 ≤ 30% → 亮度 = 3 (三档)

无人感应 → 亮度 = 0 (关闭)
```

#### 定时模式（在设定时间内）
```
环境光 > 60%  → 亮度 = 1 (一档)
环境光 ≤ 60% 且 > 30% → 亮度 = 2 (二档)
环境光 ≤ 30% → 亮度 = 3 (三档)
```

### 4. 数据上传格式

```
上传数据格式：#亮度,亮度值,功率,状态,定时标志位*

示例：#1,2,23.5,1,0*
├─ 第1位：亮度模式 (0=关闭,1=一档,2=二档,3=三档)
├─ 第2位：环境光强度 (0-100%)
├─ 第3位：功率 (瓦特，保留1位小数)
├─ 第4位：路灯状态 (0=正常,1=异常)
└─ 第5位：定时标志 (0=非定时时间,1=定时时间)
```

---

## 🔌 通信协议

### NB-IoT通信配置

```c
// 设备标识
UID: "702b87a3181340b09b56fe4e5040e4c8"
数据主题: "NBLDDATA"
控制主题: "NBLDKZ"

// 连接配置
服务器: bemfa.com
端口: 8344
协议: TCP
```

### AT指令序列

```
1. AT              → 检查模块响应
2. AT+CFUN=1       → 设置全功能模式
3. AT+CIMI         → 查询SIM卡IMSI
4. AT+CGATT=1      → 附着网络
5. AT+CGATT?       → 查询附着状态
6. AT+NSOCR=...    → 创建UDP socket
7. AT+NSOCO=...    → 连接服务器
8. AT+NSOCFG=...   → 配置通信参数
9. AT+NSOSD=...    → 发送数据
```

---

## 🛠️ 开发环境

### 硬件工具
- **编程器**：ST-Link V2
- **调试器**：J-Link (可选)
- **电源**：5V/2A稳压电源

### 软件工具
| 工具 | 版本 | 用途 |
|------|------|------|
| Keil MDK | 5.x | 集成开发环境 |
| STM32CubeMX | 6.x | 图形化配置 (可选) |
| ST-Link Utility | 4.x | 程序烧录 |
| 串口助手 | - | 调试通信 |
| App Inventor | - | 移动端APP开发 |

### 编译配置
```
芯片型号: STM32F103C8
系统时钟: 72MHz
Flash: 64KB
SRAM: 20KB
```

---

## 📱 移动端APP

### APP功能
- **远程监控**：实时查看路灯状态
- **远程控制**：开关灯、调节亮度
- **数据查看**：历史数据、能耗统计
- **参数设置**：设置定时时间、亮度等级
- **报警提醒**：路灯异常状态提醒

### 技术实现
- **开发平台**：App Inventor
- **通信方式**：NB-IoT + 云平台 (bemfa.com)
- **数据格式**：自定义协议 `#亮度,亮度值,功率,状态,定时标志位*`
- **设备标识**：UID = "702b87a3181340b09b56fe4e5040e4c8"

---

## 🔍 传感器技术参数

### 光敏电阻
- **型号**：光敏电阻模块
- **输出**：模拟量 0-3.3V
- **检测范围**：10 lux - 1000 lux
- **响应时间**：<10ms

### 人体红外传感器
- **型号**：HC-SR501
- **检测距离**：3-7米（可调）
- **输出**：数字量 3.3V/0V
- **工作电压**：5V

### INA226电力监测
- **测量参数**：电压、电流、功率
- **精度**：0.5% (典型值)
- **量程**：
  - 电压：0-26V
  - 电流：0-3.2A (取决于采样电阻)
  - 功率：0-83W
- **接口**：I2C (地址: 0x84)
- **采样电阻**：0.0025Ω (2.5mΩ) - 用于电流检测
- **校准值**：0x0800 (根据采样电阻计算)

### DS1302实时时钟
- **精度**：±2ppm (典型值)
- **供电**：3.3V + 备用电池
- **接口**：3线串行
- **功能**：年月日时分秒星期

---

## ⚙️ 核心算法

### ADC采样平均滤波
```c
u16 Get_Adc_Average(u8 ch, u8 times)
{
    u32 temp_val = 0;
    u8 t;
    for(t = 0; t < times; t++) {
        temp_val += Get_Adc(ch);
        delay_ms(5);
    }
    return temp_val / times;
}
```

### 光强转换
```c
u8 Lsens_Get_Val(void)
{
    u32 temp_val = Get_Adc_Average(LSENS_ADC_CHX, LSENS_READ_TIMES);
    if(temp_val > 4000) temp_val = 4000;
    return (u8)(temp_val / 40);  // 0-100%
}
```

### BCD码转换
```c
// BCD转十进制
uint8_t BCD_DEC(uint8_t BCD) {
    uint8_t DEC = BCD / 16 * 10 + BCD % 16;
    return DEC;
}

// 十进制转BCD
uint8_t DEC_BCD(uint8_t DEC) {
    uint8_t BCD = DEC / 10 * 16 + DEC % 10;
    return BCD;
}
```

### WS2812B驱动说明
- **驱动方式**：DMA + TIM2 PWM
- **数据线**：PA1 (TIM2_CH1)
- **时序控制**：
  - 传输"1"：50μs高电平
  - 传输"0"：25μs高电平
- **缓冲区大小**：每个LED需要24字节 + 42字节 trailing
- **DMA通道**：DMA1_Channel2
- **频率**：800kHz

### WS2812B时序控制
```c
// 1位数据传输 (50us高电平=1, 25us高电平=0)
#define TIMING_ONE   50   // 传输"1"的高电平时间
#define TIMING_ZERO  25   // 传输"0"的高电平时间
```

---

## 📈 系统性能指标

| 指标 | 参数 | 说明 |
|------|------|------|
| **主频** | 72MHz | STM32F103C8T6 |
| **通信** | NB-IoT | 中国移动OneNet平台 |
| **采样率** | 1Hz | 数据采集频率 |
| **上传频率** | 3s | 数据上传间隔 |
| **响应时间** | <100ms | 人体感应响应 |
| **亮度调节** | 4级 | 关闭/一档/二档/三档 |
| **功耗监测** | ±0.5W | INA226精度 |
| **时间精度** | ±2ppm | DS1302时钟精度 |
| **显示刷新** | 实时 | OLED显示 |

---

## 🎨 显示界面说明

### OLED显示布局

```
第0行:  [时]:[分]:[秒]
第1行:  (空行)
第2行:  环境光度: [0-100]%
第3行:  (空行)
第4行:  亮度档位: [关闭/一档/二档/三档]
第5行:  (空行)
第6行:  [功率] W  状态: [正常/异常]
第7行:  (空行)
```

### 设置界面
```
设置参数界面显示：
第0行:  设置定时时间
第1-3行: (空行)
第4行:  [时]:[分]-[时]:[分]
第5行:  (空行)
第6行:  光标指示当前设置项
```

---

## 🔐 安全特性

### 硬件安全
- **ESD保护**：所有接口防静电保护
- **过压保护**：电源输入保护
- **防反接**：电源防反接设计

### 软件安全
- **看门狗**：独立看门狗防止程序跑飞
- **ADC滤波**：平均滤波提高稳定性
- **数据校验**：上传数据格式校验
- **超时处理**：通信超时重连机制

---

## 📚 技术资料

### 参考文档
- [STM32F103C8T6数据手册](主控芯片资料/STM32F103C8T6芯片手册.PDF)
- [Cortex-M3权威指南](主控芯片资料/Cortex-M3权威指南.pdf)
- [M5310A AT指令手册](M5310-A资料/M5310A AT 命令用书使用手册_V1_20180827154312_20506.pdf)
- [OLED SSD1306驱动手册](OLED显示器资料/中景园电子0.96OLED显示屏_驱动芯片手册.pdf)
- [INA226数据手册](源程序/Library_2/INA226/)
- [DS1302数据手册](时钟芯片资料/DS1302中文手册.pdf)
- [WS2812B数据手册](源程序/Library_2/WS2812B/)
- [光敏电阻模块说明书](光敏传感器资料/)

### 相关协议
- **NB-IoT协议栈**：3GPP Release 13/14
- **TCP/IP协议**：IPv4/IPv6
- **I2C协议**：Philips I2C Bus Specification
- **WS2812B协议**：Single-Wire Protocol
- **DS1302协议**：3-Wire Serial Protocol

### 硬件原理图
- [原理图文件](原理图/Schematic_基于NB-IoT的智能路灯系统.SchDoc)
- [原理图PDF](原理图/Schematic_基于NB-IoT的智能路灯系统.pdf)

---

## 🚀 快速开始

### 硬件连接

1. **电源连接**
   - 输入电压：5V DC
   - 电流需求：>500mA (建议1A以上)
   - 接地必须良好，避免干扰

2. **传感器连接**
   - 光敏电阻：PA0, PA1 (模拟输入，需滤波)
   - 人体红外：PB5 (数字输入，3.3V电平)
   - INA226：I2C1 (PB8/PB9，需上拉电阻4.7kΩ)
   - DS1302：PB13, PB14, PB15 + 32.768kHz晶振

3. **通信连接**
   - NB-IoT模块：USART1 (PA9/PA10) - 9600bps
   - OLED：I2C (PB6/PB7 或 PB8/PB9)
   - WS2812B：PA1 (TIM2_CH1) - 注意与光敏电阻复用

4. **按键连接**
   - KEY_SET：PB12 (上拉)
   - KEY_ADD：PB13 (上拉)
   - KEY_DEL：PB14 (上拉)
   - KEY_TIME：PB15 (上拉)

5. **调试接口**
   - ST-Link：SWD接口 (PA13/SWDIO, PA14/SWCLK)
   - 串口调试：USART1 (PA9/PA10)

### 软件烧录

1. **编译项目**
   ```
   打开 Keil MDK → 打开项目 → 编译
   ```

2. **连接硬件**
   ```
   ST-Link V2 → STM32F103C8T6最小系统
   ```

3. **烧录程序**
   ```
   Keil → Project → Download → OK
   ```

4. **调试运行**
   ```
   Keil → Debug → Run
   ```

### 调试方法

1. **串口调试**
   - 使用串口助手连接USART1 (PA9/PA10)
   - 波特率：9600
   - 查看NB-IoT模块AT指令响应
   - 观察数据上传格式

2. **OLED调试**
   - 观察OLED显示信息
   - 检查时间、亮度、功率等数据
   - 查看状态提示信息

3. **LED指示**
   - PC13 LED闪烁表示系统运行正常
   - 闪烁频率：1Hz

4. **硬件检查**
   - 使用万用表测量各电压点
   - 使用示波器检查PWM波形
   - 检查I2C信号完整性
   - 验证传感器输出

---

## 🐛 常见问题

### 1. NB-IoT模块无法注册网络
**原因**：
- SIM卡未激活NB-IoT功能
- 天线未连接或接触不良
- 信号覆盖不足
- 模块未正确初始化

**解决**：
- 检查SIM卡是否开通NB-IoT业务
- 确保天线连接牢固
- 移动到信号较好的位置
- 检查AT指令初始化序列
- 查看串口输出调试信息

### 2. OLED屏幕无显示
**原因**：
- I2C通信异常
- 屏幕损坏
- 电源电压不足

**解决**：
- 检查I2C引脚连接
- 测量3.3V电源电压
- 更换OLED屏幕

### 3. 亮度调节无效
**原因**：
- PWM输出异常
- 电路故障
- 代码配置错误

**解决**：
- 用示波器检查PWM波形
- 检查驱动电路
- 核对TIM3配置

### 4. 电力监测数据异常
**原因**：
- INA226地址配置错误
- 采样电阻值不匹配
- 电源干扰

**解决**：
- 检查INA226地址 (0x84)
- 核对采样电阻阻值
- 增加滤波电容

---

## 📈 未来改进方向

1. **功能增强**
   - 增加远程固件升级(FOTA)
   - 支持更多传感器（温度、湿度）
   - 增加故障诊断功能

2. **性能优化**
   - 降低功耗（休眠模式）
   - 提高数据上传效率
   - 优化算法减少CPU占用

3. **扩展应用**
   - 支持LoRa通信
   - 增加Wi-Fi模块
   - 支持MQTT协议

---

## 📄 开源协议

本项目采用 MIT 协议开源，欢迎用于学习和商业用途。

```
MIT License

Copyright (c) 2026 基于NB-IoT的智能路灯系统项目组

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 👥 团队信息

**项目名称**：基于NB-IoT的智能路灯系统  
**开发时间**：2026年  
**技术栈**：嵌入式C、STM32、NB-IoT、传感器技术  
**应用领域**：智慧城市、节能照明、物联网应用

---

## 📞 联系方式

如有问题或建议，欢迎通过以下方式联系：

- **Email**: support@example.com
- **GitHub**: https://github.com/your-repo
- **技术论坛**: https://bbs.example.com

---

<div align="center">
  
**感谢您使用本项目！希望它能为您的智能路灯系统开发提供帮助。**

</div>

---

**最后更新时间**: 2026年4月2日
