#[allow(unused)]
#[allow(unused_mut)]

// in users: mod config;

pub mod stat {

use std::env;
use std::str::FromStr;

  // These are accessible outside the module
  pub fn mean(samples: Vec<f64>) -> (bool, f64) {
    let mut _sum: f64 = 0.0;
    let _size: f64 = samples.len() as f64;

    for i in 0..samples.len() {
      _sum += samples[i];
    }
    return (true, _sum / _size);
  }

  pub fn expectation(samples: Vec<f64>) -> (bool, f64) {
    return (true, 0.0);
  }

  pub fn variance(samples: Vec<f64>) -> (bool, f64) {
    let mut _sum: f64 = 0.0;
    let mut _av: f64 = 0.0;
    let _s: Vec<f64> = samples.clone();
    let _size: f64 = _s.len() as f64;
    let _var: f64 = 0.0;
    let _res: bool;
    let l = _s.len();

    (_res, _av) = mean(samples);
    if !_res {
      return (false, 0.0);
    }
    for i in 0..l {
      let _t = _av - _s[i];
      _sum += _t * _t;
    }
    return (true, _sum / _size);
  }

  pub fn correlation (x: Vec<f64>, y: Vec<f64>) -> (bool, f64) {
    return (true, 0.0);
  }
  pub fn byte_entropy(samples: Vec<u8>) -> (bool, f64) {
    return (true, 0.0);
  }
  pub fn bit_entropy(samples: Vec<u8>) -> (bool, f64) {
    return (true, 0.0);
  }
  pub fn rand_bytes(num_bytes: u32, mut out:&Vec<u8>) -> bool {
    return true;
  }
  pub fn uniform_sample(left: f64, right: f64) {
  }
  pub fn gaussian_sample() {
  }
  pub fn mle() {
  }
}

use crate::stat::stat::mean;
use crate::stat::stat::variance;
#[test]
fn stat_test() {
  println!("Stat test");

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
  assert!(_av == 5.5);

  (_res, _var) = variance(_samp.clone()) ;
  if _res {
    println!(" variance: {:}, std_dev: {:}", _var, _var.sqrt());
  } else {
    println!(" variance failed");
  }   

}
