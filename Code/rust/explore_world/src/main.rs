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

struct Sheep { naked: bool, name: &'static str }

trait Animal {
  // Associated function signature; `Self` refers to the implementor type.
  fn new(name: &'static str) -> Self;

  // Method signatures; these will return a string.
  fn name(&self) -> &'static str;
  fn noise(&self) -> &'static str;

  // Traits can provide default method definitions.
  fn talk(&self) {
    println!("{} says {}", self.name(), self.noise());
  }
}

impl Sheep {
  fn is_naked(&self) -> bool {
    self.naked
  }

  fn shear(&mut self) {
    if self.is_naked() {
      // Implementor methods can use the implementor's trait methods.
      println!("{} is already naked...", self.name());
    } else {
      println!("{} gets a haircut!", self.name);

      self.naked = true;
    }
  }
}

// Implement the `Animal` trait for `Sheep`.
impl Animal for Sheep {
  // `Self` is the implementor type: `Sheep`.
  fn new(name: &'static str) -> Sheep {
    Sheep { name: name, naked: false }
  }

  fn name(&self) -> &'static str {
    self.name
  }

  fn noise(&self) -> &'static str {
    if self.is_naked() {
      "baaaaah?"
    } else {
      "baaaaah!"
    }
  }

  // Default trait methods can be overridden.
  fn talk(&self) {
    // For example, we can add some quiet contemplation.
    println!("{} pauses briefly... {}", self.name, self.noise());
  }
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

  // test arrays
  let mut _array1: [i32; 5] = [1, 2, 3, 4, 5];
  println!("array length = {:}", _array1.len());
  let _n: usize = _array1.len();
  for i in 0.._n {
    print!("({:?}, {:}) ", i, _array1[i]);
  }
  print!("\n");

  // Type annotation
  println!("\ntypes");
  let mut dolly: Sheep = Animal::new("Dolly");
  dolly.talk();
  dolly.shear();
  dolly.talk();

  return;
}

// Comparison traits: Eq, PartialEq, Ord, PartialOrd.
// Clone, to create T from &T via a copy.
// Copy, to give a type ‘copy semantics’ instead of ‘move semantics’.
// Hash, to compute a hash from &T.
// Default, to create an empty instance of a data type.
// Debug, to format a value using the {:?} formatter.
// #[derive(PartialEq, PartialOrd)]

// fn main() {
//   let raw_p: *const u32 = &10;
//   unsafe {
//     assert!(*raw_p == 10);
//   }
// }
//
// use std::slice;
// fn main() {
//   let some_vector = vec![1, 2, 3, 4];

//   let pointer = some_vector.as_ptr();
//   let length = some_vector.len();
// 
//   unsafe {
//     let my_slice: &[u32] = slice::from_raw_parts(pointer, length);
// 
//     assert_eq!(some_vector.as_slice(), my_slice);
//   }
// }
//
// static S4: u8 = 0;
// #[unsafe(no_mangle)] pub fn f4() -> &'static u8 { &S4 }
// #[unsafe(no_mangle)]
// extern "C" fn foo() {}
// use std::arch::asm;
// unsafe {
//   asm!("nop");
// }
//
// Closure
// fn main() {
//   let outer_var = 42;
// A regular function can't refer to variables in the enclosing environment
//fn function(i: i32) -> i32 { i + outer_var }
// Closures are anonymous, here we are binding them to references.
// Annotation is identical to function annotation but is optional
// as are the `{}` wrapping the body. These nameless functions
// are assigned to appropriately named variables.
//   let closure_annotated = |i: i32| -> i32 { i + outer_var };
//   let closure_inferred  = |i   |      i + outer_var  ;
// Call the closures.
//   println!("closure_annotated: {}", closure_annotated(1));
//   println!("closure_inferred: {}", closure_inferred(1));
//   let one = || 1;
//   println!("closure returning one: {}", one());
// }
//
// Function pointers
// fn add(x: i32, y: i32) -> i32 {
//   x + y
// }
// let mut x = add(5,7);
// type Binop = fn(i32, i32) -> i32;
// let bo: Binop = add;
// x = bo(5,7);


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
