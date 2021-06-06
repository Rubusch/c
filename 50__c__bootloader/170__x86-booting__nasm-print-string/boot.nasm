; taken from "50 lines of code" by Martin Splitt, 2018
; taken parts from "kernel booting process, part 1" by 0xAX, 2014
;
;
; Intel Assembler
;
;
; x86 asm: basics (32 bit)
;
; general purpose registers: ax, bx, cx, dx - each -h and -l, e.g. ah+al
; source index:              si
; destination index:         di
; stack pointer:             sp
; base pointer:              bp
;
;
; intel syntax edition and nasm
;
; nasm can produce a raw binary by itself and it uses the intel
; syntax
;
; e.g. command:
;     operation target, source     ## What? To?, From? [W.T.F.]
;
; compile via
; $ nasm -o boot.bin -f bin boot.nasm
;
; $ objdump -D -b binary -mi386 -Maddr16,data16,intel boot.bin
;
;     boot.bin:     file format binary
;
;
;     Disassembly of section .data:
;
;     00000000 <.data>:
;        0:   be 0f 7c                mov    si,0x7c0f
;        3:   b4 0e                   mov    ah,0xe
;        5:   ac                      lods   al,BYTE PTR ds:[si]
;        6:   3c 00                   cmp    al,0x0
;        8:   74 04                   je     0xe
;        a:   cd 10                   int    0x10
;        c:   eb f7                   jmp    0x5
;        e:   f4                      hlt
;        f:   41                      inc    cx
;       10:   6c                      ins    BYTE PTR es:[di],dx
;       11:   6c                      ins    BYTE PTR es:[di],dx
;       12:   20 77 6f                and    BYTE PTR [bx+0x6f],dh
;       15:   72 6b                   jb     0x82
;       17:   20 61 6e                and    BYTE PTR [bx+di+0x6e],ah
;       1a:   64 20 6e 6f             and    BYTE PTR fs:[bp+0x6f],ch
;       1e:   20 70 6c                and    BYTE PTR [bx+si+0x6c],dh
;       21:   61                      popa
;       22:   79 20                   jns    0x44
;       24:   6d                      ins    WORD PTR es:[di],dx
;       25:   61                      popa
;       26:   6b 65 73 20             imul   sp,WORD PTR [di+0x73],0x20
;       2a:   4a                      dec    dx
;       2b:   61                      popa
;       2c:   63 6b 20                arpl   WORD PTR [bp+di+0x20],bp
;       2f:   61                      popa
;       30:   20 64 75                and    BYTE PTR [si+0x75],ah
;       33:   6c                      ins    BYTE PTR es:[di],dx
;       34:   6c                      ins    BYTE PTR es:[di],dx
;       35:   20 62 6f                and    BYTE PTR [bp+si+0x6f],ah
;       38:   79 21                   jns    0x5b
;             ...
;      1fe:   55                      push   bp
;      1ff:   aa                      stos   BYTE PTR es:[di],al
;
;
;

[bits 16]                  ; use 16 bits
[org 0x7c00]               ; set start address to BIOS start

init:
        mov si, msg        ; loads the addres of 'msg' into si register
        mov ah, 0x0e       ; function teletype

print_char:
        lodsb              ; load the current byte from si into al and increment the address in si
        cmp al, 0          ; already at the end?
        je print_done      ;      -> yes, jump out
        int 0x10           ;      -> else, print via interrupt 0x10
        jmp print_char     ; loop
print_done:
        hlt                ; done

msg: db "All work and no play makes Jack a dull boy!", 0           ; explicitely terminate with '0'

times 510-($-$$) db 0      ; $  - current position
                           ; $$ - start position
dw 0xaa55                  ; boot flag for BIOS (little endian, twisted)
