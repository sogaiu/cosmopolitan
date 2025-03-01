/*-*- mode:c;indent-tabs-mode:nil;c-basic-offset:2;tab-width:8;coding:utf-8 -*-│
│vi: set net ft=c ts=2 sts=2 sw=2 fenc=utf-8                                :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
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
#include "ape/sections.internal.h"
#include "libc/calls/calls.h"
#include "libc/calls/kntprioritycombos.internal.h"
#include "libc/errno.h"
#include "libc/log/log.h"
#include "libc/nexgen32e/x86feature.h"
#include "libc/runtime/runtime.h"
#include "libc/sysv/consts/mlock.h"
#include "libc/testlib/testlib.h"

double g_avx2_juiceup_doubles_[4] forcealign(32);
unsigned long long g_avx2_juiceup_quadwords_[4] forcealign(32);

void testlib_benchwarmup(void) {
  /* get mathematical parts of cpu juiced up */
  if (X86_HAVE(AVX2) && X86_HAVE(FMA)) {
    asm("vmovdqa\t%1,%%ymm0\n\t"
        "vpmaddwd\t(%2),%%ymm0,%%ymm0\n\t"
        "vmovdqa\t%%ymm0,%0\n\t"
        "vzeroall"
        : "=m"(g_avx2_juiceup_quadwords_)
        : "m"(g_avx2_juiceup_quadwords_), "r"(&_base[0]));
    asm("vmovapd\t%1,%%ymm1\n\t"
        "vfmadd132pd\t(%2),%%ymm1,%%ymm1\n\t"
        "vmovapd\t%%ymm1,%0\n\t"
        "vzeroall"
        : "=m"(g_avx2_juiceup_doubles_)
        : "m"(g_avx2_juiceup_doubles_), "r"(&_base[32]));
  }
}

/**
 * Runs all benchmark functions in sorted order.
 *
 * @see BENCH()
 */
void testlib_runallbenchmarks(void) {
  int e;
  __log_level = kLogWarn;
  testlib_runtestcases(__bench_start, __bench_end, testlib_benchwarmup);
}
