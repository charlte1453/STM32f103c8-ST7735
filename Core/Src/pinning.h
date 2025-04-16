#pragma once

// Port A
#define PIN_CAMERA_DB0          0   // PA0  - Camera data line 0
#define PIN_CAMERA_DB1          1   // PA1  - Camera data line 1
#define PIN_CAMERA_DB2          2   // PA2  - Camera data line 2
#define PIN_CAMERA_DB3          3   // PA3  - Camera data line 3
#define PIN_CAMERA_DB4          4   // PA4  - Camera data line 4
#define PIN_CAMERA_DB5          5   // PA5  - Camera data line 5
#define PIN_CAMERA_DB6          6   // PA6  - Camera data line 6
#define PIN_CAMERA_DB7          7   // PA7  - Camera data line 7
#define PIN_CAMERA_MCLK         8   // PA8  - Camera main clock (MCO/TIM1_CH1)
#define PIN_CAMERA_VSYNC        9   // PA9  - Camera VSYNC (TIM1_CH2)

// Port B
#define PIN_CAMERA_PCLK         4   // PB4  - Camera pixel clock (TIM3_CH1)
#define PIN_CAMERA_HSYNC        5   // PB5  - Camera VSYNC (TIM3_CH2)
#define PIN_CAMERA_SCL          8   // PB8  - Camera control I2C (I2C1_SCL)
#define PIN_CAMERA_SDA          9   // PB9  - Camera control I2C (I2C1_SDA)
#define PIN_CAMERA_RESET        12  // PB12 - Camera reset (active low)

// Port C
