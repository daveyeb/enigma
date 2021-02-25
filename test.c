#include <stdio.h>
#include <string.h>

int main(){

	int invert_flg = !(1&& 1);

	printf("invert = %d", invert_flg);
	return 0;
}