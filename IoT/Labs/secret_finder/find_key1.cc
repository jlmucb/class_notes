typedef unsigned char byte;
void ebc_encrypt(byte* key, byte* in, byte* out);

byte my_key[] = {
  0xdb, 0xcc, 0x05, 0x78, 0xe4, 0xfb, 0x50, 0x2b, 0x79, 0xec, 0x0f, 0x67, 0xa9, 0x4e, 0x31, 0x83
};

const int BLOCKSIZE = 16;

void program(int size, byte* plain_text, byte* cipher_text) {
  // Assumes input is BLOCKSIZE aligned and an integral multiple of BLOCKSIZE chunks
  while (size > 0) {
    ebc_encrypt(my_key, plain_text, cipher_text);
    plain_text += BLOCKSIZE;
    cipher_text += BLOCKSIZE;
    size -= BLOCKSIZE;
  }
}

