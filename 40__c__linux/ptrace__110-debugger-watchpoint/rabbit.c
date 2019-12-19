/*
  dummy example

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



  author: Lothar Rubusch
  email: L.Rubusch@gmx.ch
  original: Linux Journal, Oct 31, 2002  By Pradeep Padala
 */

#include <stdio.h> /* printf() */
#include <stdlib.h>
#include <unistd.h> /* sleep() */

int main(int argc, char **argv)
{
  int cnt;
  sleep(1);
  for (cnt = 0; cnt < 999999999; ++cnt) {
    printf("counter: '%d'\n", cnt);
  }
  return 0;
}
