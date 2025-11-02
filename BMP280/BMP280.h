#ifndef BMP280_H
#define BMP280_H

#include "stm32f10x.h"

/**
 * ============================================================================================
 * @file BMP280.h
 * @brief BMP280气压温度传感器驱动头文件
 * @details
 *   - BMP280是博世集团生产的数字气压传感器
 *   - 通过I2C接口与MCU通信
 *   - 提供精确的温度和气压测量
 *   - 工作温度范围：-40~85℃
 *   - 气压测量范围：30~110 kPa
 * ============================================================================================
 */

 /*定点计算宏,未使用,精度稍差*/
//  #define USE_FIXED_POINT_COMPENSATE 



/* ============================================================================================
                              1. I2C设备地址定义
   ============================================================================================ */

#define BMP280_ADDR 0x76                /**< BMP280 I2C从设备地址（SDO接GND时为0x76，接VCC时为0x77） */

/* ============================================================================================
                              2. 寄存器地址定义
   ============================================================================================ */

/**< @defgroup BMP280_Control_Registers 控制寄存器 */
#define BMP280_CHIPID_REG 0xD0          /**< 芯片ID寄存器地址（读取值应为0x58） */
#define BMP280_RESET_REG 0xE0           /**< 软复位寄存器地址 */
#define BMP280_RESET_VALUE 0xB6         /**< 复位寄存器写入值 */
#define BMP280_STATUS_REG 0xF3          /**< 状态寄存器地址 */
#define BMP280_CTRLMEAS_REG 0xF4        /**< 控制测量寄存器地址（配置过采样和工作模式） */
#define BMP280_CONFIG_REG 0xF5          /**< 配置寄存器地址（配置滤波和待机时间） */

/**< @defgroup BMP280_Data_Registers 数据寄存器 */
#define BMP280_PRESSURE_MSB_REG 0xF7    /**< 气压数据高字节寄存器地址 */
#define BMP280_PRESSURE_LSB_REG 0xF8    /**< 气压数据低字节寄存器地址 */
#define BMP280_PRESSURE_XLSB_REG 0xF9   /**< 气压数据小数部分寄存器地址 */

#define BMP280_TEMPERATURE_MSB_REG 0xFA /**< 温度数据高字节寄存器地址 */
#define BMP280_TEMPERATURE_LSB_REG 0xFB /**< 温度数据低字节寄存器地址 */
#define BMP280_TEMPERATURE_XLSB_REG 0xFC/**< 温度数据小数部分寄存器地址 */

/**< @defgroup BMP280_Status_Flags 状态标志位 */
#define BMP280_MEASURING 0x01           /**< 状态寄存器位0：测量进行中标志 */
#define BMP280_IM_UPDATE 0x08           /**< 状态寄存器位3：=1 表示正在从NVM复制校准数据到寄存器，=0 表示复制完成。 */

/**< @defgroup BMP280_Calibration_Registers 校准参数寄存器（地址0x88~0x9F） */
#define BMP280_DIG_T1_LSB_REG 0x88      /**< 温度校准参数T1低字节 */
#define BMP280_DIG_T1_MSB_REG 0x89      /**< 温度校准参数T1高字节 */
#define BMP280_DIG_T2_LSB_REG 0x8A      /**< 温度校准参数T2低字节 */
#define BMP280_DIG_T2_MSB_REG 0x8B      /**< 温度校准参数T2高字节 */
#define BMP280_DIG_T3_LSB_REG 0x8C      /**< 温度校准参数T3低字节 */
#define BMP280_DIG_T3_MSB_REG 0x8D      /**< 温度校准参数T3高字节 */

#define BMP280_DIG_P1_LSB_REG 0x8E      /**< 气压校准参数P1低字节 */
#define BMP280_DIG_P1_MSB_REG 0x8F      /**< 气压校准参数P1高字节 */
#define BMP280_DIG_P2_LSB_REG 0x90      /**< 气压校准参数P2低字节 */
#define BMP280_DIG_P2_MSB_REG 0x91      /**< 气压校准参数P2高字节 */
#define BMP280_DIG_P3_LSB_REG 0x92      /**< 气压校准参数P3低字节 */
#define BMP280_DIG_P3_MSB_REG 0x93      /**< 气压校准参数P3高字节 */
#define BMP280_DIG_P4_LSB_REG 0x94      /**< 气压校准参数P4低字节 */
#define BMP280_DIG_P4_MSB_REG 0x95      /**< 气压校准参数P4高字节 */
#define BMP280_DIG_P5_LSB_REG 0x96      /**< 气压校准参数P5低字节 */
#define BMP280_DIG_P5_MSB_REG 0x97      /**< 气压校准参数P5高字节 */
#define BMP280_DIG_P6_LSB_REG 0x98      /**< 气压校准参数P6低字节 */
#define BMP280_DIG_P6_MSB_REG 0x99      /**< 气压校准参数P6高字节 */
#define BMP280_DIG_P7_LSB_REG 0x9A      /**< 气压校准参数P7低字节 */
#define BMP280_DIG_P7_MSB_REG 0x9B      /**< 气压校准参数P7高字节 */
#define BMP280_DIG_P8_LSB_REG 0x9C      /**< 气压校准参数P8低字节 */
#define BMP280_DIG_P8_MSB_REG 0x9D      /**< 气压校准参数P8高字节 */
#define BMP280_DIG_P9_LSB_REG 0x9E      /**< 气压校准参数P9低字节 */
#define BMP280_DIG_P9_MSB_REG 0x9F      /**< 气压校准参数P9高字节 */

/* ============================================================================================
                              3. 传感器工作模式枚举
   ============================================================================================ */

/**
 * @enum BMP280_WORK_MODE
 * @brief BMP280工作模式枚举
 */
typedef enum {
    BMP280_SLEEP_MODE = 0x0,    /**< 睡眠模式：传感器不工作，功耗最低 */
    BMP280_FORCED_MODE = 0x1,   /**< 强制模式：执行一次测量后进入睡眠，需要手动触发 */
    BMP280_NORMAL_MODE = 0x3    /**< 正常模式：循环测量，自动更新数据寄存器 */
} BMP280_WORK_MODE;

/**
 * @enum BMP280_P_OVERSAMPLING
 * @brief 气压传感器过采样因子
 * @details 过采样次数越多，测量精度越高但功耗增加
 */
typedef enum {
    BMP280_P_MODE_SKIP = 0x0,   /**< 无过采样：跳过气压测量 */
    BMP280_P_MODE_1,            /**< x1过采样：基础精度 */
    BMP280_P_MODE_2,            /**< x2过采样 */
    BMP280_P_MODE_3,            /**< x4过采样 */
    BMP280_P_MODE_4,            /**< x8过采样 */
    BMP280_P_MODE_5             /**< x16过采样：最高精度 */
} BMP280_P_OVERSAMPLING;

/**
 * @enum BMP280_T_OVERSAMPLING
 * @brief 温度传感器过采样因子
 * @details 过采样次数越多，测量精度越高但功耗增加
 */
typedef enum {
    BMP280_T_MODE_SKIP = 0x0,   /**< 无过采样：跳过温度测量 */
    BMP280_T_MODE_1,            /**< x1过采样：基础精度 */
    BMP280_T_MODE_2,            /**< x2过采样 */
    BMP280_T_MODE_3,            /**< x4过采样 */
    BMP280_T_MODE_4,            /**< x8过采样 */
    BMP280_T_MODE_5             /**< x16过采样：最高精度 */
} BMP280_T_OVERSAMPLING;

/**
 * @enum BMP280_FILTER_COEFFICIENT
 * @brief IIR滤波器系数
 * @details 数值越大，滤波效果越明显，但测量延迟增加
 */
typedef enum {
    BMP280_FILTER_OFF = 0x0,    /**< 滤波关闭 */
    BMP280_FILTER_MODE_1,       /**< 滤波系数x2 */
    BMP280_FILTER_MODE_2,       /**< 滤波系数x4 */
    BMP280_FILTER_MODE_3,       /**< 滤波系数x8 */
    BMP280_FILTER_MODE_4        /**< 滤波系数x16*/
} BMP280_FILTER_COEFFICIENT;

/**
 * @enum BMP280_T_SB
 * @brief 正常模式下的待机时间
 * @details 决定两次测量之间的时间间隔
 */
typedef enum {
    BMP280_T_SB1 = 0x0,         /**< 待机0.5ms */
    BMP280_T_SB2,               /**< 待机62.5ms */
    BMP280_T_SB3,               /**< 待机125ms */
    BMP280_T_SB4,               /**< 待机250ms */
    BMP280_T_SB5,               /**< 待机500ms */
    BMP280_T_SB6,               /**< 待机1000ms*/
    BMP280_T_SB7,               /**< 待机2000ms */
    BMP280_T_SB8,               /**< 待机4000ms */
} BMP280_T_SB;


/**
 * @struct BMP280
 * @brief BMP280芯片校准参数结构体
 * @details 
 *   - 存储从芯片ROM中读取的校准系数
 *   - 这些系数在温度和气压补偿计算中使用
 *   - 每颗芯片的校准参数不同，需要在初始化时读取
 */
typedef struct {
    /* 温度校准参数（地址0x88~0x8D） */
    uint16_t T1;                /**< 温度校准参数T1（无符号） */
    int16_t T2;                 /**< 温度校准参数T2（有符号） */
    int16_t T3;                 /**< 温度校准参数T3（有符号） */
    
    /* 气压校准参数（地址0x8E~0x9F） */
    uint16_t P1;                /**< 气压校准参数P1（无符号） */
    int16_t P2;                 /**< 气压校准参数P2（有符号） */
    int16_t P3;                 /**< 气压校准参数P3（有符号） */
    int16_t P4;                 /**< 气压校准参数P4（有符号） */
    int16_t P5;                 /**< 气压校准参数P5（有符号） */
    int16_t P6;                 /**< 气压校准参数P6（有符号） */
    int16_t P7;                 /**< 气压校准参数P7（有符号） */
    int16_t P8;                 /**< 气压校准参数P8（有符号） */
    int16_t P9;                 /**< 气压校准参数P9（有符号） */
} BMP280;

/**
 * @struct BMP_OVERSAMPLE_MODE
 * @brief 过采样模式配置结构体
 */
typedef struct {
    BMP280_P_OVERSAMPLING P_Osample;    /**< 气压过采样因子配置 */
    BMP280_T_OVERSAMPLING T_Osample;    /**< 温度过采样因子配置 */
    BMP280_WORK_MODE WORKMODE;          /**< 工作模式配置 */
} BMP_OVERSAMPLE_MODE;

/**
 * @struct BMP_CONFIG
 * @brief 芯片配置结构体
 */
typedef struct {
    BMP280_T_SB T_SB;                           /**< 待机时间配置,即两次测量之间的间隔时间 */
    BMP280_FILTER_COEFFICIENT FILTER_COEFFICIENT; /**< IIR滤波系数配置,滤波越强延迟越大 */
    FunctionalState SPI_EN;                     /**< SPI接口使能（禁用则使用I2C） */
} BMP_CONFIG;


/**< @defgroup BMP280_Data_Types 补偿算法用数据类型 */
typedef long signed int BMP280_S32_t;           /**< 有符号32位整数 */
typedef long unsigned int BMP280_U32_t;         /**< 无符号32位整数 */
typedef long long signed int BMP280_S64_t;      /**< 有符号64位整数 */

/**
 * @defgroup BMP280_Calibration_Macros 校准参数宏定义
 * @brief 用于在补偿算法中访问全局校准参数
 */
#define dig_T1 bmp280->T1
#define dig_T2 bmp280->T2
#define dig_T3 bmp280->T3

#define dig_P1 bmp280->P1
#define dig_P2 bmp280->P2
#define dig_P3 bmp280->P3
#define dig_P4 bmp280->P4
#define dig_P5 bmp280->P5
#define dig_P6 bmp280->P6
#define dig_P7 bmp280->P7
#define dig_P8 bmp280->P8
#define dig_P9 bmp280->P9

extern BMP280* bmp280;          /**< 指向BMP280校准参数结构体的指针 */
extern BMP280_S32_t t_fine;     /**< 全局温度微调值，用于气压补偿计算 */


void BMP280_WriteReg(uint8_t reg, uint8_t value);

uint8_t BMP280_ReadReg(uint8_t reg);

uint8_t BMP280_GetID(void);

void BMP280_Set_TemOversamp(BMP_OVERSAMPLE_MODE *Oversample_Mode);

void BMP280_Set_Standby_FILTER(BMP_CONFIG *BMP_Config);

void BMP280_Init(void);


u8 BMP280_GetStatus(u8 status_flag);

#ifdef USE_FIXED_POINT_COMPENSATE
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T);
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P);
#else
double bmp280_compensate_T_double(BMP280_S32_t adc_T);
double bmp280_compensate_P_double(BMP280_S32_t adc_P);
#endif

double BMP280_Get_Pressure(void);
double BMP280_Get_Temperature(void);

#endif /* BMP280_H */
