#[allow(unused)]
fn main() {
    println!("Large arith");

    let op1: u128 = 0xffffffffffffffff;
    let mut op2: u128 = 0x03;
    let mut r: u128;

    r = op1 * op2;
    println!("{:032x} + {:032x} = {:032x}", op1, op2, r);
    r = op1 * op1;
    println!("{:032x} * {:032x} = {:032x}", op1, op1, r);
    let low: u64 = (r & 0xffffffffffffffff) as u64;
    let high: u64 = ((r >>64) & 0xffffffffffffffff) as u64;
    println!("high: {:016x}, low: {:016x}", high, low);
}
