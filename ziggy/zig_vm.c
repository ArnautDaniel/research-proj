#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

// A Zig Stack Item is simply a pointer to a character string
// holding an instruction that has to be parsed

struct zig_stack_item {
  int time;
  int tag; 
  char* data;
};

// A Zig stack is self explanatory
struct zig_stack {
  struct zig_stack_item* data;
  struct zig_stack_item* parent;
  struct zig_stack_item* child;
};

//The Zig_VM is two int registers, two float registers, a zig_stack
//and the current instruction being pointed at.
struct zig_vm {
  struct zig_stack_item* ip;
  int xp;
  int yp;
  float zf;
  float pf;
  struct zig_stack* stack;
  int current_tag;
  int current_time;
  char* name;
  int type;
};

struct zig_vm* return_zig_vm(){
  struct zig_vm *vm = calloc(1, sizeof(struct zig_vm));
  return vm;
}

struct zig_stack* return_zig_stack(){
  struct zig_stack *stack = calloc(1, sizeof(struct zig_stack));
  return stack;
}

struct zig_vm* create_object_from_template(char* template, struct template_bank* bank){
  struct template* find_template_in_bank(template, bank);
int main (int argc, char** argv){
  struct zig_stack* stack;
  struct zig_vm* vm = return_zig_vm();
  vm->stack = return_zig_stack();
  printf("Made first Zig_VM with xp: %d\n", vm->xp);
  return 0;
}

/*
Some VM instructions:
     DEPEND JOIN SPLIT ADD MULTIPLY SUBTRACT DIVIDE
     APPEND REVERSE ALPHA BETA TIME STACKTO SPLIT
     SUBSEQ DUPINTO REPLACE DESTROY!! CONVERTTO
     OUTPUT STRING-NTH EXPT PRIORITY +
*/




