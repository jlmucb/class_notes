#[allow(unused)]
#[allow(unused_mut)]

pub mod bignum {

  // helpers

  // return is (high, low)
  pub fn long_add(a: u64, b: u64, c: u64) -> (u64, u64) {
    let mut sum:u128 = 0;
    sum += a as u128;
    sum += b as u128;
    sum += c as u128;
    return (((sum >> 64) & 0xffffffffffffffff) as u64, (sum & 0xffffffffffffffff) as u64);
  }

  pub fn long_sub(a: u64, b: u64, borrow: bool) -> (u64, u64) {
    let mut op1: u128 = a as u128;
    let mut op2: u128 = b as u128;
    let mut br: u64 = 0;

    if borrow {
      op2 += 1;
    }
    if op1 < op2 {
      br += 1;
      op1 += 0x10000000000000000;
    }
    let diff: u128 = op1 - op2;
    return (((diff >> 64) & 0xffffffffffffffff) as u64, (diff & 0xffffffffffffffff) as u64);
  }

  pub fn long_mult(a: u64, b: u64) -> (u64, u64) {
    let prod: u128 = (a as u128) * (b as u128);
    return (((prod >> 64) & 0xffffffffffffffff) as u64, (prod & 0xffffffffffffffff) as u64);
  }

  pub fn long_div(a: u64, b: u64, c: u64) -> (u64, u64) {
    return (0, 0);
  }

  pub struct Bignum {
    sign_: bool,    // true is negative
    val_: Vec<u64>,
  }
  impl Default for Bignum {
    fn default() -> Self {Bignum {sign_: false, val_: Vec::<u64>::from([0, 0, 0, 0])} }
  }

  pub fn set_val(s: bool, v: Vec<u64>, num: &mut Bignum) -> bool {
    num.sign_ = s;
    let l = v.len();
    if l > num.val_.len() {
      num.val_.resize(l + 1, 0);
    }
    for i in 0..l {
      num.val_[i] = v[i];
    }
    for i in l..num.val_.len() {
      num.val_[i] = 0;
    }
    return true;
  }

  pub fn print_val(num: &mut Bignum) {

    if !num.sign_ {
      print!("- ");
    } else {
      print!("+ ");
    }
    let l = num.val_.len()-1;
    for i in 0..=l {
      print!("{:016x} ", num.val_[l - i]);
    }
    println!("");
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
  }

  pub fn modulo() {
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

#[allow(unused_imports)]
use crate::bignum::bignum::*;
#[test]
fn big_num_test() {
  let op1: u64= 0xffffffffffffffff;
  let op2: u64= 0xffffffffffffffff;
  let c: u64= 0;
  let mut high:u64 = 0;
  let mut low:u64 = 0;

  let (high, low) = long_add(op1, op2, c);
  
  assert!(high == 1 && low == 0xfffffffffffffffe, "{:016x} + {:016x} + {:016x}, high: {:016x}, low:{:016x}", op1, op2, c, high, low);
  
  let mut bn: Bignum = Bignum::default();
  let mut v: Vec<u64> = Vec::<u64>::new();
  v.resize(5,0);
  v[0] = 0;
  v[1] = 0;
  v[2] = 0;
  v[3] = 0;
  
  if !set_val(true, v, &mut bn) {
    assert!(1 == 0, "set_val failed");
  }
  print_val(&mut bn);
}

