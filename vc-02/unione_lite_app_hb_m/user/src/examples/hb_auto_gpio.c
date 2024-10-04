#include "user_config.h"
#include "user_event.h"
#include "user_gpio.h"
#include "user_player.h"
#include "user_pwm.h"
#include "user_timer.h"
#include "user_uart.h"

#include "uni_media_player.h"

#define TAG "auto_gpio"

#define UART_SEND_MAX      16

typedef struct {
  char  data[UART_SEND_MAX];
  int   len;
}uart_data_t;

const uart_data_t g_uart_buf[] = {
  {{0x00, 0x00, 0x00, 0x01}, 4}, //wakeup_uni
  {{0x00, 0x00, 0x00, 0x02}, 4}, //sleep_uni(INSERT)
  {{0x01, 0x00, 0x01, 0x00}, 4}, //TurnLeft
  {{0x01, 0x00, 0x02, 0x00}, 4}, //TurnRight
  {{0x70, 0x00, 0x00, 0x00}, 4}, //PlayMusic
  {{0x44, 0x44, 0x44, 0x44}, 4}, //Fool
  {{0x00, 0x00, 0x00, 0x03}, 4}, //Stop
  {{0x02, 0x00, 0x00, 0x01}, 4}, //TurnOn
  {{0x02, 0x00, 0x00, 0x02}, 4}, //TurnOff
  {{0x03, 0x00, 0x00, 0x01}, 4}, //Where
};

static void _custom_setting_cb(USER_EVENT_TYPE event,
                               user_event_context_t *context) {
  event_custom_setting_t *setting = NULL;
  if (context) {
    setting = &context->custom_setting;
    LOGT(TAG, "user command: %s", setting->cmd);
    if (0 == uni_strcmp(setting->cmd, "TurnLeft")) {
      user_uart_send(g_uart_buf[2].data, g_uart_buf[2].len);
    } else if (0 == uni_strcmp(setting->cmd, "TurnRight")) {
      user_uart_send(g_uart_buf[3].data, g_uart_buf[3].len);
    } else if (0 == uni_strcmp(setting->cmd, "PlayMusic")) {
      user_uart_send(g_uart_buf[4].data, g_uart_buf[4].len);
    } else if (0 == uni_strcmp(setting->cmd, "Fool")) {
      user_uart_send(g_uart_buf[5].data, g_uart_buf[5].len);
    } else if (0 == uni_strcmp(setting->cmd, "Stop")) {
      user_uart_send(g_uart_buf[6].data, g_uart_buf[6].len);
    } else if (0 == uni_strcmp(setting->cmd, "TurnOn")) {
      user_uart_send(g_uart_buf[7].data, g_uart_buf[7].len);
    } else if (0 == uni_strcmp(setting->cmd, "TurnOff")) {
      user_uart_send(g_uart_buf[8].data, g_uart_buf[8].len);
    } else if (0 == uni_strcmp(setting->cmd, "Where")) {
      user_uart_send(g_uart_buf[9].data, g_uart_buf[9].len);
    } else {
      LOGT(TAG, "Unconcerned command: %s", setting->cmd);
    }
    user_player_reply_list_random(setting->reply_files);
  }
}

static void _goto_awakened_cb(USER_EVENT_TYPE event,
                               user_event_context_t *context) {
  event_goto_awakend_t *awkened = NULL;
  if (context) {
    awkened = &context->goto_awakend;
    user_uart_send(g_uart_buf[0].data, g_uart_buf[0].len);
    user_player_reply_list_random(awkened->reply_files);
  }
}

static void _goto_sleeping_cb (USER_EVENT_TYPE event,
                                     user_event_context_t *context) {
  event_goto_sleeping_t *sleeping = NULL;
  if (context) {
    sleeping = &context->goto_sleeping;
    user_uart_send(g_uart_buf[1].data, g_uart_buf[1].len);
    user_player_reply_list_random(sleeping->reply_files);
  }
}

static void _register_event_callback(void) {
  user_event_subscribe_event(USER_CUSTOM_SETTING, _custom_setting_cb);
  user_event_subscribe_event(USER_GOTO_AWAKENED, _goto_awakened_cb);
  user_event_subscribe_event(USER_GOTO_SLEEPING, _goto_sleeping_cb);
}

int hb_auto_gpio(void) {
  MediaPlayerVolumeSet(3);

  user_gpio_init();
  user_gpio_set_mode(GPIO_NUM_A25, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_A25, 0);
  user_gpio_set_mode(GPIO_NUM_A26, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_A26, 0);
  user_gpio_set_mode(GPIO_NUM_A27, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_A27, 0);
  user_gpio_set_mode(GPIO_NUM_A28, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_A28, 0);
  user_gpio_set_mode(GPIO_NUM_B2, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_B2, 0);
  user_gpio_set_mode(GPIO_NUM_B3, GPIO_MODE_OUT);
  user_gpio_set_value(GPIO_NUM_B3, 0);
  user_uart_init(NULL);
  _register_event_callback();
  return 0;
}

