#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char a_user_name[100];

int     verify_user_name(void) {
	puts("verifying username....\n");
	return strncmp(a_user_name, "dat_wil", 7);
}

int     verify_user_pass(char *pass) {
	return strncmp(pass, "admin", 5);
}

int     main(void) {
	char    pass[64] = {};
	int     ret = 0;

	puts("********* ADMIN LOGIN PROMPT *********");
	printf("Enter Username: ");
	fgets(a_user_name, 256, stdin);
	ret = verify_user_name();
	if (ret) {
		puts("nope, incorrect username...\n");
		return (1);
	}
	puts("Enter Password: \n");
	fgets(pass, 100, stdin);
	ret = verify_user_pass(pass);
	if (!ret || ret) {
		puts("nope, incorrect password...\n");
		return (1);
	}
	return (0);
}
