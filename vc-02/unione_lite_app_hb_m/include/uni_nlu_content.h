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
  eCMD_Sleep,
  eCMD_Stop,
  eCMD_TurnLeft,
  eCMD_TurnRight,
  eCMD_LightOn,
  eCMD_LightOff,
  eCMD_BTOn,
  eCMD_BTOff,
  eCMD_APOn,
  eCMD_APOff,
  eCMD_PlayMusic,
  eCMD_Fool,
  eCMD_LookAtMe,
  eCMD_Attention,
  eCMD_Help,
};

const char* const g_nlu_content_str[] = {
[eCMD_wakeup_uni] = "{\"asr\":\"hello\",\"cmd\":\"wakeup_uni\",\"pcm\":\"[103]\"}",
[eCMD_exitUni] = "{\"asr\":\"Go to sleep\",\"cmd\":\"exitUni\",\"pcm\":\"[105]\"}",
[eCMD_Sleep] = "{\"asr\":\"bye\",\"cmd\":\"Sleep\",\"pcm\":\"[107]\"}",
[eCMD_Stop] = "{\"asr\":\"stop\",\"cmd\":\"Stop\",\"pcm\":\"[108]\"}",
[eCMD_TurnLeft] = "{\"asr\":\"turn left\",\"cmd\":\"TurnLeft\",\"pcm\":\"[109]\"}",
[eCMD_TurnRight] = "{\"asr\":\"turn right\",\"cmd\":\"TurnRight\",\"pcm\":\"[110]\"}",
[eCMD_LightOn] = "{\"asr\":\"turn on\",\"cmd\":\"LightOn\",\"pcm\":\"[111]\"}",
[eCMD_LightOff] = "{\"asr\":\"turn off\",\"cmd\":\"LightOff\",\"pcm\":\"[112]\"}",
[eCMD_BTOn] = "{\"asr\":\"bluetooth on\",\"cmd\":\"BTOn\",\"pcm\":\"[113]\"}",
[eCMD_BTOff] = "{\"asr\":\"bluetooth off\",\"cmd\":\"BTOff\",\"pcm\":\"[114]\"}",
[eCMD_APOn] = "{\"asr\":\"ap on\",\"cmd\":\"APOn\",\"pcm\":\"[115]\"}",
[eCMD_APOff] = "{\"asr\":\"ap off\",\"cmd\":\"APOff\",\"pcm\":\"[116]\"}",
[eCMD_PlayMusic] = "{\"asr\":\"play music\",\"cmd\":\"PlayMusic\",\"pcm\":\"[117]\"}",
[eCMD_Fool] = "{\"asr\":\"fool\",\"cmd\":\"Fool\",\"pcm\":\"[118]\"}",
[eCMD_LookAtMe] = "{\"asr\":\"look at me\",\"cmd\":\"LookAtMe\",\"pcm\":\"[119]\"}",
[eCMD_Attention] = "{\"asr\":\"attention\",\"cmd\":\"Attention\",\"pcm\":\"[120]\"}",
[eCMD_Help] = "{\"asr\":\"help\",\"cmd\":\"Help\",\"pcm\":\"[121]\"}",
};

/*TODO perf sort by hashcode O(logN), now version O(N)*/
const uni_nlu_content_mapping_t g_nlu_content_mapping[] = {
  {99162322U/*hello*/, eCMD_wakeup_uni, NULL},
  {95268U/*b b*/, eCMD_wakeup_uni, NULL},
  {3136U/*bb*/, eCMD_wakeup_uni, NULL},
  {4275161354U/*Go to sleep*/, eCMD_exitUni, NULL},
  {98030U/*bye*/, eCMD_Sleep, NULL},
  {2120738059U/*good bye*/, eCMD_Sleep, NULL},
  {1969601234U/*see you*/, eCMD_Sleep, NULL},
  {109522647U/*sleep*/, eCMD_Sleep, NULL},
  {3540994U/*stop*/, eCMD_Stop, NULL},
  {4112307658U/*turn left*/, eCMD_TurnLeft, NULL},
  {3317767U/*left*/, eCMD_TurnLeft, NULL},
  {2933146809U/*turn right*/, eCMD_TurnRight, NULL},
  {108511772U/*right*/, eCMD_TurnRight, NULL},
  {3329415266U/*turn on*/, eCMD_LightOn, NULL},
  {3717175913U/*turn on the light*/, eCMD_LightOn, NULL},
  {132657996U/*turn off*/, eCMD_LightOff, NULL},
  {279435475U/*turn off the light*/, eCMD_LightOff, NULL},
  {2900728977U/*bluetooth on*/, eCMD_BTOn, NULL},
  {1251886721U/*turn on the bluetooth*/, eCMD_BTOn, NULL},
  {4023252221U/*bluetooth off*/, eCMD_BTOff, NULL},
  {2705145835U/*turn off the bluetooth*/, eCMD_BTOff, NULL},
  {92952432U/*ap on*/, eCMD_APOn, NULL},
  {2684280764U/*turn on the ap*/, eCMD_APOn, NULL},
  {2881525246U/*ap off*/, eCMD_APOff, NULL},
  {3733146002U/*turn off the ap*/, eCMD_APOff, NULL},
  {116807225U/*play music*/, eCMD_PlayMusic, NULL},
  {3148902U/*fool*/, eCMD_Fool, NULL},
  {3403077335U/*stupid*/, eCMD_Fool, NULL},
  {3148632036U/*look at me*/, eCMD_LookAtMe, NULL},
  {3199109U/*hey!*/, eCMD_LookAtMe, NULL},
  {3941015838U/*attention*/, eCMD_Attention, NULL},
  {3198785U/*help*/, eCMD_Help, NULL},
  {805757655U/*help me*/, eCMD_Help, NULL},
};

#endif
