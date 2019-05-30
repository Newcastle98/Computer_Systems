.globl main
.data
    hello: .asciiz "Hello,\n the str\ning is:\n"
    names: 

.text
main:

    #print the first member of the names array
    li $v0, 4
    la $a0, names
    addi $a0, $a0, 0
    syscall

    li $v0, 4
    lb $a0, hello
    #syscall
    #exit
    
    la $t0, hello
    addi $t0, $t0, 1
    addi $t2, $0, 68
    sb $t2, ($t0)
    print_char:    #void print_char(int c)     { putchar(c); }
    li $v0, 11
    addi $a0,$0, 76
    #lb $a0, ($s1)
    syscall 
    #jr $ra
    addi $a1 ,$zero, 16
    li $v0, 11
    lb $a0, hello($a1)
    syscall
    
