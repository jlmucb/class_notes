#[allow(unused)]
#[allow(unused_mut)]

pub mod sort {

  // These are accessible outside the module
  pub fn quicksort() -> bool {
    return true;
  }

  pub fn topsort() -> bool {
    return true;
  }

  pub fn bubblesort(in_vec: Vec<i64>, mut out_vec: &Vec<i64>) -> bool {

    for k in 0..in_vec.len() {
      out_vec.push(in_vec[k]);
    }

    let mut _max: i64 = 0;
    let mut _max_pos: usize = 0;
    let mut _i: usize = 0;
    let mut _j: usize = 0;

    loop {
      if (out_vec.len()-1) >= _i {
        break;
      }
      _j = _i + 1;
      _max = out_vec[_i];
      _max_pos = _j;
      loop {
        if (out_vec.len()-1) >= _j {
          break;
        }
        if out_vec[_j] > _max {
          _max_pos = _j;
          _max = out_vec[_j];
        }
      }
      if _max_pos != _i {
        let mut _t: i64;
        _t = out_vec[_i];
        out_vec[_i]= out_vec[_max_pos];
        out_vec[_max_pos] = _t;
      }
    }

    return true;
  }

  pub fn heapsort() -> bool {
    return true;
  }
}

#[allow(unused_imports)]
use crate::sort::sort::bubblesort;
use crate::sort::sort::quicksort;
use crate::sort::sort::topsort;
use crate::sort::sort::heapsort;
#[test]
fn sort_test() {
}
