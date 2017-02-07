#ifndef MPU6050_H_
#define	MPU6050_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

typedef union
{
  struct
  {
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
  } reg;
  struct
  {
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
  } value;
}gyro_accel_data_t;

typedef struct {
    int16_t     acc_x,
                acc_y,
                acc_z,
                gyr_x,
                gyr_y,
                gyr_z;
} mpu6050_offsets_t ;

typedef enum {
    MPU6050_DLPF_BW_256 = 0x00,
    MPU6050_DLPF_BW_188 = 0x01,
    MPU6050_DLPF_BW_98  = 0x02,
    MPU6050_DLPF_BW_42  = 0x03,
    MPU6050_DLPF_BW_20  = 0x04,
    MPU6050_DLPF_BW_10  = 0x05,
    MPU6050_DLPF_BW_5   = 0x06
} mpu6050_bandwidth_t;

int mpu6050_init ( i2c_module_t i2c_module, uart_module_t debug );
gyro_accel_data_t *mpu6050_get_raw_data ( void );
int mpu6050_receive_gyro_accel_raw_data ( void );
void mpu6050_set_bandwidth ( mpu6050_bandwidth_t bw );
void mpu6050_calibration ( void );
void mpu6050_set_offsets ( mpu6050_offsets_t *offsets );

// DMP

bool mpu6050_dmp_packet_available();
int mpu6050_dmp_init();

typedef struct
{
    float w;
    float x;
    float y;
    float z;
}quaternion_t;

typedef struct
{
    float roll;
    float pitch;
    float yaw;
}euler_angles_t;

void mpu6050_dmp_get_euler_angles(euler_angles_t *a);

#ifdef __cplusplus
 }
#endif

#endif	/* MPU6050_H_ */

