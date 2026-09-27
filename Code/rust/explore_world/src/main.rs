#[allow(unused)]

use std::env;
use std::str::FromStr;

fn gcd(a:u64, b:u64) -> (bool, u64) {
  let mut g: u64;
  let mut q: u64;
  let mut r: u64;
  let mut t: u64;
  let mut x: u64;
  let mut y: u64;

  if b > a {
    x= b;
    y= a;
  } else {
    x= b;
    y= a;
  }

  while true {
  
    q = x / y;
    r = x - q * y;

    if r == 0 {
      return (true, y);
    }

  x = y;
  y = r;

  }

  return (false, 0); 
}


fn main() {
  println!("Explore world!");

  // Argument test
  println!("\nargument test!");

  let mut n = Vec::new();

  let mut i: usize = 0;
  for ar in env::args().skip(1) {
    print!("  arg {} ", ar);
    n.push(u64::from_str(&ar).expect("parsing error"));
    println!("as integer {:?}", n[i]);
    i += 1;
  }

  // gcd test
  println!("\ngcd test!");
  let mut a: u64 = n[0];
  let mut b: u64 = n[1];
  let mut g: u64 = 0;
  let mut res: bool;
  (res, g) = gcd(a, b);
  if res {
    println!("({:?}, {:?}) = {:?}", a, b, g);
  } else {
    println!("failed");
  }

  return;
}
