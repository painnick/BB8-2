#ifndef INC_UNI_CMD_CODE_H_
#define INC_UNI_CMD_CODE_H_

typedef struct {
  uni_u8      cmd_code; /* cmd code fro send base on SUCP */
  const char  *cmd_str; /* action string on UDP */;
} cmd_code_map_t;

const cmd_code_map_t g_cmd_code_arry[] = {
  {0x0, "wakeup_uni"},
  {0x1, "exitUni"},
  {0x2, "Sleep"},
  {0x3, "Stop"},
  {0x4, "TurnLeft"},
  {0x5, "TurnRight"},
  {0x6, "LightOn"},
  {0x7, "LightOff"},
  {0x8, "BTOn"},
  {0x9, "BTOff"},
  {0xa, "APOn"},
  {0xb, "APOff"},
  {0xc, "PlayMusic"},
  {0xd, "Fool"},
  {0xe, "LookAtMe"},
  {0xf, "Attention"},
  {0x10, "Help"},
};

#endif
