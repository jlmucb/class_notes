#[allow(unused)]
#[allow(unused_mut)]
#[allow(unused_imports)]
#[allow(deprecated)]

pub mod time {

  use chrono::prelude::*;
  use chrono::Utc;
  use chrono::DateTime;
  use chrono::TimeDelta;
  use std::format;

  // These are accessible outside the module
  pub struct Time {
    year_: i32,
    month_: i32,
    day_: i32,
    hour_: i32,
    min_: i32,
    secs_: f64,
  }
  impl Default for Time {
    fn default() -> Self {Time {year_: 0, month_: 0, day_: 0, hour_: 0, min_: 0, secs_: 0.0} }
  }

  pub fn print(t: &mut Time) {
    print!("{:}-{:}-{:}T{:}:{:}:{:}Z", t.year_, t.month_, t.day_, t.hour_, t.min_, t.secs_);
  }

  pub fn now(t: &mut Time) -> bool {
    let utc: DateTime<Utc> = Utc::now();
    t.year_ = utc.year();
    t.month_ = utc.month() as i32;
    t.day_ = utc.day() as i32;
    t.hour_ = utc.hour() as i32;
    t.min_ = utc.minute() as i32;
    let sec = utc.second();
    let nano_sec = utc.nanosecond();
    let fsec = sec as f64 + (nano_sec as f64) / 1000000000.0;
    t.secs_ = fsec;
    return true;
  }

  pub fn to_str() {
  }

  pub fn from_str() {
    // let dt2= Utc.datetime_from_str(&str2, "%Y-%m-%dT%H:%M:%S.%fZ");
  }

  // returns 1 if t1 is later than t2
  // returns 0 if t1 is the same as t2
  // returns -1 if t1 is earlier than t2
  pub fn compare(t1: &mut Time, t2: &mut Time) -> i32 {
    if t1.year_ > t2.year_ {
      return 1;
    }
    if t1.year_ < t2.year_ {
      return -1;
    }
    if t1.month_ > t2.month_ {
      return 1;
    }
    if t1.month_ < t2.month_ {
      return -1;
    }
    if t1.day_ > t2.day_ {
      return 1;
    }
    if t1.day_ < t2.day_ {
      return -1;
    }
    if t1.hour_ > t2.hour_ {
      return 1;
    }
    if t1.hour_ > t2.hour_ {
      return 1;
    }
    if t1.min_ < t2.min_ {
      return -1;
    }
    if t1.min_ < t2.min_ {
      return -1;
    }
    if t1.secs_ > t2.secs_ {
      return 1;
    }
    if t1.secs_ < t2.secs_ {
      return -1;
    }
    return 0;
  }

  // add delta seconds to t1
  pub fn add_interval(t1: &mut Time, t2: &mut Time, delta: f64) -> bool {
    let mut str = format!("{:}-{:}-{:}T{:}:{:}:{:}Z", t1.year_, t1.month_, t1.day_, t1.hour_, t1.min_, t1.secs_);
    let mut ut1 = Utc.datetime_from_str(&str, "%Y-%m-%dT%H:%M:%S.%fZ").unwrap();
    let del = TimeDelta::new(delta as i64, 0);
    let ut2 = ut1 + del.unwrap();
    let y = ut2.year();
    let m = ut2.month();
    let d = ut2.day();
    let h = ut2.hour();
    let min = ut2.minute();
    let sec = ut2.second();
    let nano_sec = ut2.nanosecond();
    let fsec = sec as f64 + (nano_sec as f64) / 1000000000.0;
    t2.year_ = y;
    t2.month_ = m as i32;
    t2.day_ = d as i32;
    t2.hour_= h as i32;
    t2.min_= min as i32;
    t2.secs_ = fsec;

    return true;
  }

}

use crate::time::time::*;
#[test]
fn time_test() {

  let mut t1: Time = Time::default();
  let mut r: bool = now(&mut t1);
  print(&mut t1);
  println!("");
  assert!(r);
  let mut t2: Time = Time::default();

  let delta: f64 = 3600.0 * 24.0 * 400.0;
  r = add_interval(&mut t1, &mut t2, delta);
  assert!(r);
  print(&mut t1);
  print!(" + {:} secs = ", delta);
  print(&mut t2);
  println!("");
}

