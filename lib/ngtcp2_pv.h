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
#ifndef NGTCP2_PV_H
#define NGTCP2_PV_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif /* HAVE_CONFIG_H */

#include <ngtcp2/ngtcp2.h>

#include "ngtcp2_cid.h"
#include "ngtcp2_ringbuf.h"

/* NGTCP2_PV_MAX_ENTRIES is the maximum number of entries that
   ngtcp2_pv can contain.  It must be power of 2. */
#define NGTCP2_PV_MAX_ENTRIES 4

struct ngtcp2_log;
typedef struct ngtcp2_log ngtcp2_log;

struct ngtcp2_frame_chain;
typedef struct ngtcp2_frame_chain ngtcp2_frame_chain;

typedef struct {
  /* expiry is the timestamp when this PATH_CHALLENGE expires. */
  ngtcp2_tstamp expiry;
  /* data is a byte string included in PATH_CHALLENGE. */
  uint8_t data[8];
} ngtcp2_pv_entry;

void ngtcp2_pv_entry_init(ngtcp2_pv_entry *pvent, const uint8_t *data,
                          ngtcp2_tstamp expiry);

typedef enum {
  NGTCP2_PV_FLAG_NONE,
  /* NGTCP2_PV_FLAG_BLOCKING indicates that the path validation must
     finish before sending non-probing packets. */
  NGTCP2_PV_FLAG_BLOCKING = 0x01,
  /* NGTCP2_PV_FLAG_DONT_CARE indicates that the outcome of the path
     validation does not matter. */
  NGTCP2_PV_FLAG_DONT_CARE = 0x02,
  /* NGTCP2_PV_FLAG_RETIRE_DCID_ON_FINISH indicates that DCID should
     be retired after path validation finishes regardless of its
     result. */
  NGTCP2_PV_FLAG_RETIRE_DCID_ON_FINISH = 0x04,
  /* NGTCP2_PV_FLAG_VERIFY_OLD_PATH_ON_SUCCESS indicates that the path
     validation against the old path should be done after successful
     path validation. */
  NGTCP2_PV_FLAG_VERIFY_OLD_PATH_ON_SUCCESS = 0x08,
  /* NGTCP2_PV_FLAG_INVOKE_CALLBACK indicates that callback must be
     called after path validation finishes. */
  NGTCP2_PV_FLAG_INVOKE_CALLBACK = 0x10,
} ngtcp2_pv_flag;

struct ngtcp2_pv;
typedef struct ngtcp2_pv ngtcp2_pv;

/*
 * ngtcp2_pv is the context of a single path validation.
 */
struct ngtcp2_pv {
  ngtcp2_mem *mem;
  ngtcp2_log *log;
  /* dcid is DCID and path this path validation uses. */
  ngtcp2_dcid dcid;
  /* ents is the ring buffer of ngtcp2_pv_entry */
  ngtcp2_ringbuf ents;
  /* timeout is the duration within which this path validation should
     succeed. */
  ngtcp2_duration timeout;
  /* started_ts is the timestamp this path validation starts. */
  ngtcp2_tstamp started_ts;
  /* loss_count is the number of lost PATH_CHALLENGE */
  size_t loss_count;
  /* flags is bitwise-OR of zero or more of ngtcp2_pv_flag. */
  uint8_t flags;
};

/*
 * ngtcp2_pv_new creates new ngtcp2_pv object and assigns its pointer
 * to |*ppv|.  This function makes a copy of |dcid|.  |timeout| is a
 * duration within which this path validation must succeed.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGTCP2_ERR_NOMEM
 *     Out of memory
 */
int ngtcp2_pv_new(ngtcp2_pv **ppv, const ngtcp2_dcid *dcid,
                  ngtcp2_duration timeout, uint8_t flags, ngtcp2_log *log,
                  ngtcp2_mem *mem);

/*
 * ngtcp2_pv_del deallocates |pv|.  This function frees memory |pv|
 * points too.
 */
void ngtcp2_pv_del(ngtcp2_pv *pv);

/*
 * ngtcp2_pv_ensure_start sets started_ts field to |ts| if it is zero.
 */
void ngtcp2_pv_ensure_start(ngtcp2_pv *pv, ngtcp2_tstamp ts);

/*
 * ngtcp2_pv_add_entry adds new entry with |data|.  |expiry| is the
 * expiry time of the entry.
 */
void ngtcp2_pv_add_entry(ngtcp2_pv *pv, const uint8_t *data,
                         ngtcp2_tstamp expiry);

/*
 * ngtcp2_pv_full returns nonzero if |pv| is full of ngtcp2_pv_entry.
 */
int ngtcp2_pv_full(ngtcp2_pv *pv);

/*
 * ngtcp2_pv_validate validates that the |data| received from |path|
 * matches the one of the existing entry.
 *
 * This function returns 0 if it succeeds, or one of the following
 * negative error codes:
 *
 * NGTCP2_ERR_INVALID_STATE
 *     |pv| includes no entry
 * NGTCP2_ERR_INVALID_ARGUMENT
 *     |pv| does not have an entry which has |data| and |path|
 */
int ngtcp2_pv_validate(ngtcp2_pv *pv, const ngtcp2_path *path,
                       const uint8_t *data);

/*
 * ngtcp2_pv_handle_entry_expiry checks expiry for each entry.
 */
void ngtcp2_pv_handle_entry_expiry(ngtcp2_pv *pv, ngtcp2_tstamp ts);

/*
 * ngtcp2_pv_validation_timed_out returns nonzero if the path
 * validation fails because of timeout.
 */
int ngtcp2_pv_validation_timed_out(ngtcp2_pv *pv, ngtcp2_tstamp ts);

/*
 * ngtcp2_pv_next_expiry returns the earliest expiry.
 */
ngtcp2_tstamp ngtcp2_pv_next_expiry(ngtcp2_pv *pv);

#endif /* NGTCP2_PV_H */
