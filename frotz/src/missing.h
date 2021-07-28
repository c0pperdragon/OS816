/*
 * missing.h
 *
 * Declarations and definitions for standard things that may be missing
 * on older systems.
 *
 */

#ifndef MISSING_H
#define MISSING_H

#ifdef NO_MEMMOVE
void *my_memmove(void *, const void *, size_t);
#define memmove my_memmove
#endif

#ifdef NO_STRDUP
char *my_strdup(const char *);
char *my_strndup(const char *, size_t);
#ifndef my_strdup
#define strdup	my_strdup
#endif
#ifndef my_strndup
#define strndup	my_strndup
#endif
#endif

#ifdef NO_BASENAME
#ifdef basename
#undef basename
#endif
#define basename(c) c
#endif

#endif /* MISSING_H */
