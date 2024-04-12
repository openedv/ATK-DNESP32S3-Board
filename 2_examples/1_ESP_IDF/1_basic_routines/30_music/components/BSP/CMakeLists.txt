set(src_dirs
            IIC
            LCD
            LED
            SDIO
            SPI
            XL9555
            ES8388
            I2S)

set(include_dirs
            IIC
            LCD
            LED
            SDIO
            SPI
            XL9555
            ES8388
            I2S)

set(requires
            driver
            fatfs)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)