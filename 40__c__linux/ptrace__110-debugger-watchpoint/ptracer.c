/*
  debugging with ptrace


  Implements a watchpoint on a supplied memory location. This will
  let you know when this address gets overwritten.

  ---

  tools:

  nm
  nm reads symbols from the ELF image and prints out starting addresses of any
  statically linked area. Dymanically resolved symbols are also printed out,
  but obviously with no address information. Since all local routines are
  statically linked, you can see the names of each routine in the process

  objdump
  objdump(1) utility provides much more useful information than the nm(1)
  utility, dumping all ELF sections, as well as providing disassembly listings
  of all code sections. Objdump(1) comes as part of the GCC toolset, but may
  not be available on all systems

  examples:

  $ nm ./rabbit.exe
    0000000000004038 B __bss_start
    0000000000004038 b completed.7325
                     w __cxa_finalize@@GLIBC_2.2.5
    0000000000004028 D __data_start
    0000000000004028 W data_start
    0000000000001090 t deregister_tm_clones
    0000000000001100 t __do_global_dtors_aux
    0000000000003de0 t __do_global_dtors_aux_fini_array_entry
    0000000000004030 D __dso_handle
    0000000000003de8 d _DYNAMIC
    0000000000004038 D _edata
    0000000000004040 B _end
    0000000000001204 T _fini
    0000000000001140 t frame_dummy
    0000000000003dd8 t __frame_dummy_init_array_entry
    0000000000002154 r __FRAME_END__
    0000000000004000 d _GLOBAL_OFFSET_TABLE_
                     w __gmon_start__
    0000000000002014 r __GNU_EH_FRAME_HDR
    0000000000001000 t _init
    0000000000003de0 t __init_array_end
    0000000000003dd8 t __init_array_start
    0000000000002000 R _IO_stdin_used
                     w _ITM_deregisterTMCloneTable
                     w _ITM_registerTMCloneTable
    0000000000001200 T __libc_csu_fini
    00000000000011a0 T __libc_csu_init
                     U __libc_start_main@@GLIBC_2.2.5
    0000000000001145 T main
                     U printf@@GLIBC_2.2.5
    00000000000010c0 t register_tm_clones
                     U sleep@@GLIBC_2.2.5
    0000000000001060 T _start
    0000000000004038 D __TMC_END__




  $ objdump -s ./rabbit.exe

    ./rabbit.exe:     file format elf64-x86-64

    Contents of section .interp:
     02a8 2f6c6962 36342f6c 642d6c69 6e75782d  /lib64/ld-linux-
     02b8 7838362d 36342e73 6f2e3200           x86-64.so.2.
    Contents of section .note.ABI-tag:
     02c4 04000000 10000000 01000000 474e5500  ............GNU.
     02d4 00000000 03000000 02000000 00000000  ................
    Contents of section .note.gnu.build-id:
     02e4 04000000 14000000 03000000 474e5500  ............GNU.
     02f4 f0dda725 62dc799d 6ecd3647 a60f0c05  ...%b.y.n.6G....
     0304 adf42b18                             ..+.
    Contents of section .gnu.hash:
     0308 02000000 07000000 01000000 06000000  ................
     0318 00008100 00000000 07000000 00000000  ................
     0328 d165ce6d                             .e.m
    Contents of section .dynsym:
     0330 00000000 00000000 00000000 00000000  ................
     0340 00000000 00000000 0b000000 20000000  ............ ...
     0350 00000000 00000000 00000000 00000000  ................
     0360 5a000000 12000000 00000000 00000000  Z...............
     0370 00000000 00000000 76000000 12000000  ........v.......
     0380 00000000 00000000 00000000 00000000  ................
     0390 27000000 20000000 00000000 00000000  '... ...........
     03a0 00000000 00000000 36000000 20000000  ........6... ...
     03b0 00000000 00000000 00000000 00000000  ................
     03c0 61000000 12000000 00000000 00000000  a...............
     03d0 00000000 00000000 67000000 22000000  ........g..."...
    (...)




  $ readelf -a ./rabbit.exe

    ELF Header:
      Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
      Class:                             ELF64
      Data:                              2's complement, little endian
      Version:                           1 (current)
      OS/ABI:                            UNIX - System V
      ABI Version:                       0
      Type:                              DYN (Shared object file)
      Machine:                           Advanced Micro Devices X86-64
      Version:                           0x1
      Entry point address:               0x1060
      Start of program headers:          64 (bytes into file)
      Start of section headers:          16928 (bytes into file)
      Flags:                             0x0
      Size of this header:               64 (bytes)
      Size of program headers:           56 (bytes)
      Number of program headers:         11
      Size of section headers:           64 (bytes)
      Number of section headers:         35
      Section header string table index: 34

    Section Headers:
      [Nr] Name              Type             Address           Offset
           Size              EntSize          Flags  Link  Info  Align
      [ 0]                   NULL             0000000000000000  00000000
           0000000000000000  0000000000000000           0     0     0
      [ 1] .interp           PROGBITS         00000000000002a8  000002a8
           000000000000001c  0000000000000000   A       0     0     1
      [ 2] .note.ABI-tag     NOTE             00000000000002c4  000002c4
           0000000000000020  0000000000000000   A       0     0     4
      [ 3] .note.gnu.build-i NOTE             00000000000002e4  000002e4
           0000000000000024  0000000000000000   A       0     0     4
      [ 4] .gnu.hash         GNU_HASH         0000000000000308  00000308
           0000000000000024  0000000000000000   A       5     0     8
      [ 5] .dynsym           DYNSYM           0000000000000330  00000330
           00000000000000c0  0000000000000018   A       6     1     8
      [ 6] .dynstr           STRTAB           00000000000003f0  000003f0
           0000000000000094  0000000000000000   A       0     0     1
      [ 7] .gnu.version      VERSYM           0000000000000484  00000484
           0000000000000010  0000000000000002   A       5     0     2
      [ 8] .gnu.version_r    VERNEED          0000000000000498  00000498
           0000000000000020  0000000000000000   A       6     1     8
      [ 9] .rela.dyn         RELA             00000000000004b8  000004b8
           00000000000000c0  0000000000000018   A       5     0     8
      [10] .rela.plt         RELA             0000000000000578  00000578
           0000000000000030  0000000000000018  AI       5    23     8
      [11] .init             PROGBITS         0000000000001000  00001000
           0000000000000017  0000000000000000  AX       0     0     4
      [12] .plt              PROGBITS         0000000000001020  00001020
           0000000000000030  0000000000000010  AX       0     0     16
      [13] .plt.got          PROGBITS         0000000000001050  00001050
           0000000000000008  0000000000000008  AX       0     0     8
      [14] .text             PROGBITS         0000000000001060  00001060
           00000000000001a1  0000000000000000  AX       0     0     16
      [15] .fini             PROGBITS         0000000000001204  00001204
           0000000000000009  0000000000000000  AX       0     0     4
      [16] .rodata           PROGBITS         0000000000002000  00002000
           0000000000000013  0000000000000000   A       0     0     4
      [17] .eh_frame_hdr     PROGBITS         0000000000002014  00002014
           000000000000003c  0000000000000000   A       0     0     4
      [18] .eh_frame         PROGBITS         0000000000002050  00002050
           0000000000000108  0000000000000000   A       0     0     8
      [19] .init_array       INIT_ARRAY       0000000000003dd8  00002dd8
           0000000000000008  0000000000000008  WA       0     0     8
      [20] .fini_array       FINI_ARRAY       0000000000003de0  00002de0
           0000000000000008  0000000000000008  WA       0     0     8
      [21] .dynamic          DYNAMIC          0000000000003de8  00002de8
           00000000000001f0  0000000000000010  WA       6     0     8
      [22] .got              PROGBITS         0000000000003fd8  00002fd8
           0000000000000028  0000000000000008  WA       0     0     8
      [23] .got.plt          PROGBITS         0000000000004000  00003000
           0000000000000028  0000000000000008  WA       0     0     8
      [24] .data             PROGBITS         0000000000004028  00003028
           0000000000000010  0000000000000000  WA       0     0     8
      [25] .bss              NOBITS           0000000000004038  00003038
           0000000000000008  0000000000000000  WA       0     0     1
      [26] .comment          PROGBITS         0000000000000000  00003038
           000000000000001c  0000000000000001  MS       0     0     1
      [27] .debug_aranges    PROGBITS         0000000000000000  00003054
           0000000000000030  0000000000000000           0     0     1
      [28] .debug_info       PROGBITS         0000000000000000  00003084
           0000000000000311  0000000000000000           0     0     1
      [29] .debug_abbrev     PROGBITS         0000000000000000  00003395
           00000000000000f7  0000000000000000           0     0     1
      [30] .debug_line       PROGBITS         0000000000000000  0000348c
           000000000000012d  0000000000000000           0     0     1
      [31] .debug_str        PROGBITS         0000000000000000  000035b9
           0000000000000273  0000000000000001  MS       0     0     1
      [32] .symtab           SYMTAB           0000000000000000  00003830
           0000000000000690  0000000000000018          33    50     8
      [33] .strtab           STRTAB           0000000000000000  00003ec0
           0000000000000219  0000000000000000           0     0     1
      [34] .shstrtab         STRTAB           0000000000000000  000040d9
           0000000000000147  0000000000000000           0     0     1
    Key to Flags:
      W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
      L (link order), O (extra OS processing required), G (group), T (TLS),
      C (compressed), x (unknown), o (OS specific), E (exclude),
      l (large), p (processor specific)

    There are no section groups in this file.

    Program Headers:
      Type           Offset             VirtAddr           PhysAddr
                     FileSiz            MemSiz              Flags  Align
      PHDR           0x0000000000000040 0x0000000000000040 0x0000000000000040
                     0x0000000000000268 0x0000000000000268  R      0x8
      INTERP         0x00000000000002a8 0x00000000000002a8 0x00000000000002a8
                     0x000000000000001c 0x000000000000001c  R      0x1
          [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
      LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                     0x00000000000005a8 0x00000000000005a8  R      0x1000
      LOAD           0x0000000000001000 0x0000000000001000 0x0000000000001000
                     0x000000000000020d 0x000000000000020d  R E    0x1000
      LOAD           0x0000000000002000 0x0000000000002000 0x0000000000002000
                     0x0000000000000158 0x0000000000000158  R      0x1000
      LOAD           0x0000000000002dd8 0x0000000000003dd8 0x0000000000003dd8
                     0x0000000000000260 0x0000000000000268  RW     0x1000
      DYNAMIC        0x0000000000002de8 0x0000000000003de8 0x0000000000003de8
                     0x00000000000001f0 0x00000000000001f0  RW     0x8
      NOTE           0x00000000000002c4 0x00000000000002c4 0x00000000000002c4
                     0x0000000000000044 0x0000000000000044  R      0x4
      GNU_EH_FRAME   0x0000000000002014 0x0000000000002014 0x0000000000002014
                     0x000000000000003c 0x000000000000003c  R      0x4
      GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                     0x0000000000000000 0x0000000000000000  RW     0x10
      GNU_RELRO      0x0000000000002dd8 0x0000000000003dd8 0x0000000000003dd8
                     0x0000000000000228 0x0000000000000228  R      0x1

     Section to Segment mapping:
      Segment Sections...
       00
       01     .interp
       02     .interp .note.ABI-tag .note.gnu.build-id .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt
       03     .init .plt .plt.got .text .fini
       04     .rodata .eh_frame_hdr .eh_frame
       05     .init_array .fini_array .dynamic .got .got.plt .data .bss
       06     .dynamic
       07     .note.ABI-tag .note.gnu.build-id
       08     .eh_frame_hdr
       09
       10     .init_array .fini_array .dynamic .got

    Dynamic section at offset 0x2de8 contains 27 entries:
      Tag        Type                         Name/Value
     0x0000000000000001 (NEEDED)             Shared library: [libm.so.6]
     0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
     0x000000000000000c (INIT)               0x1000
     0x000000000000000d (FINI)               0x1204
     0x0000000000000019 (INIT_ARRAY)         0x3dd8
     0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
     0x000000000000001a (FINI_ARRAY)         0x3de0
     0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
     0x000000006ffffef5 (GNU_HASH)           0x308
     0x0000000000000005 (STRTAB)             0x3f0
     0x0000000000000006 (SYMTAB)             0x330
     0x000000000000000a (STRSZ)              148 (bytes)
     0x000000000000000b (SYMENT)             24 (bytes)
     0x0000000000000015 (DEBUG)              0x0
     0x0000000000000003 (PLTGOT)             0x4000
     0x0000000000000002 (PLTRELSZ)           48 (bytes)
     0x0000000000000014 (PLTREL)             RELA
     0x0000000000000017 (JMPREL)             0x578
     0x0000000000000007 (RELA)               0x4b8
     0x0000000000000008 (RELASZ)             192 (bytes)
     0x0000000000000009 (RELAENT)            24 (bytes)
     0x000000006ffffffb (FLAGS_1)            Flags: PIE
     0x000000006ffffffe (VERNEED)            0x498
     0x000000006fffffff (VERNEEDNUM)         1
     0x000000006ffffff0 (VERSYM)             0x484
     0x000000006ffffff9 (RELACOUNT)          3
     0x0000000000000000 (NULL)               0x0

    Relocation section '.rela.dyn' at offset 0x4b8 contains 8 entries:
      Offset          Info           Type           Sym. Value    Sym. Name + Addend
    000000003dd8  000000000008 R_X86_64_RELATIVE                    1140
    000000003de0  000000000008 R_X86_64_RELATIVE                    1100
    000000004030  000000000008 R_X86_64_RELATIVE                    4030
    000000003fd8  000100000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_deregisterTMClone + 0
    000000003fe0  000300000006 R_X86_64_GLOB_DAT 0000000000000000 __libc_start_main@GLIBC_2.2.5 + 0
    000000003fe8  000400000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0
    000000003ff0  000500000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_registerTMCloneTa + 0
    000000003ff8  000700000006 R_X86_64_GLOB_DAT 0000000000000000 __cxa_finalize@GLIBC_2.2.5 + 0

    Relocation section '.rela.plt' at offset 0x578 contains 2 entries:
      Offset          Info           Type           Sym. Value    Sym. Name + Addend
    000000004018  000200000007 R_X86_64_JUMP_SLO 0000000000000000 printf@GLIBC_2.2.5 + 0
    000000004020  000600000007 R_X86_64_JUMP_SLO 0000000000000000 sleep@GLIBC_2.2.5 + 0

    The decoding of unwind sections for machine type Advanced Micro Devices X86-64 is not currently supported.

    Symbol table '.dynsym' contains 8 entries:
       Num:    Value          Size Type    Bind   Vis      Ndx Name
         0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
         1: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
         2: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@GLIBC_2.2.5 (2)
         3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@GLIBC_2.2.5 (2)
         4: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
         5: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
         6: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND sleep@GLIBC_2.2.5 (2)
         7: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@GLIBC_2.2.5 (2)

    Symbol table '.symtab' contains 70 entries:
       Num:    Value          Size Type    Bind   Vis      Ndx Name
         0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND
         1: 00000000000002a8     0 SECTION LOCAL  DEFAULT    1
         2: 00000000000002c4     0 SECTION LOCAL  DEFAULT    2
         3: 00000000000002e4     0 SECTION LOCAL  DEFAULT    3
         4: 0000000000000308     0 SECTION LOCAL  DEFAULT    4
         5: 0000000000000330     0 SECTION LOCAL  DEFAULT    5
         6: 00000000000003f0     0 SECTION LOCAL  DEFAULT    6
         7: 0000000000000484     0 SECTION LOCAL  DEFAULT    7
         8: 0000000000000498     0 SECTION LOCAL  DEFAULT    8
         9: 00000000000004b8     0 SECTION LOCAL  DEFAULT    9
        10: 0000000000000578     0 SECTION LOCAL  DEFAULT   10
        11: 0000000000001000     0 SECTION LOCAL  DEFAULT   11
        12: 0000000000001020     0 SECTION LOCAL  DEFAULT   12
        13: 0000000000001050     0 SECTION LOCAL  DEFAULT   13
        14: 0000000000001060     0 SECTION LOCAL  DEFAULT   14
        15: 0000000000001204     0 SECTION LOCAL  DEFAULT   15
        16: 0000000000002000     0 SECTION LOCAL  DEFAULT   16
        17: 0000000000002014     0 SECTION LOCAL  DEFAULT   17
        18: 0000000000002050     0 SECTION LOCAL  DEFAULT   18
        19: 0000000000003dd8     0 SECTION LOCAL  DEFAULT   19
        20: 0000000000003de0     0 SECTION LOCAL  DEFAULT   20
        21: 0000000000003de8     0 SECTION LOCAL  DEFAULT   21
        22: 0000000000003fd8     0 SECTION LOCAL  DEFAULT   22
        23: 0000000000004000     0 SECTION LOCAL  DEFAULT   23
        24: 0000000000004028     0 SECTION LOCAL  DEFAULT   24
        25: 0000000000004038     0 SECTION LOCAL  DEFAULT   25
        26: 0000000000000000     0 SECTION LOCAL  DEFAULT   26
        27: 0000000000000000     0 SECTION LOCAL  DEFAULT   27
        28: 0000000000000000     0 SECTION LOCAL  DEFAULT   28
        29: 0000000000000000     0 SECTION LOCAL  DEFAULT   29
        30: 0000000000000000     0 SECTION LOCAL  DEFAULT   30
        31: 0000000000000000     0 SECTION LOCAL  DEFAULT   31
        32: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
        33: 0000000000001090     0 FUNC    LOCAL  DEFAULT   14 deregister_tm_clones
        34: 00000000000010c0     0 FUNC    LOCAL  DEFAULT   14 register_tm_clones
        35: 0000000000001100     0 FUNC    LOCAL  DEFAULT   14 __do_global_dtors_aux
        36: 0000000000004038     1 OBJECT  LOCAL  DEFAULT   25 completed.7325
        37: 0000000000003de0     0 OBJECT  LOCAL  DEFAULT   20 __do_global_dtors_aux_fin
        38: 0000000000001140     0 FUNC    LOCAL  DEFAULT   14 frame_dummy
        39: 0000000000003dd8     0 OBJECT  LOCAL  DEFAULT   19 __frame_dummy_init_array_
        40: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS rabbit.c
        41: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
        42: 0000000000002154     0 OBJECT  LOCAL  DEFAULT   18 __FRAME_END__
        43: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS
        44: 0000000000003de0     0 NOTYPE  LOCAL  DEFAULT   19 __init_array_end
        45: 0000000000003de8     0 OBJECT  LOCAL  DEFAULT   21 _DYNAMIC
        46: 0000000000003dd8     0 NOTYPE  LOCAL  DEFAULT   19 __init_array_start
        47: 0000000000002014     0 NOTYPE  LOCAL  DEFAULT   17 __GNU_EH_FRAME_HDR
        48: 0000000000004000     0 OBJECT  LOCAL  DEFAULT   23 _GLOBAL_OFFSET_TABLE_
        49: 0000000000001000     0 FUNC    LOCAL  DEFAULT   11 _init
        50: 0000000000001200     1 FUNC    GLOBAL DEFAULT   14 __libc_csu_fini
        51: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterTMCloneTab
        52: 0000000000004028     0 NOTYPE  WEAK   DEFAULT   24 data_start
        53: 0000000000004038     0 NOTYPE  GLOBAL DEFAULT   24 _edata
        54: 0000000000001204     0 FUNC    GLOBAL HIDDEN    15 _fini
        55: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND printf@@GLIBC_2.2.5
        56: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_main@@GLIBC_
        57: 0000000000004028     0 NOTYPE  GLOBAL DEFAULT   24 __data_start
        58: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
        59: 0000000000004030     0 OBJECT  GLOBAL HIDDEN    24 __dso_handle
        60: 0000000000002000     4 OBJECT  GLOBAL DEFAULT   16 _IO_stdin_used
        61: 00000000000011a0    93 FUNC    GLOBAL DEFAULT   14 __libc_csu_init
        62: 0000000000004040     0 NOTYPE  GLOBAL DEFAULT   25 _end
        63: 0000000000001060    43 FUNC    GLOBAL DEFAULT   14 _start
        64: 0000000000004038     0 NOTYPE  GLOBAL DEFAULT   25 __bss_start
        65: 0000000000001145    76 FUNC    GLOBAL DEFAULT   14 main
        66: 0000000000004038     0 OBJECT  GLOBAL HIDDEN    24 __TMC_END__
        67: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMCloneTable
        68: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND sleep@@GLIBC_2.2.5
        69: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@@GLIBC_2.2

    Histogram for `.gnu.hash' bucket list length (total of 2 buckets):
     Length  Number     % of total  Coverage
          0  1          ( 50.0%)
          1  1          ( 50.0%)    100.0%

    Version symbols section '.gnu.version' contains 8 entries:
     Addr: 0000000000000484  Offset: 0x000484  Link: 5 (.dynsym)
      000:   0 (*local*)       0 (*local*)       2 (GLIBC_2.2.5)   2 (GLIBC_2.2.5)
      004:   0 (*local*)       0 (*local*)       2 (GLIBC_2.2.5)   2 (GLIBC_2.2.5)

    Version needs section '.gnu.version_r' contains 1 entry:
     Addr: 0x0000000000000498  Offset: 0x000498  Link: 6 (.dynstr)
      000000: Version: 1  File: libc.so.6  Cnt: 1
      0x0010:   Name: GLIBC_2.2.5  Flags: none  Version: 2

    Displaying notes found in: .note.ABI-tag
      Owner                 Data size       Description
      GNU                  0x00000010       NT_GNU_ABI_TAG (ABI version tag)
        OS: Linux, ABI: 3.2.0

    Displaying notes found in: .note.gnu.build-id
      Owner                 Data size       Description
      GNU                  0x00000014       NT_GNU_BUILD_ID (unique build ID bitstring)
        Build ID: f0dda72562dc799d6ecd3647a60f0c05adf42b18



  while the rabbit.exe runs and the tracer stopped it, try doing the following

  $ cat /proc/$(pidof rabbit.exe)/maps
    560ddfec5000-560ddfec6000 r--p 00000000 08:12 24642488                   /opt/github__c/40__c__linux/ptrace__110-debugger-watchpoint/rabbit.exe
    560ddfec6000-560ddfec7000 r-xp 00001000 08:12 24642488                   /opt/github__c/40__c__linux/ptrace__110-debugger-watchpoint/rabbit.exe
    560ddfec7000-560ddfec8000 r--p 00002000 08:12 24642488                   /opt/github__c/40__c__linux/ptrace__110-debugger-watchpoint/rabbit.exe
    560ddfec8000-560ddfec9000 r--p 00002000 08:12 24642488                   /opt/github__c/40__c__linux/ptrace__110-debugger-watchpoint/rabbit.exe
    560ddfec9000-560ddfeca000 rw-p 00003000 08:12 24642488                   /opt/github__c/40__c__linux/ptrace__110-debugger-watchpoint/rabbit.exe
    7f1a56c70000-7f1a56c73000 rw-p 00000000 00:00 0
    7f1a56c73000-7f1a56c95000 r--p 00000000 08:12 3670064                    /lib/x86_64-linux-gnu/libc-2.28.so
    7f1a56c95000-7f1a56ddd000 r-xp 00022000 08:12 3670064                    /lib/x86_64-linux-gnu/libc-2.28.so
    7f1a56ddd000-7f1a56e29000 r--p 0016a000 08:12 3670064                    /lib/x86_64-linux-gnu/libc-2.28.so
    7f1a56e29000-7f1a56e2a000 ---p 001b6000 08:12 3670064                    /lib/x86_64-linux-gnu/libc-2.28.so
    7f1a56e2a000-7f1a56e2e000 r--p 001b6000 08:12 3670064                    /lib/x86_64-linux-gnu/libc-2.28.so
    7f1a56e2e000-7f1a56e30000 rw-p 001ba000 08:12 3670064                    /lib/x86_64-linux-gnu/libc-2.28.so
    7f1a56e30000-7f1a56e34000 rw-p 00000000 00:00 0
    7f1a56e34000-7f1a56e41000 r--p 00000000 08:12 3670257                    /lib/x86_64-linux-gnu/libm-2.28.so
    7f1a56e41000-7f1a56ee0000 r-xp 0000d000 08:12 3670257                    /lib/x86_64-linux-gnu/libm-2.28.so
    7f1a56ee0000-7f1a56fb5000 r--p 000ac000 08:12 3670257                    /lib/x86_64-linux-gnu/libm-2.28.so
    7f1a56fb5000-7f1a56fb6000 r--p 00180000 08:12 3670257                    /lib/x86_64-linux-gnu/libm-2.28.so
    7f1a56fb6000-7f1a56fb7000 rw-p 00181000 08:12 3670257                    /lib/x86_64-linux-gnu/libm-2.28.so
    7f1a56fb7000-7f1a56fb9000 rw-p 00000000 00:00 0
    7f1a56fdb000-7f1a56fdc000 r--p 00000000 08:12 3670019                    /lib/x86_64-linux-gnu/ld-2.28.so
    7f1a56fdc000-7f1a56ffa000 r-xp 00001000 08:12 3670019                    /lib/x86_64-linux-gnu/ld-2.28.so
    7f1a56ffa000-7f1a57002000 r--p 0001f000 08:12 3670019                    /lib/x86_64-linux-gnu/ld-2.28.so
    7f1a57002000-7f1a57003000 r--p 00026000 08:12 3670019                    /lib/x86_64-linux-gnu/ld-2.28.so
    7f1a57003000-7f1a57004000 rw-p 00027000 08:12 3670019                    /lib/x86_64-linux-gnu/ld-2.28.so
    7f1a57004000-7f1a57005000 rw-p 00000000 00:00 0
    7ffef7f90000-7ffef7fb2000 rw-p 00000000 00:00 0                          [stack]
    7ffef7fe3000-7ffef7fe6000 r--p 00000000 00:00 0                          [vvar]
    7ffef7fe6000-7ffef7fe8000 r-xp 00000000 00:00 0                          [vdso]

  try one of the addresses, e.g. 0x560ddfec5000, above

  ---


  AUTHOR: Lothar Rubusch, L.Rubusch@gmx.ch


  RESOURCES:
  * based on a true example, Jim Blakey (jdblakey@innovative-as.com),
  http://www.secretmango.com/jimb/Whitepapers/ptrace/ptrace.html
*/


#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>
#include <wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define M_OFFSETOF(STRUCT, ELEMENT)                                            \
  ( long unsigned int )&(( STRUCT * )NULL)->ELEMENT;

int main(int argc, char *argv[])
{
  pid_t tracee_pid, stat, res;
  long pres;
  int signo;
  int ipoffs, spoffs;
  unsigned int memcontents = 0, startcontents = 0, watchaddr = 0;

  if (argv[1] == NULL || argv[2] == NULL) {
    fprintf(stderr, "Usage: %s <pid> <watchpoint addr>\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  tracee_pid = strtoul(argv[1], NULL, 10);
  watchaddr = strtoul(argv[2], NULL, 16);
  printf("tracing pid %d. checking for change to 0x%016lx \n", tracee_pid, (long)watchaddr);

  ipoffs = M_OFFSETOF(struct user, regs.rip);
  fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - ipoffs\n", __func__, __LINE__, ipoffs);

  spoffs = M_OFFSETOF(struct user, regs.rsp);
  fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - spoffs\n", __func__, __LINE__, spoffs);

  printf("Attaching to process %d\n", tracee_pid);
  if (0 != (pres = ptrace(PTRACE_ATTACH, tracee_pid, 0, 0))) {
    fprintf(stderr, "FAILED! attach result %lx\n", pres);
  }

  if ((tracee_pid != (res = waitpid(tracee_pid, &stat, WUNTRACED))) || !(WIFSTOPPED(stat))) {
    fprintf(stderr, "FAILED! unexpected wait result res %d stat %x\n", res, stat);
    exit(EXIT_FAILURE);
  }

  fprintf(stderr, "DEBUG %s()[%d]:\twait result stat 0x%016x, pid %d\n", __func__, __LINE__, stat, res);
  stat = 0;
  signo = 0;

  // Get the starting value at the requested watch location. The PTRACE_PEEKEXT
  // option allows you to reach into the tartet process address space, using
  // its relocation maps, and read/change values
  startcontents = ptrace(PTRACE_PEEKTEXT, tracee_pid, watchaddr, 0);
  fprintf(stderr, "DEBUG %s()[%d]:\tstarting value at 0x%016lx, is 0x%016lx\n", __func__, __LINE__, (long)watchaddr, (long)startcontents);

  sleep(1);

  while (1) {
    // Ok, now we will continue the child, but set the single step bit in
    // the psw. This will cause the child to exeute just one instruction and
    // trap us again. The wait(2) catches the trap.
    if (0 > (pres = ptrace(PTRACE_SINGLESTEP, tracee_pid, 0, signo))) {
      perror("Ptrace singlestep error");
      exit(EXIT_FAILURE);
    }

    res = wait(&stat);
    // The previous call to wait(2) returned the child's signal number.
    // If this is a SIGTRAP, then we set it to zero (this does not get
    // passed on to the child when we PTRACE_CONT or PTRACE_SINGLESTEP
    // it).
    if ((signo = WSTOPSIG(stat)) == SIGTRAP) {
      fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - signo (SIGTRAP)\n", __func__, __LINE__, signo);
      signo = 0;
    }

    // If it is the SIGHUP, then PTRACE_CONT the child and we can exit.
    if ((signo == SIGHUP) || (signo == SIGINT)) {
      ptrace(PTRACE_CONT, tracee_pid, 0, signo);
      fprintf(stderr, "DEBUG %s()[%d]:\t0x%016x - signo (SIGHUP or SIGINT). Child took a SIGHUP or SIGINT. We are done\n", __func__, __LINE__, signo);
      break;
    }

    printf("press ENTER\n");
    getchar();

    // get the current value from the watched address and see if it is
    // different from the starting value. If so, then get the instruction
    // pointer from the target's user area, 'cause this is the instruction
    // that changed the value!
    memcontents = ptrace(PTRACE_PEEKTEXT, tracee_pid, watchaddr, 0);
    if (memcontents != startcontents) {
      printf("!!!!! Watchpoint address changed !!!!!\n");
      printf("Instruction that changed it at 0x%016lx\n", ptrace(PTRACE_PEEKUSER, tracee_pid, ipoffs, 0));
      printf("New contents of address 0x%x\n", memcontents);
      break;
    }
  }
  printf("Debugging complete\n");
  return (0);
}
