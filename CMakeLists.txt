cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD "pico")

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(carrito)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

if (TARGET tinyusb_device)
	add_executable(carrito
		carrito.c
	)

	# Add pico_stdlib library which aggregates commonly used features
	target_link_libraries(carrito pico_stdlib hardware_irq hardware_pwm pico_stdlib)

	pico_enable_stdio_usb(carrito 1)

	# create map/bin/hex/uf2 file in addition to ELF.
	pico_add_extra_outputs(carrito)

elseif(PICO_ON_DEVICE)
	message(WARNING "not building hello_usb because TinyUSB sumbodule is not initialized in the 
	SDK")
endif()