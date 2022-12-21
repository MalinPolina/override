nclude <stdlib.h>
#include <stdio.h>
#include <string.h>

void        clear_stdin(void) {
	char c;
	while (42) {
		c = getchar();
		if (c == '\n' || c == EOF)	{
			break;
		}
	}
}

int         get_unum(void) {
	unsigned int    nbr;

	fflush(stdin);
	scanf("%u", &nbr);
	clear_stdin();
	return nbr;
}

int         store_number(unsigned int *data) {
	unsigned int val;
	unsigned int idx;

	printf(" Number: ");
	val = get_unum();
	printf(" Index: ");
	idx = get_unum();

	if (idx % 3 == 0 || (val >> 24) == 183) {
		puts(" *** ERROR! ***\n");
		puts("   This index is reserved for wil!\n");
		puts(" *** ERROR! ***\n");
		return 1;
	}
	data[idx * 4] = val;
	return 0;
}

int         read_number(unsigned int *data) {
	unsigned int idx;

	printf(" Index: ");
	idx = get_unum();
	printf(" Number at data[%u] is %u\n", idx, data[idx * 4]);
	return 0;
}

int         main(int argc, char **argv, char **envp) {
	int ret;
	char cmd[20];
	unsigned int data[100];

	while (*argv) {
		memset(*argv, 0, strlen(*argv));
		++argv;
	}
	while (*env) {
		memset(*env, 0, strlen(*env));
		++env;
	}

	puts("----------------------------------------------------\n"\
	"  Welcome to wil's crappy number storage service!   \n"\
	"----------------------------------------------------\n"\
	" Commands:                                          \n"\
	"    store - store a number into the data storage    \n"\
	"    read  - read a number from the data storage     \n"\
	"    quit  - exit the program                        \n"\
	"----------------------------------------------------\n"\
	"   wil has reserved some storage :>                 \n"\
	"----------------------------------------------------\n"\
	"\n");

	while(1) {
		printf("Input command: ");
		ret = 1;
		fgets(cmd, sizeof(cmd), stdin);
		cmd[strlen(cmd)-1] = '\0';

		if(!strncmp(cmd, "store", 5)) {
			ret = store_number(data);
		} else if(!strncmp(cmd, "read", 4)) {
			ret = read_number(data);
		} else if(!strncmp(cmd, "quit", 4)) {
			break;
		}
		
		if(ret) {
			printf(" Failed to do %s command\n", cmd);
		} else {
			printf(" Completed %s command successfully\n", cmd);
		}
	}
	return 0;
}
