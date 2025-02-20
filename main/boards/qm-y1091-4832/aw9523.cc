#include "aw9523.h"
#include "string.h"
#include "config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TAG "Aw9523"

#define AW9523_CHECK_NUM(pin_num) if ((pin_num) > 7) { return; }

Aw9523::Aw9523(i2c_master_bus_handle_t i2c_bus, uint8_t addr) : I2cDevice(i2c_bus, addr)
{
    // WriteReg(0x11, 0b00000000) ; // 设置最大电流为37mA
    // WriteReg(0x13, 0b11110000) ; // 设置P1_0-P1_3为推挽输出模式

    // WriteReg(0x20, 0b10000000) ; // 设置P1_0调光为128
    // WriteReg(0x21, 0b10000000) ; // 设置P1_1调光为128
    // WriteReg(0x22, 0b10000000) ; // 设置P1_2调光为128
    // WriteReg(0x23, 0b10000000) ; // 设置P1_3调光为128

    // aw9523_io_set_gpio_or_led((aw9523_port_t)(BOARD_RESET_PIN >> 4), BOARD_RESET_PIN & 0x0f, AW9523_MODE_GPIO);

    aw9523_io_set_gpio_or_led((aw9523_port_t)(BOARD_RESET_PIN >> 4), BOARD_RESET_PIN & 0x0f, AW9523_MODE_GPIO);
    aw9523_io_set_inout((aw9523_port_t)(BOARD_RESET_PIN >> 4), BOARD_RESET_PIN & 0x0f, AW9523_MODE_OUTPUT);
    aw9523_io_set_level((aw9523_port_t)(BOARD_RESET_PIN >> 4), BOARD_RESET_PIN & 0x0f, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    aw9523_io_set_level((aw9523_port_t)(BOARD_RESET_PIN >> 4), BOARD_RESET_PIN & 0x0f, 1);
    vTaskDelay(pdMS_TO_TICKS(10));  

    aw9523_set_led_max_current(AW9523_37mA);
    aw9523_io_set_gpio_or_led((aw9523_port_t)(LCD_BL_0_PIN >> 4), LCD_BL_0_PIN & 0x0f, AW9523_MODE_LED);
    aw9523_io_set_gpio_or_led((aw9523_port_t)(LCD_BL_1_PIN >> 4), LCD_BL_1_PIN & 0x0f, AW9523_MODE_LED);
    aw9523_io_set_gpio_or_led((aw9523_port_t)(LCD_BL_2_PIN >> 4), LCD_BL_2_PIN & 0x0f, AW9523_MODE_LED);
    aw9523_io_set_gpio_or_led((aw9523_port_t)(LCD_BL_3_PIN >> 4), LCD_BL_3_PIN & 0x0f, AW9523_MODE_LED);
    aw9523_io_set_gpio_or_led((aw9523_port_t)(LCD_BL_4_PIN >> 4), LCD_BL_4_PIN & 0x0f, AW9523_MODE_LED);
    aw9523_io_set_gpio_or_led((aw9523_port_t)(LCD_BL_5_PIN >> 4), LCD_BL_5_PIN & 0x0f, AW9523_MODE_LED);
    aw9523_led_set_duty((aw9523_port_t)(LCD_BL_0_PIN >> 4), LCD_BL_0_PIN & 0x0f, 128);
    aw9523_led_set_duty((aw9523_port_t)(LCD_BL_1_PIN >> 4), LCD_BL_1_PIN & 0x0f, 128);
    aw9523_led_set_duty((aw9523_port_t)(LCD_BL_2_PIN >> 4), LCD_BL_2_PIN & 0x0f, 128);
    aw9523_led_set_duty((aw9523_port_t)(LCD_BL_3_PIN >> 4), LCD_BL_3_PIN & 0x0f, 128);
    aw9523_led_set_duty((aw9523_port_t)(LCD_BL_4_PIN >> 4), LCD_BL_4_PIN & 0x0f, 128);
    aw9523_led_set_duty((aw9523_port_t)(LCD_BL_5_PIN >> 4), LCD_BL_5_PIN & 0x0f, 128);

    aw9523_io_set_gpio_or_led((aw9523_port_t)(PA_CTRL_PIN >> 4), PA_CTRL_PIN & 0x0f, AW9523_MODE_GPIO);
    aw9523_io_set_inout((aw9523_port_t)(PA_CTRL_PIN >> 4), PA_CTRL_PIN & 0x0f, AW9523_MODE_OUTPUT);
    aw9523_io_set_level((aw9523_port_t)(PA_CTRL_PIN >> 4), PA_CTRL_PIN & 0x0f, 1);
}


uint8_t Aw9523::aw9523_read_level(aw9523_port_t port)
{
    uint8_t data = 0x00;
    i2c_read_byte((port == AW9523_PORT_0) ? 0x00 : 0x01, &data);
    return data;
}

void Aw9523::aw9523_set_level(aw9523_port_t port, uint8_t value)
{
    uint8_t reg = (port == AW9523_PORT_0) ? 0x02 : 0x03;
    i2c_write_byte(reg, value);
}

void Aw9523::aw9523_io_set_level(aw9523_port_t port, uint8_t pin_num, uint8_t value)
{
    AW9523_CHECK_NUM(pin_num);
    uint8_t reg = (port == AW9523_PORT_0) ? 0x02 : 0x03;
    i2c_write_bit(reg, value > 0, pin_num);
}

void Aw9523::aw9523_set_inout(aw9523_port_t port, uint8_t mode)
{
    uint8_t reg = (port == AW9523_PORT_0) ? 0x04 : 0x05;
    i2c_write_byte(reg, mode);
}

void Aw9523::aw9523_io_set_inout(aw9523_port_t port, uint8_t pin_num, aw9523_inout_mode_t mode)
{
    AW9523_CHECK_NUM(pin_num);
    uint8_t reg = (port == AW9523_PORT_0) ? 0x04 : 0x05;
    uint8_t value = (mode == AW9523_MODE_INPUT) ? 1 : 0;
    i2c_write_bit(reg, value, pin_num);
}

void Aw9523::aw9523_set_port0_pp(uint8_t pp_enable)
{
    i2c_write_bit(0x11, pp_enable ? 1 : 0, 4);
}

void Aw9523::aw9523_set_led_max_current(aw9523_current_t current)
{
    i2c_write_bits(0x11, current, 0, 2);
}

void Aw9523::aw9523_set_gpio_or_led(aw9523_port_t port, uint8_t mode)
{
    uint8_t reg = (port == AW9523_PORT_0) ? 0x12 : 0x13;
    i2c_write_byte(reg, mode);
}

void Aw9523::aw9523_io_set_gpio_or_led(aw9523_port_t port, uint8_t pin_num, aw9523_mode_t mode) {
    AW9523_CHECK_NUM(pin_num);
    uint8_t reg = (port == AW9523_PORT_0) ? 0x12 : 0x13;
    uint8_t value = (mode == AW9523_MODE_GPIO) ? 1 : 0;
    i2c_write_bit(reg, value, pin_num);
}

void Aw9523::aw9523_led_set_duty(aw9523_port_t port, uint8_t pin_num, uint8_t duty)
{
    AW9523_CHECK_NUM(pin_num);
    uint8_t reg;
    if (port == AW9523_PORT_0) {
        reg = 0x24 + pin_num;
    } else {
        reg = 0x20 + pin_num + ((pin_num > 3) ? 0x08 : 0x00);
    }
    i2c_write_byte(reg, duty);
}

void Aw9523::aw9523_leds_set_duty(aw9523_port_t port, uint8_t pin_num, uint8_t nums, uint8_t duty)
{
    AW9523_CHECK_NUM(pin_num + nums - 1);
    uint8_t dutys[8] = {0};
    memset(dutys, duty, 8);
    uint8_t reg;

    if (port == AW9523_PORT_0) {
        reg = 0x24 + pin_num;
        i2c_write_bytes(reg, dutys, nums);
    } else {
        if (pin_num < 4) {
            uint8_t need_write = 4 - pin_num;
            if (need_write > nums) {
                need_write = nums;
            }
            nums -= need_write;
            pin_num = 4;
            i2c_write_bytes(0x20 + pin_num, dutys, need_write);
        }
        if (nums) {
            i2c_write_bytes(0x20 + 0x08 + pin_num, dutys, nums);
        }
    }
}

void Aw9523::aw9523_softreset()
{
    i2c_write_byte(0x7f, 0x00);
}