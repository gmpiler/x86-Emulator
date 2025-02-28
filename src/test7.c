int abs(int i)
{
    return (i >= 0) ? i : -i;
}

int sum(int a, int b)
{
    int abs_a, abs_b;
    abs_a = abs(a);
    abs_b = abs(b);

    int sum = 0;
    while (abs_a <= abs_b) {
        sum += abs_a;
        abs_a++;
    }
    return sum;
}

int main(void){
    return sum(-1, 10);
}
