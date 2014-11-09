#include <stdio.h>

void function(int* num1,int* num2,int* num3)
{
    *num1 = 10;
    *num2 = 20;
    *num3 = 30;
}

int main()
{
    int one=1, two=2, three=3;
    function(&one, &two, &three);
    printf("%d %d %d", one,two,three);
    return 0;
}
