set(src_dirs
            IIC
            LCD
            LED
            SPI
            XL9555
            KEY
            24CXX
            ADC
            AP3216C
            QMA6100P
            ES8388
            I2S
            SDIO
            CAMERA
            SENSOR
            RTC
            EMISSION
            RENC
            ESPTIM)

set(include_dirs
            IIC
            LCD
            LED
            SPI
            XL9555
            KEY
            24CXX
            ADC
            AP3216C
            QMA6100P
            ES8388
            I2S
            SDIO
            CAMERA
            SENSOR
            RTC
            EMISSION
            RENC
            ESPTIM)

set(requires
            driver
            fatfs
            esp_adc
            esp32-camera
            newlib
            esp_timer)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)