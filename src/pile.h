#ifndef PILE_H
#define PILE_H

struct elem {
  struct string * path;
  struct elem * suivant;
}typedef elem;

struct pile {
  elem * hd;
} typedef pile;



elem * elem_init(struct string * s);
pile * pile_init();
pile * empile (pile *p,elem *e);
elem * depile (pile *p);  
#endif