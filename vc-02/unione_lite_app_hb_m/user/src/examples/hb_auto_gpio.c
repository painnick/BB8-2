#include "user_config.h"
#include "user_event.h"
#include "user_gpio.h"
#include "user_player.h"
#include "user_pwm.h"
#include "user_timer.h"
#include "user_uart.h"
#include "idle_detect.h"
#include "user_asr.h"
#include "uni_black_board.h"

#define TAG "auto_gpio"

#define UART_SEND_MAX      16

typedef struct {
  char data[UART_SEND_MAX];
  int len;
} uart_data_t;

/*
Sleep=Bye|Good bye|See you|Sleep@Go to sleep
Stop=Stop@Stop
TurnLeft=Turn left|Left@Turn left
TurnRight=Turn right|Right@Turn right
LightOn=Turn on|Turn on the light@Turn on the light
LightOff=Turn off|Turn off the light@Turn off the light
BTOn=Bluetooth on|Turn on the Bluetooth@Turn on the Bluetooth
BTOff=Bluetooth off|Turn off the Bluetooth@Turn off the Bluetooth
APOn=AP on|Turn on the AP@Turn on the AP
APOff=AP off|Turn off the AP@Turn off the AP
PlayMusic=Play music@Play music
Fool=Fool|Stupid@Fool
LookAtMe=Look at me|Hey!@Where are you?
Attention=Attention@Yes, Sir!
Help=Help|Help me@Wait a moment
*/

const uart_data_t g_uart_buf[] = {
    {{0x01}, 1}, //wakeup_uni
    {{0x02}, 1}, //sleep_uni, Sleep
    {{0x03}, 1}, //Stop

    {{0x11}, 1}, //TurnLeft
    {{0x12}, 1}, //TurnRight

    {{0x41}, 1}, //LightOn
    {{0x42}, 1}, //LightOff

    {{0x51}, 1}, //APOn
    {{0x52}, 1}, //APOff
    {{0x53}, 1}, //BTOn
    {{0x54}, 1}, //BTOff

    {{0x21}, 1}, //PlayMusic

    {{0x31}, 1}, //Fool
    {{0x32}, 1}, //LookAtMe
    {{0x33}, 1}, //Attention
};

/* ========================================
 * System
 * 103 Awakened(I'm ready)
 * 104 Sleeping(Go to sleep)
 * 105 Exit(Good bye)
 * 106 Service On(Booting message)
 * ----------------------------------------
 * User
 * 107 Fool
 * 108 ~ 111
 * 112 Help
 * ======================================== */

static void _custom_setting_cb(USER_EVENT_TYPE event,
                               user_event_context_t *context) {
  event_custom_setting_t *setting = NULL;
  if (context) {
    setting = &context->custom_setting;
    LOGT(TAG, "user command: %s", setting->cmd);
    if (0 == uni_strcmp(setting->cmd, "Sleep")) {
      user_uart_send(g_uart_buf[1].data, g_uart_buf[1].len);
    } else if (0 == uni_strcmp(setting->cmd, "Stop")) {
      user_uart_send(g_uart_buf[2].data, g_uart_buf[2].len);
    } else if (0 == uni_strcmp(setting->cmd, "TurnLeft")) {
      user_uart_send(g_uart_buf[3].data, g_uart_buf[3].len);
    } else if (0 == uni_strcmp(setting->cmd, "TurnRight")) {
      user_uart_send(g_uart_buf[4].data, g_uart_buf[4].len);
    } else if (0 == uni_strcmp(setting->cmd, "LightOn")) {
      user_uart_send(g_uart_buf[5].data, g_uart_buf[5].len);
    } else if (0 == uni_strcmp(setting->cmd, "LightOff")) {
      user_uart_send(g_uart_buf[6].data, g_uart_buf[6].len);
    } else if (0 == uni_strcmp(setting->cmd, "APOn")) {
      user_uart_send(g_uart_buf[7].data, g_uart_buf[7].len);
    } else if (0 == uni_strcmp(setting->cmd, "APOff")) {
      user_uart_send(g_uart_buf[8].data, g_uart_buf[8].len);
    } else if (0 == uni_strcmp(setting->cmd, "BTOn")) {
      user_uart_send(g_uart_buf[9].data, g_uart_buf[9].len);
    } else if (0 == uni_strcmp(setting->cmd, "BTOff")) {
      user_uart_send(g_uart_buf[10].data, g_uart_buf[10].len);
    } else if (0 == uni_strcmp(setting->cmd, "PlayMusic")) {
      // TODO : Send 0x3FFCA4E7
      user_uart_send(g_uart_buf[11].data, g_uart_buf[11].len);
      return;
    } else if (0 == uni_strcmp(setting->cmd, "Fool")) {
      user_uart_send(g_uart_buf[12].data, g_uart_buf[12].len);
      user_player_reply_list_random("[107]");
      return;
    } else if (0 == uni_strcmp(setting->cmd, "LookAtMe")) {
      user_uart_send(g_uart_buf[13].data, g_uart_buf[13].len);
    } else if (0 == uni_strcmp(setting->cmd, "Attention")) {
      user_uart_send(g_uart_buf[14].data, g_uart_buf[14].len);
    } else if (0 == uni_strcmp(setting->cmd, "Help")) {
      user_player_reply_list_random("[112]");
      return;
    } else {
      LOGT(TAG, "Unconcerned command: %s", setting->cmd);
    }
    user_player_reply_list_random("[108,109,110,111]");
  }
}

static void _goto_awakened_cb(USER_EVENT_TYPE event,
                              user_event_context_t *context) {
  event_goto_awakend_t *awkened = NULL;
  if (context) {
    awkened = &context->goto_awakend;
    user_uart_send(g_uart_buf[0].data, g_uart_buf[0].len);
    user_player_reply_list_random("[103]"); // 103.mp3
  }
}

static void _goto_sleeping_cb(USER_EVENT_TYPE event,
                              user_event_context_t *context) {
  event_goto_sleeping_t *sleeping = NULL;
  if (context) {
    sleeping = &context->goto_sleeping;
    user_uart_send(g_uart_buf[1].data, g_uart_buf[1].len);
    user_player_reply_list_random("[104]"); // 104.mp3
  }
}

static void _register_event_callback(void) {
  user_event_subscribe_event(USER_CUSTOM_SETTING, _custom_setting_cb);
  user_event_subscribe_event(USER_GOTO_AWAKENED, _goto_awakened_cb);
  user_event_subscribe_event(USER_GOTO_SLEEPING, _goto_sleeping_cb);
}

static void _hb_uart_recv(char *buf, int len) {
  // TODO. Do not call on playing music
  if (buf[0] == 0x04) { // KeepAlive
    user_player_reply_list_random("[108,109,110,111]");
  }
}

int hb_auto_gpio(void) {
  MediaPlayerVolumeSet(3);
  user_asr_timeout_disable();
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

  user_uart_init(_hb_uart_recv);
  uart_data_t ready = {{0x00}, 1};
  user_uart_send(ready.data, ready.len);

  _register_event_callback();

  return 0;
}

