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

#define BUFSIZE 2048

//  Todo:
//    if exec can't figure out the executable type, it will try to start a shell
//    with the file as input so we have to figure out whether files are shell
//    scripts.
//    while we identify archives, we haven't checked to see the components are
//    object modules which would make it a static library.

//---------------------------------------------------------------------------------------


void printBytes(string& s) {
  byte* p = (byte*)s.data();

  for (int i = 0; i < (int)s.size(); i++)
    printf("%02x", *(p++));
}

void printBytes(int n, byte* in) {
  for (int i = 0; i < n; i++)
    printf("%02x", in[i]);
}

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

string* timeString(struct tm* t) {
  char buffer[256];
  strftime(buffer, 256 ,"%Y:%m:%d:%H:%M.%SZ", t);
  string* s = new string(buffer);
  return s;
}

void hashFile(string& fullName, string* hash) {
  int fd = open(fullName.c_str(), O_RDONLY);
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


//---------------------------------------------------------------------------------------

const byte UNKNOWN = 0;

// File types
const byte REGULARFILE = 0x01;
const byte LINK = 0x02;
const byte CHANNEL = 0x03;
const byte DEVICE = 0x04;
const byte DIRECTORY = 0x80;

// File Format types
const byte EXECUTABLE = 1;
const byte OBJECT = 2;
const byte LIBRARY = 3;
const byte SHAREDLIBRARY = 4;
const byte SCRIPT = 5;
const byte ARCHIVE = 6;

const byte READ = 0x4;
const byte WRITE = 0x2;
const byte EXEC = 0x1;

class FileData {
public:
  string file_name_;
  byte file_type_;
  byte file_format_type_;
  byte owner_rwe_;
  byte group_rwe_;
  byte world_rwe_;
  uid_t uid_;
  gid_t gid_;
  string sha1_hash_;
  int file_size_;
  string modified_time_;

  FileData() { file_type_= UNKNOWN; file_format_type_ = 0; file_size_= 0;
               owner_rwe_ = 0; group_rwe_ =0; world_rwe_ =0;};
  void Init() { file_type_= UNKNOWN; file_format_type_ = 0; file_size_= 0;
               owner_rwe_ = 0; group_rwe_ =0; world_rwe_ =0;};
  void printFileInfo();
};

void printFileType(byte type) {
  switch (type) {
    default:
      printf("unknown");
      break;
    case REGULARFILE:
      printf("file");
      break;
    case LINK:
      printf("link");
      break;
    case CHANNEL:
      printf("channel");
      break;
    case DEVICE:
      printf("device");
      break;
    case DIRECTORY:
      printf("directory");
      break;
  }
}

void printFileFormatType(byte type) {
  switch (type) {
    default:
      printf("unknown");
      break;
    case EXECUTABLE:
      printf("executable");
      break;
    case OBJECT:
      printf("object");
      break;
    case LIBRARY:
      printf("library");
      break;
    case SHAREDLIBRARY:
      printf("shared library");
      break;
    case SCRIPT:
      printf("script");
      break;
    case ARCHIVE:
      printf("archive");
      break;
  }
}

void printRWE(byte rwe) {
  if (rwe & READ)
    printf("R");
  else
    printf("-");
  if (rwe & WRITE)
    printf("W");
  else
    printf("-");
  if (rwe & EXEC)
    printf("X");
  else
    printf("-");
}

byte get_owner_rwe(struct stat& s) {
  byte x = 0;

  if (s.st_mode & S_IRUSR)
    x |= READ;
  if (s.st_mode & S_IWUSR)
    x |= WRITE;
  if (s.st_mode & S_IXUSR)
    x |= EXEC;
  return x;
}

byte get_group_rwe(struct stat& s) {
  byte x = 0;

  if (s.st_mode & S_IRGRP)
    x |= READ;
  if (s.st_mode & S_IWGRP)
    x |= WRITE;
  if (s.st_mode & S_IXGRP)
    x |= EXEC;
  return x;
}

byte get_world_rwe(struct stat& s) {
  byte x = 0;

  if (s.st_mode & S_IROTH)
    x |= READ;
  if (s.st_mode & S_IWOTH)
    x |= WRITE;
  if (s.st_mode & S_IXOTH)
    x |= EXEC;
  return x;
}

bool isDirectory(struct stat& s) {
  if ((s.st_mode & S_IFMT) == S_IFDIR)
    return true;
  return false;
}

bool isFile(struct stat& s) {
  if ((s.st_mode & S_IFMT) == S_IFREG)
    return true;
  return false;
}

bool isDevice(struct stat& s) {
  if (((s.st_mode & S_IFMT) == S_IFBLK) ||
      ((s.st_mode & S_IFMT) == S_IFCHR))
    return true;
  return false;
}

bool isChannel(struct stat& s) {
  if (((s.st_mode & S_IFMT) == S_IFSOCK) ||
      ((s.st_mode & S_IFMT) == S_IFIFO))
    return true;
  return false;
}

bool isLINK(struct stat& s) {
  if ((s.st_mode & S_IFMT) == S_IFLNK)
    return true;
  return false;
}

byte discoverFileType(struct stat& s) {
  if (isDirectory(s)) {
    return DIRECTORY;
  } else if (isFile(s)) {
    return REGULARFILE;
  } 
  return UNKNOWN;
}

#define ET_NONE  0x00
#define ET_REL   0x01
#define ET_EXEC  0x02
#define ET_DYN   0x03

byte ar_header[8] = {
  0x21, 0x3c, 0x61, 0x72, 0x63, 0x68, 0x3e, 0x0a
};

// readelf -h libtest.so 
// ELF Header:
//   Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 

byte discoverFileFormatType(string fullName) {
  int fd = open(fullName.c_str(), O_RDONLY);
  if (fd < 0) {
    return UNKNOWN;
  }
  byte in[64];
  int n = read(fd, in, 64);
  close(fd);
  if (n <= 0)
    return UNKNOWN;

  // ELF format
  //  First four bytes are header: 0x7f 0x45 0x4c 0x46
  //    Offset 4 bytes: 1 (32 bits), 2 (64 bits)
  //    Offset 7: Linux (0x03)
  //    Offset 0x10 (2 bytes) - type
  if (in[0] == 0x7f && in[1] == 0x45 && in[2] == 0x4c && in[3] == 0x46) {
    short unsigned int* p = (short unsigned int*) (in + 0x10);
    switch(*p) {
      default:
        break;
      case ET_REL:
        return OBJECT;
      case ET_EXEC:
        return EXECUTABLE;
      case ET_DYN:
        return SHAREDLIBRARY;
    }
    return UNKNOWN;
  }
  if (memcmp(in, ar_header, 8) == 0) {
    // need to check elements are of right type
    return ARCHIVE;
  }
  return UNKNOWN;
}

// file_name is full path (e.g. /usr/jlm/src/file)
void FileData::printFileInfo() {
  if (file_name_.size() == 0)
    return;
  printf("%s, ", file_name_.c_str());
  printFileType(file_type_);
  if (file_format_type_ != UNKNOWN) {
    printf(", ");
    printFileFormatType(file_format_type_);
  }
  printf(", owner: %d, group: %d", uid_, gid_);
  printf("\n  ");
  printRWE(owner_rwe_);
  printf(", ");
  printRWE(group_rwe_);
  printf(", ");
  printRWE(world_rwe_);
  if (modified_time_.size() > 0) {
  printf(", %s", modified_time_.c_str());
  }

  if (file_type_ != REGULARFILE) {
    printf("\n");
    return;
  }

  printf(", %d bytes", file_size_);
  if (sha1_hash_.size() > 0) {
    printf(", sha1: ");
    printBytes(sha1_hash_);
  }
  printf("\n");
}


// -----------------------------------------------------------------------------------------------------------


// Flags
const byte ALLFILES = 1;
const byte EXECFILEINVENTORY = 2;

void collectFiles(string& name, struct stat& s, byte flag) {
  FileData file_data;

  file_data.file_name_ = name;
  file_data.file_type_ = discoverFileType(s);
  file_data.file_format_type_ = discoverFileFormatType(name);
  file_data.owner_rwe_ = get_owner_rwe(s);
  file_data.group_rwe_ = get_group_rwe(s);
  file_data.world_rwe_ = get_world_rwe(s);
  file_data.uid_ = s.st_uid;
  file_data.gid_ = s.st_gid;
  file_data.file_size_ = s.st_size;
  hashFile(name, &file_data.sha1_hash_);
  time_t t = s.st_mtim.tv_sec;
  struct tm* mTime = gmtime(&t);
  if (mTime != nullptr) {
    string* str = timeString(mTime);
    if (str != nullptr)
      file_data.modified_time_.assign(str->c_str());
    // free(mTime);
    mTime = nullptr;
  }

  file_data.printFileInfo();
  printf("\n");
}

// recurse through directory and apply action to non-directory objects
// dirName is full path: e.g.: /usr/jlm/src
int enumerate(string& dirName, void (*action)(string&, struct stat&, byte), byte flag) {
  int numFiles= 0;
  DIR* dirp;
  struct dirent* direntp;
  struct stat stat_buf;

  if ((dirp = opendir(dirName.c_str())) == nullptr)
    return 0;

  for(;;) {
    direntp = readdir(dirp);
    if (direntp == nullptr)
      break;
    if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
      continue;

    string fullName(dirName);
    fullName += "/";
    fullName += direntp->d_name;

    if (stat(fullName.c_str(), &stat_buf) < 0) {
      printf("stat error\n");
      continue;
    }

    byte file_type = discoverFileType(stat_buf);
    switch(file_type) {
      default:
        // don't care about other types?
        break;
      case REGULARFILE:
        (*action)(fullName, stat_buf, flag);
        numFiles++;
      case DIRECTORY:
        numFiles += enumerate(fullName, action, flag);
        break;
    }
  }

  return numFiles;
}


// -----------------------------------------------------------------------------------------------------------


int main(int an, char* av[]) {
  string baseDir;
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
  struct tm* currentTime = gmtime(&now);
  
  string* currentTimeString = timeString(currentTime);
  printf("Current time: %s\n\n", currentTimeString->c_str());

  // Recursively inventory directory
  numFiles= enumerate(baseDir, collectFiles, ALLFILES);
  printf("\nDirectory %s contains %d files\n", baseDir.c_str(), numFiles);

  return 0;
}

