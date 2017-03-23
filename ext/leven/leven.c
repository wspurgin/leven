#include <string.h>
#include <stdlib.h>

#include "utf8.h"
#include "leven.h"

VALUE rb_mLeven;

void
Init_leven(void)
{
  rb_mLeven = rb_define_module("Leven");
  rb_define_singleton_method(rb_mLeven, "distance", leven_distance, 2);
}

# define __leven_eq(x, y) ((x) == (y))
# define __leven_min(x, y) ((x) < (y) ? (x) : (y))

static VALUE leven_distance(VALUE klass, VALUE rb_obj1, VALUE rb_obj2) {
  const char *raw_word1 = StringValueCStr(rb_obj1),
        *raw_word2 = StringValueCStr(rb_obj2);

  // Convert words to UTF-8
  unsigned int utf8_len1 = u8_strlen(raw_word1),
      utf8_len2 = u8_strlen(raw_word2);
  u_int32_t *word1_buffer, *word2_buffer, *word1, *word2;

  word1_buffer = calloc(utf8_len1, sizeof(u_int32_t));
  word2_buffer = calloc(utf8_len2, sizeof(u_int32_t));

  word1 = word1_buffer;
  word2 = word2_buffer;

  u8_toucs(word1_buffer, utf8_len1+1, raw_word1, -1);
  u8_toucs(word2_buffer, utf8_len2+1, raw_word2, -1);

  unsigned int len1 = utf8_len1,
         len2 = utf8_len2;
  u_int32_t *v = calloc(len2 + 1, sizeof(u_int32_t));
  unsigned int i, j, current, next, cost;

  /* strip common prefixes */
  while (len1 > 0 && len2 > 0 && __leven_eq(word1[0], word2[0]))
    word1++, word2++, len1--, len2--;

  /* handle degenerate cases */
  if (!len1) {
    free(word1_buffer);
    free(word2_buffer);
    free(v);
    return INT2FIX(len2);
  }

  if (!len2) {
    free(word1_buffer);
    free(word2_buffer);
    free(v);
    return INT2FIX(len1);
  }

  /* initialize the column vector */
  for (j = 0; j < len2 + 1; j++)
    v[j] = j;

  for (i = 0; i < len1; i++) {
    /* set the value of the first row */
    current = i + 1;
    /* for each row in the column, compute the cost */
    for (j = 0; j < len2; j++) {
      /*
       * cost of replacement is 0 if the two chars are the same, or have
       * been transposed with the chars immediately before. otherwise 1.
       */
      cost = !(__leven_eq(word1[i], word2[j]) || (i && j &&
            __leven_eq(word1[i-1], word2[j]) &&
            __leven_eq(word1[i],word2[j-1])));
      /* find the least cost of insertion, deletion, or replacement */
      next = __leven_min(__leven_min( v[j+1] + 1,
            current + 1 ),
          v[j] + cost );
      /* stash the previous row's cost in the column vector */
      v[j] = current;
      /* make the cost of the next transition current */
      current = next;
    }
    /* keep the final cost at the bottom of the column */
    v[len2] = next;
  }
  free(word1_buffer);
  free(word2_buffer);
  free(v);
  return INT2FIX(next);
}

