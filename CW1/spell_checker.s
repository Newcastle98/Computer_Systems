
#=========================================================================
# Spell checker 
#=========================================================================
# Marks misspelled words in a sentence according to a dictionary
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
dictionary_file_name:   .asciiz  "dictionary.txt"
newline:                .asciiz  "\n"
        
#-------------------------------------------------------------------------
# Global variables in memory
#-------------------------------------------------------------------------
# 
content:                .space 2049     # Maximun size of input_file + NULL
.align 4                                # The next field will be aligned
dictionary:             .space 200001   # Maximum number of words in dictionary *
                                        # maximum size of each word + NULL
#copyed from tokenizer
tokens_number:          .word 0
tokens:                 .space 411849 #from piazza question
newLine:                .asciiz  "\n"
punctuations:           .asciiz ",.!?"
space:                  .asciiz " "
null:                   .ascii  "\0"
#new ones
marks:                  .space 2049
# You can add your data here!
        
#=========================================================================
# TEXT SEGMENT  
#=========================================================================
.text
######################
#copyed from tokenizer
#####################
    
addnewline:          #saves a newline, as a token has just ended
     addi $a1, $0, 0               # 92 = "\0"
     sb $a1, tokens($s0)              #store  "\0"
     addi $s7,$s7, 1            #increment $s0 by 1
     jr $ra
startnewtoken:    #increment tokens_number as we just started a new token
     lw $a1, tokens_number
     addi $a1,$a1, 1
     sw $a1, tokens_number
     mul $s7, $a1, 201    #index to save to, cell num 0 so we dont add it
     jr $ra
#############
# end of copy
#############     

#new functions
#1
initialize_marks:
    li $s1, 0   #initialize variable
    li $s2, 2047   #idx2048 is space for '\0'
loop1:
    sb $zero, marks($s1)   #saves one byte
    beq $s2, $s1, end1   #if counter = 2047, we are finished
    addi $s1, $s1, 1
    j loop1
end1:
    jr $ra    #back to call
#2   
toLower:  #changes uppercase char to lowercase
          #argument:$a0(char)
          #use: $a1(compare char)
    addi $a1, $zero, 65
    blt $a0, $a1, donothing     #donothing as char < 'A' so its not a char
    addi $a1, $zero, 90
    bgt $a0, $a1, donothing  #change to lowercase as char >= 'A' && char <= 'Z'
change:
    addi $a0, $a0, 32
donothing: 
    jr $ra    
#3
isCharorEoF:  #check if $a0 is char, argument: $a0(char), variables:  $a1(compare char),$a3(return value
    beq $a0, $zero, true
    addi $a1, $zero, 65
    blt $a0, $a1, false     #false as c < 'A' so its not a char
    addi $a1, $zero, 90
    ble $a0, $a1, true      #true as c >= 'A' && c <= 'Z'
    addi $a1, $zero, 97
    blt $a0, $a1, false     #false as c > 'Z' && c < 'a' so its not a char
    addi $a1, $zero, 122
    ble $a0, $a1, true      #true as c >= 'a' && c <= 'z'
true:
    li $a3, 1   #return one for true
    jr $ra
false:
    li $a3, 0   #return 0 for false
    jr $ra
#4    
output_tokens: #can use all registers, as we only use it at the end of the program
    lw $t0,  tokens_number   #number of tokens in file
    beq $t0, $zero, endofprint #if no tokens, dont print anything
    li $s0, 0    #loop counter for printing
markedprintloop: 
    lw $t1, tokens_number      #load total num of tokens
    #addi $t1,$t1, 2
    bgt $s0, $t1,  endofprint   #end printing if we reach last token
    lb $t0, marks($s0)
    beq $t0 , $zero, normalprint #not marked, so it is correct, print normally
    #li $v0, 1
    #add $a0, $t0,$zero
    #syscall
    li $v0, 11           #print char
    li $a0, 95           #underscore ascii
    syscall
    la $a0, tokens       # tokens[]'s address
    mul $a1, $s0, 201    # sum = current token_number*MaxTokenLength(201)
    add $a0, $t2, $a1    # address of current token($a0) = address+sum
    li $v0, 4
    la $a0, tokens($a1)  #load from address of curent token
    syscall
    li $v0, 11           #print char
    li $a0, 95           #underscore ascii
    syscall
    addi $s0, $s0,1      #increment counter
    j markedprintloop
normalprint:
    la $a0, tokens       # tokens[]'s address
    mul $a1, $s0, 201    # sum = current token_number*MaxTokenLength(201)
    add $a0, $t2, $a1    # address of current token($a0) = address+sum
    li $v0, 4
    la $a0, tokens($a1)  #load from address of curent token
    syscall
    addi $s0, $s0,1      #increment counter
    j markedprintloop
endofprint:
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
        sb   $0,  content($t0)          # content[idx] = '\0'

        # Close the file 

        li   $v0, 16                    # system call for close file
        move $a0, $s0                   # file descriptor to close
        syscall                         # fclose(input_file) 

        # opening file for reading

        li   $v0, 13                    # system call for open file
        la   $a0, dictionary_file_name  # input file name
        li   $a1, 0                     # flag for reading
        li   $a2, 0                     # mode is ignored
        syscall                         # fopen(dictionary_file, "r")
        
        move $s0, $v0                   # save the file descriptor 

        # reading from file just opened

        move $t0, $0                    # idx = 0

READ_LOOP2:                             # do {
        li   $v0, 14                    # system call for reading from file
        move $a0, $s0                   # file descriptor
                                        # dictionary[idx] = c_input
        la   $a1, dictionary($t0)       # address of buffer from which to read
        li   $a2,  1                    # read 1 char
        syscall                         # c_input = fgetc(dictionary_file);
        blez $v0, END_LOOP2             # if(feof(dictionary_file)) { break }
        lb   $t1, dictionary($t0)                             
        beq  $t1, $0,  END_LOOP2        # if(c_input == '\0')
        addi $t0, $t0, 1                # idx += 1
        j    READ_LOOP2
END_LOOP2:
        sb   $0,  dictionary($t0)       # dictionary[idx] = '\0'

        # Close the file 

        li   $v0, 16                    # system call for close file
        move $a0, $s0                   # file descriptor to close
        syscall                         # fclose(dictionary_file)
#------------------------------------------------------------------
# End of reading file block.
#------------------------------------------------------------------
######################
#copyed from tokenizer
#####################
TOKENIZER:            #Split content into tokens   in use: $s0(c_idx),$s1(c),$s2(loopcounter),$s3(tokencharcount)
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
charl:
     addi $s3, $s3, 1     #tokencharcounter incremented by 1
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
     sb $s1, tokens($s7)
     addi $s7,$s7, 1               #increment $s0 by 1
     j tokenloop
end:


#############
# end of copy
############# 

jal initialize_marks
#gets a token index($s7), checks if it is in the dictionary 
#marks and returns if not found, just returns if found
SPELL_CHECKER:
    li $s5, 0     #main couter for dict_checker usage
markloop:
    lw $t1, tokens_number
    la $t2, tokens
    bgt $s5, $t1 S_C_END #if counter==maxtokennumber, next one would be out of the array so finish
    mul $s7, $s5, 201    # sum = token_number*MaxTokenLength(201)
DICT_CHECKER:
    li $s0, 0   #d_idx = index of the dictionary letter
    #  $s1 = index of the char in words
    li $s2, 0   #char d = dictionary[d_idx], 
    li $s3, 0   #char t = tokens($s7(sartaddress)+$s1(char_idx)]
    la $s6, tokens($s7)  #full address of token with address $s7
WORDLOOP:
    li $s1, 0   #char_idx = index of the char in words, reset at new word
    beq $s0, $zero, CHARLOOP   #start of every program can skip few checks
    li $t1, 10  #loads newline char
    lb, $s2, dictionary($s0) 
    beq $t1, $s2, atnewline #no change if already at newline, so we can check next word
    addi $s0, $s0, 1        # increment d_idx
    j WORDLOOP       
atnewline:     #set char d to first letter after newline
    bne $t1, $s2, CHARLOOP 
    addi $s0, $s0, 1        # increment d_idx
CHARLOOP:
    add $a1, $s6, $s1        #token_address +char_idx = address of char we want at the moment
    lb $s3, ($a1)            #store byte from address
    add $a0, $zero,$s3       #pass char to function argument
    jal toLower              #check-function
    add $s3, $zero, $a0      #save in case of change
    jal isCharorEoF          #check function 2, returns 0/1 in $a3
    beq $zero, $a3, END      #if not char,or '\0'($a3=0) we are not interested, so return
    lb, $s2, dictionary($s0)   
    bne $s2, $zero, notend   #checks for end of file char in dictionary
    li $a0, 1
    sb $a0, marks($s5)       #marks token
    j END                    #returns f
notend:
    
    li $t1, 10               #load a newline
charOrEndofToken:
    bne $t1, $s2, NotEndofWord #checks if its the end of the dictionary word marked with '\n'
    bne $s3, $zero, NotEndofToken # checks for end of token(while if d='\n')
    j END             #word FOUND so go to end
NotEndofToken:
    bne $s0, $zero, WORDLOOP  #make sure d_idx($s0)>0
    li $s0, 1                 #set d_idx=1
    j WORDLOOP                #go to next word, as token is longer than dictionary word
NotEndofWord:
    bne $s3, $zero, NotEndofToken2 # checks for end of token in general
    bne $s0, $zero, WORDLOOP  #make sure d_idx($s0)>0
    li $s0, 1                 #set d_idx=1
    j WORDLOOP                #go to next word, as token is longer than dictionary word
NotEndofToken2:
    beq $s3, $s2, CharsEqual 
    bne $s0, $zero, WORDLOOP  #make sure d_idx($s0)>0
    li $s0, 1                 #set d_idx=1
    j WORDLOOP 
CharsEqual:     #the original aim, find equal chars
    addi $s0, $s0,1     #increment d_idx($s0)
    addi $s1, $s1,1     #increment char_index
    j CHARLOOP
END:
    addi $s5, $s5, 1    #increment SPELL_CHECKER's counter
    j markloop
S_C_END:

jal output_tokens  #print tokens with marks where needed

#------------------------------------------------------------------
# Exit, DO NOT MODIFY THIS BLOCK
#------------------------------------------------------------------
main_end:      
        li   $v0, 10          # exit()
        syscall

#----------------------------------------------------------------
# END OF CODE
#----------------------------------------------------------------
