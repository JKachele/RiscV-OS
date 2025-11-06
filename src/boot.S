.global boot
.section .text.boot

boot:
    la sp, __stack_top
    call kernel_main
    j .
