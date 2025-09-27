
int foo(int one, int two) {
    return one + two;
}

int main(){
    int x = 0;
    int y = x + 1;
    int z = foo(y,x);
    return z;
}