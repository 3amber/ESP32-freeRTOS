#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

#define LED_RED GPIO_NUM_18
#define LED_GREEN GPIO_NUM_4
#define LED_YELLOW GPIO_NUM_5
#define LED_BLUE GPIO_NUM_2
#define BTN_START GPIO_NUM_13
#define DELAY_TIME 200

volatile bool button_pressed = false;



struct led_task_parameters_t
{
  gpio_num_t led_gpio;
  TickType_t blink_time;
};
static void gpio_isr_handler(void* arg)
{
  button_pressed = true;
}

static led_task_parameters_t red_led_gpio = {LED_RED, 2000};
static led_task_parameters_t yellow_led_gpio = {LED_YELLOW, 1500};
static led_task_parameters_t green_led_gpio = {LED_GREEN, 1000};
static led_task_parameters_t blue_led_gpio = {LED_BLUE, 500};

void button_config()
{
  gpio_install_isr_service(0);
  printf("configuring button\n");
  gpio_reset_pin(BTN_START);
  gpio_set_direction(BTN_START, GPIO_MODE_INPUT);
  gpio_pullup_en(BTN_START);
  gpio_set_intr_type(BTN_START, GPIO_INTR_POSEDGE);
  gpio_isr_handler_add(BTN_START, gpio_isr_handler, NULL);
  printf("config complete\n");
}
void led_task(void *pvParameter)
{
  gpio_num_t led_gpio = ((led_task_parameters_t *)pvParameter)->led_gpio;
  TickType_t blink_time = ((led_task_parameters_t *)pvParameter)->blink_time;
  uint8_t led_value = 0;
  gpio_reset_pin(led_gpio);
  gpio_set_direction(led_gpio, GPIO_MODE_OUTPUT);


  while (1) {
    gpio_set_level(led_gpio, led_value);
    led_value = !led_value;
    vTaskDelay(blink_time / portTICK_PERIOD_MS);
  }
  vTaskDelete( NULL );
}


extern "C" void app_main()
{
  button_config();
  uint8_t led_value = 0;
  while (1) {
    if (button_pressed) {
      printf("*\n");
      button_pressed = false;
      led_value = !led_value;
      gpio_set_level(LED_RED, led_value);
      gpio_set_level(LED_GREEN, led_value);
      gpio_set_level(LED_YELLOW, led_value);
      gpio_set_level(LED_BLUE, led_value);

      vTaskDelay(DELAY_TIME / portTICK_PERIOD_MS);

      xTaskCreate(
        &led_task, // task function
        "red_led_task", // task name
        2048, // stack size in words
        &red_led_gpio, // pointer to parameters
        5, // priority
        NULL); // out pointer to task handle

      xTaskCreate(
        &led_task, // task function
        "yellow_led_task", // task name
        2048, // stack size in words
        &yellow_led_gpio, // pointer to parameters
        5, // priority
        NULL); // out pointer to task handle

      xTaskCreate(
        &led_task, // task function
        "green_led_task", // task name
        2048, // stack size in words
        &green_led_gpio, // pointer to parameters
        5, // priority
        NULL); // out pointer to task handle

      xTaskCreate(
        &led_task, // task function
        "blue_led_task", // task name
        2048, // stack size in words
        &blue_led_gpio, // pointer to parameters
        5, // priority
        NULL); // out pointer to task handle
    }
  }
}