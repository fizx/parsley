/*
 * $Id: printbuf.c,v 1.5 2006/01/26 02:16:28 mclark Exp $
 *
 * Copyright (c) 2004, 2005 Metaparadigm Pte. Ltd.
 * Michael Clark <michael@metaparadigm.com>
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the MIT license. See COPYING for details.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "printbuf.h"
#define max(a,b) ((a) > (b) ? (a) : (b))

struct printbuf* printbuf_new()
{
  struct printbuf *p;

  if(!(p = calloc(1, sizeof(struct printbuf)))) return NULL;
  p->size = 32;
  p->bpos = 0;
  if(!(p->buf = malloc(p->size))) {
    free(p);
    return NULL;
  }
  return p;
}


int printbuf_memappend(struct printbuf *p, char *buf, int size)
{
  char *t;
  if(p->size - p->bpos <= size) {
    int new_size = max(p->size * 2, p->bpos + size + 8);
    if(!(t = realloc(p->buf, new_size))) return -1;
    p->size = new_size;
    p->buf = t;
  }
  memcpy(p->buf + p->bpos, buf, size);
  p->bpos += size;
  p->buf[p->bpos]= '\0';
  return size;
}

int sprintbuf(struct printbuf *p, const char *msg, ...)
{
  va_list ap;
  char *t;
  int size;
  char buf[128];

  /* user stack buffer first */
  va_start(ap, msg);
  size = vsnprintf(buf, 128, msg, ap);
  va_end(ap);
  /* if string is greater than stack buffer, then use dynamic string
     with vasprintf.  Note: some implementation of vsnprintf return -1
     if output is truncated whereas some return the number of bytes that
     would have been writen - this code handles both cases. */
  if(size == -1 || size > 127) {
    int ret;
    va_start(ap, msg);
    if((size = vasprintf(&t, msg, ap)) == -1) return -1;
    va_end(ap);
    ret = printbuf_memappend(p, t, size);
    free(t);
    return ret;
  } else {
    return printbuf_memappend(p, buf, size);
  }
}

void printbuf_reset(struct printbuf *p)
{
  p->buf[0] = '\0';
  p->bpos = 0;
}

void printbuf_free(struct printbuf *p)
{
  if(p) {
    free(p->buf);
    free(p);
  }
}
