#ifndef _MPU_H
#define _MPU_H
#define NULL_POINTER_EXCEPTION 1
#ifdef TEST
#define STATIC
#else
#define STATIC static
#endif
#define MPU_PWR_MGMT_1_REGISTER 0x6B
#define MPU_GYRO_CONFIG_REGISTER 0x1B
#define MPU_ACCEL_CONFIG_REGISTER 0x1C
#define MPU_INT_ENABLE_REGISTER 0x38
#define MPU_CONFIG_REGISTER 0x1A
typedef struct mpu_struct *mpu;
typedef enum GYROSCOPE_FULL_SCALE_RANGES_ENUM
{
    RANGE_250DPS,
    RANGE_500DPS,
    RANGE_1000DPS,
    RANGE_2000DPS
} GYROSCOPE_FULL_SCALE_RANGES_t;
typedef enum ACCELEROMETER_FULL_SCALE_RANGE_ENUM
{
    RANGE_2G,
    RANGE_4G,
    RANGE_8G,
    RANGE_16G,
} ACCELEROMETER_FULL_SCALE_RANGE_t;
typedef enum DLPF_SETTING_ENUM
{
    DLPF_CFG_0,
    DLPF_CFG_1,
    DLPF_CFG_2,
    DLPF_CFG_3,
    DLPF_CFG_4,
    DLPF_CFG_5,
    DLPF_CFG_6
} DLPF_SETTING_t;
typedef struct mpu_struct *mpu;
mpu mpu_ctor(void (*mpu_write_memory)(uint16_t address, uint32_t size, uint8_t *buffer), void (*mpu_read_memory)(uint16_t address, uint32_t size, uint8_t *buffer));
void mpu_set_bit(mpu this, uint16_t address, uint8_t bit);
void mpu_clear_bit(mpu this, uint16_t address, uint8_t bit);
void mpu_write_byte(mpu this, uint16_t address, uint8_t *value);
uint8_t mpu_read_byte(mpu this, uint16_t address);
void mpu_set_gyroscope_full_scale_range(mpu this, uint8_t option);
void mpu_set_accelerometer_full_scale_range(mpu this, uint8_t option);
void mpu_set_dlpf(mpu this, uint8_t option);
void mpu_set_gyroscope_x_offset(mpu this, float offset);
void mpu_set_gyroscope_y_offset(mpu this, float offset);
void mpu_set_gyroscope_z_offset(mpu this, float offset);
void mpu_set_accelerometer_x_offset(mpu this, float offset);
void mpu_set_accelerometer_y_offset(mpu this, float offset);
void mpu_set_accelerometer_z_offset(mpu this, float offset);
float mpu_get_accel_x(mpu this);
float mpu_get_accel_y(mpu this);
float mpu_get_accel_z(mpu this);
float mpu_get_gyro_x(mpu this);
float mpu_get_gyro_y(mpu this);
float mpu_get_gyro_z(mpu this);
void mpu_collect_data(mpu this);
#endif // _MPU_H
