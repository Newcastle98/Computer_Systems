/***********************************************************************
 * File       : <spell_checker.c>
 *
 * Author     : <Siavash Katebzadeh>
 *
 * Description:
 *
 * Date       : 08/10/18
 *
 ***********************************************************************/
// ==========================================================================
// Spell checker
// ==========================================================================
// Marks misspelled words in a sentence according to a dictionary

// Inf2C-CS Coursework 1. Task B/C
// PROVIDED file, to be used as a skeleton.

// Instructor: Boris Grot
// TA: Siavash Katebzadeh
// 08 Oct 2018

#include <stdio.h>

// maximum size of input file
#define MAX_INPUT_SIZE 2048
// maximum number of words in dictionary file
#define MAX_DICTIONARY_WORDS 10000
// maximum size of each word in the dictionary
#define MAX_WORD_SIZE 20

int read_char() { return getchar(); }
int read_int()
{
    int i;
    scanf("%i", &i);
    return i;
}
void read_string(char* s, int size) { fgets(s, size, stdin); }

void print_char(int c)     { putchar(c); }
void print_int(int i)      { printf("%i", i); }
void print_string(char* s) { printf("%s", s); }
void output(char *string)  { print_string(string); }

// dictionary file name
char dictionary_file_name[] = "dictionary.txt";
// input file name
char input_file_name[] = "input.txt";
// content of input file
char content[MAX_INPUT_SIZE + 1];
// valid punctuation marks
char punctuations[] = ",.!?";
// tokens of input file
char tokens[MAX_INPUT_SIZE + 1][MAX_INPUT_SIZE + 1];
// number of tokens in input file
int tokens_number = 0;
// content of dictionary file
char dictionary[MAX_DICTIONARY_WORDS * MAX_WORD_SIZE + 1];

///////////////////////////////////////////////////////////////////////////////
/////////////// Do not modify anything above
///////////////////////////////////////////////////////////////////////////////
// You can define your global variables here!
int marks[2049];

void fillMarks() {//inicializes marks[] as 0s
    int i;
    for (i = 0; i<2048; i++){
        marks[i] = 0;
    }
}
// Task B
//if t is Uppercase char, return a lowercase version
//doesn't modify original
char toLower(char t){
    if(t>64 && t<91){
        char t1;
        t1 = t+32;    //make it lowercase ascii
        return t1;
    }
    return t;
}
int isChar(char t){      //checks if t is char, returs 1(true) or 0(false)
    if((t>64 && t<91) || (t>96 && t<123)){
        return 1;
    }
    return 0;
}
//iterate through the whole dictionary for one token
//mark and return if not found, return if found
void dict_checker(int k) {
    int d_idx;     //dict index
    d_idx = 0;     //reset when function is called
    int char_idx;  //char index
    char d;        //dictionary char
    //d = dictionary[d_idx];
    char t;        //token char
    
    do{      // word by word, \n between words
        
        char_idx = 0; //reset char index when we take a new word
        
        if (d_idx>0 && d!='\n'){ //set d_idx to the next newLine in the dictionary
            do{         //doesnt change d_idx if is already a '\n'
                d_idx++;
                d = dictionary[d_idx];
                if (d=='\n'){
                    break;
                }
            }while (1);
        }
        if (d=='\n') { //set d_idx to first char of next word
            d_idx++;
        }
        
        do{//char by char comparison
            //d ends in \n
            //t ends in \0
            t = tokens[k][char_idx]; //tokens char under inspection
            t = toLower(t);          //formatting char if needed
            if((!isChar(t)) && (t!='\0')){          //returns f on non-char, we are not concerned
                return;
            }
            d = dictionary[d_idx];
            if(d=='\0'){//check for end of file
                marks[k] = 1;  //marks token as dictionary ended without a match
                //printf("Marked\n");
                return;
            }
                
            if(d=='\n'){    //end of dictionary word
                if(t=='\0'){  //'\n' && '\0' means equal length, and equal characters
                    //printf("Found a match%c",d);
                    return;     //return f as we found a match
                }else{   //next dict word
                    break;
                }
            }
            if(t=='\0'){ //dict word is longer(d!='\n' yet), so next dict word
                //break;   //increment index to move to next word
                //printf("%c", d);
                break;
            }
            if(d!=t){   //next word if chars are not equal ()
                break;
            }
            char_idx++;  //none of the above, so move to next char
            d_idx++;
        }while(1); //dict_char = token_char
        if (d_idx==0) {
            d_idx++;
        }
        
    }while(1);//until we reach he end of dictionary
}
void spell_checker(){ //feeds every token to dict_checker and
    int a;
    for (a = 0; a<=tokens_number; a++){
        dict_checker(a);//marks fed words if they are not in dict=misspelled
    }
    return;
}

// Task B
//prints every token,
// if marked, put '_' around them
void output_tokens() {
    int count;
    count = 0;
    do{
        char* print;
        print = tokens[count];
        if (marks[count]){
            printf("_%s_", print);
        }else{
            printf("%s", print);
        }
        count+=1;
    }while(count<tokens_number);
    return;
}

//---------------------------------------------------------------------------
// Tokenizer function
// Split content into tokens
//---------------------------------------------------------------------------
void tokenizer(){
    char c;
    
    // index of content
    int c_idx = 0;
    c = content[c_idx];
    do {
        
        // end of content
        if(c == '\0'){
            break;
        }
        
        // if the token starts with an alphabetic character
        if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            
            int token_c_idx = 0;
            // copy till see any non-alphabetic character
            do {
                tokens[tokens_number][token_c_idx] = c;
                
                token_c_idx += 1;
                c_idx += 1;
                
                c = content[c_idx];
            } while((c >= 'A' && c <= 'Z' )||( c >= 'a' && c <= 'z'));
            tokens[tokens_number][token_c_idx] = '\0';
            tokens_number += 1;
            
            // if the token starts with one of punctuation marks
        } else if(c == ',' || c == '.' || c == '!' || c == '?') {
            
            int token_c_idx = 0;
            // copy till see any non-punctuation mark character
            do {
                tokens[tokens_number][token_c_idx] = c;
                
                token_c_idx += 1;
                c_idx += 1;
                
                c = content[c_idx];
            } while(c == ',' || c == '.' || c == '!' || c == '?');
            tokens[tokens_number][token_c_idx] = '\0';
            tokens_number += 1;
            
            // if the token starts with space
        } else if(c == ' ') {
            
            int token_c_idx = 0;
            // copy till see any non-space character
            do {
                tokens[tokens_number][token_c_idx] = c;
                
                token_c_idx += 1;
                c_idx += 1;
                
                c = content[c_idx];
            } while(c == ' ');
            tokens[tokens_number][token_c_idx] = '\0';
            tokens_number += 1;
        }
    } while(1);
}
//---------------------------------------------------------------------------
// MAIN function
//---------------------------------------------------------------------------

int main (void)
{
    
    
    /////////////Reading dictionary and input files//////////////
    ///////////////Please DO NOT touch this part/////////////////
    int c_input;
    int idx = 0;
    
    // open input file
    FILE *input_file = fopen(input_file_name, "r");
    // open dictionary file
    FILE *dictionary_file = fopen(dictionary_file_name, "r");
    
    // if opening the input file failed
    if(input_file == NULL){
        print_string("Error in opening input file.\n");
        return -1;
    }
    
    // if opening the dictionary file failed
    if(dictionary_file == NULL){
        print_string("Error in opening dictionary file.\n");
        return -1;
    }
    
    // reading the input file
    do {
        c_input = fgetc(input_file);
        // indicates the the of file
        if(feof(input_file)) {
            content[idx] = '\0';
            break;
        }
        
        content[idx] = c_input;
        
        if(c_input == '\n'){
            content[idx] = '\0';
        }
        
        idx += 1;
        
    } while (1);
    
    // closing the input file
    fclose(input_file);
    
    idx = 0;
    
    // reading the dictionary file
    do {
        c_input = fgetc(dictionary_file);
        // indicates the end of file
        if(feof(dictionary_file)) {
            dictionary[idx] = '\0';
            break;
        }
        
        dictionary[idx] = c_input;
        idx += 1;
    } while (1);
    
    // closing the dictionary file
    fclose(dictionary_file);
    //////////////////////////End of reading////////////////////////
    ////////////////////////////////////////////////////////////////
    fillMarks();
    tokenizer();
    
    spell_checker();
    
    output_tokens();
    /*char k;
     int c;
     c = 0;
     do{
     char k = dictionary[c++];
     printf("%c", k);
     }while(c<100);*/
    return 0;
}
