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
  // TODO Safely determine if we can use a string or array pointer here
  const char *raw_word1 = StringValueCStr(rb_obj1),
        *raw_word2 = StringValueCStr(rb_obj2);
  unsigned int str_len1 = strlen(raw_word1),
    str_len2 = strlen(raw_word2);
  return leven_distance_utf8(raw_word1, raw_word2);
}

static VALUE leven_distance_utf8(const char *raw_word1, const char *raw_word2) {

  unsigned int utf8_len1 = u8_strlen(raw_word1),
      utf8_len2 = u8_strlen(raw_word2);
  unsigned int len1 = utf8_len1,
         len2 = utf8_len2;

  // degenerate cases
  if (strcmp(raw_word1, raw_word2) == 0)
    return INT2FIX(0);
  if (len1 == 0)
    return INT2FIX(len2);
  if (len2 == 0)
    return INT2FIX(len1);

  // Convert words to UTF-8
  u_int32_t *word1, *word2;

  word1 = ALLOC_N(u_int32_t, utf8_len1+1);
  word2 = ALLOC_N(u_int32_t, utf8_len2+1);

  u8_toucs(word1, utf8_len1+1, raw_word1, -1);
  u8_toucs(word2, utf8_len2+1, raw_word2, -1);
  unsigned int i, j, cost;

  unsigned int *v0 = ALLOC_N(u_int32_t, len2 + 1);
  unsigned int *v1 = ALLOC_N(u_int32_t, len2 + 1);
  unsigned int *swap;

  for (i = 0; i < len2; i++)
    v0[i] = i;

  for (i = 0; i < len1; i++)
  {
    swap = v1;
    v1 = v0;
    v0 = swap;
    // calculate current row distances from the previous row

    //   edit distance is delete (i+1) chars from word1 to match empty word2
    v1[0] = i + 1;

    for (j = 0; j < len2; j++)
    {
      cost = (word1[i] == word2[j]) ? 0 : 1;
      v1[j + 1] = __leven_min(
          __leven_min(v1[j] + 1, v0[j + 1] + 1),
          v0[j] + cost
          );
    }
  }

  unsigned int dist = v1[len2];

  xfree(word1);
  xfree(word2);
  xfree(v0);
  xfree(v1);

  return INT2FIX(dist);
}
