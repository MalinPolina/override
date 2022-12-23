#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct save {
	char    msg[140];
  	char    username[40];
  	size_t  size;
};

void	secret_backdoor(void) {
  	char buf[128];

  	fgets(buf, 128, stdin);
  	system(buf);
}

void	set_msg(struct save *info) {
	char message[128];

  	puts(">: Msg @Unix-Dude");
  	printf(">>: ");
  	fgets(message, 1024, stdin);
  	strncpy(info->msg, message, info->size);
}

void	set_username(struct save *info) {
  	char    username[16];
  	size_t  i;

  	puts(">: Enter your username");
  	printf(">>: ");
  	fgets(username, 128, stdin);
  	i = 0;
  	while (i <= 40 && username[i]) {
    	info->username[idx] = username[i];
    	++i;
  	}
	printf(">: Welcome, %s", info->username);
}

void	handle_msg(void) {
	struct save info;

  	info.size = 140;
  	set_username(&info);
  	set_msg(&info);
  	puts(">: Msg sent!");
  	return;
}

int		main(void) {
	puts("--------------------------------------------\n"
    	"|   ~Welcome to l33t-m$n ~    v1337        |\n"
    	"--------------------------------------------");
  	handle_msg();
  	return 0;
}

