mod sort;
use crate::sort::sort::bubblesort;

#[allow(unused)]
#[allow(unused_mut)]
#[allow(unused_imports)]
fn main() {
  println!("sorting!");
  let mut _in: Vec<i64> = Vec::new();
  let mut _out: Vec<i64> = Vec::new();

  _in.push(1_i64);
  _in.push(1_i64);
  _in.push(4_i64);
  _in.push(5_i64);
  _in.push(3_i64);
  _in.push(2_i64);
  _in.push(7_i64);
  _in.push(7_i64);
  _in.push(111_i64);
  _in.push(10_i64);
  _in.push(118_i64);
  _in.push(13_i64);

  let l = _in.len();

  print!("input : ");
  for n in 0..l {
    print!("{:} ", _in[n]);
  }
  print!("\n");

  if ! bubblesort(_in, &mut _out) {
    println!("bubblesort failed");
    return;
  }

  print!("output: ");
  for n in 0..l {
    print!("{:} ", _out[n]);
  }
  print!("\n");
}

