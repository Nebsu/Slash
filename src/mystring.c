#include <stdlib.h>
#include <string.h>

#include "mystring.h"

// TODO: implémenter les fonctions déclarées dans mystring.h
struct string * string_new(size_t capacity) {
    struct string * s = malloc(sizeof(struct string));
    if (!s) return NULL;
    s -> data = malloc((capacity + 1) * sizeof(char));
    if (!s) {
        free(s);
        return NULL;
    } 
    s-> capacity = capacity;
    s-> length = 0;
    s-> data[0] = '\0';
    return s;
}

void string_delete(struct string * str) {
    free(str -> data);
    free(str);
}

int string_append (struct string * dest, char * src) {
    size_t len = strlen(src);
    if (len + dest->length >= dest -> capacity ) {
        dest -> data = realloc(dest -> data,2*dest -> capacity);
        if (!dest -> data) return 0;
        dest -> capacity *=2;
    }
    
    strncat(dest -> data,src,strlen(src));
    dest -> length += len;
    return 1;
}

void string_truncate (struct string * str, size_t nchars) {
    if(nchars > (str -> length) ) {
        str -> data[0] = '\0';
        str -> length = 0;
    }
    else {
        str -> data [str -> length - nchars] = '\0';
        str -> length -= nchars;
    }
}

