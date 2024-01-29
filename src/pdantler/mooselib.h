/** This file is managed by pdantler.

    Any changes you make to it directly will be overwritten next time pdantler
   runs
*/

// Playdate-compatable stdlib replacement. Also includes some common
// structs/functions for working with points, etc

// If compiling for playdate, be sure to define MOOSELIB_PLAYDATE before
// including

#ifndef MOOSELIB_INCLUDE
#define MOOSELIB_INCLUDE
#include <math.h>
#include <stdbool.h>

//
// Graphic-related shared structs
//

typedef struct {
  int width;
  int height;
} MLIBSize;

static inline MLIBSize MLIBSIZE_CREATE(int width, int height) {
  return (MLIBSize){width, height};
}

typedef struct {
  int x;
  int y;
} MLIBPoint;

static inline MLIBPoint MLIBPOINT_CREATE(int x, int y) {
  return (MLIBPoint){x, y};
}

typedef struct {
  float x;
  float y;
} MLIBFPoint;

static inline MLIBFPoint MLIBFPOINT_CREATE(float x, float y) {
  return (MLIBFPoint){x, y};
}

typedef struct {
  int x;
  int y;
  int width;
  int height;
} MLIBRect;

static inline MLIBRect MLIBRECT_CREATE(int x, int y, int width, int height) {
  return (MLIBRect){x, y, width, height};
}

static inline MLIBRect MLIBRECT_TRANSLATE(MLIBRect r, int x, int y) {
  return MLIBRECT_CREATE(r.x + x, r.y + y, r.width, r.height);
}

static inline MLIBPoint MLIBRECT_CENTER(MLIBRect r) {
  return MLIBPOINT_CREATE(r.x + r.width / 2, r.y + r.height / 2);
}

//
// Debug macros
//

#define MLIBPOINT_EXPAND(p) (p.x), (p.y)
#define MLIBSIZE_EXPAND(s) (s.width), (s.height)
#define MLIBRECT_EXPAND(r) (r.x), (r.y), (r.width), (r.height)
//
// Utility functions
//

// Make sure v is in range min to max (inclusive). if < min, return max,
// if > max, return min
static inline int MLIB_CLAMP_TO_RANGE_MOD(int v, int min, int max) {
  if (v < min) {
    return max;
  }
  if (v > max) {
    return min;
  }
  return v;
}

// Make sure v is in range min to max (inclusive). If < min, return min
// if > max, return max
static inline int MLIB_CLAMP_TO_RANGE(int v, int min, int max) {
  if (v < min) {
    return min;
  }
  if (v > max) {
    return max;
  }
  return v;
}

// Return if point is fully contained in a rect
static inline bool MLIB_POINT_IN_RECT(MLIBPoint p, MLIBRect rect) {
  return (p.x >= rect.x && p.y >= rect.y && p.x < rect.x + rect.width &&
          p.y < rect.y + rect.height);
}

// Return a translated version of the point
static inline MLIBPoint MLIB_POINT_TRANSLATE(MLIBPoint p, int x, int y) {
  return MLIBPOINT_CREATE(p.x + x, p.y + y);
}

static inline int MLIB_ABS(int x) {
  if (x < 0) {
    return x * -1;
  }

  return x;
}

// Return the maximum of two values
static inline int MLIB_MAX(int x, int y) {
  if (x > y) {
    return x;
  }

  return y;
}

// Return the magnitude of a size vector
static inline int MLIB_MAGNITUDE(MLIBSize s) {
  return MLIB_ABS(s.width) * MLIB_ABS(s.height);
}

// Return the CHEBYSHEV distance between two points, not the geometric distance
// Diagonals shouldn't be longer
// https://en.wikipedia.org/wiki/Chebyshev_distance
static inline int MLIB_DISTANCE(MLIBPoint p1, MLIBPoint p2) {
  int dx = MLIB_ABS(p1.x - p2.x);
  int dy = MLIB_ABS(p1.y - p2.y);

  return MLIB_MAX(dx, dy);
}

// Copy the string from source to destination. If max length is reached,
// destination will have a valid null-terminated string of length
// max_buffer_length-1
extern void mlib_strcpy(char *destination, const char *source,
                        int max_buffer_length);

// Return true if two strings are equal, false otherwise
extern bool mlib_streq(const char *s1, const char *s2);

//
// Replace assert with something that works both in tests
// and uses playdate error for playadte
//
#ifndef MOOSELIB_PLAYDATE
#include <assert.h>
#endif

#ifdef MOOSELIB_PLAYDATE
#define MLIB_ASSERT(context, condition, ...)                                   \
  if (!(condition)) {                                                          \
    (context)->system->error(__VA_ARGS__);                                     \
  }
#endif

#ifndef MOOSELIB_PLAYDATE
#define MLIB_ASSERT(context, condition, ...) assert((condition))
#endif

// Wrappers for allocators. This allows overriding for systems like the playdate
// with dedicated allocators
#if !defined(MLIB_MALLOC) || !defined(MLIB_REALLOC) || !defined(MLIB_FREE)
#include <stdlib.h>
#define MLIB_MALLOC(size, ctx) (malloc(size))
#define MLIB_REALLOC(ptr, size, ctx) (realloc(ptr, size))
#define MLIB_FREE(ptr, ctx) (free(ptr))
#endif

// Custom variant of calloc that uses wrapper functions to allow for allocator
static inline void *MLIB_CALLOC(size_t nitems, size_t size, void *mem_ctx) {
  void *p = MLIB_MALLOC(nitems * size, mem_ctx);

  // This should probably be replaced with a memcpy
  for (size_t i = 0; i < nitems * size; i++) {
    ((char *)p)[i] = 0;
  }

  return p;
}

// Try wrap all/most stdlib features for visibility, even if they are just using
// stdlib
#include <stdio.h>
#include <string.h>
#define MLIB_VSPRINTF(msg, format, args) (vsprintf(msg, format, args))
#define MLIB_STRLEN(s) (strlen(s))
#endif
