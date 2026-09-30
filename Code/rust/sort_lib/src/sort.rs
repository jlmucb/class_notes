#[allow(unused)]
#[allow(unused_mut)]
pub mod sort {

  pub fn bucketsort() -> bool {
    return true;
  }
  
  pub fn quicksort() -> bool {
    return true;
  }

  pub fn topsort() -> bool {
    return true;
  }

  pub fn bubblesort(in_vec: Vec<i64>, out_vec: &mut Vec<i64>) -> bool {

    for k in 0..in_vec.len() {
      out_vec.push(in_vec[k]);
    }

    let mut _max: i64 = 0;
    let mut _max_pos: usize = 0;
    let mut _i: usize = 0;
    let mut _j: usize = 0;
    let l = out_vec.len();

    loop {
      if _i >= (l-1) {
        break;
      }
      _j = _i + 1;
      _max = out_vec[_i];
      _max_pos = _j;
      loop {
        if _j > (l-1) {
          break;
        }
        if out_vec[_j] > _max {
          _max_pos = _j;
          _max = out_vec[_j];
        }
        _j += 1;
      }
      if _max_pos != _i {
        let mut _t: i64;
        _t = out_vec[_i];
        out_vec[_i]= out_vec[_max_pos];
        out_vec[_max_pos] = _t;
      }
      _i += 1;
    }
    return true;
  }

  pub fn heapsort() -> bool {
    return true;
  }
}

#[allow(unused_imports)]
use crate::sort::sort::bubblesort;
// use crate::sort::sort::quicksort;
// use crate::sort::sort::topsort;
// use crate::sort::sort::heapsort;
#[test]
fn sort_test() {
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

  if ! bubblesort(_in, &mut _out) {
    assert!(1 == 0, "Bubblesort failed");
  }
  assert!(_out.len() == 12);
  assert!(_out[0]  == 118_i64 && _out[l - 1] == 1, "_out[0]: {:}, _out[{:}]: {:}\n", _out[0], l-1, _out[l-1]);
}
