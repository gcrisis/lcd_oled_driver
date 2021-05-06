# lcd_oled_driver
kinds of lcd and oled drivers

支持的oled包括：
0.91in
0.96in
0.95rgb
1.3
1.5&1.5rgb




支持的lcd包括：
ili9341




工程结构：
.
├── Core
│   ├── Inc
│   │   ├── Config/
│   │   ├── devices/
│   │   ├── fonts/
│   │   ├── images/
│   │   ├── interface/
│   │   ├── main.h
│   │   ├── misc/
│   │   ├── stm32_assert.h
│   │   ├── stm32f1xx_hal_conf.h
│   │   └── stm32f1xx_it.h
│   └── Src
│       ├── devices/
│       ├── fonts/
│       ├── images/
│       ├── interface/
│       ├── main.c
│       ├── misc/
│       ├── stm32f1xx_it.c
│       └── system_stm32f1xx.c
├── Drivers
│   ├── CMSIS
│   │   ├── Core
│   │   ├── Core_A
│   │   ├── Device
│   │   ├── docs
│   │   ├── DSP
│   │   ├── Include
│   │   ├── Lib
│   │   ├── LICENSE.txt
│   │   ├── NN
│   │   ├── RTOS
│   │   └── RTOS2
│   └── STM32F1xx_HAL_Driver
│       ├── Inc
│       └── Src
├── lcd-oled.ioc
├── LICENSE
├── Makefile
├── README.md
├── startup_stm32f103xe.s
└── STM32F103ZETx_FLASH.ld







