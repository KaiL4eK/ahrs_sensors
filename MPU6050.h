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


int mpu6050_init ( i2c_module_t i2c_module, uart_module_t debug );
gyro_accel_data_t *mpu6050_get_raw_data ( void );
int mpu6050_receive_gyro_accel_raw_data ( void );                   // Call after gyro range setup
void mpu6050_calibration ( void );

typedef enum {
    MPU6050_DLPF_BW_256 = 0x00,
    MPU6050_DLPF_BW_188 = 0x01,
    MPU6050_DLPF_BW_98  = 0x02,
    MPU6050_DLPF_BW_42  = 0x03,
    MPU6050_DLPF_BW_20  = 0x04,
    MPU6050_DLPF_BW_10  = 0x05,
    MPU6050_DLPF_BW_5   = 0x06
} mpu6050_bandwidth_t;
void mpu6050_set_bandwidth ( mpu6050_bandwidth_t bw );

typedef struct {
    int16_t     acc_x,
                acc_y,
                acc_z,
                gyr_x,
                gyr_y,
                gyr_z;
} mpu6050_offsets_t ;
void mpu6050_set_offsets ( mpu6050_offsets_t *offsets );

#define MPU6050_GYRO_FS_250         0x00
#define MPU6050_GYRO_FS_500         0x01
#define MPU6050_GYRO_FS_1000        0x02
#define MPU6050_GYRO_FS_2000        0x03
void mpu6050_set_gyro_fullscale ( uint8_t value );
float mpu6050_get_gyro_sensitivity_rate ( void );

#define MPU6050_ACCEL_FS_2          0x00
#define MPU6050_ACCEL_FS_4          0x01
#define MPU6050_ACCEL_FS_8          0x02
#define MPU6050_ACCEL_FS_16         0x03
void mpu6050_set_accel_fullscale ( uint8_t value );

// Counted as 1kHz/(1 + <value>)
void mpu6050_set_sample_rate_divider ( uint8_t value );

// DMP

bool mpu6050_dmp_packet_available();
int mpu6050_dmp_init();

#ifdef __cplusplus
 }
#endif

#endif	/* MPU6050_H_ */

