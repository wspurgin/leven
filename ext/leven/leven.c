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
  // Safely determine if we can use a string or array pointer here
  const char *raw_word1 = StringValueCStr(rb_obj1),
        *raw_word2 = StringValueCStr(rb_obj2);
  unsigned int str_len1 = strlen(raw_word1),
    str_len2 = strlen(raw_word2);
  if (u8_strlen(raw_word1) == str_len1 && u8_strlen(raw_word2) == str_len2)
    leven_distance_ascii(raw_word1, raw_word2);
  else
    leven_distance_utf8(raw_word1, raw_word2);
}

static VALUE leven_distance_ascii(const char* raw_word1,const char* raw_word2) {
  unsigned int len1 = strlen(raw_word1),
    len2 = strlen(raw_word2);

  return INT2FIX(0);
}

static VALUE leven_distance_utf8(const char *raw_word1, const char *raw_word2) {

  unsigned int utf8_len1 = u8_strlen(raw_word1),
      utf8_len2 = u8_strlen(raw_word2);
  unsigned int len1 = utf8_len1,
         len2 = utf8_len2;

  // degenerate cases
  if (strcmp(raw_word1, raw_word2))
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

  free(word1);
  free(word2);
  free(v0);
  free(v1);

  return INT2FIX(dist);
}

// static unsigned int levenshtein(void* word1, void* word2, const unsigned int len1, const unsigned int len2) {
//   u_int32_t *v = ALLOC_N(u_int32_t, len2 + 1);
//   unsigned int i, j, current, next, cost;

//   /* strip common prefixes */
//   while (len1 > 0 && len2 > 0 && __leven_eq(word1[0], word2[0]))
//     word1++, word2++, len1--, len2--;

//   /* handle degenerate cases */
//   if (!len1) {
//     free(v);
//     return INT2FIX(len2);
//   }

//   if (!len2) {
//     free(v);
//     return INT2FIX(len1);
//   }

//   /* initialize the column vector */
//   for (j = 0; j < len2 + 1; j++)
//     v[j] = j;

//   for (i = 0; i < len1; i++) {
//     /* set the value of the first row */
//     current = i + 1;
//     /* for each row in the column, compute the cost */
//     for (j = 0; j < len2; j++) {
//       /*
//        * cost of replacement is 0 if the two chars are the same, or have
//        * been transposed with the chars immediately before. otherwise 1.
//        */
//       cost = !(__leven_eq(word1[i], word2[j]) || (i && j &&
//             __leven_eq(word1[i-1], word2[j]) &&
//             __leven_eq(word1[i],word2[j-1])));
//       /* find the least cost of insertion, deletion, or replacement */
//       next = __leven_min(__leven_min( v[j+1] + 1,
//             current + 1 ),
//           v[j] + cost );
//       /* stash the previous row's cost in the column vector */
//       v[j] = current;
//       /* make the cost of the next transition current */
//       current = next;
//     }
//     /* keep the final cost at the bottom of the column */
//     v[len2] = next;
//   }
// }
