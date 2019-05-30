
#=========================================================================
# Tokenizer
#=========================================================================
# Split a string into alphabetic, punctuation and space tokens
# 
# Inf2C Computer Systems
# 
# Siavash Katebzadeh
# 8 Oct 2018
# 
#
#=========================================================================
# DATA SEGMENT
#=========================================================================
.data
#-------------------------------------------------------------------------
# Constant strings
#-------------------------------------------------------------------------

input_file_name:        .asciiz  "input.txt"   
newLine:                .asciiz  "\n"
inputInt:               .asciiz  "Input an integer: "
inputChar:              .asciiz  "Input a char: "
inputString:            .asciiz  "Input a string: "
punctuations:           .asciiz ",.!?"
space:                  .asciiz " "
null:                   .ascii  "\0"
        
#-------------------------------------------------------------------------
# Global variables in memory
#-------------------------------------------------------------------------
# 
content:                .space 2049     # Maximun size of input_file + NULL
string:                 .space 5
char:                   .word 'a'
int:                    .word 0
tokens_number:          .word 0
tokens:                 .space 411849 #from piazza question
c:                      .space 4 

# You can add your data here!
        
#=========================================================================
# TEXT SEGMENT  
#=========================================================================
.text
 
read_char :    #int read_char() { return getchar(); }
    li $v0, 4
    la $a0, inputChar
    syscall
    li $v0, 12
    syscall
    sw $v0, char
    li $v0, 4
    la $a0, newLine
    syscall
    jr $ra
    
read_int:            #int read_int()
    li $v0, 4        
    la $a0, inputInt  #print message
    syscall          
    li $v0, 5
    syscall         #scanf("%i", &i);
    sw $v0, int
    jr $ra
    
read_string:       #void read_string(char* s, int size)
    li $v0, 4
    la $a0, inputString
    syscall
    li $v0, 8
    la $a0, string    #save string to string
    li $a1, 5            #length of string
    syscall              #ask for string
    li $v0, 4
    la $a0, newLine
    syscall
    jr $ra
    
print_char:    #void print_char(int c)     { putchar(c); }
    li $v0, 11
    add $a0, $0, $s1
    syscall 
    jr $ra
    
print_int:     #print_int(int i)      { printf("%i", i); }
    li $v0, 1
    lw $a0, int
    syscall 
    jr $ra
    
print_string:   #print_string(char* s) { printf("%s", s); }
    li $v0, 4
    la $a0, string
    syscall 
    jr $ra
    
print_newline:
    li $v0, 4
    la $a0, newLine
    syscall 
    jr $ra
    
output_tokens:        #Prints all tokens, one per line, using "counter" variable
    lw $t0,  tokens_number   #number of tokens in file
    beq $t0, $zero, printlast #if no tokens, dont print anything                  
    addi $s0, $zero, 0    #index counter(tokens_number)
    li $s1, 0         #charcount
    li $s2, 0         #totalcount
printloop: 
    lw $t0,  tokens_number   #number of tokens in file
    beq $s0, $t0, lastloop  #checkes if its last of tokens
    mul $t0, $s0, 201
    add $s2, $s1, $t0
    li $v0, 11
    lb $a0, tokens($s2)
    beqz $a0, printnew   #jumps if we reach \0 
    syscall                   #prints one char
    addi $s1, $s1, 1          #increment $s1 by 1
    j printloop
printnew: 
    addi $s0, $s0, 1          #increment counter by one
    li $v0, 4        
    la  $a0, newLine          #print \n
    li $s1, 0                 #reset charcount
    syscall          #print newLine char
    j printloop
lastloop: 
    lw $t0,  tokens_number   #number of tokens in file
    mul $t0, $s0, 201
    add $s2, $s1, $t0
    li $v0, 11
    lb $a0, tokens($s2)
    beqz $a0, printlast   #jumps if we reach \0 
    syscall                   #prints one char
    addi $s1, $s1, 1          #increment $s1 by 1
    j lastloop
printlast:
    jr $ra                  #jumps back to where the function was called
    
    
addnewline:          #saves a newline, as a token has just ended
     addi $a1, $0, 0               # 92 = "\0"
     sb $a1, tokens($s0)              #store  "\0"
     addi $s7,$s7, 1            #increment $s0 by 1
     jr $ra
startnewtoken:    #increment tokens_number as we just started a new token
     lw $a1, tokens_number
     addi $a1,$a1, 1
     sw $a1, tokens_number
     mul $s7, $a1, 201     #index to save to, cell num 0 so we dont add it
     jr $ra

#-------------------------------------------------------------------------
# MAIN code block
#-------------------------------------------------------------------------


.globl main                     # Declare main label to be globally visible.
                                # Needed for correct operation with MARS
main:
        
#-------------------------------------------------------------------------
# Reading file block. DO NOT MODIFY THIS BLOCK
#-------------------------------------------------------------------------

# opening file for reading

        li   $v0, 13                    # system call for open file
        la   $a0, input_file_name       # input file name
        li   $a1, 0                     # flag for reading
        li   $a2, 0                     # mode is ignored
        syscall                         # open a file
        
        move $s0, $v0                   # save the file descriptor 

# reading from file just opened

        move $t0, $0                    # idx = 0

READ_LOOP:                              # do {
        li   $v0, 14                    # system call for reading from file
        move $a0, $s0                   # file descriptor
                                        # content[idx] = c_input
        la   $a1, content($t0)          # address of buffer from which to read
        li   $a2,  1                    # read 1 char
        syscall                         # c_input = fgetc(input_file);
        blez $v0, END_LOOP              # if(feof(input_file)) { break }
        lb   $t1, content($t0)          
        addi $v0, $0, 10                # newline \n
        beq  $t1, $v0, END_LOOP         # if(c_input == '\n')
        addi $t0, $t0, 1                # idx += 1
        j    READ_LOOP
END_LOOP:
        sb   $0,  content($t0)

        li   $v0, 16                    # system call for close file
        move $a0, $s0                   # file descriptor to close
        syscall                         # fclose(input_file)
#------------------------------------------------------------------
# End of reading file block.
#------------------------------------------------------------------

tokenizer:            #Split content into tokens   in use: $s0(c_idx),$s1(c),$s2(loopcounter),$s3(tokencharcount)
                      # $s4(punct.count),$s5(spacecount) 
    add $s3, $0,$0                #set them 0 to be sure
    add $s4, $0,$0
    add $s5, $0,$0
    li $s7, 0         #index to save to (not including base address)
    li $s0, -1               #  $s0 = c_idx = 0;
tokenloop:                #the big while loop, ends at "\0"
    addi $s0, $s0, 1
    lb  $s1, content($s0)         #char c = $s1
    beqz $s1, end           #if c='0' jump to end label                   
ifsForChar:               # if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z')
    addi $t1, $zero, 65
    blt $s1, $t1, ifsForPunct     #branch to if.punct if c < 'A' so its not a char
    addi $t1, $zero, 90
    ble $s1, $t1, characterToken  #branch to char as c >= 'A' && c <= 'Z'
    addi $t1, $zero, 97
    blt $s1, $t1, ifsForPunct     #branch to if.punct as c > 'Z' && c < 'a' so its not a char
    addi $t1, $zero, 122
    ble $s1, $t1, characterToken  #branch to char as c >= 'a' && c <= 'z'
ifsForPunct:             # else if(c == ',' || c == '.' || c == '!' || c == '?')
    addi $s2, $zero, 0            #$s2 = counter(0-3) for punctuation bytes
punctloop: lb $t1, punctuations($s2)          # load punctuation mark 
           beq $t1, $s1, punctuationToken     #check if equal to c
           addi $t1, $zero, 3
           beq $s2, $t1, ifsForSpace          #if $s2 = 3, we checked all punctuations, so go to space check
           addi $s2, $s2, 1                   #increment $s1 by 1 to get next punctuation       -?
           j punctloop
ifsForSpace:
    lb $t1, space
    beq $t1, $s1, spaceToken  #if anything gets this far it is a space according
                              # to input restrictions so no need for any more cases
    
characterToken:
     bnez $s4, skip3          #if not zero, save a newLine(if equal, go on)
     beqz $s5, charl            #if $s5 = 0 DON'T print newLine
skip3:
     addi $s4, $0, 0            # reset punctcount
     addi $s5, $0, 0            # reset spacecount
     jal addnewline
     jal startnewtoken
     #implement move to next tokens start
charl:
     addi $s3, $s3, 1     #tokencharcounter incremented by 1
     #la $t1, tokens         #load address
     #sb $t1, $s1
     #add $t1, $t1, $t2    #add token number
     #add $t1, $t1, $s3    #add tokencharcounter
     sb $s1, tokens($s7)
     addi $s7,$s7, 1            #increment $s0 by 1
     j tokenloop
punctuationToken:
     bnez $s3, skip4          #if not zero, save a newLine(if equal, go on)
     beqz $s5, punctl            #if $s5 = 0 DON'T print newLine
skip4:
     addi $s3, $0, 0            # reset punctcount
     addi $s5, $0, 0            # reset spacecount
     jal addnewline
     jal startnewtoken
punctl:
     addi $s4, $s4, 1     #punctcount incremented by 1
     #la $t1, tokens         #load address
     #sb $t1, $s1
     #add $t1, $t1, $t2    #add token number
     #add $t1, $t1, $s3    #add tokencharcounter
     sb $s1, tokens($s7)
     addi $s7,$s7, 1               #increment $s0 by 1
     j tokenloop
spaceToken:
     bnez $s3, skip5          #if not zero, save a newLine(if equal, go on)
     beqz $s4, spacel            #if $s4 = 0 DON'T print newLine
skip5:
     addi $s3, $0, 0            # reset charcount
     addi $s4, $0, 0            # reset punctcount
     jal addnewline
     jal startnewtoken
spacel:
     addi $s5, $s5, 1     #punctcount incremented by 1
     #la $t1, tokens         #load address
     #sb $t1, $s1
     #add $t1, $t1, $t2    #add token number
     #add $t1, $t1, $s3    #add tokencharcounter
     sb $s1, tokens($s7)
     addi $s7,$s7, 1               #increment $s0 by 1
     j tokenloop
end:
jal output_tokens
        
        
#------------------------------------------------------------------
# Exit, DO NOT MODIFY THIS BLOCK
#------------------------------------------------------------------
main_end:      
        li   $v0, 10          # exit()
        syscall

#----------------------------------------------------------------
# END OF CODE
#----------------------------------------------------------------
