#ifndef DATAFILE_H_INCLUDED
#define DATAFILE_H_INCLUDED

#ifdef __ASSEMBLER__

.altmacro
.macro datafile x y
  .globl \x&_begin
\x&_begin:
  .incbin \y
\x&end:
  .byte 0
.endm

#else

#define DATA_FILE(_DATAFILE) \
  extern char _DATAFILE##_begin; \
  extern char _DATAFILE##_end;

#endif //__ASSEMBLER__

#endif //DATAFILE_H_INCLUDED
