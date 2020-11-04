
// filecrawl.cc
//  Search tree structure
//  John Manferdelli, 2003

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>

#include <string>

using std::string;
#ifndef byte
typedef unsigned char byte;
#endif

#include "sha1.h"

//---------------------------------------------------------------------------------------


class MergeData {
public:
  char* fileName_;
  uint32_t fileType_;
  byte sha1Hash_[20];
  int fileSize_;
  tm modifiedTime_;

  MergeData() { fileName_= NULL; fileType_= 0; fileSize_= 0; };
  void Init() { fileName_= NULL; fileType_= 0; fileSize_= 0; };
};

#define BUFSIZE 2048

//---------------------------------------------------------------------------------------

inline char convertHex(byte in) {
  if ((in >= 0) && (in <= 9))
    return in + '0';
  if ((in >= 10) && (in < 16))
    return in - 10 + 'a';
  return '*';
}

inline void tohex(byte in, string* out) {
  out->clear();
  byte b1, b2;

  b1= (in>>4) & 0xf;
  b2= in & 0xf;
  *out += convertHex(b1);
  *out += convertHex(b2);
}

void hashFile(const char* fullName, string* hash) {
  int fd = open(fullName, O_RDONLY);
  Sha1 hash_obj;
  byte in[BUFSIZE];
  byte hash_out[20];
  int n = 0;

  hash_obj.Init();
 
  for(;;) {
    n = read(fd, in, BUFSIZE);
    if (n <= 0)
      break;
    hash_obj.AddToHash(n, (const byte*)in);
  } 
  close(fd);
  hash_obj.Final();
  hash_obj.GetDigest(20, hash_out);
  hash->assign((const char*)hash_out, 20);
}


// fileName is full path (e.g. /usr/jlm/src/file)
void printFileInfo(const char* fullName, struct stat& s, bool execOnly) {
  if ((s.st_mode & S_IFMT) != S_IFREG)
    return;
  if (execOnly && ((s.st_mode & 0111) == 0) )
    return;
  printf("%s: %d b, (%03o), ", fullName,
            (int)s.st_size, s.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));

  string binary_hash;
  string ascii_hash;
  hashFile(fullName, &binary_hash);
  string temp_ascii;
  byte* hash_bytes= (byte*) binary_hash.data();

  for (int i = 0; i < 20; i++) {
    tohex(hash_bytes[i], &temp_ascii);
    ascii_hash += temp_ascii;
  }
  printf("hash: %s\n", ascii_hash.c_str());
}

// recurse through directory and apply action to non-directory objects
// dirName is full path: e.g.: /usr/jlm/src
int enumerate(const char* dirName, void (*action)(const char* fullName, struct stat& s, bool), bool execOnly) {
  int numFiles= 0;
  DIR* dirp;
  struct dirent* direntp;
  struct stat stat_buf;

  if ((dirp = opendir(dirName)) == nullptr)
    return 0;

  for(;;) {
    direntp = readdir(dirp);
    if (direntp == nullptr)
      break;
    if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
      continue;
    string fullName(dirName);
    fullName+= "/";
    fullName+= direntp->d_name;
    if (stat(fullName.c_str(), &stat_buf) < 0) {
      printf("stat error\n");
      continue;
    }
    // st_uid, st_gid
    switch(stat_buf.st_mode & S_IFMT) {
      case S_IFDIR:
        numFiles += enumerate(fullName.c_str(), action, execOnly);
        break;
      case S_IFREG:
        (*action)(fullName.c_str(), stat_buf, execOnly);
        numFiles++;
        break;
      default:
        break;
    }
  }

  return numFiles;
}

int main(int an, char* av[]) {
  string baseDir;
  char buffer[256];
  int numFiles = 0;


  // Command Line Parameters
  if (an > 1 && strcmp(av[1],"-help") == 0) {
    printf("filecrawl Directory-to-audit\n");
    return 0;
  }

  if (an > 1) {
    baseDir = av[1];
  } else {
    baseDir = "/";
  }
  printf("Searching directory: %s\n", baseDir.c_str());

  // Get Current Time
  time_t now;
  time(&now);
  struct tm*  currentTime = gmtime(&now);
  strftime(buffer, 256 ,"%Y:%m:%d:%H:%M.%SZ", currentTime);
  
  string currentTimeString(buffer);
  printf("Current time: %s\n", currentTimeString.c_str());

  // Recursively inventory directory
  numFiles= enumerate(baseDir.c_str(), printFileInfo, true);
  printf("\nDirectory %s contains %d files\n", baseDir.c_str(), numFiles);

  return 0;
}

