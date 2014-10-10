/************* t.c file ********************/

#include <stdio.h>

main(int argc, char *argv[], char *env[])
{
	int a,b,c;
	printf("enter main\n");

	printf("&argc=%x argv=%x env=%x\n", &argc, argv, env);
	printf("&a=%x &b=%x &c=%x\n", &a, &b, &c);

	a=1; b=2; c=3;
	A(a,b);
	printf("exit main\n");
}

int A(int x, int y)
{
	int d,e,f;
	printf("enter A\n");

	printf("&d=%x &e=%x &f=%x\n", &d, &e, &f);
	d=3; e=4; f=5;

	B(d,e);
	printf("exit A\n");
}

int B(int x, int y)
{
	int g,h,i;
	printf("enter B\n");

	printf("&g=%x &h=%x &i=%x\n", &g, &h, &i);
	g=6; h=7; i=8;

	C(g,h);
	printf("exit B\n");
}

int *p; 
int ebp;

int C(int x, int y)
{
	int u, v, w, i;
	printf("enter C\n");

	printf("&u=%x &v=%x &w=%x\n", &u, &v, &w);
	u=9; v=10; w=11;

	ebp = get_ebp(); // call get_ebp() in s.s file
	printf("ebp=%x\n", ebp);

	p = &w;
	printf("Address\tContents\n");
	for(i = 0; i < 100; i++)
	{
		printf("%x\t%x\n", p, *p);
		p++;
	}


	printf("exit C\n");
}
