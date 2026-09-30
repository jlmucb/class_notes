#[allow(unused)]
#[allow(unused_mut)]

mod bignum {

use std::env;
use std::str::FromStr;

  // These are accessible outside the module
  pub struct Bignum {
    sign_: bool, // false is negative
    val_: Vec<u64>,
  }
  pub fn print() {
  }
  pub fn negate() {
  }
  pub fn add() {
    // let (sum, carry) = 5u32.carrying_add(10u32, true);
  }
  pub fn sub() {
    // let (diff0, borrow1) = a0.borrowing_sub(b0, borrow0);
  }
  pub fn mult() {
    // let (low, high) = a.widening_mul(b);
    // let (low, high) = a.carrying_mul(b, carry);
  }
  pub fn fulldiv() {
  }
  pub fn div() {
    wrapping_div(self, rhs: i64) -> i64
  }
  pub fn mod() {
  }
  pub fn gcd() {
  }
  pub fn byte_size() {
  }
  pub fn bit_size() {
  }
  pub fn u64_size() {
  }

  pub fn ff_normalize() {
  }
  pub fn ff_print() {
  }
  pub fn ff_negate() {
  }
  pub fn ff_add() {
  }
  pub fn ff_sub() {
  }
  pub fn ff_mult() {
  }
  pub fn ff_div() {
  }
}

#[test]
fn bignum_test() {
}
