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
  eCMD_Where,
  eCMD_Bye,
};

const char* const g_nlu_content_str[] = {
[eCMD_wakeup_uni] = "{\"asr\":\"hello\",\"cmd\":\"wakeup_uni\",\"pcm\":\"[103, 104]\"}",
[eCMD_exitUni] = "{\"asr\":\"Go to sleep\",\"cmd\":\"exitUni\",\"pcm\":\"[106]\"}",
[eCMD_TurnLeft] = "{\"asr\":\"turn left\",\"cmd\":\"TurnLeft\",\"pcm\":\"[108]\"}",
[eCMD_TurnRight] = "{\"asr\":\"turn right\",\"cmd\":\"TurnRight\",\"pcm\":\"[109]\"}",
[eCMD_PlayMusic] = "{\"asr\":\"play music\",\"cmd\":\"PlayMusic\",\"pcm\":\"[110]\"}",
[eCMD_Fool] = "{\"asr\":\"fool\",\"cmd\":\"Fool\",\"pcm\":\"[111]\"}",
[eCMD_Stop] = "{\"asr\":\"stop\",\"cmd\":\"Stop\",\"pcm\":\"[112]\"}",
[eCMD_TurnOn] = "{\"asr\":\"turn on the light\",\"cmd\":\"TurnOn\",\"pcm\":\"[113]\"}",
[eCMD_TurnOff] = "{\"asr\":\"turn off the light\",\"cmd\":\"TurnOff\",\"pcm\":\"[114]\"}",
[eCMD_Where] = "{\"asr\":\"where are you\",\"cmd\":\"Where\",\"pcm\":\"[115]\"}",
[eCMD_Bye] = "{\"asr\":\"good bye\",\"cmd\":\"Bye\",\"pcm\":\"[116]\"}",
};

/*TODO perf sort by hashcode O(logN), now version O(N)*/
const uni_nlu_content_mapping_t g_nlu_content_mapping[] = {
  {99162322U/*hello*/, eCMD_wakeup_uni, NULL},
  {95268U/*b b*/, eCMD_wakeup_uni, NULL},
  {3136U/*bb*/, eCMD_wakeup_uni, NULL},
  {3502236238U/*hello bb*/, eCMD_wakeup_uni, NULL},
  {4275161354U/*Go to sleep*/, eCMD_exitUni, NULL},
  {4112307658U/*turn left*/, eCMD_TurnLeft, NULL},
  {3317767U/*left*/, eCMD_TurnLeft, NULL},
  {2933146809U/*turn right*/, eCMD_TurnRight, NULL},
  {108511772U/*right*/, eCMD_TurnRight, NULL},
  {116807225U/*play music*/, eCMD_PlayMusic, NULL},
  {3148902U/*fool*/, eCMD_Fool, NULL},
  {3540994U/*stop*/, eCMD_Stop, NULL},
  {3717175913U/*turn on the light*/, eCMD_TurnOn, NULL},
  {3329415266U/*turn on*/, eCMD_TurnOn, NULL},
  {279435475U/*turn off the light*/, eCMD_TurnOff, NULL},
  {132657996U/*turn off*/, eCMD_TurnOff, NULL},
  {3565949466U/*where are you*/, eCMD_Where, NULL},
  {2120738059U/*good bye*/, eCMD_Bye, NULL},
  {339501756U/*bye bye*/, eCMD_Bye, NULL},
  {1969601234U/*see you*/, eCMD_Bye, NULL},
  {2225387253U/*good night*/, eCMD_Bye, NULL},
};

#endif
