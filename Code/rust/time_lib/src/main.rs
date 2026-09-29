mod time;

// use crate::time::time::now;
// use crate::time::time::print;
// use crate::time::time::to_str;
// use crate::time::time::from_str;
// use crate::time::time::compare;
// use crate::time::time::add_interval;

use chrono::prelude::*;
use chrono::DateTime;
use chrono::Utc;
use chrono::TimeDelta;
use std::format;

fn main() {
  println!("Time");

  let utc: DateTime<Utc> = Utc::now();
  println!("{:}", utc);
  println!("utc timestamp: {:}", utc.timestamp());

  // let mut dt: DateTime<Utc> = Utc.with_ymd_and_hms(2026, 9, 29, 12, 0, 0).unwrap();
  // println!("{:}", dt);
  // println!("timestamp: {:}", dt.timestamp());

  let delta = TimeDelta::new(3600_i64, 0_u32);
  let utc2 = utc.checked_add_signed(delta.expect("reason"));
  println!("new utc: {:?}", utc2.unwrap());

  let y = utc.year();
  let m = utc.month();
  let d = utc.day();
  let h = utc.hour();
  let min = utc.minute();
  let sec = utc.second();
  let nano_sec = utc.nanosecond();
  let fsec = sec as f64 + (nano_sec as f64) / 1000000000.0;
  println!("Myformat: {:}-{:}-{:}T{:}:{:}:{:}Z", y, m, d, h, min, fsec);

  let str2 = format!("{:}-{:}-{:}T{:}:{:}:{:}Z", y, m, d, h, min, fsec);
  println!("str: {:?}", str2);
  let dt2= Utc.datetime_from_str(&str2, "%Y-%m-%dT%H:%M:%S.%fZ");
  println!("Deserialized: {:?}", dt2.unwrap());
}
