.global boot
.section .text.boot

boot:
    LA sp, __stack_top
    CALL kernel_main
    J .
