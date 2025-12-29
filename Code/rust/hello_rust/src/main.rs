#![allow(dead_code)]

struct Person {
    name: String,
    age: u8,
}

fn sub1(p: &mut Person) -> bool  {
    p.name = String::from("Tori");
    return true;
}


fn main() {

    let mut s = Person { name: String::from(""), age: 25};
    let mut i_matrix: [i32; 5] = [11, 12, 13, 14, 15];

    println!("Hello, world!");
    i_matrix[2] = 22;

    s.name = String::from("Sedge");
    s.age = 25;
    println!("\nName: {}\nAge: {}\n", s.name, s.age);
    sub1(&mut s);
    print!("\nName: { }\nAge: { :}\n", s.name, s.age);

    print!("\nArray:\n");
    for i in 0..i_matrix.len() {
        print!("a[{}]: {:}, ", i, i_matrix[i]);
    }
    print!("\n");
}
