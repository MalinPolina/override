#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int         main(void) {
	unsigned int    j = 0;
	char            buff[100];
	
	fgets(buff, 100, stdin);
	i = 0;
	while (strlen(buff) >= i) {
		if (buff[j] > '@' && buff[j] <= 'Z') {
			buff[j] += 32;
		}
		j++;
	}
	printf(buff);
	exit(0);
}
