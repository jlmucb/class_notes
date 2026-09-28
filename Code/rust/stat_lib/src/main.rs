#[allow(unused)]
#[allow(unused_mut)]

mod stat;
use crate::stat::stat::mean;
use crate::stat::stat::variance;

fn main() {
  println!("Statistics!");
  let mut _samp: Vec<f64> = Vec::new();
  let mut _res: bool;
  let mut _av: f64;
  let mut _var: f64;

  for n in 0..10 {
    _samp.push((n + 1) as f64);
  }

  print!("samples: ");
  for n in 0..10 {
    print!("{:} ", _samp[n]);
  }
  print!("\n");

  (_res, _av) = mean(_samp.clone()) ;
  if _res {
    println!(" mean: {:}", _av);
  } else {
    println!(" mean failed");
  }

  (_res, _var) = variance(_samp.clone()) ;
  if _res {
    println!(" variance: {:}, std_dev: {:}", _var, _var.sqrt());
  } else {
    println!(" variance failed");
  }
}
