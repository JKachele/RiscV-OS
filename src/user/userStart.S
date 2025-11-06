.global start
.section .text.start

start:
    la sp, __stack_top
    call main
    call exit
