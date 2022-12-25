Проанализировав код программы, приходим к выводу, что программа запрашивает у пользователя его имя и серийный номер. Эти данные отправляются в функцию аутентификации и если из функции возвращается 0, то вызывается shell.

Внутри функции аутентификации имя пользователя хэшируется определенным образом и сравнивается с введенным серийным номером. В случае совпадения из функции возвращается 0.

Таким образом, устанавим две точки останова. Первый, чтобы перепрыгнуть проверку возвращаемого значения из вызова ptrace, а второй, чтобы получить окончательное значение.

```
(gdb) b *0x080487ba
Breakpoint 1 at 0x80487ba
(gdb) b *0x08048866
Breakpoint 2 at 0x8048866
(gdb) r
Starting program: /home/users/level06/level06
***********************************
*               level06           *
***********************************
-> Enter Login: telman
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial: 42

Breakpoint 1, 0x080487ba in auth ()
(gdb) p $eax=0
$1 = 0
(gdb) c
Continuing.

Breakpoint 2, 0x08048866 in auth ()
(gdb)  x/d $ebp-0x10
0xffffd698:     6232794
(gdb) q
A debugging session is active.

Inferior 1 [process 2600] will be killed.

Quit anyway? (y or n) y
level06@OverRide:~$ ./level06
***********************************
*               level06           *
***********************************
-> Enter Login: telman
***********************************
***** NEW ACCOUNT DETECTED ********
***********************************
-> Enter Serial:  6232794
Authenticated!
$ whoami
level07
$ cat /home/users/level07/.pass
GbcPDRgsFK77LNnnuh7QyFYA2942Gp8yKj9KrWD8
```