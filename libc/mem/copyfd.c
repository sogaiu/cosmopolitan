/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2022 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚─────────────────────────────────────────────────────────────────────────────*/
#include "libc/calls/calls.h"
#include "libc/errno.h"
#include "libc/mem/mem.h"

#define CHUNK 32768

/**
 * Copies data between fds the old fashioned way.
 *
 * @return bytes successfully exchanged
 */
ssize_t _copyfd(int infd, int outfd, size_t n) {
  int e;
  char *buf;
  ssize_t rc;
  size_t i, j, got, sent;
  rc = 0;
  if (n) {
    if ((buf = malloc(CHUNK))) {
      for (e = errno, i = 0; i < n; i += j) {
        rc = read(infd, buf, CHUNK);
        if (rc == -1) {
          // eintr may interrupt the read operation
          if (i && errno == EINTR) {
            // suppress error if partially completed
            errno = e;
            rc = i;
          }
          break;
        }
        got = rc;
        if (!got) {
          rc = i;
          break;
        }
        // write operation must complete
        for (j = 0; j < got; j += sent) {
          rc = write(outfd, buf + j, got - j);
          if (rc != -1) {
            sent = rc;
          } else if (errno == EINTR) {
            // write operation must be uninterruptible
            errno = e;
            sent = 0;
          } else {
            break;
          }
        }
        if (rc == -1) break;
      }
      free(buf);
    } else {
      rc = -1;
    }
  }
  return rc;
}
