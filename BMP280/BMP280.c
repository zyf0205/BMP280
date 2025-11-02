#include "BMP280.h"
#include "myiic.h"
#include "USART.h"
#include <stdio.h>

BMP280 bmp280_inst;              /**< BMP280校准参数实例 */
BMP280* bmp280 = &bmp280_inst;   /**< 校准参数结构体指针 */
BMP280_S32_t t_fine;             /**< 温度微调值，用于气压补偿 */

/**
 * @brief 向BMP280寄存器写入数据
 * @param[in] reg 寄存器地址
 * @param[in] value 写入数据
 */
void BMP280_WriteReg(uint8_t reg, uint8_t value)
{
    IIC_Start();
    IIC_Send_Byte((BMP280_ADDR << 1) | 0);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Send_Byte(value);
    IIC_Wait_Ack();
    IIC_Stop();
}

/**
 * @brief 从BMP280寄存器读取数据
 * @param[in] reg 寄存器地址
 * @return 读取的数据值
 */
uint8_t BMP280_ReadReg(uint8_t reg)
{
    uint8_t value;
    
    IIC_Start();
    IIC_Send_Byte((BMP280_ADDR << 1) | 0);
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    
    IIC_Start();
    IIC_Send_Byte((BMP280_ADDR << 1) | 1);
    IIC_Wait_Ack();
    
    value = IIC_Read_Byte(0);
    IIC_Stop();
    
    return value;
}

/**
 * @brief 读取芯片ID（应为0x58）
 * @return 芯片ID值
 */
uint8_t BMP280_GetID(void)
{
    return BMP280_ReadReg(BMP280_CHIPID_REG);
}


/**
 * @brief 设置过采样因子和工作模式
 * @param[in] Oversample_Mode 过采样配置结构体
 */
void BMP280_Set_TemOversamp(BMP_OVERSAMPLE_MODE *Oversample_Mode)
{
    uint8_t Regtmp = ((Oversample_Mode->T_Osample) << 5) |
                     ((Oversample_Mode->P_Osample) << 2) |
                     ((Oversample_Mode->WORKMODE));
    BMP280_WriteReg(BMP280_CTRLMEAS_REG, Regtmp);
}

/**
 * @brief 设置待机时间和IIR滤波系数
 * @param[in] BMP_Config 配置结构体
 */
void BMP280_Set_Standby_FILTER(BMP_CONFIG *BMP_Config)
{
    uint8_t Regtmp = ((BMP_Config->T_SB) << 5) |
                     ((BMP_Config->FILTER_COEFFICIENT) << 2) |
                     ((BMP_Config->SPI_EN));
    BMP280_WriteReg(BMP280_CONFIG_REG, Regtmp);
}

/**
 * @brief 获取BMP280当前状态
 * @param[in] status_flag 要查询的状态标志（BMP280_MEASURING或BMP280_IM_UPDATE）
 * @return SET或RESET
 */
uint8_t BMP280_GetStatus(uint8_t status_flag)
{
    uint8_t flag = BMP280_ReadReg(BMP280_STATUS_REG);
    return (flag & status_flag) ? SET : RESET;
}


/**
 * @brief 初始化BMP280传感器
 * @details 读取校准参数、验证ID、配置工作模式
 */
void BMP280_Init(void)
{
    uint8_t Lsb, Msb;
    
    printf("BMP280: Start initialization...\r\n");
    
    /* 验证芯片ID */
    if (BMP280_GetID() != 0x58) {
        printf("BMP280: ERROR - Invalid chip ID!\r\n");
        return;
    }
    printf("BMP280: Chip ID verified (0x58)\r\n");
    
    /* 读取温度校准参数(T1~T3) */
    Lsb = BMP280_ReadReg(BMP280_DIG_T1_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_T1_MSB_REG);
    bmp280->T1 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_T2_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_T2_MSB_REG);
    bmp280->T2 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_T3_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_T3_MSB_REG);
    bmp280->T3 = (((uint16_t)Msb) << 8) + Lsb;
    
    printf("BMP280: Temperature calibration parameters loaded\r\n");
    
    /* 读取气压校准参数(P1~P9) */
    Lsb = BMP280_ReadReg(BMP280_DIG_P1_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P1_MSB_REG);
    bmp280->P1 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_P2_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P2_MSB_REG);
    bmp280->P2 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_P3_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P3_MSB_REG);
    bmp280->P3 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_P4_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P4_MSB_REG);
    bmp280->P4 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_P5_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P5_MSB_REG);
    bmp280->P5 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_P6_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P6_MSB_REG);
    bmp280->P6 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_P7_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P7_MSB_REG);
    bmp280->P7 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_P8_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P8_MSB_REG);
    bmp280->P8 = (((uint16_t)Msb) << 8) + Lsb;
    
    Lsb = BMP280_ReadReg(BMP280_DIG_P9_LSB_REG);
    Msb = BMP280_ReadReg(BMP280_DIG_P9_MSB_REG);
    bmp280->P9 = (((uint16_t)Msb) << 8) + Lsb;
    
    printf("BMP280: Pressure calibration parameters loaded\r\n");
    
    /* 复位芯片 */
    BMP280_WriteReg(BMP280_RESET_REG, BMP280_RESET_VALUE);
    printf("BMP280: Chip reset completed\r\n");
    
    /* 配置过采样和工作模式 */
    BMP_OVERSAMPLE_MODE oversample_config;
    oversample_config.P_Osample = BMP280_P_MODE_3;
    oversample_config.T_Osample = BMP280_T_MODE_1;
    oversample_config.WORKMODE = BMP280_NORMAL_MODE;
    BMP280_Set_TemOversamp(&oversample_config);
    
    /* 配置待机时间和滤波器 */
    BMP_CONFIG chip_config;
    chip_config.T_SB = BMP280_T_SB1;
    chip_config.FILTER_COEFFICIENT = BMP280_FILTER_MODE_4;
    chip_config.SPI_EN = DISABLE;
    BMP280_Set_Standby_FILTER(&chip_config);
    
    printf("BMP280: Initialization completed successfully!\r\n");
}


#ifdef USE_FIXED_POINT_COMPENSATE

/**
 * @brief 定点数温度补偿（整数运算）
 * @param[in] adc_T 原始ADC温度值
 * @return 温度值（单位0.01℃）
 */
BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
{
    BMP280_S32_t var1, var2, T;
    
    var1 = ((((adc_T >> 3) - ((BMP280_S32_t)dig_T1 << 1))) * ((BMP280_S32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BMP280_S32_t)dig_T1)) * ((adc_T >> 4) - ((BMP280_S32_t)dig_T1))) >> 12) * 
            ((BMP280_S32_t)dig_T3)) >> 14;
    
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    
    return T;
}

/**
 * @brief 定点数气压补偿（整数运算）
 * @param[in] adc_P 原始ADC气压值
 * @return 气压值（Q24.8格式，需除以256得到Pa）
 */
BMP280_U32_t bmp280_compensate_P_int64(BMP280_S32_t adc_P)
{
    BMP280_S64_t var1, var2, p;
    
    var1 = ((BMP280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BMP280_S64_t)dig_P6;
    var2 = var2 + ((var1 * (BMP280_S64_t)dig_P5) << 17);
    var2 = var2 + (((BMP280_S64_t)dig_P4) << 35);
    
    var1 = ((var1 * var1 * (BMP280_S64_t)dig_P3) >> 8) + ((var1 * (BMP280_S64_t)dig_P2) << 12);
    var1 = (((((BMP280_S64_t)1) << 47) + var1)) * ((BMP280_S64_t)dig_P1) >> 33;
    
    if (var1 == 0)
        return 0;
    
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    
    var1 = (((BMP280_S64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((BMP280_S64_t)dig_P8) * p) >> 19;
    
    p = ((p + var1 + var2) >> 8) + (((BMP280_S64_t)dig_P7) << 4);
    
    return (BMP280_U32_t)p;
}

#else

/**
 * @brief 浮点数温度补偿
 * @param[in] adc_T 原始ADC温度值
 * @return 温度值（℃）
 */
double bmp280_compensate_T_double(BMP280_S32_t adc_T)
{
    double var1, var2, T;
    
    var1 = (((double)adc_T) / 16384.0 - ((double)dig_T1) / 1024.0) * ((double)dig_T2);
    var2 = ((((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0) *
            (((double)adc_T) / 131072.0 - ((double)dig_T1) / 8192.0)) * ((double)dig_T3);
    
    t_fine = (BMP280_S32_t)(var1 + var2);
    T = (var1 + var2) / 5120.0;
    
    return T;
}

/**
 * @brief 浮点数气压补偿
 * @param[in] adc_P 原始ADC气压值
 * @return 气压值（Pa）
 */
double bmp280_compensate_P_double(BMP280_S32_t adc_P)
{
    double var1, var2, p;
    
    var1 = ((double)t_fine / 2.0) - 64000.0;
    var2 = var1 * var1 * ((double)dig_P6) / 32768.0;
    var2 = var2 + var1 * ((double)dig_P5) * 2.0;
    var2 = (var2 / 4.0) + (((double)dig_P4) * 65536.0);
    
    var1 = (((double)dig_P3) * var1 * var1 / 524288.0 + ((double)dig_P2) * var1) / 524288.0;
    var1 = (1.0 + var1 / 32768.0) * ((double)dig_P1);
    
    if (var1 == 0.0)
        return 0;
    
    p = 1048576.0 - (double)adc_P;
    p = (p - (var2 / 4096.0)) * 6250.0 / var1;
    
    var1 = ((double)dig_P9) * p * p / 2147483648.0;
    var2 = p * ((double)dig_P8) / 32768.0;
    
    p = p + (var1 + var2 + ((double)dig_P7)) / 16.0;
    
    return p;
}

#endif


/**
 * @brief 获取气压值
 * @return 气压值（Pa）
 */
double BMP280_Get_Pressure(void)
{
    uint8_t XLsb, Lsb, Msb;
    long signed Bit32;
    
    Msb = BMP280_ReadReg(BMP280_PRESSURE_MSB_REG);
    Lsb = BMP280_ReadReg(BMP280_PRESSURE_LSB_REG);
    XLsb = BMP280_ReadReg(BMP280_PRESSURE_XLSB_REG);
    
    Bit32 = ((long)(Msb << 12)) | ((long)(Lsb << 4)) | (XLsb >> 4);
    
    return bmp280_compensate_P_double(Bit32);
}

/**
 * @brief 获取温度值
 * @return 温度值（℃）
 */
double BMP280_Get_Temperature(void)
{
    uint8_t XLsb, Lsb, Msb;
    long signed Bit32;
    
    Msb = BMP280_ReadReg(BMP280_TEMPERATURE_MSB_REG);
    Lsb = BMP280_ReadReg(BMP280_TEMPERATURE_LSB_REG);
    XLsb = BMP280_ReadReg(BMP280_TEMPERATURE_XLSB_REG);
    
    Bit32 = ((long)(Msb << 12)) | ((long)(Lsb << 4)) | (XLsb >> 4);
    
    return bmp280_compensate_T_double(Bit32);
}

