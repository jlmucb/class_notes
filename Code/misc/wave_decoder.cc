#include <stdio.h>
#include <math.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef byte
typedef unsigned char byte;
#endif

const int BITSINBYTE = 8;

class riff_header {
public:
  char label_[4];
  int size_;
  char format_[4];
};

class fmt_header {
public:
  char label_[4];
  int size_;
  short int audio_format_;
  short int num_channels_;
  int sample_rate_;
  int byte_rate_;
  short int block_align_;
  short int bits_per_sample_;
};

class data_header {
public:
  char label_[4];
  int size_;
};

void print_label(char* value) {
  for (int i = 0; i < 4; i++)
    printf("%c", value[i]);
}

void print_riff_header(riff_header& rh) {
  printf("riff label: ");
  print_label(rh.label_);
  printf("\n");
  printf("size: %d\n", rh.size_);
  printf("format: ");
  print_label(rh.format_);
  printf("\n");
}

void print_fmt_header(fmt_header& fh) {
  printf("format label: ");
  print_label(fh.label_);
  printf("\n");
  printf("format size: %d\n", fh.size_);
  printf("audio format: %d\n", fh.audio_format_);
  printf("number of channels: %d\n", fh.num_channels_);
  printf("sample rate: %d\n", fh.sample_rate_);
  printf("byte rate: %d\n", fh.byte_rate_);
  printf("block align: %d\n", fh.block_align_);
  printf("bits per sample: %d\n", fh.bits_per_sample_);
}

void print_data_header(data_header& dh) {
  printf("data label: ");
  print_label(dh.label_);
  printf("\n");
  printf("data size: %d\n", dh.size_);
}

double maxf(short int* in, int num_samples) {
  short int max = -10000;

  for (int i = 0; i < num_samples; i++) {
    if (in[i] > max)
      max = in[i];
  }
  return ((double)max);
}

double array_max(double* in, int num_samples) {
  double max = in[0];

  for (int i = 1; i < num_samples; i++) {
    if (in[i] > max)
      max = in[i];
  }
  return max;
}

void print_channel_data(const char* name, short int* in, int num_samples) {
  printf("%s\n", name);
  while (num_samples > 0) {
    int k;
    if (num_samples < 10)
      k = num_samples;
    else
      k = 10;
    for (int i = 0; i < k; i++)
      printf("%6d, ", *(in++));
    num_samples -= k;
    printf("\n");
  }
  printf("\n");
}

int next_sync_up(short int* in, int num_samples) {
  for (int i = 0; i < num_samples; i++)
    if (in[i] > 0)
      return i;
  return num_samples;
}

int next_sync_down(short int* in, int num_samples) {
  for (int i = 0; i < num_samples; i++)
    if (in[i] < 0)
      return i;
  return num_samples;
}

const double pi = 3.1415926535;

double fft_step(short int* in, int k, int num_samples, double delta_t) {
  double x = 0.0;
  double u;

  for (int n = 0; n < num_samples; n++) {
    u = -2.0 * pi * ((double)k) * ((double) n) * delta_t;
    x += ((double)in[n]) * cos(u);
  }
  return x / (((double)num_samples) * maxf(in, num_samples));
}

void frequency_analysis(short int* in, int num_samples, double delta_t) {
  double out[1024];

  for (int i = 0; i < num_samples; i++) {
    out[i] = fft_step(in, i, num_samples, delta_t);
  }
#if 0
  for (int i = 0; i < num_samples; i++) {
    printf("out[%3d]: %8.4lf    ", i, out[i]);
    if ((i%5) == 4)
      printf("\n");
  }
#else
  double max = array_max(out, num_samples);
  for (int i = 0; i < num_samples; i++) {
    if (out[i] == max)
      printf("(%d, %6.4f), ", i, out[i]);
  }
#endif
  printf("\n\n");
}

int main(int an, char**av) {
  if (an < 2 ) {
    printf("Wrong number of arguments\n");
    return 1;
  }

  int fd = open(av[1], O_RDONLY);
  if (fd < 0) {
    printf("Can't open %s\n", av[1]);
    return 1;
  }

  riff_header rh;
  fmt_header fh;
  data_header dh;
  int n;
  n = read(fd, &rh, sizeof(riff_header));
  if (n != sizeof(riff_header)) {
    printf("riff read size mismatch\n");
  }
  n = read(fd, &fh, sizeof(fmt_header));
  if (n != sizeof(fmt_header)) {
    printf("fmt read size mismatch\n");
  }
  n = read(fd, &dh, sizeof(data_header));
  if (n != sizeof(data_header)) {
    printf("data read size mismatch\n");
  } 
  printf("\nfile: %s\n\n", av[1]);
  print_riff_header(rh);
  printf("\n");
  print_fmt_header(fh);
  printf("\n");
  print_data_header(dh);
  printf("\n");

  // one second of data
  byte interleaved[1000000];
  int num_samples = 1 * fh.sample_rate_;
  double delta_t = 1.0 / ((double)fh.sample_rate_);
  int bytes_to_read = num_samples * fh.block_align_;
  n = read(fd, interleaved, bytes_to_read);

  printf("num_samples: %d, delta_t: %10.5f\n", num_samples, delta_t);

  short int left_channel[1280000];
  short int right_channel[1280000];
  for (int i = 0; i < num_samples; i++) {
    short int* p = (short int*)&interleaved[fh.block_align_ * i];
    left_channel[i] = *(p++);
    right_channel[i] = *(p++);
  }
  print_channel_data("left: ", left_channel, num_samples);
  print_channel_data("right: ", right_channel, num_samples);

  printf("\nAnalyzing samples\n\n");
  int num_samples_remaining = num_samples;
  int current_window_size;
  double time_from_inception = 0.0;
  short int* current_sync_window = left_channel;
  short int* current_analysis_window = right_channel;
 
  while (num_samples_remaining > 0) {
    int start = next_sync_up(current_sync_window, num_samples_remaining);
    num_samples_remaining -= start;
    time_from_inception += delta_t * ((double)start);
    current_sync_window += start;
    current_analysis_window += start;
    int end = next_sync_down(current_sync_window, num_samples_remaining);
    current_window_size = end;
    double window_span = ((double)current_window_size)*delta_t;
    double cycle_time = ((double) (start + end)) * delta_t;
    printf("t: %6.4f, current_window_size: %d, cycle time: %6.4f, window span: %6.4f\n", time_from_inception,
            current_window_size, cycle_time, window_span);
    frequency_analysis(current_analysis_window, current_window_size, delta_t);
    current_sync_window += end;
    current_analysis_window += end;
    time_from_inception += delta_t * ((double)end);
    num_samples_remaining -= end;
  }

  close(fd);
  return 0;
}

