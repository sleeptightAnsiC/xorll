# Generic implementation of XOR Linked List

XOR linked list is a type of data structure. It takes advantage of the bitwise XOR operation to decrease memory usage by storing the composition of both addresses in one field. The adress of next Node can be deduced during traversal by combining adressess of current and previous Nodes. [reference](https://en.wikipedia.org/wiki/XOR_linked_list)

[![image](https://github.com/sleeptightAnsiC/generic_xor_linked_list/assets/91839286/45b0e033-a35f-481b-b487-5f47631f25d1)](https://www.geeksforgeeks.org/xor-linked-list-a-memory-efficient-doubly-linked-list-set-1/)

This implementation is generic by using `void*`. By design, almost everything is hidden inside of `.c` file. API-user can only access the adress of Item, nothing else. 

Should build with any C/C++ compiler on any OS, but mostly tested on Linux with gcc
```bash
cc list.h list.c example.c -Wall -Werror -Wextra -std=c99 -fsanitize=address,undefined
```

Inspired by [part of this video](https://youtu.be/sZ8GJ1TiMdk?feature=shared&t=528) talking about problems with implementing such List in systems using Garbage Collection.

I wrote it because I really wanted to write something generic in C...
