#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void log_wrapper(FILE *log, char *msg, char *file)
{
	char	 buf[0x100];

	strcpy(buf, msg);

	size_t len = strlen(msg);
	char *ptr = buf;
	ptr += len;

	snprintf(ptr, 0xFF - len, file);
	ptr = buf + strcspn(buf, "\n");
	*ptr = '\0';

	fprintf(log, "LOG: %s\n", buf);
}

int main(int argc, char **argv)
{
	FILE	*log;
	FILE	*file;
	int		fd;
	char	c;
	char	path[100];

	if (argc != 2)
		printf("Usage: %s filename\n", argv[0]);

	log = fopen("./backups/.log", "w");
	if (!log)
	{
		printf("ERROR: Failed to open ./backups/.log\n");
		exit(1);
	}

	log_wrapper(log, "Starting back up: ", argv[1]);

	file = fopen(argv[1], "r");
	if (!file)
	{
		printf("ERROR: Failed to open %s\n", argv[1]);
		exit(1);
	}

	strcpy(path, "./backups/");
	strncat(path, argv[1], 99 - strlen("./backups/"));
	fd = open(path, O_EXCL | O_CREAT | O_WRONLY, 0660);

	if (fd < 0)
	{
		printf("ERROR: Failed to open %s%s\n", "./backups/.log", argv[1]);
		exit(1);
	}

	while (1)
	{
		c = fgetc(file);
		if (c == -1)
			break;
		write(fd, &c, 1);
	}

	log_wrapper(log, "Finished back up ", argv[1]);
	fclose(file);
	close(fd);
}