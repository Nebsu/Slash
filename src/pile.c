#include <stdlib.h>


#include "pile.h"

elem * elem_init(struct string * s) {
  elem * e = malloc(sizeof(elem));
  e -> path = s;
  e -> suivant = NULL;
  return e;
}

pile * pile_init() {
  pile * p = malloc(sizeof(pile));
  if (!p) return NULL;
  p->hd = NULL ;
  return p;
}

pile * empile (pile *p,elem *e) {
  if (!p) return NULL;
  if (!p -> hd) {
    p -> hd = e;
  }
  else {
    e -> suivant = p -> hd;
    p -> hd = e;
  }
  return p;
}

elem * depile (pile *p) {
  if (!p || !p -> hd ) return NULL;
  elem * r = p -> hd;
  p -> hd = p -> hd -> suivant;
  return r;
}
