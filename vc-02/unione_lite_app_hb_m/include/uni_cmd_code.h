#ifndef INC_UNI_CMD_CODE_H_
#define INC_UNI_CMD_CODE_H_

typedef struct {
  uni_u8      cmd_code; /* cmd code fro send base on SUCP */
  const char  *cmd_str; /* action string on UDP */;
} cmd_code_map_t;

const cmd_code_map_t g_cmd_code_arry[] = {
  {0x0, "wakeup_uni"},
  {0x1, "exitUni"},
  {0x2, "TurnLeft"},
  {0x3, "TurnRight"},
  {0x4, "PlayMusic"},
  {0x5, "Fool"},
  {0x6, "Stop"},
  {0x7, "TurnOn"},
  {0x8, "TurnOff"},
  {0x9, "Where"},
  {0xa, "Bye"},
};

#endif
