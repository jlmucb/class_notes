#[allow(unused)]
#[allow(unused_mut)]
#[allow(unused_imports)]

use chrono::prelude::*;

pub mod time {

  // These are accessible outside the module
  pub struct Time {
    year_: i32,
    month_: i32,
    day_: i32,
    day_of_year_: i32,
    hour_: i32,
    min_: i32,
    secs_: f64,
  }
  pub fn print() {
  }
  pub fn now() {
  }
  pub fn to_str() {
  }
  pub fn from_str() {
  }
  pub fn compare() {
  }
  pub fn add_interval() {
  }
}

// use crate::time::time::now;
// use crate::time::time::print;
// use crate::time::time::to_str;
// use crate::time::time::from_str;
// use crate::time::time::compare;
// use crate::time::time::add_interval;

#[test]
fn time_test() {
}
