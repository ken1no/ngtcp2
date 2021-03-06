/*
 * ngtcp2
 *
 * Copyright (c) 2019 ngtcp2 contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef NGTCP2_PATH_H
#define NGTCP2_PATH_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <ngtcp2/ngtcp2.h>

/*
 * ngtcp2_path_init initializes |path| with the given addresses.  Note
 * that the buffer pointed by local->addr and remote->addr are not
 * copied.  Their pointer values are assigned instead.
 */
void ngtcp2_path_init(ngtcp2_path *path, const ngtcp2_addr *local,
                      const ngtcp2_addr *remote);

/*
 * ngtcp2_path_copy copies |src| into |dest|.  This function assumes
 * that |dest| has enough buffer to store the deep copy of src->local
 * and src->remote.
 */
void ngtcp2_path_copy(ngtcp2_path *dest, const ngtcp2_path *src);

/*
 * ngtcp2_path_eq returns nonzero if |a| equals |b| such that
 * ngtcp2_addr_eq(&a->local, &b->local) && ngtcp2_addr_eq(&a->remote,
 * &b->remote) is true.
 */
int ngtcp2_path_eq(const ngtcp2_path *a, const ngtcp2_path *b);

#endif /* NGTCP2_PATH_H */
