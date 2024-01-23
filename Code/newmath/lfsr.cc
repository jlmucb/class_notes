# include <stdint.h>
# include <stdio.h>


uint16_t feedback(uint16_t state, uint16_t taps) {
  uint16_t bit = 0;

  for (int i = 0; i < 16; i++) {
    bit ^= (state&taps&1);
    taps >>= 1;
    state >>= 1;
  }
  return bit;
}

int main(int an, char** av) {
  uint16_t state_size = 3;
  uint16_t start_state = 0x7;
  uint16_t taps = 0x3;

  if (an < 2)
    printf("lfsr state-size taps initial-state\n");

  if (an > 1)
    sscanf(av[1], "%hd", &state_size);
  if (an > 2)
    sscanf(av[2], "%hx", &taps);
  if (an > 3)
    sscanf(av[3], "%hx", &start_state);

  printf("size: %d, taps: %04x, initial state: %04x\n", state_size, taps, start_state);
  uint16_t lfsr = start_state;
  uint16_t bit;
  uint16_t out;
  unsigned period = 0;

  do {
    printf("state: %04x,", lfsr);
    out = lfsr&1;
    bit = feedback(lfsr, taps);
    lfsr =  (lfsr >> 1) | (bit << (state_size - 1));
    period++;
    // printf(" out: %01x, feedback: %1x\n", out, bit);
    printf(" out: %01x\n", out);
  } while (lfsr != start_state && period < (1 << state_size));

  if (period < (1 << state_size))
    printf("period: %d\n", period);
  else
    printf("Failure\n");
  return 0;
}
