#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000
#define AUDIO_DEFAULT_OUTPUT_VOLUME 80

#define AUDIO_INPUT_REFERENCE    true

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_47
#define AUDIO_I2S_GPIO_WS GPIO_NUM_13
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_14
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_21
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_12

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_46
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_5
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_4
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR
#define AUDIO_CODEC_ES7210_ADDR  ES7210_CODEC_DEFAULT_ADDR

#define BUILTIN_LED_GPIO        GPIO_NUM_NC
#define BOOT_BUTTON_GPIO        GPIO_NUM_0
#define VOLUME_UP_BUTTON_GPIO   GPIO_NUM_NC
#define VOLUME_DOWN_BUTTON_GPIO GPIO_NUM_NC

#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  480
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y true
#define DISPLAY_SWAP_XY false

#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_40
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT false


#define AW9523_P1_6         (0x16)

// BOARD BASE PIN
#define BOARD_RESET_PIN     (AW9523_P1_6)

#define AW9523_P0_0         (0x00)
#define AW9523_P0_1         (0x01)
#define AW9523_P0_2         (0x02)
#define AW9523_P0_3         (0x03)
#define AW9523_P0_4         (0x04)
#define AW9523_P0_5         (0x05)
#define AW9523_P0_6         (0x06)
#define AW9523_P0_7         (0x07)
#define AW9523_P1_0         (0x10)
#define AW9523_P1_1         (0x11)
#define AW9523_P1_2         (0x12)
#define AW9523_P1_3         (0x13)
#define AW9523_P1_4         (0x14)
#define AW9523_P1_5         (0x15)
#define AW9523_P1_6         (0x16)
#define AW9523_P1_7         (0x17)



// LCD PIN
#define LCD_BL_PIN          (-1)
#define LCD_BL_0_PIN        (AW9523_P1_0)
#define LCD_BL_1_PIN        (AW9523_P1_1)
#define LCD_BL_2_PIN        (AW9523_P1_2)
#define LCD_BL_3_PIN        (AW9523_P1_3)
#define LCD_BL_4_PIN        (AW9523_P0_0)
#define LCD_BL_5_PIN        (AW9523_P0_1)

// LCD 8080 PIN
#define LCD_RST_PIN         (-1)
#define LCD_RS_PIN          (45)
#define LCD_CS_PIN          (-1)
#define LCD_TE_PIN          (-1)
#define LCD_WR_PIN          (10)
#define LCD_RD_PIN          (-1)

#define LCD_D0_PIN          (9)
#define LCD_D1_PIN          (3)
#define LCD_D2_PIN          (8)
#define LCD_D3_PIN          (18)
#define LCD_D4_PIN          (17)
#define LCD_D5_PIN          (16)
#define LCD_D6_PIN          (15)
#define LCD_D7_PIN          (7)

// TOUCH PAD PIN
#define TP_I2C_SDA_PIN      (BOARD_I2C_SDA_PIN)
#define TP_I2C_SCL_PIN      (BOARD_I2C_SCL_PIN)
#define TP_I2C_INT_PIN      (6)
#define TP_I2C_RST_PIN      (-1)

// USB
#define USB_DP_PIN          (20)
#define USB_DN_PIN          (19)

// I2S PIN
#define I2S_SCL_PIN          (BOARD_I2C_SCL_PIN)
#define I2S_SDA_PIN          (BOARD_I2C_SDA_PIN)
#define I2S_MCLK_PIN         (47)
#define I2S_SCLK_PIN         (14)
#define I2S_LRCK_PIN         (13)
#define I2S_DOUT_PIN         (12)
#define I2S_DIN_PIN          (21)
#define PA_CTRL_PIN          (AW9523_P1_5)

#endif // _BOARD_CONFIG_H_
