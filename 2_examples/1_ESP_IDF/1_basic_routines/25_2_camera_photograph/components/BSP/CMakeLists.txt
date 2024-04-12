set(src_dirs
            CAMERA
            IIC
            LCD
            LED
            SPI
            XL9555
            SDIO)

set(include_dirs
            CAMERA
            IIC
            LCD
            LED
            SPI
            XL9555
            SDIO)

set(requires
            driver
            esp32-camera
            fatfs)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)