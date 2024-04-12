set(src_dirs
            LED
            IIC
            XL9555
            SPI
            LCD
            CAMERA)

set(include_dirs
            LED
            IIC
            XL9555
            SPI
            LCD
            CAMERA)

set(requires
            driver
            esp32-camera)

idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

component_compile_options(-ffast-math -O3 -Wno-error=format=-Wno-format)