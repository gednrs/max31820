#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "esp32/rom/ets_sys.h"
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_err.h"
#include "esp_log.h"

#define MAX_GPIO_NUM 4

static const char *TAG = "MAX31820";
static unsigned char verify_device = 0;

void max31820_send(char bit)
{
  gpio_set_direction(MAX_GPIO_NUM, GPIO_MODE_OUTPUT);
  gpio_set_level(MAX_GPIO_NUM, 0);
  ets_delay_us(5);
  if(bit == 1)gpio_set_level(MAX_GPIO_NUM, 1);
  ets_delay_us(80);
  gpio_set_level(MAX_GPIO_NUM , 1);
}

// Reads one bit from bus
unsigned char max31820_read(void)
{
  unsigned char max_event = 0;
  gpio_set_direction(MAX_GPIO_NUM, GPIO_MODE_OUTPUT);
  gpio_set_level(MAX_GPIO_NUM, 0);
  ets_delay_us(2);
  gpio_set_level(MAX_GPIO_NUM, 1);
  ets_delay_us(15);
  gpio_set_direction(MAX_GPIO_NUM, GPIO_MODE_INPUT);

  if(gpio_get_level(MAX_GPIO_NUM) == 1) 
    max_event = 1; 
  else 
    max_event = 0;

  return(max_event);
}

// Sends one byte to bus
void max31820_send_byte(char data)
{
  unsigned char i;
  unsigned char x;

  for(i = 0; i < 8; i++){
    x = data >> i;
    x &= 0x01;
    max31820_send(x);
  }
  ets_delay_us(100);
}

// Reads one byte from bus
unsigned char max31820_read_byte(void)
{
  unsigned char i;
  unsigned char data = 0;

  for (i = 0; i < 8; i++)
  {
    if(max31820_read()) data|= 0x01 << i;
    ets_delay_us(15);
  }
  return(data);
}

// Sends reset pulse
unsigned char max31820_send_reset_pulse(void)
{
  unsigned char max_event;
  gpio_set_direction(MAX_GPIO_NUM, GPIO_MODE_OUTPUT);
  gpio_set_level(MAX_GPIO_NUM, 0);
  ets_delay_us(500);
  gpio_set_level(MAX_GPIO_NUM, 1);
  gpio_set_direction(MAX_GPIO_NUM, GPIO_MODE_INPUT);
  ets_delay_us(30);

  if(gpio_get_level(MAX_GPIO_NUM) == 0) 
    max_event = 1; 
  else 
    max_event = 0;

  ets_delay_us(470);
/*
  if(gpio_get_level(MAX_GPIO_NUM) == 1) 
    max_event = 1; 
  else 
    max_event = 0;
*/
  return max_event;
}

// Returns temperature from sensor
float max31820_get_temp(void) {

    char temp1 = 0, temp2 = 0;
    verify_device = max31820_send_reset_pulse();

      if(verify_device == 1)
      {
        max31820_send_byte(0xCC);
        max31820_send_byte(0x44);
        vTaskDelay(750 / portTICK_RATE_MS);
        verify_device = max31820_send_reset_pulse();
        max31820_send_byte(0xCC);
        max31820_send_byte(0xBE);
        temp1 = max31820_read_byte();
        temp2 = max31820_read_byte();
        verify_device = max31820_send_reset_pulse();
        float temp = (float)(temp1 + (temp2 * 256)) / 16;
        ESP_LOGI(TAG, "Temperatura: %.3f", temp);

        return temp;
      }
      else{
        ESP_LOGE(TAG, "Erro na leitura");
        return -100;  
      }

      
}
void max31820_init(){

  ESP_LOGI(TAG, "Iniciando o max31820");
  gpio_pad_select_gpio(MAX_GPIO_NUM);
  verify_device = max31820_send_reset_pulse();

  if(verify_device == 1)
    ESP_LOGI(TAG, "max31820 pronto!");
  else
    ESP_LOGE(TAG, "max31820 não encontrado!");
}
