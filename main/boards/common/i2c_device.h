#ifndef I2C_DEVICE_H
#define I2C_DEVICE_H

#include <driver/i2c_master.h>
#include "esp_err.h"

#define I2C_OK              ESP_OK
#define I2C_FAIL            ESP_FAIL

#define I2C_REG_8BIT            0
#define I2C_REG_16BIT_BIG       1
#define I2C_REG_16BIT_LITTLE    2
#define I2C_NO_REG              3

#define I2C_TIMEOUT_MS (100)
#define I2C_DEVICE_TAG "I2C-Dev"

#define WRITE_DATA_STASH_SIZE 200

// #define CONFIG_I2C_DEVICE_DEBUG_INFO
#define CONFIG_I2C_DEVICE_DEBUG_ERROR
// #define CONFIG_I2C_DEVICE_DEBUG_REG

#ifdef CONFIG_I2C_DEVICE_DEBUG_INFO
#define i2c_log_i(format...) ESP_LOGI(I2C_DEVICE_TAG, format)
#else
#define i2c_log_i(format...)
#endif

#ifdef CONFIG_I2C_DEVICE_DEBUG_ERROR
#define i2c_log_e(format...) ESP_LOGE(I2C_DEVICE_TAG, format)
#else
#define i2c_log_e(format...)
#endif

#ifdef CONFIG_I2C_DEVICE_DEBUG_REG
#define i2c_log_reg(buffer, buffer_len) ESP_LOG_BUFFER_HEX(I2C_DEVICE_TAG, buffer, buffer_len)
#else
#define i2c_log_reg(buffer, buffer_len)
#endif

class I2cDevice {

public:

    uint8_t reg_bit = I2C_REG_8BIT;

public:
    I2cDevice(i2c_master_bus_handle_t i2c_bus, uint8_t addr);

protected:
    i2c_master_dev_handle_t i2c_device_;

    void WriteReg(uint8_t reg, uint8_t value);
    uint8_t ReadReg(uint8_t reg);
    void ReadRegs(uint8_t reg, uint8_t* buffer, size_t length);


    int i2c_dev_write_bytes(uint32_t reg_addr, uint8_t reg_len, const uint8_t *data, uint16_t length);

    int i2c_dev_read_bytes(uint32_t reg_addr, uint8_t reg_len, uint8_t *data, uint16_t length);

    int i2c_read_byte(uint32_t reg_addr, uint8_t* data);

    int i2c_read_bytes(uint32_t reg_addr, uint8_t *data, uint16_t length);

    int i2c_read_bit(uint32_t reg_addr, uint8_t *data, uint8_t bit_pos);

    /*
    Read bits from 8 bit reg
    bit_pos = 4, bit_length = 3
    read ->  0b|1|0|1|0|1|1|0|0| 
             0b|-|x|x|x|-|-|-|-|   
    data = 0b00000010
    */
    int i2c_read_bits(uint32_t reg_addr, uint8_t *data, uint8_t bit_pos, uint8_t bit_length);

    uint32_t i2c_adjust_reg(uint32_t reg_addr, uint8_t* len_out);

    int i2c_write_bytes(uint32_t reg_addr, uint8_t *data, uint16_t length);

    int i2c_write_byte(uint32_t reg_addr, uint8_t data);

    int i2c_write_bit(uint32_t reg_addr, uint8_t data, uint8_t bit_pos);

    /*
    Read before bits from 8 bit reg, then update write bits
    1. Read data 0b10101100
    2. write, 0b0101, bit_pos = 4, bit_length = 3 
    read ->  0b|1|0|1|0|1|1|0|0| 
             0b|-|x|x|x|-|-|-|-| 
    write -> 0b|1|1|0|1|1|1|0|0|  
    data = 0b00000101
    */
    int i2c_write_bits(uint32_t reg_addr, uint8_t data, uint8_t bit_pos, uint8_t bit_length);

};

#endif // I2C_DEVICE_H
