#include<stdio.h>

int main(){
	unsigned int a = 0xf;
	int x = 1;
	a = a >> x;
	printf("%u", a);

}

