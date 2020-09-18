#include <string.h>
#include <math.h>
#include "CException.h"
#include "salloc.h"
#include "mpu.h"
#define SHIFTED_DATA_BUFFER_SIZE 6
#define RAW_DATA_BUFFER_SIZE 14
#define MPU_ACCELEROMETER_MEASUREMENT_REGISTERS_START 0x3B
#define MPU_BITS 16
#define ACCELEROMETER_X_AXIS_INDEX 0
#define ACCELEROMETER_Y_AXIS_INDEX 1
#define ACCELEROMETER_Z_AXIS_INDEX 2
#define GYROSCOPE_X_AXIS_INDEX 3
#define GYROSCOPE_Y_AXIS_INDEX 4
#define GYROSCOPE_Z_AXIS_INDEX 5
typedef struct mpu_struct {
	float accel_x;
	float accel_y;
	float accel_z;
	float gyro_x;
	float gyro_y;
	float gyro_z;
	float gyro_x_offset;
	float gyro_y_offset;
	float gyro_z_offset;
	float accel_x_offset;
	float accel_y_offset;
	float accel_z_offset;
	float gyro_lsb;
	float accel_lsb;
	int16_t *shifted_data_buffer;
	uint8_t *raw_data;
	void (*mpu_write_memory)(uint16_t address, uint32_t size, uint8_t *buffer);
	void (*mpu_read_memory)(uint16_t address, uint32_t size, uint8_t *buffer);
} mpu_t;
STATIC int16_t get_int16_from_u8(uint8_t high_byte, uint8_t low_byte);
STATIC void get_shifted_buffer(mpu this, int16_t *buffer);
STATIC void convert_data(mpu this);
mpu mpu_ctor(
		void (*mpu_write_memory)(uint16_t address, uint32_t size,
				uint8_t *buffer),
		void (*mpu_read_memory)(uint16_t address, uint32_t size,
				uint8_t *buffer)) {
	salloc_enable();
	mpu this;
	this = salloc(sizeof(mpu_t));
	if (is_null(this)) {
		Throw(NULL_POINTER_EXCEPTION);
	}
	this->shifted_data_buffer = salloc(
			sizeof(uint16_t) * SHIFTED_DATA_BUFFER_SIZE);
	if (is_null(this->shifted_data_buffer)) {
		Throw(NULL_POINTER_EXCEPTION);
	}
	this->raw_data = salloc(sizeof(uint8_t) * RAW_DATA_BUFFER_SIZE);
	if (is_null(this->raw_data)) {
		Throw(NULL_POINTER_EXCEPTION);
	}
	salloc_disable();
	this->mpu_write_memory = mpu_write_memory;
	this->mpu_read_memory = mpu_read_memory;
	//disable sleep mode
	mpu_clear_bit(this, MPU_PWR_MGMT_1_REGISTER, 0x06);
	//enable interrupt
	mpu_set_bit(this,MPU_INT_ENABLE_REGISTER,0);
	return this;
}
float mpu_get_accel_x(mpu this) {
	this->accel_x = (this->shifted_data_buffer[ACCELEROMETER_X_AXIS_INDEX]
			/ this->accel_lsb) - this->accel_x_offset;
	return this->accel_x;
}
float mpu_get_accel_y(mpu this) {
	this->accel_y = (this->shifted_data_buffer[ACCELEROMETER_Y_AXIS_INDEX]
			/ this->accel_lsb) - this->accel_y_offset;
	return this->accel_y;
}
float mpu_get_accel_z(mpu this) {
	this->accel_z = (this->shifted_data_buffer[ACCELEROMETER_Z_AXIS_INDEX]
			/ this->accel_lsb) - this->accel_z_offset;
	return this->accel_z;
}
float mpu_get_gyro_x(mpu this) {
	this->gyro_x = (this->shifted_data_buffer[GYROSCOPE_X_AXIS_INDEX]
			/ this->gyro_lsb) - this->gyro_x_offset;
	return this->gyro_x;
}
float mpu_get_gyro_y(mpu this) {
	this->gyro_y = (this->shifted_data_buffer[GYROSCOPE_Y_AXIS_INDEX]
			/ this->gyro_lsb) - this->gyro_y_offset;
	return this->gyro_y;
}
float mpu_get_gyro_z(mpu this) {
	this->gyro_z = (this->shifted_data_buffer[GYROSCOPE_Z_AXIS_INDEX]
			/ this->gyro_lsb) - this->gyro_z_offset;
	return this->gyro_z;
}
void mpu_collect_data(mpu this) {
	this->mpu_read_memory(MPU_ACCELEROMETER_MEASUREMENT_REGISTERS_START, 14,
			this->raw_data);
	convert_data(this);
}
void mpu_set_bit(mpu this, uint16_t address, uint8_t bit) {
	uint8_t register_value = 0;
	this->mpu_read_memory(address, 1, &register_value);
	register_value |= (1 << bit);
	this->mpu_write_memory(address, 1, &register_value);
}
void mpu_clear_bit(mpu this, uint16_t address, uint8_t bit) {
	uint8_t register_value = 0;
	this->mpu_read_memory(address, 1, &register_value);
	register_value &= ~(1 << bit);
	this->mpu_write_memory(address, 1, &register_value);
}
void mpu_write_byte(mpu this, uint16_t address, uint8_t *value) {
	this->mpu_write_memory(address, 1, value);
}
uint8_t mpu_read_byte(mpu this, uint16_t address) {
	uint8_t byte = 0;
	this->mpu_read_memory(address, 1, &byte);
	return byte;
}
void mpu_set_gyroscope_full_scale_range(mpu this, uint8_t option) {
	uint8_t byte = mpu_read_byte(this, MPU_GYRO_CONFIG_REGISTER);
	byte &= ~(1 << 3);
	byte &= ~(1 << 4);
	byte |= (option << 3);
	float base = 500;
	this->gyro_lsb = (powf(2, MPU_BITS) / 2) / ((base) * (option + 1));
	mpu_write_byte(this, MPU_GYRO_CONFIG_REGISTER, &byte);
}
void mpu_set_accelerometer_full_scale_range(mpu this, uint8_t option) {
	uint8_t byte = mpu_read_byte(this, MPU_ACCEL_CONFIG_REGISTER);
	byte &= ~(1 << 3);
	byte &= ~(1 << 4);
	byte |= (option << 3);
	float base = 4;
	this->accel_lsb = (powf(2, MPU_BITS) / 2) / ((base) * (option + 1));
	mpu_write_byte(this, MPU_ACCEL_CONFIG_REGISTER, &byte);
}
void mpu_set_dlpf(mpu this, uint8_t option) {
	uint8_t byte = mpu_read_byte(this, MPU_CONFIG_REGISTER);
	byte &= ~(1 << 0);
	byte &= ~(1 << 1);
	byte &= ~(1 << 2);
	byte |= option;
	mpu_write_byte(this, MPU_CONFIG_REGISTER, &byte);
}
void mpu_set_gyroscope_x_offset(mpu this, float offset) {
	this->gyro_x_offset = offset;
}
void mpu_set_gyroscope_y_offset(mpu this, float offset) {
	this->gyro_y_offset = offset;
}
void mpu_set_gyroscope_z_offset(mpu this, float offset) {
	this->gyro_z_offset = offset;
}
void mpu_set_accelerometer_x_offset(mpu this, float offset) {
	this->accel_x_offset = offset;
}
void mpu_set_accelerometer_y_offset(mpu this, float offset) {
	this->accel_y_offset = offset;
}
void mpu_set_accelerometer_z_offset(mpu this, float offset) {
	this->accel_z_offset = offset;
}

STATIC int16_t get_int16_from_u8(uint8_t high_byte, uint8_t low_byte) {
	return (high_byte << 8) | (low_byte & 0xFF);
}
STATIC void get_shifted_buffer(mpu this, int16_t *buffer) {
	memcpy(buffer, this->shifted_data_buffer, SHIFTED_DATA_BUFFER_SIZE);
}
STATIC void convert_data(mpu this) {
	//get_accel
	this->shifted_data_buffer[0] = get_int16_from_u8(this->raw_data[0],
			this->raw_data[1]);
	this->shifted_data_buffer[1] = get_int16_from_u8(this->raw_data[2],
			this->raw_data[3]);
	this->shifted_data_buffer[2] = get_int16_from_u8(this->raw_data[4],
			this->raw_data[5]);
	//get gyro
	this->shifted_data_buffer[3] = get_int16_from_u8(this->raw_data[8],
			this->raw_data[9]);
	this->shifted_data_buffer[4] = get_int16_from_u8(this->raw_data[10],
			this->raw_data[11]);
	this->shifted_data_buffer[5] = get_int16_from_u8(this->raw_data[12],
			this->raw_data[13]);
}
