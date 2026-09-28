#[allow(unused)]
#[allow(unused_mut)]

use std::env;
use std::str::FromStr;

fn gcd(a:u64, b:u64) -> (bool, u64) {
  let mut _g: u64;
  let mut _q: u64;
  let mut _r: u64;
  let mut _t: u64;
  let mut _x: u64;
  let mut _y: u64;

  if b > a {
    _x= b;
    _y= a;
  } else {
    _x= b;
    _y= a;
  }

  loop {
    _q = _x / _y;
    _r = _x - _q * _y;
    if _r == 0 {
      return (true, _y);
    }
  _x = _y;
  _y = _r;
  }
  // return (false, 0); 
}

#[derive(Default, Debug)]
// #[derive(Debug, Clone, Copy)]
struct Plucker {
  a_: i64,
  b_: i64,
  x_: i64,
  y_: i64,
  g_: i64
}

fn generalized_gcd(a:u64, b:u64, o: &mut Plucker) -> bool {
  let mut q: i64;
  let mut r: i64;
  let mut t1: i64;
  let mut t2: i64;
  let mut x1: i64;
  let mut y1: i64;
  let mut x2: i64;
  let mut y2: i64;
  let mut x: i64;
  let mut y: i64;

  if b > a {
    o.a_= b as i64;
    o.b_= a as i64;
  } else {
    o.a_= a as i64;
    o.b_= b as i64;
  }

  t1 = o.a_;
  t2 = o.b_;
  x1 = 1;
  y1 = 0;
  x2 = 0;
  y2 = 1;

  loop {
  
    q = t1 / t2;
    r = t1 - q * t2;
    if r == 0 {
      o.g_ = t2 as i64;
      o.x_ = x2 as i64;
      o.y_ = y2 as i64;
      return true;
    }
  x = x1-q*x2;
  y = y1-q*y2;
  x1 = x2;
  y1 = y2;
  t1 = t2;
  t2 = r;
  x2 = x;
  y2 = y;
  }
  // return false;
}

fn main() {
  println!("Explore world!");

  // Argument test
  println!("\nargument test");
  let mut n:Vec<u64> = Vec::new();
  let mut i: usize = 0;
  for ar in env::args().skip(1) {
    print!("  arg {} ", ar);
    n.push(u64::from_str(&ar).expect("parsing error"));
    println!("as integer {:?}", n[i]);
    i += 1;
  }

  // gcd test 1
  println!("\ngcd test 1");
  let a: u64 = n[0];
  let b: u64 = n[1];
  let g: u64;
  let  res: bool;
  (res, g) = gcd(a, b);
  if res {
    println!("  ({:?}, {:?}) = {:?}, a/g: {:}, b/g: {:}", a, b, g, a/g, b/g);
  } else {
    println!("failed");
  }

  // gcd test 2
  println!("\ngcd test 2");
  let mut p:Plucker = Plucker::default();
  if generalized_gcd(a, b, &mut p) {
     println!("  ({:?}) * {:?} + ({:?}) * {:?} = {:?}", p.x_, p.a_, p.y_, p.b_, p.g_);
  } else {
    println!("failed");
  }

  return;
}

#[test]
fn pluck() {
  let a: u64 = 7;
  let b: u64 = 11;
  let mut p:Plucker = Plucker::default();
  let res: bool;

  res = generalized_gcd(a, b, &mut p);
  assert!(res);
  assert!( p.x_ == 2 && p.y_ == -3 && p.g_ == 1);
}
