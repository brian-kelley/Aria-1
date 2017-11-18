#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"
#include "ctype.h"

typedef struct
{
  int opcode;
  int n1;     //first nibble of arguments
  int n2;     //second nibble
  char* label;  //label used as imm8 (resolved after processing whole program)
} Instruction;

Instruction program[256];

char mne[16][8] = {
  "set", "lod", "str", "cpy",
  "jl", "je", "addi", "sel",
  "add", "sub", "not", "and",
  "or", "xor", "shl", "shr"};

typedef struct
{
  char name[16];
  int addr;
} Label;

//Support one label per address (max practical number)
Label labels[256];

//Get register code
int getReg(char* c)
{
  if(c[0] == 'f' && c[1] == 'l')
    return 14; //flags
  if(c[0] == 'i')
    return 15; //ip
  //all others: some character relative to a
  return c[0] - 'a';
}

//Read in a line (up to 79 characters)
void safeGets(char* line)
{
  int i = 0;
  line[79] = 0;
  while(i < 79 && !feof(stdin))
  {
    char c = getchar();
    if(c == '\n' || c == EOF)
    {
      break;
    }
    line[i++] = c;
  }
  line[i] = 0;
}

int main(int argc, const char** argv)
{
  //Program length
  int length = 0;
  int numLabels = 0;
  while(!feof(stdin))
  {
    char line[80];
    safeGets(line);
    int len = strlen(line);
    //empty line signals end of file
    if(len == 0)
      break;
    //can have comment line
    if(line[0] == ';')
      continue;
    if(line[len - 1] == ':')
    {
      //Add new label
      line[len - 1] = 0;
      strcpy(labels[numLabels].name, line);
      labels[numLabels].addr = length;
      numLabels++;
    }
    else
    {
      if(length == 256)
      {
        puts("Program too long to fit in ROM.");
        exit(1);
      }
      //get opcode
      len = 0;
      while(isalpha(line[len]))
        len++;
      line[len] = 0;
      bool foundOpcode = false;
      for(int i = 0; i < 16; i++)
      {
        if(strcmp(mne[i], line) == 0)
        {
          //emit instruction
          program[length].opcode = i;
          foundOpcode = true;
          break;
        }
      }
      if(!foundOpcode)
      {
        printf("Unknown mnemonic %s.\n", line);
        exit(1);
      }
      char* op = line + len + 1;
      program[length].label = NULL;
      switch(program[length].opcode)
      {
        case 0:
        case 4:
        case 5:
        {
          //get imm8 (label or integer)
          if(isdigit(op[0]) || op[0] == '-')
          {
            //integer operand
            unsigned imm8 = atoi(op);
            program[length].n1 = (imm8 & 0xF0) >> 4;
            program[length].n2 = imm8 & 0xF;
          }
          else
          {
            //label operand (save label text for later resolution)
            char* end = op;
            while(isalpha(*end))
              end++;
            *end = 0;
            program[length].label = malloc(strlen(op) + 1);
            strcpy(program[length].label, op);
          }
          break;
        }
        case 6:
        {
          //get reg, then imm4
          program[length].n1 = getReg(op);
          //scan ahead to signed integer
          while(*op != '-' && !isdigit(*op))
            op++;
          //set imm4 (signed 4-bit value)
          program[length].n2 = ((unsigned) atoi(op)) & 0xF;
          break;
        }
        default:
        {
          //get two reg operands
          program[length].n1 = getReg(op);
          while(isalpha(*op))
            op++;
          while(!isalpha(*op))
            op++;
          program[length].n2 = getReg(op);
        }
      }
      length++;
    }
  }
  //Have all instructions
  //Resolve labels (set imm8 arguments as label addresses),
  //then emit machine code
  //Always emit hex to stdout, and if output filename is provided, write binary also
  for(int i = 0; i < length; i++)
  {
    char* label = program[i].label;
    if(label)
    {
      bool resolved = false;
      for(int j = 0; j < numLabels; j++)
      {
        if(strcmp(labels[j].name, label) == 0)
        {
          resolved = true;
          int addr = labels[j].addr;
          program[i].n1 = (addr & 0xF0) >> 4;
          program[i].n2 = addr & 0xF;
          break;
        }
      }
      if(!resolved)
      {
        printf("Unresolved label %s.\n", label);
        exit(1);
      }
    }
    //Emit instruction
    printf("%x%x%x ", program[i].opcode, program[i].n1, program[i].n2);
    if(i % 16 == 15)
      putchar('\n');
  }
  putchar('\n');
  return 0;
}

