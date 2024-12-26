int sum(int a, int b)
{
    int sum;
    sum = 0;
    while (a <= b) {
        sum += a;
        a++;
    }
    return sum;
}

int loop(int upper1, int upper2)
{
    int acm = 0;
    for(int i = 0; i < upper1; i++) {
        for(int j = 0; j < upper2; j++) {
            acm++;
        }
    }
    return acm;
}

int main(void){
    return loop(10, 10);
}
