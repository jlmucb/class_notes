#[allow(unused)]
#[allow(unused_mut)]

// in users: mod config;

mod stat {

use std::env;
use std::str::FromStr;

  // These are accessible outside the module
  pub fn mean(samples: Vec<f64>) -> f64 {
  }
  pub fn expectation(samples: Vec<f64>) -> f64 {
  }
  pub fn variance(samples: Vec<f64>) -> f64 {
  }
  pub fn correlation (X: Vec<f64>, Y: Vec<f64>) -> f64 {
  }
  pub fn byte_entropy(samples: Vec<u8>) -> f64 {
  }
  pub fn bit_entropy(samples: Vec<u8>) -> f64 {
  }
  pub fn rand_bytes(num_bytes: u32) {
  }
  pub fn uniform_sample(left: f64, right: f64) {
  }
  pub fn gaussian_sample() {
  }
  pub fn mle() {
  }
}

#[test]
fn stat_test() {
}
