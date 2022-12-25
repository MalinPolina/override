#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <stdio.h>
#include <sys/prctl.h>

int         main(void)
{
    int     pid = fork();
    char    str[32];

    memset(str, '\0', 32);
    int     ptrace_ret = 0;
    int     stat_loc = 0;

    if (pid == 0)
    {
        prctl(1, 1);
        ptrace(0, 0, NULL, NULL);
        puts("Give me some shellcode, k");
        gets(str);
    }
    else
    {
        while (1 == 1)
        {
            wait(&stat_loc);
            if ((stat_loc & 0x7f) != 0) {
                if (WIFEXITED(stat_loc) > 0) {
                    puts("child is exiting...");
                    return (0);
                }
                ptrace_ret = ptrace(3, pid, 44, NULL);
                if (ptrace_ret == 11)
                    break ;
            }
        }
        puts("no exec() for you");
        kill(pid, SIGKILL);
    }
    return (0);
}