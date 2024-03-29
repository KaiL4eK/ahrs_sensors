#define CORE_DEBUG_ENABLED
#include <MPU6050.h>
#include <MPU6050_private.h>

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

  gy_521_gyro_accel_data_t value;

} gyro_accel_data_t;

#define MAX_CONNECT_ATTEMPTS    1000

/* Internal data */

static uint8_t              buffer[14];
static bool                 initialized 	= false;

static gyro_accel_data_t    raw_gyr_acc;
       
static i2c_module_t			m_i2c_module 	= NULL;

static float                m_gyro_sens_rate = 0;

#define SWAP( x, y ) { uint8_t tmp = x; x = y; y = tmp; }

int mpu6050_init ( i2c_module_t i2c_module )
{
	m_i2c_module  = i2c_module;

    if ( !mpu6050_test_connection() )
    {
        dprintf( "[%s]: Test connection failed\n", __FUNCTION__ );
        return EFAULT;
    }
    
    mpu6050_set_sleep_bit( 0 );
    mpu6050_set_clock_source( MPU6050_CLOCK_PLL_XGYRO );
    
    memset( &raw_gyr_acc, 0, sizeof( raw_gyr_acc ) );
    
    initialized = true;
    
    return EOK;
}

void mpu6050_set_offsets ( const mpu6050_offsets_t *offsets )
{
    mpu6050_setXAccelOffset( offsets->acc_x );
    mpu6050_setYAccelOffset( offsets->acc_y );
    mpu6050_setZAccelOffset( offsets->acc_z );
    
    mpu6050_setXGyroOffset( offsets->gyr_x );
    mpu6050_setYGyroOffset( offsets->gyr_y );
    mpu6050_setZGyroOffset( offsets->gyr_z );
}

void mpu6050_set_bandwidth ( mpu6050_bandwidth_t bw )
{
    mpu6050_set_DLPF( bw );
}

gy_521_gyro_accel_data_t *mpu6050_get_raw_data ( void )
{
    return (gy_521_gyro_accel_data_t *)(&raw_gyr_acc);
}

int mpu6050_receive_gyro_accel_raw_data ( void )
{
    if ( !initialized )
        return( -1 );
    
    i2c_read_bytes( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_ACCEL_XOUT_H, 14, (uint8_t *)(&raw_gyr_acc) );

    SWAP (raw_gyr_acc.reg.x_accel_h, raw_gyr_acc.reg.x_accel_l);
    SWAP (raw_gyr_acc.reg.y_accel_h, raw_gyr_acc.reg.y_accel_l);
    SWAP (raw_gyr_acc.reg.z_accel_h, raw_gyr_acc.reg.z_accel_l);
    SWAP (raw_gyr_acc.reg.t_h,       raw_gyr_acc.reg.t_l);
    SWAP (raw_gyr_acc.reg.x_gyro_h,  raw_gyr_acc.reg.x_gyro_l);
    SWAP (raw_gyr_acc.reg.y_gyro_h,  raw_gyr_acc.reg.y_gyro_l);
    SWAP (raw_gyr_acc.reg.z_gyro_h,  raw_gyr_acc.reg.z_gyro_l);

    return( 0 );
}

uint8_t mpu6050_get_id ( void )
{
    return( i2c_read_byte ( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_WHO_AM_I ) );
}

bool mpu6050_test_connection( void )
{
    int     iTries      = 0;
    bool    connected   = false;
    
    for ( iTries = 0; iTries < MAX_CONNECT_ATTEMPTS; iTries++ ) {
        uint8_t result = mpu6050_get_id();
        
        // dprintf( "[%s]: Test connection result: 0x%x\n", __FUNCTION__, result );
        
        if ( (connected = (result == 0x68)) )
            break;
        
        mpu6050_reset();
    }
    
    return( connected );
}

void mpu6050_set_sleep_bit ( uint8_t value )
{
    i2c_write_bit ( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, value );
}

void mpu6050_set_clock_source ( uint8_t value )
{
    i2c_write_bits ( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_PWR_MGMT_1,
            MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, value );
}

void mpu6050_set_bypass_mode ( bool enable )
{
    if ( enable )
    {
        i2c_write_bit( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, 0 );
        i2c_write_bit( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, 1 );
    } else 
        i2c_write_bit( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, 0 );
}

void mpu6050_reset ( void )
{
    i2c_write_bit ( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_DEVICE_RESET_BIT, 1 );
}

void mpu6050_set_DLPF ( uint8_t value )
{
    i2c_write_bits( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_CONFIG,
            MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, value );
}

void mpu6050_set_gyro_fullscale ( uint8_t value )
{
    switch ( value )
    {
        case MPU6050_GYRO_FS_250:
            m_gyro_sens_rate = 250.0 / INT16_MAX;
            break;
            
        case MPU6050_GYRO_FS_500:
            m_gyro_sens_rate = 500.0 / INT16_MAX;
            break;
            
        case MPU6050_GYRO_FS_1000:
            m_gyro_sens_rate = 1000.0 / INT16_MAX;
            break;
            
        case MPU6050_GYRO_FS_2000:
            m_gyro_sens_rate = 2000.0 / INT16_MAX;
            break;
            
        default:
            return;
    }
            
    i2c_write_bits( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_GYRO_CONFIG,
            MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, value );
}

float mpu6050_get_gyro_sensitivity_rate ( void )
{
    return m_gyro_sens_rate;
}

void mpu6050_set_accel_fullscale ( uint8_t value )
{
    i2c_write_bits( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_ACCEL_CONFIG,
            MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, value );
}

void mpu6050_set_interrupt_data_rdy_bit ( uint8_t value )
{
    i2c_write_bits( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_INT_ENABLE,
            MPU6050_INTERRUPT_DATA_RDY_BIT, 1, value );
}


/** Get byte from FIFO buffer.
 * This register is used to read and write data from the FIFO buffer. Data is
 * written to the FIFO in order of register number (from lowest to highest). If
 * all the FIFO enable flags (see below) are enabled and all External Sensor
 * Data registers (Registers 73 to 96) are associated with a Slave device, the
 * contents of registers 59 through 96 will be written in order at the Sample
 * Rate.
 *
 * The contents of the sensor data registers (Registers 59 to 96) are written
 * into the FIFO buffer when their corresponding FIFO enable flags are set to 1
 * in FIFO_EN (Register 35). An additional flag for the sensor data registers
 * associated with I2C Slave 3 can be found in I2C_MST_CTRL (Register 36).
 *
 * If the FIFO buffer has overflowed, the status bit FIFO_OFLOW_INT is
 * automatically set to 1. This bit is located in INT_STATUS (Register 58).
 * When the FIFO buffer has overflowed, the oldest data will be lost and new
 * data will be written to the FIFO.
 *
 * If the FIFO buffer is empty, reading this register will return the last byte
 * that was previously read from the FIFO until new data is available. The user
 * should check FIFO_COUNT to ensure that the FIFO buffer is not read when
 * empty.
 *
 * @return Byte from FIFO buffer
 */
uint8_t mpu6050_getFIFOByte( void ) {
    return i2c_read_byte (m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_FIFO_R_W);
}
void mpu6050_getFIFOBytes(uint8_t *data, uint8_t length) {
    i2c_read_bytes (m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_FIFO_R_W, length, data);
}

/** Get current FIFO buffer size.
 * This value indicates the number of bytes stored in the FIFO buffer. This
 * number is in turn the number of bytes that can be read from the FIFO buffer
 * and it is directly proportional to the number of samples available given the
 * set of sensor data bound to be stored in the FIFO (register 35 and 36).
 * @return Current FIFO buffer size
 */
uint16_t mpu6050_getFIFOCount( void ) {
    uint8_t fifo_buffer[2];
    i2c_read_bytes(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_FIFO_COUNTH, 2, fifo_buffer);
    return (((uint16_t)fifo_buffer[0]) << 8) | fifo_buffer[1];
}

/** Reset the FIFO.
 * This bit resets the FIFO buffer when set to 1 while FIFO_EN equals 0. This
 * bit automatically clears to 0 after the reset has been triggered.
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_FIFO_RESET_BIT
 */
void mpu6050_resetFIFO( void ) {
    i2c_write_bit(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_RESET_BIT, true);
}

/** Set FIFO enabled status.
 * @param enabled New FIFO enabled status
 * @see getFIFOEnabled()
 * @see MPU6050_RA_USER_CTRL
 * @see MPU6050_USERCTRL_FIFO_EN_BIT
 */
void mpu6050_setFIFOEnabled(bool enabled) {
    i2c_write_bit(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_FIFO_EN_BIT, enabled);
}


/*
 *  8bit unsigned, sample rate = gyroscope rate / (1 + <value>)
 *  if MPU6050_RA_CONFIG - DLPF_CFG<3> == MPU6050_DLPF_BW_256 -> gyroscope rate = 8kHz
 *  else -> gyroscope rate = 1kHz
 */

void mpu6050_set_sample_rate_divider ( uint8_t value )
{
    i2c_write_byte( m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_SMPLRT_DIV, value );
}

// XA_OFFS_* registers

int16_t MPU6050_getXAccelOffset ( void ) 
{
    i2c_read_bytes(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_XA_OFFS_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
void mpu6050_setXAccelOffset(int16_t offset) {
    i2c_write_word(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_XA_OFFS_H, offset);
}

// YA_OFFS_* register

int16_t MPU6050_getYAccelOffset() {
    i2c_read_bytes(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_YA_OFFS_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
void mpu6050_setYAccelOffset ( int16_t offset ) 
{
    i2c_write_word(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_YA_OFFS_H, offset);
}

// ZA_OFFS_* register

int16_t MPU6050_getZAccelOffset ( void ) 
{
    i2c_read_bytes(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_ZA_OFFS_H, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
void mpu6050_setZAccelOffset ( int16_t offset ) 
{
    i2c_write_word(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_ZA_OFFS_H, offset);
}

// XG_OFFS_USR* registers

int16_t MPU6050_getXGyroOffset ( void ) 
{
    i2c_read_bytes(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_XG_OFFS_USRH, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
void mpu6050_setXGyroOffset ( int16_t offset ) 
{
    i2c_write_word(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_XG_OFFS_USRH, offset);
}

// YG_OFFS_USR* register

int16_t MPU6050_getYGyroOffset ( void ) 
{
    i2c_read_bytes(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_YG_OFFS_USRH, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
void mpu6050_setYGyroOffset ( int16_t offset ) 
{
    i2c_write_word(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_YG_OFFS_USRH, offset);
}

// ZG_OFFS_USR* register

int16_t MPU6050_getZGyroOffset ( void ) 
{
    i2c_read_bytes(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_ZG_OFFS_USRH, 2, buffer);
    return (((int16_t)buffer[0]) << 8) | buffer[1];
}
void mpu6050_setZGyroOffset ( int16_t offset ) 
{
    i2c_write_word(m_i2c_module, MPU6050_I2C_ADDRESS, MPU6050_RA_ZG_OFFS_USRH, offset);
}

///////////////////////////////////   CONFIGURATION   /////////////////////////////
//Change this 3 variables if you want to fine tune the skecth to your needs.
uint16_t buffersize=1000;     //Amount of readings used to average, make it higher to get more precision but sketch will be slower  (default:1000)
uint16_t acel_deadzone=8;     //Acelerometer error allowed, make it lower to get more precision, but sketch may not converge  (default:8)
uint16_t giro_deadzone=1;     //Giro error allowed, make it lower to get more precision, but sketch may not converge  (default:1)

int32_t mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz, state = 0;
int32_t ax_offset,ay_offset,az_offset,gx_offset,gy_offset,gz_offset;

static void mean_sensors ( void )
{
    int64_t     i       = 0,
                buff_ax = 0,
                buff_ay = 0,
                buff_az = 0,
                buff_gx = 0,
                buff_gy = 0,
                buff_gz = 0;
    
    while ( i < (buffersize + 101) )
    {
        mpu6050_receive_gyro_accel_raw_data();
        if ( i > 100 && i <= (buffersize + 100) )
        { //First 100 measures are discarded
            buff_ax += raw_gyr_acc.value.x_accel;
            buff_ay += raw_gyr_acc.value.y_accel;
            buff_az += raw_gyr_acc.value.z_accel; 
            
            buff_gx += raw_gyr_acc.value.x_gyro;
            buff_gy += raw_gyr_acc.value.y_gyro;
            buff_gz += raw_gyr_acc.value.z_gyro;
            
// Changed the direction
//            buff_ax += raw_gyr_acc.value.x_accel;
//            buff_ay += raw_gyr_acc.value.y_accel;
//            buff_az += raw_gyr_acc.value.z_accel;
//            buff_gx += raw_gyr_acc.value.x_gyro;
//            buff_gy += raw_gyr_acc.value.y_gyro;
//            buff_gz += raw_gyr_acc.value.z_gyro;
        }
        if ( i == (buffersize + 100) )
        {
            mean_ax = buff_ax / buffersize;
            mean_ay = buff_ay / buffersize;
            mean_az = buff_az / buffersize;
            mean_gx = buff_gx / buffersize;
            mean_gy = buff_gy / buffersize;
            mean_gz = buff_gz / buffersize;
        }

        i++;

        delay_ms( 5 ); //Needed so we don't get repeated measures
    }
}

void mpu6050_calibration ( void )
{
    mpu6050_set_accel_fullscale( MPU6050_ACCEL_FS_2 );
    mpu6050_setXAccelOffset(0);
    mpu6050_setYAccelOffset(0);
    mpu6050_setZAccelOffset(0);
    mpu6050_setXGyroOffset(0);
    mpu6050_setYGyroOffset(0);
    mpu6050_setZGyroOffset(0);
    dprintf( "\nReading sensors for first time...\n" );
    mean_sensors();
    dprintf( "\nCalculating offsets...\n" );
    {
        ax_offset=-mean_ax/8;
        ay_offset=-mean_ay/8;
        az_offset=(16384-mean_az)/8;

        gx_offset=-mean_gx/4;
        gy_offset=-mean_gy/4;
        gz_offset=-mean_gz/4;
        while (1)
        {
            int ready=0;
            mpu6050_setXAccelOffset(ax_offset);
            mpu6050_setYAccelOffset(ay_offset);
            mpu6050_setZAccelOffset(az_offset);
            mpu6050_setXGyroOffset(gx_offset);
            mpu6050_setYGyroOffset(gy_offset);
            mpu6050_setZGyroOffset(gz_offset);
            
            mean_sensors();

            if (abs(mean_ax)<=acel_deadzone) 
                ready++;
            else 
                ax_offset = ax_offset - mean_ax / acel_deadzone;

            if (abs(mean_ay)<=acel_deadzone) ready++;
            else ay_offset=ay_offset-mean_ay/acel_deadzone;

            if (abs(16384-mean_az)<=acel_deadzone) ready++;
            else az_offset=az_offset+(16384-mean_az)/acel_deadzone;

            if (abs(mean_gx)<=giro_deadzone) ready++;
            else gx_offset=gx_offset-mean_gx/(giro_deadzone+1);

            if (abs(mean_gy)<=giro_deadzone) ready++;
            else gy_offset=gy_offset-mean_gy/(giro_deadzone+1);

            if (abs(mean_gz)<=giro_deadzone) ready++;
            else gz_offset=gz_offset-mean_gz/(giro_deadzone+1);

            dprintf( "Reading data... %d / 6\n", ready );
            // dprintf( "Data: %d / %d / %d\n", mean_ax, mean_ay, mean_az );
            
            if (ready==6) break;
        } 
    }
    mean_sensors();
    dprintf( "\nFINISHED!\n\r" );
    dprintf( "Sensor readings with offsets:\n\t%ld\n\t%ld\n\t%ld\n\t%ld\n\t%ld\n\t%ld\n", mean_ax, mean_ay, mean_az, mean_gx, mean_gy, mean_gz );
    dprintf( "Your offsets:\n\t%ld\n\t%ld\n\t%ld\n\t%ld\n\t%ld\n\t%ld\n", ax_offset, ay_offset, az_offset, gx_offset, gy_offset, gz_offset );
    dprintf( "\nData is printed as: acelX acelY acelZ giroX giroY giroZ\n" );

    mpu6050_offsets_t mpu_offsets = {
        ax_offset,
        ay_offset,
        az_offset,
        gx_offset,
        gy_offset,
        gz_offset
    };

    mpu6050_set_offsets( &mpu_offsets );
}
