#include <stdbool.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/cyw43_arch.h"
/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Function to check if a number is in an array
bool is_reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main() {
    // Initialize stdio for serial communication
    stdio_init_all();

    // Initial delay to allow for serial monitor connection
    for (int i = 15; i > 0; i--) {
        printf("Starting scan in %d seconds...\n", i);
        sleep_ms(1000);
    }
    printf("----------------------------------\n");
    printf("Starting I2C Scanner for Pico W\n");
    printf("----------------------------------\n");

    // Array of GPIO pins to test for SCL and SDA
    const uint pins_to_scan[] = {18, 19, 20, 21, 22};


    // Iterate over all possible SDA pins
    for (int sda_idx = 0; sda_idx < count_of(pins_to_scan); sda_idx++) {
        uint sda_pin = pins_to_scan[sda_idx];
        
        // Iterate over all possible SCL pins
        for (int scl_idx = 0; scl_idx < count_of(pins_to_scan); scl_idx++) {
            uint scl_pin = pins_to_scan[scl_idx];

            // A pin cannot be both SDA and SCL
            if (scl_pin == sda_pin) {
                continue;
            }

            printf("\n\nScanning pair: SDA on GPIO %d, SCL on GPIO %d\n", sda_pin, scl_pin);
            printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

            // Initialize I2C instance 0
            i2c_init(i2c0, 100 * 1000); // 100kHz
            
            // Set the GPIO pins for I2C
            gpio_set_function(sda_pin, GPIO_FUNC_I2C);
            gpio_set_function(scl_pin, GPIO_FUNC_I2C);
            gpio_pull_up(sda_pin);
            gpio_pull_up(scl_pin);

            int devices_found = 0;
            for (int addr = 0; addr < (1 << 7); ++addr) {
                if (addr % 16 == 0) {
                    printf("%02x ", addr);
                }

                // Skip reserved addresses
                if (is_reserved_addr(addr)) {
                    printf(" ");
                } else {
                    uint8_t rxdata;
                    // Use a timeout to prevent getting stuck
                    int ret = i2c_read_timeout_us(i2c0, addr, &rxdata, 1, false, 10000); // 10ms timeout

                    if (ret >= 0) {
                        printf("%02x ", addr);
                        devices_found++;
                    } else {
                        printf(".  ");
                    }
                }
                 if (addr % 16 == 15) {
                    printf("\n");
                }
            }
            if (devices_found == 0) {
              printf("\nNo I2C devices found on this pin combination.\n");
            } else {
              printf("\nFound %d device(s).\n", devices_found);
            }
            // De-initialize I2C to release pins for the next scan
            i2c_deinit(i2c0);
            gpio_disable_pulls(sda_pin);
            gpio_set_function(sda_pin, GPIO_FUNC_NULL);
            gpio_disable_pulls(scl_pin);
            gpio_set_function(scl_pin, GPIO_FUNC_NULL);
        }
    }
    
    printf("\n\n------------------\n");
    printf("Scan complete.\n");
    printf("------------------\n");


    // Loop forever
    while (1) {
        tight_loop_contents();
    }

    return 0;
}


