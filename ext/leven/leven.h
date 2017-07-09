#ifndef LEVEN_H
#define LEVEN_H 1

#include "ruby.h"

static VALUE leven_distance(VALUE, VALUE, VALUE);

static VALUE leven_distance_utf8(const char*, const char*);

#endif /* LEVEN_H */
