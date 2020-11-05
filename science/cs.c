int foo(int a, int b, int c, int d, int e, int f) {
    int   bar[2];
    bar[0]= (int)'A';
    return bar[0]+7;
}

int main(void) {
    int i= 1;
    int j= 0;
    j= foo(1,2,3,4,5,6);
    i+= j;
    return i;
}



