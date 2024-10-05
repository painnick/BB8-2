#ifndef INC_UNI_NLU_CONTENT_H_
#define INC_UNI_NLU_CONTENT_H_

typedef struct {
  uni_u32 key_word_hash_code; /* 存放识别词汇对应的hashcode */
  uni_u8  nlu_content_str_index; /* 存放nlu映射表中的索引，实现多个识别词汇可对应同一个nlu，暂支持256条，如果不够换u16 */
  char    *hash_collision_orginal_str /* 类似Java String equal，当hash发生碰撞时，赋值为识别词汇，否则设置为NULL */;
} uni_nlu_content_mapping_t;

enum {
  eCMD_wakeup_uni,
  eCMD_exitUni,
  eCMD_TurnLeft,
  eCMD_TurnRight,
  eCMD_PlayMusic,
  eCMD_Fool,
  eCMD_Stop,
  eCMD_TurnOn,
  eCMD_TurnOff,
  eCMD_LookAtMe,
  eCMD_Help,
  eCMD_WiFiOn,
  eCMD_WiFiOff,
  eCMD_BTOn,
  eCMD_BTOff,
  eCMD_VolumeUpUni,
  eCMD_VolumeDownUni,
  eCMD_ComeOn,
};

const char* const g_nlu_content_str[] = {
[eCMD_wakeup_uni] = "{\"asr\":\"hello\",\"cmd\":\"wakeup_uni\",\"pcm\":\"[103]\"}",
[eCMD_exitUni] = "{\"asr\":\"Bye\",\"cmd\":\"exitUni\",\"pcm\":\"[105]\"}",
[eCMD_TurnLeft] = "{\"asr\":\"turn left\",\"cmd\":\"TurnLeft\",\"pcm\":\"[107]\"}",
[eCMD_TurnRight] = "{\"asr\":\"turn right\",\"cmd\":\"TurnRight\",\"pcm\":\"[108]\"}",
[eCMD_PlayMusic] = "{\"asr\":\"play music\",\"cmd\":\"PlayMusic\",\"pcm\":\"[109]\"}",
[eCMD_Fool] = "{\"asr\":\"fool\",\"cmd\":\"Fool\",\"pcm\":\"[110]\"}",
[eCMD_Stop] = "{\"asr\":\"stop\",\"cmd\":\"Stop\",\"pcm\":\"[111]\"}",
[eCMD_TurnOn] = "{\"asr\":\"turn on the light\",\"cmd\":\"TurnOn\",\"pcm\":\"[112]\"}",
[eCMD_TurnOff] = "{\"asr\":\"turn off the light\",\"cmd\":\"TurnOff\",\"pcm\":\"[113]\"}",
[eCMD_LookAtMe] = "{\"asr\":\"where are you\",\"cmd\":\"LookAtMe\",\"pcm\":\"[114]\"}",
[eCMD_Help] = "{\"asr\":\"help me\",\"cmd\":\"Help\",\"pcm\":\"[115]\"}",
[eCMD_WiFiOn] = "{\"asr\":\"wi-fi on\",\"cmd\":\"WiFiOn\",\"pcm\":\"[116]\"}",
[eCMD_WiFiOff] = "{\"asr\":\"wi-fi off\",\"cmd\":\"WiFiOff\",\"pcm\":\"[117]\"}",
[eCMD_BTOn] = "{\"asr\":\"bluetooth on\",\"cmd\":\"BTOn\",\"pcm\":\"[118]\"}",
[eCMD_BTOff] = "{\"asr\":\"bluetooth off\",\"cmd\":\"BTOff\",\"pcm\":\"[119]\"}",
[eCMD_VolumeUpUni] = "{\"asr\":\"volume up\",\"cmd\":\"VolumeUpUni\",\"pcm\":\"[120]\"}",
[eCMD_VolumeDownUni] = "{\"asr\":\"volume down\",\"cmd\":\"VolumeDownUni\",\"pcm\":\"[121]\"}",
[eCMD_ComeOn] = "{\"asr\":\"come on\",\"cmd\":\"ComeOn\",\"pcm\":\"[122]\"}",
};

/*TODO perf sort by hashcode O(logN), now version O(N)*/
const uni_nlu_content_mapping_t g_nlu_content_mapping[] = {
  {99162322U/*hello*/, eCMD_wakeup_uni, NULL},
  {95268U/*b b*/, eCMD_wakeup_uni, NULL},
  {3136U/*bb*/, eCMD_wakeup_uni, NULL},
  {67278U/*Bye*/, eCMD_exitUni, NULL},
  {4275161354U/*Go to sleep*/, eCMD_exitUni, NULL},
  {3634254514U/*See you*/, eCMD_exitUni, NULL},
  {4112307658U/*turn left*/, eCMD_TurnLeft, NULL},
  {3317767U/*left*/, eCMD_TurnLeft, NULL},
  {2933146809U/*turn right*/, eCMD_TurnRight, NULL},
  {108511772U/*right*/, eCMD_TurnRight, NULL},
  {116807225U/*play music*/, eCMD_PlayMusic, NULL},
  {3148902U/*fool*/, eCMD_Fool, NULL},
  {3403077335U/*stupid*/, eCMD_Fool, NULL},
  {3540994U/*stop*/, eCMD_Stop, NULL},
  {3717175913U/*turn on the light*/, eCMD_TurnOn, NULL},
  {3329415266U/*turn on*/, eCMD_TurnOn, NULL},
  {279435475U/*turn off the light*/, eCMD_TurnOff, NULL},
  {132657996U/*turn off*/, eCMD_TurnOff, NULL},
  {3565949466U/*where are you*/, eCMD_LookAtMe, NULL},
  {3148632036U/*look at me*/, eCMD_LookAtMe, NULL},
  {805757655U/*help me*/, eCMD_Help, NULL},
  {3198785U/*help*/, eCMD_Help, NULL},
  {1320278945U/*wi-fi on*/, eCMD_WiFiOn, NULL},
  {2273941485U/*wi-fi off*/, eCMD_WiFiOff, NULL},
  {2900728977U/*bluetooth on*/, eCMD_BTOn, NULL},
  {4023252221U/*bluetooth off*/, eCMD_BTOff, NULL},
  {2166624609U/*volume up*/, eCMD_VolumeUpUni, NULL},
  {3361574376U/*volume down*/, eCMD_VolumeDownUni, NULL},
  {950093947U/*come on*/, eCMD_ComeOn, NULL},
};

#endif
