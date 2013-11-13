#include<stdio.h>
#include<stdarg.h>
#define PROGRAM_LEN 100
#define STACK_SIZE 100
#define MEMORY_SIZE 1000
#define OPE_KIND 20

typedef signed char BYTE;
typedef signed int WORD;
typedef signed long LONG_WORD;


WORD eax,ebx,ecx,edx,esp,ebp,esi,eip;//general register
BYTE CF,ZF,SF,OF;//eflags

//operations
WORD CALL(WORD (*p)(const char*,...),WORD* args);//function pointer
WORD MOV(WORD* x,WORD* y);
WORD MOVI(WORD* x,WORD num);
WORD JUMP(WORD* x);
WORD PUSH(WORD* x);
WORD POP();
WORD NOP();
WORD ADD(WORD* x,WORD* y);
WORD SUB(WORD* x,WORD* y);
WORD INC(WORD* x);
WORD DEC(WORD* x);
WORD LEA(WORD* x,WORD* y);
WORD AND(WORD* x,WORD* y);
WORD OR(WORD* x,WORD* y);
WORD XOR(WORD* x,WORD* y);
WORD JZ(WORD* x);
WORD JNZ(WORD* x);
WORD IDIVL(WORD* x,WORD* y);

void RUN_PROGRAM(const WORD*);

WORD sign[OPE_KIND];
WORD STACK[STACK_SIZE];
WORD MEMORY[MEMORY_SIZE];

void Debug(){
  printf("eax:%d ebx:%d ecx:%d edx:%d esp:%d esi:%d eip:%d\n",eax,ebx,ecx,edx,esp,esi,eip);
}
int main(void){
  sign[0] = &NOP,sign[1] = &CALL,sign[2] = &MOV; sign[3] = &JUMP; sign[4] = &PUSH;
  sign[5] = &POP,sign[6] = &ADD,sign[7] = &SUB; sign[8] = &INC; sign[9] = &DEC;
  sign[10] = &LEA; sign[11] = &MOVI; sign[12] = &AND; sign[13] = &OR;
  sign[14] = &XOR; sign[15] = &JZ; sign[16] = &JNZ; sign[17] = &IDIVL;

  //Code------
  const WORD PROGRAM[] = {11,2,&eax,&MEMORY[ebp],
			  4,1,&eax,
  			  11,2,&eax,1,
  			  1,2,&scanf,&eax,
			  5,0,
			  2,2,&ebx,&MEMORY[ebp],
			  4,1,&ebx,
			  11,2,&eax,1,
			  5,0,
			  11,2,&ecx,2,
			  11,2,&MEMORY[ebp + 6],0,
			  2,2,&ebx,&ecx,
			  4,1,&ecx,
			  2,2,&MEMORY[ebp + 1],&ecx,
			  11,2,&MEMORY[ebp + 2],60,
			  11,2,&MEMORY[ebp + 3],0,
			  17,2,&MEMORY[ebp + 1],&MEMORY[ebp + 2],
			  2,2,&MEMORY[ebp + 3],&edx,
			  11,2,&edx,0,
			  14,2,&MEMORY[ebp + 3],&edx,
			  2,2,&MEMORY[ebp + 1],&MEMORY[ebp + 2],
			  2,2,&MEMORY[ebp + 2],&MEMORY[ebp + 3],
			  11,2,&eax,57,
			  16,1,&eax,
			  2,2,&MEMORY[ebp + 4],&MEMORY[ebp + 1],
			  11,2,&MEMORY[ebp + 5],1,
			  2,2,&MEMORY[ebp + 7],&MEMORY[ebp + 4],
			  14,2,&MEMORY[ebp + 7],&MEMORY[ebp + 5],
			  11,2,&eax,114,
			  16,1,&eax,
			  8,1,&MEMORY[ebp + 6],
			  14,2,&ebx,&MEMORY[ebp],
			  5,0,
			  8,1,&ecx,			  
			  11,2,&edx,38,
			  16,1,&edx,
			  4,1,&MEMORY[ebp + 6],
			  11,2,&esi,1,
			  1,2,&printf,&esi,
			  5,0,
			  -1};

  //------------  
  RUN_PROGRAM(PROGRAM);
}

void RUN_PROGRAM(const WORD* PROGRAM){
  eip = 0; esp = 0,ebp = 0;
  WORD word;
  for(word = PROGRAM[eip]; word != -1; word = PROGRAM[eip]){
    if(PROGRAM[eip + 1] == 0){
      WORD (*p)() = sign[word];
      (*p)();
    }else if(PROGRAM[eip + 1] == 1){
      WORD (*p)(WORD) = sign[word];
      (*p)(PROGRAM[eip + 2]);
    }else if(PROGRAM[eip + 1] == 2){
      WORD (*p)(WORD,WORD) = sign[word];
      (*p)(PROGRAM[eip + 2],PROGRAM[eip + 3]);
    }
  }
}

WORD CALL(WORD (*p)(const char*,...),WORD* args){
  switch(*args){
  case 1:
    eax = (*p)("%d",STACK[esp - 1]);
    break;
  case 2:
    eax = (*p)("%d %d",STACK[esp - 1],STACK[esp - 2]);
    break;
  }
  eip += 4;
  return eip;
}

WORD MOV(WORD* x,WORD* y){ *x = *y; eip += 4; return eip; }
WORD MOVI(WORD* x,WORD num){ *x = num; eip += 4; return eip; }
WORD JUMP(WORD* x){  eip = *x; return eip; }
WORD PUSH(WORD* x){  STACK[esp++] = *x; eip += 3; return eip; }
WORD POP(){  eax = STACK[--esp];eip += 2;return eip; }
WORD ADD(WORD* x,WORD* y){  *x += *y; eip += 4; return eip; }
WORD SUB(WORD* x,WORD* y){  *x -= *y; eip += 4; return eip; }
WORD INC(WORD* x){  (*x)++; eip += 3; return eip; }
WORD DEC(WORD* x){  (*x)--; eip += 3; return eip; }
WORD LEA(WORD* x,WORD* y){  *x = (WORD)y; eip += 4; return eip; }
WORD NOP(){  eip += 2; return eip; }
WORD AND(WORD* x,WORD* y){ *x &= *y; ZF = (*x == 0); eip += 4; return eip; }
WORD OR(WORD* x,WORD* y){ *x |= *y; ZF = (*x == 0); eip += 4; return eip; }
WORD XOR(WORD* x,WORD* y){ *x ^= *y; ZF = (*x == 0); eip += 4; return eip; }
WORD JZ(WORD* x){ eip += 3; if(ZF) JUMP(x); return eip;  }
WORD JNZ(WORD* x){ eip += 3; if(!ZF) JUMP(x); return eip; }
WORD IDIVL(WORD* x,WORD* y){ eax = (*x) / (*y); edx = (*x) % (*y); eip += 4; return eip; }
