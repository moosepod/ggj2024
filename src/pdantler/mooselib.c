#include "mooselib.h"

// Max length of string for compare -- past this point will just return not
// equal
#define MAX_STREQ_LENGTH 32000

void mlib_strcpy(char *destination, const char *source, int max_buffer_length) {
  int i = 0;
  for (i = 0; i < max_buffer_length; i++) {
    if (source[i] == '\0') {
      break;
    }

    destination[i] = source[i];
  }
  destination[i] = '\0';
}

bool mlib_streq(const char *s1, const char *s2) {
  if (!s1 && !s2) {
    // If both strings are null, count as equal
    return true;
  }

  if (!s1 || !s2) {
    return false;
  }

  for (int i = 0; i < MAX_STREQ_LENGTH; i++) {
    if (s1[i] != s2[i]) {
      return false;
    }

    if (s1[i] == '\0' && s2[i] == '\0') {
      break;
    }
  }

  return true;
}
