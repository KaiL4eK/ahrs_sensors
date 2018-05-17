#ifndef MPU6050_H_
#define	MPU6050_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <ahrs_common.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

     
typedef struct 
{
    int16_t x_accel;
    int16_t y_accel;
    int16_t z_accel;
    int16_t temperature;
    int16_t x_gyro;
    int16_t y_gyro;
    int16_t z_gyro;
} gy_521_gyro_accel_data_t;

int                         mpu6050_init ( i2c_module_t i2c_module );
gy_521_gyro_accel_data_t *  mpu6050_get_raw_data ( void );
int                         mpu6050_receive_gyro_accel_raw_data ( void ); // Call after gyro range setup
void                        mpu6050_calibration ( void );

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
} mpu6050_offsets_t;
void mpu6050_set_offsets ( const mpu6050_offsets_t *offsets );

#define MPU6050_GYRO_FS_250         0x00
#define MPU6050_GYRO_FS_500         0x01
#define MPU6050_GYRO_FS_1000        0x02
#define MPU6050_GYRO_FS_2000        0x03
void mpu6050_set_gyro_fullscale ( uint8_t value );
float mpu6050_get_gyro_sensitivity_rate ( void );

void mpu6050_set_interrupt_data_rdy_bit ( uint8_t value );

#define MPU6050_ACCEL_FS_2          0x00
#define MPU6050_ACCEL_FS_4          0x01
#define MPU6050_ACCEL_FS_8          0x02
#define MPU6050_ACCEL_FS_16         0x03
void mpu6050_set_accel_fullscale ( uint8_t value );

// Counted as 1kHz/(1 + <value>)
void mpu6050_set_sample_rate_divider ( uint8_t value );

// Enable/disable i2c bus bypass mode (not supported when gy-512 is master for another chip)
void mpu6050_set_bypass_mode ( bool enable );


// DMP
//bool mpu6050_dmp_packet_available();
//int mpu6050_dmp_init();

#ifdef __cplusplus
 }
#endif

#endif	/* MPU6050_H_ */

