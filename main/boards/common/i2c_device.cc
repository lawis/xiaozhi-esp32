#include "i2c_device.h"
#include <hal/i2c_types.h>
#include <driver/i2c_types.h>
// #include "i2c_private.h"
#include <inttypes.h>
#include "string.h"
#include <esp_log.h>

#define TAG "I2cDevice"

I2cDevice::I2cDevice(i2c_master_bus_handle_t i2c_bus, uint8_t addr)
{
    i2c_device_config_t i2c_device_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = addr,
        .scl_speed_hz = 100000,
        .scl_wait_us = 0,
        .flags = {
            .disable_ack_check = 0,
        },
    };
    ESP_ERROR_CHECK(i2c_master_bus_add_device(i2c_bus, &i2c_device_cfg, &i2c_device_));
    assert(i2c_device_ != NULL);
}

void I2cDevice::WriteReg(uint8_t reg, uint8_t value)
{
    uint8_t buffer[2] = {reg, value};
    ESP_ERROR_CHECK(i2c_master_transmit(i2c_device_, buffer, 2, 100));
}

uint8_t I2cDevice::ReadReg(uint8_t reg)
{
    uint8_t buffer[1];
    ESP_ERROR_CHECK(i2c_master_transmit_receive(i2c_device_, &reg, 1, buffer, 1, 100));
    return buffer[0];
}

void I2cDevice::ReadRegs(uint8_t reg, uint8_t *buffer, size_t length)
{
    ESP_ERROR_CHECK(i2c_master_transmit_receive(i2c_device_, &reg, 1, buffer, length, 100));
}


int I2cDevice::i2c_dev_write_bytes(uint32_t reg_addr, uint8_t reg_len, const uint8_t *data, uint16_t length)
{
    if (length > 0 && data == NULL) {
        return I2C_FAIL;
    }

    // if (reg_len + length == 0) { // i2c_scan
    //     return i2c_master_probe(i2c_device_->master_bus, i2c_device_->device_address, I2C_TIMEOUT_MS) == ESP_OK ? I2C_OK : I2C_FAIL;
    // }

    if (reg_len == 0) {
        return i2c_master_transmit(i2c_device_, data, length, I2C_TIMEOUT_MS) == ESP_OK ? I2C_OK : I2C_FAIL;
    }

    if (reg_len + length >= WRITE_DATA_STASH_SIZE) {
        i2c_log_e("data stash size is not enough, reg_len:%d, length:%d, pls used no reg setting", reg_len, length);
        return I2C_FAIL;
    }

    uint8_t data_stash[WRITE_DATA_STASH_SIZE] = {0};
    memcpy(data_stash, &reg_addr, reg_len);
    memcpy(data_stash + reg_len, data, length);
    return i2c_master_transmit(i2c_device_, data_stash, reg_len + length, I2C_TIMEOUT_MS) == ESP_OK ? I2C_OK : I2C_FAIL;

}

int I2cDevice::i2c_read_byte(uint32_t reg_addr, uint8_t* data)
{
    return i2c_read_bytes(reg_addr, data, 1);
}

int I2cDevice::i2c_read_bytes(uint32_t reg_addr, uint8_t *data, uint16_t length)
{
    if (length > 0 && data == NULL) {
        return I2C_FAIL;
    }

    uint8_t reg_len = 0;
    int err = I2C_FAIL;

    reg_addr = i2c_adjust_reg(reg_addr, &reg_len);

    err = i2c_dev_read_bytes(reg_addr, reg_len, data, length);

    if (err != I2C_OK) {
        // i2c_log_e("I2C Read Error, addr: 0x%02x, reg: 0x%04" PRIx32 ", length: %" PRIu16 ", Code: 0x%x", device->addr, reg_addr, length, err);
    } else {
        // i2c_log_i("I2C Read Success, addr: 0x%02x, reg: 0x%04" PRIx32 ", length: %" PRIu16, device->addr, reg_addr, length);
        i2c_log_reg(data, length);
    }

    return err;
}

int I2cDevice::i2c_read_bit(uint32_t reg_addr, uint8_t *data, uint8_t bit_pos)
{
    if (data == NULL) {
        return I2C_FAIL;
    }

    int err = I2C_FAIL;
    uint8_t bit_data = 0x00;
    err = i2c_read_byte(reg_addr, &bit_data);
    if (err != I2C_OK) {
        return err;
    }

    *data = (bit_data >> bit_pos) & 0x01;
    return I2C_OK; 
}

int I2cDevice::i2c_read_bits(uint32_t reg_addr, uint8_t *data, uint8_t bit_pos, uint8_t bit_length)
{
    if ((bit_pos + bit_length > 8) || data == NULL) {
        return I2C_FAIL;
    }

    uint8_t bit_data = 0x00;
    int err = I2C_FAIL;
    err = i2c_read_byte(reg_addr, &bit_data);
    if (err != I2C_OK) {
        return err;
    }

    bit_data = bit_data >> bit_pos;
    bit_data &= (1 << bit_length) - 1;
    *data = bit_data;
    return I2C_OK;
}

int I2cDevice::i2c_dev_read_bytes(uint32_t reg_addr, uint8_t reg_len, uint8_t *data, uint16_t length)
{
    if (length > 0 && data == NULL) {
        return I2C_FAIL;
    }

    if (reg_len == 0) {
        return i2c_master_receive(i2c_device_, data, length, I2C_TIMEOUT_MS) == ESP_OK ? I2C_OK : I2C_FAIL;
    }

    return i2c_master_transmit_receive(i2c_device_, (uint8_t *)&reg_addr, reg_len, data, length, I2C_TIMEOUT_MS) == ESP_OK ? I2C_OK : I2C_FAIL;
}

uint32_t I2cDevice::i2c_adjust_reg(uint32_t reg_addr, uint8_t *len_out)
{
    // only support max 16bit reg now
    uint32_t reg_addr_adjust = reg_addr & 0xffff;
    uint8_t reg_len = 0;

    if (this->reg_bit == I2C_NO_REG)
    {
        reg_addr_adjust = 0xfff;
        reg_len = 0;
    }
    else if (this->reg_bit == I2C_REG_16BIT_LITTLE)
    {
        reg_len = 2;
    }
    else if (this->reg_bit == I2C_REG_16BIT_BIG)
    {
        reg_addr_adjust = (reg_addr_adjust >> 8) | ((reg_addr_adjust & 0xff) << 8);
        reg_len = 2;
    }
    else
    {
        reg_addr_adjust = reg_addr_adjust & 0xff;
        reg_len = 1;
    }
    *len_out = reg_len;
    return reg_addr_adjust;
}

int I2cDevice::i2c_write_byte(uint32_t reg_addr, uint8_t data)
{
    return i2c_write_bytes(reg_addr, &data, 1);
}

int I2cDevice::i2c_write_bytes(uint32_t reg_addr, uint8_t *data, uint16_t length)
{
    if (length > 0 && data == NULL)
    {
        return I2C_FAIL;
    }

    uint8_t reg_len = 0;
    int err = I2C_FAIL;

    reg_addr = i2c_adjust_reg(reg_addr, &reg_len);

    err = i2c_dev_write_bytes(reg_addr, reg_len, data, length);

    if (err != I2C_OK)
    {
        // i2c_log_e("I2C Write Error, addr: 0x%02x, reg: 0x%04" PRIx32 ", length: %" PRIu16 ", Code: 0x%x", i2c_device_->device_address, reg_addr, length, err);
    }
    else
    {
        // i2c_log_i("I2C Write Success, addr: 0x%02x, reg: 0x%04" PRIx32 ", length: %" PRIu16, i2c_device_->device_address, reg_addr, length);
        if (length)
        {
            i2c_log_reg(data, length);
        }
    }

    return err;
}

int I2cDevice::i2c_write_bit(uint32_t reg_addr, uint8_t data, uint8_t bit_pos)
{

    uint8_t value = 0x00;
    int err = I2C_FAIL;
    err = i2c_read_byte(reg_addr, &value);
    if (err != I2C_OK) {
        return err;
    }

    value &= ~(1 << bit_pos);
    value |= (data & 0x01) << bit_pos;
    return i2c_write_byte(reg_addr, value);
    return 1;
}

int I2cDevice::i2c_write_bits(uint32_t reg_addr, uint8_t data, uint8_t bit_pos, uint8_t bit_length)
{
    if ((bit_pos + bit_length) > 8) {
        return I2C_FAIL;
    }

    uint8_t value = 0x00;
    int err = I2C_FAIL;
    err = i2c_read_byte(reg_addr, &value);
    if (err != I2C_OK) {
        return err;
    }

    value &= ~(((1 << bit_length) - 1) << bit_pos);
    data &= (1 << bit_length) - 1;
    value |= data << bit_pos;

    return i2c_write_byte(reg_addr, value);
}