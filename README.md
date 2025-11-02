# 🌡️ BMP280 气压计驱动代码
本仓库提供 **BMP280 传感器驱动程序**，实现温度与气压数据的读取与补偿计算。  
支持 **浮点** 与 **定点** 两种算法，可灵活适配不同性能的 MCU。

---

## 🚀 功能简介
+ ✅ 读取 **温度** 与 **气压** 数据
+ ✅ 支持 **浮点数** 与 **定点数** 补偿算法
+ ✅ 已添加详细注释，开箱即用
+ ✅ 兼容 **软件 I²C（Soft IIC）** 驱动

---

## ⚙️ 编译配置
默认使用 **浮点计算方式**。  
如需启用 **定点补偿算法（整数运算）**，请在代码中取消以下宏定义的注释：

```plain
#define USE_FIXED_POINT_COMPENSATE
```

| 计算方式 | 特点 | 推荐使用平台 |
| --- | --- | --- |
| 浮点计算 | 精度高、易调试 | STM32F4 / ESP32 / 含 FPU 的 MCU |
| 定点计算 | 占用少、速度快 | STM32F1 / 无 FPU 的 MCU |


---

## 🔌 接线说明
| BMP280 引脚 | 连接说明 | 备注 |
| --- | --- | --- |
| VCC | 3.3V 电源 | **禁止接 5V！** |
| GND | GND 地 | 共地 |
| SCL | PB6 | 可根据代码修改 |
| SDA | PB7 | 可根据代码修改 |
| CSB | 悬空 | 使用 I²C 模式 |
| SDO | GND | 器件地址为 `0x76`<br/>（若接 3.3V，则为 `0x77`<br/>） |


---

## 📡 通信方式
本驱动使用 **软件 I²C（Soft IIC）** 实现通信。  
如果你还没有软件 I²C 驱动，可以访问我的另一个仓库：

👉 [SoftIIC 仓库](https://github.com/zyf0205/SoftIIC)

---

## 🧠 使用说明
1. 将本驱动源文件添加到你的工程中；
2. 确保已正确配置 `SoftIIC` 驱动；
3. 调用初始化与读取函数，即可获取温度与气压数据。

示例：

```plain
int main(void)
{
    double BMP_Pressure, BMP_Temperature;
    USART1_Init(115200);
    IIC_Init();
    BMP280_Init();
    printf("\r\nBMP280大气压传感器实验");
    Delay_ms(50);
    printf("\r\nBMP280 ID:0x%02x\r\n", BMP280_GetID());
    while (1) {
        while (BMP280_GetStatus(BMP280_MEASURING) != RESET);
        while (BMP280_GetStatus(BMP280_IM_UPDATE) != RESET);
        BMP_Temperature = BMP280_Get_Temperature();
        BMP_Pressure    = BMP280_Get_Pressure();
        printf("Temperature %f C Pressure %f Pa\r\n", BMP_Temperature, BMP_Pressure);
        Delay_ms(500);
    }
}
```

---



