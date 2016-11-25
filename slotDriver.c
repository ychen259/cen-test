#include <stdlib.h>

#include <assert.h>

#include "slot.h"
#include "slot.h"

int main(){
  int x = 5;
  int y = 6;
  printf("Test make_slot function\n");
  struct slot setSlot = make_slot(x,y);
  printf("Expected result is\n");
  printf("x: %i\ty: %i\n", x, y);
  printf("Actual value is \n");
  printf("x: %i\ty: %i\n", setSlot.x, setSlot.y);
  assert(setSlot.x == x);
  assert(setSlot.y == y);
  printf("function make_slot works fine\n\n");

  struct slot slotX = make_slot(1, 2);
  struct slot slotY = make_slot(2, 3);
  printf("Test function compare_slots\n");
  printf("Expected result of compare_slots(x, y): -1\n");
  printf("Actual value: %i\n", compare_slots(slotX,slotY));
  assert(compare_slots(slotX,slotY) == -1);
  printf("function compare_slots works fine\n");
}
