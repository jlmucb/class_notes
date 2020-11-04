//  Manferdelli
//    Raspberry Pi, infrared

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <lirc/lirc_client.h>
#include <time.h>

#ifndef byte
typedef unsigned char byte;
#endif

//  Connection scheme
//    Same as arduino
//    sig goes to gpio 4 (pin 7)

const int sig_pin = 7;
const char* key_map[21] = {
  " KEY_CHANNELDOWN ",
  " KEY_CHANNEL ",
  " KEY_CHANNELUP ",
  " KEY_PREVIOUS ",
  " KEY_NEXT ",
  " KEY_PLAYPAUSE ",
  " KEY_VOLUMEDOWN ",
  " KEY_VOLUMEUP ",
  " KEY_EQUAL ",
  " KEY_NUMERIC_0 ",
  " BTN_0 ",
  " BTN_1 ",
  " KEY_NUMERIC_1 ",
  " KEY_NUMERIC_2 ",
  " KEY_NUMERIC_3 ",
  " KEY_NUMERIC_4 ",
  " KEY_NUMERIC_5 ",
  " KEY_NUMERIC_6 ",
  " KEY_NUMERIC_7 ",
  " KEY_NUMERIC_8 ",
  " KEY_NUMERIC_9 "
};

int main(int an, char** av) {

  if (wiringPiSetup() < 0) {
    printf("Can't initialize Wiring Pi\n");
    return 1;
  }
  linc_config *config;
  char* code;
  int button_timer = millis();

  if (lirc_init("lirc", 1) < 0) {
    printf("Can't initialize ir library\n");
    return 1;
  }
  if (lirc_readconfig(NULL, &config, NULL) == 0) {
    while (lirc_nextcode(&code) == 0) {
      if (code == NULL)
        continue;
      if ((millis() - button_timer) > 400) {
      }
      printf("Code: %s\n", code);
      free(code);
    }
    lirc_freeconfig(config);
  }
  lirc_deinit();
  return 0;
}
