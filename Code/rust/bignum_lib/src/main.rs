#[allow(unused)]
#[allow(unused_mut)]
#[allow(unused_imports)]

mod bignum;
use crate::bignum::bignum::*;

fn main() {
    println!("Large arith");

    let op1: u128 = 0xffffffffffffffff;
    let mut op2: u128 = 0x03;
    let mut r: u128;

    r = op1 * op2;
    println!("{:032x} + {:032x} = {:032x}", op1, op2, r);
    r = op1 * op1;
    println!("{:032x} * {:032x} = {:032x}", op1, op1, r);
    let low: u64 = (r & 0xffffffffffffffff) as u64;
    let high: u64 = ((r >>64) & 0xffffffffffffffff) as u64;
    println!("high: {:016x}, low: {:016x}", high, low);
}

   
#[allow(unused_imports)]
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
  return;

  let mut v: Vec<u64> = Vec::<u64>::with_capacity(4);
  v[0] = 0;
  v[1] = 0;
  v[2] = 0;
  v[3] = 0;

  if !set_val(true, v, &mut bn) {
    assert!(1 == 0, "set_val failed");
  }
  print_val(&mut bn);
  assert!(false, "Can't print val");
}       

