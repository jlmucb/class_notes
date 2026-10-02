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

  pub fn long_div(a: u64, b: u64, c: u64, q: u64, r: u64) -> (u64, u64) {
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

  pub fn zero(num: &mut Bignum) {
    for i in 0..num.val_.len() {
      num.val_[i] = 0;
    }
    num.sign_ = false;
  }

  pub fn is_zero(num: &mut Bignum) -> bool {
    for i in 0..num.val_.len() {
      if num.val_[i] != 0 {
        return false;
      }
    }
    return true;
  }

  //returns 1 if left>right, 0 if left == right, -1 if left<right
  pub fn compare(left: &mut Bignum, right: &mut Bignum) -> i32 {
    if left.sign_ && !right.sign_ {
      return 1;
    }
    if !left.sign_ && right.sign_ {
      return -1;
    }
    let left_size: usize = left.val_.len();
    let right_size: usize= right.val_.len();
    if left_size > right_size {
      for i in (left_size-1..right_size).rev() {
        if left.val_[i] > 0 {
          if left.sign_ {
            return 1;
          } else {
            return -1;
          }
        }
      }
      for i in (right_size-1 ..=0).rev() {
        if left.val_[i] > right.val_[i] {
          if !left.sign_ {
            return 1;
          } else {
            return -1;
          }
        }
      }
      return 0;
    } else  if right_size < left_size {
      for i in (right_size-1..left_size).rev() {
        if left.val_[i] > 0 {
          if left.sign_ {
            return 1;
          } else {
            return -1;
          }
        }
      }
      for i in (left_size-1 ..=0).rev() {
        if left.val_[i] > right.val_[i] {
          if !left.sign_ {
            return 1;
          } else {
            return -1;
          }
        }
      }
      return 0;
    } else {
      for i in (left_size-1..=0).rev() {
        if left.val_[i] > right.val_[i] {
          if !left.sign_ {
            return 1;
          } else {
            return -1;
          }
        }
        if right.val_[i] > left.val_[i] {
          if left.sign_ {
            return 1;
          } else {
            return -1;
          }
        }
      }
      return 0;
    }
  }

  pub fn negate(num: &mut Bignum) {
    if is_zero(num) {
      num.sign_ = true;
      return;
    }
    num.sign_ = !num.sign_;
  }

  pub fn add(a: &mut Bignum, b: &mut Bignum, s: &mut Bignum) -> bool {
    // let (sum, carry) = 5u32.carrying_add(10u32, true);
    return false;
  }

  pub fn sub(a: &mut Bignum, b: &mut Bignum, d: &mut Bignum) -> bool {
    // let (diff0, borrow1) = a0.borrowing_sub(b0, borrow0);
    return false;
  }

  pub fn mult(a: &mut Bignum, b: &mut Bignum, p: &mut Bignum) -> bool {
    // let (low, high) = a.widening_mul(b);
    // let (low, high) = a.carrying_mul(b, carry);
    return false;
  }

  pub fn euclid_div(a: &mut Bignum, b: &mut Bignum,  q: &mut Bignum, r: &mut Bignum) -> bool {
    return false;
  }

  pub fn div(a: &mut Bignum, b: &mut Bignum, q: &mut Bignum) -> bool {
    return false;
  }

  pub fn modulo(a: &mut Bignum, b: &mut Bignum,  r: &mut Bignum) -> bool {
    return false;
  }

  // ax + by = g
  pub fn gcd(a: &mut Bignum, b: &mut Bignum, x: &mut Bignum, y: &mut Bignum, g: &mut Bignum) -> bool {
    return false;
  }

  pub fn byte_size(num: &mut Bignum) -> i32{
    for i in (num.val_.len()-1 ..=0).rev() {
      if num.val_[i] > 0 {
        return (i + 1) as i32;
      }
    }
    return 0;
  }

  pub fn bit_size(num: &mut Bignum) -> i32 {
    for i in (num.val_.len()-1 ..=0).rev() {
      if num.val_[i] > 0 {
        return (i + 1) as i32;
      }
    }
    return 0;
  }

  pub fn u64_size(num: &mut Bignum) -> i32 {
    for i in (num.val_.len()-1 ..=0).rev() {
      if num.val_[i] > 0 {
        return (i + 1) as i32;
      }
    }
    return 0;
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

  // long_sub
  // long_mult
  // long_div
  // byte_size
  // bit_size
  // u64_size
  // normalize
  // compare
  // zero
  // negate
  // add
  // sub
  // mult
  // fulldiv
  // div
  // modulo
  // gcd
  // is_zero
  // ff_negate
  // ff_add
  // ff_sub
  // ff_mult
  // ff_div
}

