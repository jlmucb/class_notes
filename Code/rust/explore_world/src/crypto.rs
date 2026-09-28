#[allow(unused)]
#[allow(unused_mut)]

mod crypto {

use std::env;
use std::str::FromStr;
use Time;

  // These are accessible outside the module
  pub struct RsaKey {
  }
  pub struct SymKey {
  }
  pub struct EccKey {
  }
  pub struct LweKey {
  }
  pub struct Key {
    type: string,
    not_before_: Time,
    not_after_: Time,
    sym_key_: SymKey,
    rsa_key_: RsaKey,
    ecc_key_: EccKey,
    lwe_key_: LweKey,
  }
  pub fn sha1() {
  }
  pub fn sha256() {
  }
  pub fn sha512() {
  }
  pub fn sha3() {
  }
  pub fn print_key() {
  }
  // aes, rsa, ecc
  pub fn encrypt() {
  }
  pub fn decrypt() {
  }
  pub fn sign() {
  }
  pub fn wrap() {
  }
  pub fn cbc() {
  }
  pub fn gcm() {
  }
}

#[test]
fn crypto_test() {
}
