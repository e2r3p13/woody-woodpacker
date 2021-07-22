int main(void)
{
    int a;
    int b;

    a = 0;
    b = 0x42;
    while (a < 100)
    {
        b ^= a + b;
        a++;
    }
    return (0);
}
