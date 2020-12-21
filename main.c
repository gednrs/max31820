#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "max31820.h" 

void app_main()
{

   max31820_init();

    while (1) 
    {
        max31820_get_temp();
       // printf("Temperature: %.3f\n", );
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}