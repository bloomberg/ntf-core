// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_DISABLE_PERFCRIT_LOCKS

#include <ntcd_compression.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_compression_cpp, "$Id$ $CSID$")

#include <ntci_log.h>
#include <ntcs_blobutil.h>
#include <ntcs_datapool.h>
#include <ntsa_data.h>
#include <bdlb_chartype.h>
#include <bdlb_random.h>
#include <bdlb_string.h>
#include <bdlbb_blobstreambuf.h>
#include <bdlbb_blobutil.h>
#include <bdlde_crc32.h>
#include <bdlde_crc32c.h>
#include <bdlf_bind.h>
#include <bdlf_memfn.h>
#include <bdlf_placeholder.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_spinlock.h>
#include <bsl_fstream.h>
#include <bsl_sstream.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

namespace BloombergLP {
namespace ntcd {

ByteSequence::ByteSequence(bdlbb::Blob* blob, bsl::size_t base)
: d_blob_p(blob)
, d_base(base)
, d_sentinel(0)
{
}

ByteSequence::~ByteSequence()
{
}

bsl::uint8_t& ByteSequence::operator[](bsl::size_t index)
{
    BSLS_ASSERT(index < 1024);

    if (d_base + index >= static_cast<bsl::size_t>(d_blob_p->length())) {
        d_blob_p->setLength(static_cast<int>(d_base + index + 1));
    }

    const int numDataBuffers = d_blob_p->numDataBuffers();

    bsl::size_t numBytesRemaining = d_base + index;

    bsl::uint8_t* bufferData = 0;
    bsl::size_t   bufferSize = 0;

    for (int bufferIndex = 0; bufferIndex < numDataBuffers; ++bufferIndex) {
        const bdlbb::BlobBuffer& buffer = d_blob_p->buffer(bufferIndex);

        bufferData = reinterpret_cast<bsl::uint8_t*>(buffer.data());

        if (bufferIndex != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize =
                static_cast<bsl::size_t>(d_blob_p->lastDataBufferLength());
        }

        if (numBytesRemaining >= bufferSize) {
            numBytesRemaining -= bufferSize;
            continue;
        }
        else {
            break;
        }
    }

    BSLS_ASSERT(bufferData);
    BSLS_ASSERT(numBytesRemaining < bufferSize);

    return bufferData[numBytesRemaining];
}

bsl::uint8_t ByteSequence::operator[](bsl::size_t index) const
{
    BSLS_ASSERT(index < 1024);

    const int numDataBuffers = d_blob_p->numDataBuffers();

    bsl::size_t numBytesRemaining = d_base + index;

    const bsl::uint8_t* bufferData = 0;
    bsl::size_t         bufferSize = 0;

    for (int bufferIndex = 0; bufferIndex < numDataBuffers; ++bufferIndex) {
        const bdlbb::BlobBuffer& buffer = d_blob_p->buffer(bufferIndex);

        bufferData = reinterpret_cast<bsl::uint8_t*>(buffer.data());

        if (bufferIndex != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize =
                static_cast<bsl::size_t>(d_blob_p->lastDataBufferLength());
        }

        if (numBytesRemaining >= bufferSize) {
            numBytesRemaining -= bufferSize;
            continue;
        }
        else {
            break;
        }
    }

    BSLS_ASSERT(bufferData);
    BSLS_ASSERT(numBytesRemaining < bufferSize);

    return bufferData[numBytesRemaining];
}

namespace {

// MRM
#if 0
void encodeRle8(ntcd::ByteSequence& outbuff,
                int*                outlen,
                const bsl::uint8_t* inbuff,
                int                 inlen)
{
    int i, k, l, o;

    if (!inbuff || inlen < 1) {
        return;
    }

    k = o        = 0;
    outbuff[o++] = 0;
    for (i = 0; i < inlen; i++) {
        for (l = 1; l < 128 && i + l < inlen && inbuff[i] == inbuff[i + l];
             l++)
            ;
        if (l > 1) {
            l--;
            if (outbuff[k]) {
                outbuff[k]--;
                outbuff[o++] = 0x80 | l;
            }
            else
                outbuff[k] = 0x80 | l;
            outbuff[o++]  = inbuff[i];
            k             = o;
            outbuff[o++]  = 0;
            i            += l;
            continue;
        }
        outbuff[k]++;
        outbuff[o++] = inbuff[i];
        if (outbuff[k] > 127) {
            outbuff[k]--;
            k            = o;
            outbuff[o++] = 0;
        }
    }
    if (!(outbuff[k] & 0x80)) {
        if (outbuff[k])
            outbuff[k]--;
        else
            o--;
    }
    *outlen = o;
}

void decodeRle8(ntcd::ByteSequence& outbuff,
                int*                outlen,
                const bsl::uint8_t* inbuff,
                int                 inlen)
{
    int                 l, o = 0;
    const bsl::uint8_t* end = inbuff + inlen;

    if (!inbuff || inlen < 2)
        return;

    while (inbuff < end) {
        l = ((*inbuff++) & 0x7F) + 1;
        if (inbuff[-1] & 0x80) {
            while (l--)
                outbuff[o++] = *inbuff;
            inbuff++;
        }
        else
            while (l--)
                outbuff[o++] = *inbuff++;
    }
    *outlen = o;
}
#endif

#ifndef LZ4X_REALLOC
#define LZ4X_REALLOC realloc
#endif

#define LZ4X_BLOCK_SIZE (8 << 20)  // 8 MB
#define LZ4X_PADDING_LITERALS 5

#define LZ4X_WINDOW_BITS 16
#define LZ4X_WINDOW_SIZE (1 << LZ4X_WINDOW_BITS)
#define LZ4X_WINDOW_MASK (LZ4X_WINDOW_SIZE - 1)

#define LZ4X_MIN_MATCH 4

#define LZ4X_EXCESS (16 + (LZ4X_BLOCK_SIZE / 255))

#define LZ4X_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define LZ4X_MAX(a, b) (((a) > (b)) ? (a) : (b))

#define LZ4X_LOAD_16(p) (*(const uint16_t*)(p))
#define LZ4X_LOAD_32(p) (*(const uint32_t*)(p))
#define LZ4X_STORE_16(p, x) (*(uint16_t*)(p) = (x))
#define LZ4X_COPY_32(d, s) (*(uint32_t*)(d) = LZ4X_LOAD_32(s))

#define LZ4X_HASH_BITS 18
#define LZ4X_HASH_SIZE (1 << LZ4X_HASH_BITS)
#define LZ4X_NIL (-1)

#define LZ4X_HASH_32(p)                                                       \
    ((LZ4X_LOAD_32(p) * 0x9E3779B9) >> (32 - LZ4X_HASH_BITS))

#define lz4x_wild_copy(d, s, n)                                               \
    do {                                                                      \
        LZ4X_COPY_32(d, s);                                                   \
        LZ4X_COPY_32(d + 4, s + 4);                                           \
        for (int i_ = 8; i_ < n; i_ += 8) {                                   \
            LZ4X_COPY_32(d + i_, s + i_);                                     \
            LZ4X_COPY_32(d + 4 + i_, s + 4 + i_);                             \
        }                                                                     \
    } while (0)

int lz4x_compress_optimal(const uint8_t* in,
                          size_t         inlen,
                          uint8_t*       out,
                          size_t         outlen)
{
    (void)(outlen);

    static int head[LZ4X_HASH_SIZE];
    static int nodes[LZ4X_WINDOW_SIZE][2];
    struct lz4x_path {
        int cum;
        int len;
        int dist;
    }* path;  //path[LZ4X_BLOCK_SIZE+1];
    path = (struct lz4x_path*)LZ4X_REALLOC(0, sizeof(path[0]) * (inlen + 1));

    int n = (int)inlen;

    // Pass 1: Find all matches

    for (int i = 0; i < LZ4X_HASH_SIZE; ++i)
        head[i] = LZ4X_NIL;

    for (int p = 0; p < n; ++p) {
        int best_len = 0;
        int dist     = 0;

        const int max_match = (n - LZ4X_PADDING_LITERALS) - p;
        if (max_match >= LZ4X_MAX(12 - LZ4X_PADDING_LITERALS, LZ4X_MIN_MATCH))
        {
            const int limit = LZ4X_MAX(p - LZ4X_WINDOW_SIZE, LZ4X_NIL);

            int* left  = &nodes[p & LZ4X_WINDOW_MASK][1];
            int* right = &nodes[p & LZ4X_WINDOW_MASK][0];

            int left_len  = 0;
            int right_len = 0;

            const uint32_t h = LZ4X_HASH_32(&in[p]);
            int            s = head[h];
            head[h]          = p;

            while (s > limit) {
                int len = LZ4X_MIN(left_len, right_len);

                if (in[s + len] == in[p + len]) {
                    while (++len < max_match && in[s + len] == in[p + len])
                        ;

                    if (len > best_len) {
                        best_len = len;
                        dist     = p - s;

                        if (len == max_match || len >= (1 << 16))
                            break;
                    }
                }

                if (in[s + len] < in[p + len])  // in/out out/in ?
                {
                    *right    = s;
                    right     = &nodes[s & LZ4X_WINDOW_MASK][1];
                    s         = *right;
                    right_len = len;
                }
                else {
                    *left    = s;
                    left     = &nodes[s & LZ4X_WINDOW_MASK][0];
                    s        = *left;
                    left_len = len;
                }
            }

            *left  = LZ4X_NIL;
            *right = LZ4X_NIL;
        }

        path[p].len  = best_len;
        path[p].dist = dist;
    }

    // Pass 2: Build the shortest path

    path[n].cum = 0;

    int count = 15;

    for (int p = n - 1; p > 0; --p) {
        int c0 = path[p + 1].cum + 1;

        if (--count == 0) {
            count = 255;
            ++c0;
        }

        int len = path[p].len;
        if (len >= LZ4X_MIN_MATCH) {
            int c1 = 1 << 30;

            const int j = LZ4X_MAX(len - 255, LZ4X_MIN_MATCH);
            for (int i = len; i >= j; --i) {
                int tmp = path[p + i].cum + 3;

                if (i >= (15 + LZ4X_MIN_MATCH))
                    tmp += 1 + ((i - (15 + LZ4X_MIN_MATCH)) / 255);

                if (tmp < c1) {
                    c1  = tmp;
                    len = i;
                }
            }

            if (c1 <= c0) {
                path[p].cum = c1;
                path[p].len = len;

                count = 15;
            }
            else {
                path[p].cum = c0;
                path[p].len = 0;
            }
        }
        else
            path[p].cum = c0;
    }

    // Pass 3: Output the codes

    int op = 0;
    int pp = 0;

    int p = 0;
    while (p < n) {
        if (path[p].len >= LZ4X_MIN_MATCH) {
            int       len = path[p].len - LZ4X_MIN_MATCH;
            const int nib = LZ4X_MIN(len, 15);

            if (pp != p) {
                const int run = p - pp;
                if (run >= 15) {
                    out[op++] = (15 << 4) + nib;

                    int j = run - 15;
                    for (; j >= 255; j -= 255)
                        out[op++] = 255;
                    out[op++] = j;
                }
                else
                    out[op++] = (run << 4) + nib;

                lz4x_wild_copy(&out[op], &in[pp], run);
                op += run;
            }
            else
                out[op++] = nib;

            LZ4X_STORE_16(&out[op], path[p].dist);
            op += 2;

            if (len >= 15) {
                len -= 15;
                for (; len >= 255; len -= 255)
                    out[op++] = 255;
                out[op++] = len;
            }

            p += path[p].len;

            pp = p;
        }
        else
            ++p;
    }

    if (pp != p) {
        const int run = p - pp;
        if (run >= 15) {
            out[op++] = 15 << 4;

            int j = run - 15;
            for (; j >= 255; j -= 255)
                out[op++] = 255;
            out[op++] = j;
        }
        else
            out[op++] = run << 4;

        lz4x_wild_copy(&out[op], &in[pp], run);
        op += run;
    }

    (void)LZ4X_REALLOC(path, 0);

    const int comp_len = op;
    return comp_len;
}

int lz4x_compress(const uint8_t* in,
                  size_t         inlen,
                  uint8_t*       out,
                  size_t         outlen,
                  unsigned       max_chain)
{
    (void)(outlen);

    static int head[LZ4X_HASH_SIZE];
    static int tail[LZ4X_WINDOW_SIZE];

    int n = (int)inlen;

    for (int i = 0; i < LZ4X_HASH_SIZE; ++i)
        head[i] = LZ4X_NIL;

    int op = 0;
    int pp = 0;

    int p = 0;
    while (p < n) {
        int best_len = 0;
        int dist     = 0;

        const int max_match = (n - LZ4X_PADDING_LITERALS) - p;
        if (max_match >= LZ4X_MAX(12 - LZ4X_PADDING_LITERALS, LZ4X_MIN_MATCH))
        {
            const int limit     = LZ4X_MAX(p - LZ4X_WINDOW_SIZE, LZ4X_NIL);
            int       chain_len = max_chain;

            int s = head[LZ4X_HASH_32(&in[p])];
            while (s > limit) {
                if (in[s + best_len] == in[p + best_len] &&
                    LZ4X_LOAD_32(&in[s]) == LZ4X_LOAD_32(&in[p]))
                {
                    int len = LZ4X_MIN_MATCH;
                    while (len < max_match && in[s + len] == in[p + len])
                        ++len;

                    if (len > best_len) {
                        best_len = len;
                        dist     = p - s;

                        if (len == max_match)
                            break;
                    }
                }

                if (--chain_len <= 0)
                    break;

                s = tail[s & LZ4X_WINDOW_MASK];
            }
        }

        if (best_len >= LZ4X_MIN_MATCH) {
            int       len = best_len - LZ4X_MIN_MATCH;
            const int nib = LZ4X_MIN(len, 15);

            if (pp != p) {
                const int run = p - pp;
                if (run >= 15) {
                    out[op++] = (15 << 4) + nib;

                    int j = run - 15;
                    for (; j >= 255; j -= 255)
                        out[op++] = 255;
                    out[op++] = j;
                }
                else
                    out[op++] = (run << 4) + nib;

                lz4x_wild_copy(&out[op], &in[pp], run);
                op += run;
            }
            else
                out[op++] = nib;

            LZ4X_STORE_16(&out[op], dist);
            op += 2;

            if (len >= 15) {
                len -= 15;
                for (; len >= 255; len -= 255)
                    out[op++] = 255;
                out[op++] = len;
            }

            pp = p + best_len;

            while (p < pp) {
                const uint32_t h           = LZ4X_HASH_32(&in[p]);  // out?
                tail[p & LZ4X_WINDOW_MASK] = head[h];
                head[h]                    = p++;
            }
        }
        else {
            const uint32_t h           = LZ4X_HASH_32(&in[p]);  // out?
            tail[p & LZ4X_WINDOW_MASK] = head[h];
            head[h]                    = p++;
        }
    }

    if (pp != p) {
        const int run = p - pp;
        if (run >= 15) {
            out[op++] = 15 << 4;

            int j = run - 15;
            for (; j >= 255; j -= 255)
                out[op++] = 255;
            out[op++] = j;
        }
        else
            out[op++] = run << 4;

        lz4x_wild_copy(&out[op], &in[pp], run);
        op += run;
    }

    const int comp_len = op;
    return comp_len;
}

int lz4x_decompress(const uint8_t* in,
                    size_t         inlen,
                    uint8_t*       out,
                    size_t         outlen)
{
    int n = (int)inlen;

    int       p      = 0;
    int       ip     = 0;
    const int ip_end = ip + n;

    for (;;) {
        const int token = in[ip++];
        if (token >= 16) {
            int run = token >> 4;
            if (run == 15) {
                for (;;) {
                    const int c  = in[ip++];
                    run         += c;
                    if (c != 255)
                        break;
                }
            }
            if ((size_t)(p + run) > outlen)
                return 0;  // -1

            lz4x_wild_copy(&out[p], &in[ip], run);
            p  += run;
            ip += run;
            if (ip >= ip_end)
                break;
        }

        int s  = p - LZ4X_LOAD_16(&in[ip]);
        ip    += 2;
        if (s < 0)
            return 0;  // -1

        int len = (token & 15) + LZ4X_MIN_MATCH;
        if (len == (15 + LZ4X_MIN_MATCH)) {
            for (;;) {
                const int c  = in[ip++];
                len         += c;
                if (c != 255)
                    break;
            }
        }
        if ((size_t)(p + len) > outlen)
            return 0;  // -1

        if ((p - s) >= 4) {
            lz4x_wild_copy(&out[p], &out[s], len);
            p += len;
        }
        else {
            while (len-- != 0)
                out[p++] = out[s++];
        }
    }

    return p;
}

unsigned lz4x_encode(void*       out,
                     unsigned    outlen,
                     const void* in,
                     unsigned    inlen,
                     unsigned    flags /*[1..15*/)
{
    unsigned level = (unsigned)(flags > 15 ? 15 : flags < 1 ? 1 : flags);
    if (level >= 15)
        return lz4x_compress_optimal((const uint8_t*)in,
                                     inlen,
                                     (uint8_t*)out,
                                     outlen);
    return (unsigned)
        lz4x_compress((const uint8_t*)in, inlen, (uint8_t*)out, outlen, level);
}
unsigned lz4x_decode(void*       out,
                     unsigned    outlen,
                     const void* in,
                     unsigned    inlen)
{
    return (unsigned)lz4x_decompress((const uint8_t*)in,
                                     (size_t)inlen,
                                     (uint8_t*)out,
                                     (size_t)outlen);
}
unsigned lz4x_bounds(unsigned inlen, unsigned flags)
{
    (void)(flags);
    return (unsigned)(inlen + (inlen / 255) + 16);
}

}  // close unnamed namespace

bsl::uint32_t CompressionCrc32::calculate(const void*   data,
                                          bsl::size_t   size,
                                          bsl::uint32_t crc)
{
    return bdlde::Crc32c::calculate(data, size, crc);
}

bsl::uint32_t CompressionCrc32::calculate(const bdlbb::Blob& data,
                                          bsl::size_t        size,
                                          bsl::uint32_t      crc)
{
    bsl::size_t numBytesRemaining = size;

    bsl::uint32_t crcInProgress = crc;

    if (size == 0) {
        return crcInProgress;
    }

    const int numDataBuffers = data.numDataBuffers();

    for (int i = 0; i < numDataBuffers; ++i) {
        const bdlbb::BlobBuffer& buffer = data.buffer(i);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (i != numDataBuffers - 1) {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(data.lastDataBufferLength());
        }

        if (bufferSize < numBytesRemaining) {
            crcInProgress = bdlde::Crc32c::calculate(bufferData,
                                                     bufferSize,
                                                     crcInProgress);

            numBytesRemaining -= bufferSize;
        }
        else {
            crcInProgress = bdlde::Crc32c::calculate(bufferData,
                                                     numBytesRemaining,
                                                     crcInProgress);

            numBytesRemaining = 0;
            break;
        }
    }

    return crcInProgress;
}

#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
const bsl::uint32_t CompressionFrameHeader::k_MAGIC = 1380730184;
#else
const bsl::uint32_t CompressionFrameHeader::k_MAGIC = 1212501074;
#endif

ntsa::Error CompressionFrameHeader::encode(bsl::size_t* numBytesEncoded,
                                           bdlbb::Blob* destination)
{
    BSLMF_ASSERT(sizeof(*this) == 16);

    ntcs::BlobUtil::append(destination, this, sizeof(*this));
    *numBytesEncoded += sizeof(*this);

    return ntsa::Error();
}

ntsa::Error CompressionFrameHeader::encode(bsl::size_t* numBytesEncoded,
                                           bdlbb::Blob* destination,
                                           std::size_t  position)
{
    BSLMF_ASSERT(sizeof(*this) == 16);

    ntsa::Error error;

    bdlbb::OutBlobStreamBuf osb(destination);
    std::streampos p = osb.pubseekpos(static_cast<bsl::streampos>(position),
                                      bsl::ios_base::out);
    if (static_cast<bsl::size_t>(p) != position) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    bsl::streamsize n = osb.sputn(reinterpret_cast<const char*>(this),
                                  static_cast<bsl::streamsize>(sizeof(*this)));

    if (static_cast<bsl::size_t>(n) != sizeof(*this)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *numBytesEncoded += sizeof(*this);

    return ntsa::Error();
}

ntsa::Error CompressionFrameHeader::decode(bsl::size_t* numBytesDecoded,
                                           const void*  source,
                                           bsl::size_t  size)
{
    BSLMF_ASSERT(sizeof(*this) == 16);

    if (size < sizeof(*this)) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bsl::memcpy(this, source, sizeof(*this));
    *numBytesDecoded += sizeof(*this);

    if (d_magic != CompressionFrameHeader::k_MAGIC) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error CompressionFrameHeader::decode(bsl::size_t*        numBytesDecoded,
                                           const bsl::uint8_t* sourceBegin,
                                           const bsl::uint8_t* sourceEnd)
{
    return this->decode(numBytesDecoded, sourceBegin, sourceEnd - sourceBegin);
}

ntsa::Error CompressionFrameHeader::decode(bsl::size_t*    numBytesDecoded,
                                           bsl::streambuf* source)
{
    bsl::uint8_t buffer[sizeof(*this)];

    bsl::streamsize numAvailable = source->in_avail();
    if (numAvailable < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (numAvailable < static_cast<bsl::streamsize>(sizeof(*this))) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bsl::streamsize n =
        source->sgetn(reinterpret_cast<char*>(buffer), sizeof(*this));

    if (static_cast<bsl::size_t>(n) != sizeof(*this)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *numBytesDecoded = sizeof(*this);

    return ntsa::Error();
}

ntsa::Error CompressionFrameHeader::decode(bsl::size_t*       numBytesDecoded,
                                           const bdlbb::Blob& source)
{
    bsl::size_t numBytesAvailable = ntcs::BlobUtil::size(source);

    if (numBytesAvailable > sizeof(*this)) {
        const bdlbb::BlobBuffer& buffer = source.buffer(0);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<const bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (source.numDataBuffers() == 1) {
            bufferSize =
                static_cast<bsl::size_t>(source.lastDataBufferLength());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }

        if (bufferSize >= sizeof(*this)) {
            return this->decode(numBytesDecoded, bufferData, bufferSize);
        }
        else {
            bdlbb::InBlobStreamBuf isb(&source);
            return this->decode(numBytesDecoded, &isb);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
}

bool CompressionFrameHeader::equals(const CompressionFrameHeader& other) const
{
    return d_magic == other.d_magic && d_length == other.d_length &&
           d_flags == other.d_flags && d_checksum == other.d_checksum;
}

bool CompressionFrameHeader::less(const CompressionFrameHeader& other) const
{
    if (d_magic < other.d_magic) {
        return true;
    }

    if (other.d_magic < d_magic) {
        return false;
    }

    if (d_length < other.d_length) {
        return true;
    }

    if (other.d_length < d_length) {
        return false;
    }

    if (d_flags < other.d_flags) {
        return true;
    }

    if (other.d_flags < d_flags) {
        return false;
    }

    return d_checksum < other.d_checksum;
}

bsl::ostream& CompressionFrameHeader::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("magic", d_magic);
    printer.printAttribute("length", d_length);
    printer.printAttribute("flags", d_flags);
    printer.printAttribute("checksum", d_checksum);
    printer.end();
    return stream;
}

#if defined(BSLS_PLATFORM_IS_BIG_ENDIAN)
const bsl::uint32_t CompressionFrameFooter::k_MAGIC = 1380730182;
#else
const bsl::uint32_t CompressionFrameFooter::k_MAGIC = 1178946642;
#endif

ntsa::Error CompressionFrameFooter::encode(bsl::size_t* numBytesEncoded,
                                           bdlbb::Blob* destination)
{
    BSLMF_ASSERT(sizeof(*this) == 8);

    ntcs::BlobUtil::append(destination, this, sizeof(*this));
    *numBytesEncoded += sizeof(*this);

    return ntsa::Error();
}

ntsa::Error CompressionFrameFooter::decode(bsl::size_t* numBytesDecoded,
                                           const void*  source,
                                           bsl::size_t  size)
{
    BSLMF_ASSERT(sizeof(*this) == 8);

    if (size < sizeof(*this)) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bsl::memcpy(this, source, sizeof(*this));
    *numBytesDecoded += sizeof(*this);

    if (d_magic != CompressionFrameFooter::k_MAGIC) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error CompressionFrameFooter::decode(bsl::size_t*        numBytesDecoded,
                                           const bsl::uint8_t* sourceBegin,
                                           const bsl::uint8_t* sourceEnd)
{
    return this->decode(numBytesDecoded, sourceBegin, sourceEnd - sourceBegin);
}

ntsa::Error CompressionFrameFooter::decode(bsl::size_t*    numBytesDecoded,
                                           bsl::streambuf* source)
{
    bsl::uint8_t buffer[sizeof(*this)];

    bsl::streamsize numAvailable = source->in_avail();
    if (numAvailable < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (numAvailable < static_cast<bsl::streamsize>(sizeof(*this))) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bsl::streamsize n =
        source->sgetn(reinterpret_cast<char*>(buffer), sizeof(*this));

    if (static_cast<bsl::size_t>(n) != sizeof(*this)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *numBytesDecoded = sizeof(*this);

    return ntsa::Error();
}

ntsa::Error CompressionFrameFooter::decode(bsl::size_t*       numBytesDecoded,
                                           const bdlbb::Blob& source)
{
    bsl::size_t numBytesAvailable = ntcs::BlobUtil::size(source);

    if (numBytesAvailable > sizeof(*this)) {
        const bdlbb::BlobBuffer& buffer = source.buffer(0);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<const bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (source.numDataBuffers() == 1) {
            bufferSize =
                static_cast<bsl::size_t>(source.lastDataBufferLength());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }

        if (bufferSize >= sizeof(*this)) {
            return this->decode(numBytesDecoded, bufferData, bufferSize);
        }
        else {
            bdlbb::InBlobStreamBuf isb(&source);
            return this->decode(numBytesDecoded, &isb);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
}

bool CompressionFrameFooter::equals(const CompressionFrameFooter& other) const
{
    return d_magic == other.d_magic && d_checksum == other.d_checksum;
}

bool CompressionFrameFooter::less(const CompressionFrameFooter& other) const
{
    if (d_magic < other.d_magic) {
        return true;
    }

    if (other.d_magic < d_magic) {
        return false;
    }

    return d_checksum < other.d_checksum;
}

bsl::ostream& CompressionFrameFooter::print(bsl::ostream& stream,
                                            int           level,
                                            int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("magic", d_magic);
    printer.printAttribute("checksum", d_checksum);
    printer.end();
    return stream;
}

int CompressionBlockType::fromInt(CompressionBlockType::Value* result,
                                  int                          number)
{
    switch (number) {
    case CompressionBlockType::e_UNDEFINED:
    case CompressionBlockType::e_RAW:
    case CompressionBlockType::e_RLE:
        *result = static_cast<CompressionBlockType::Value>(number);
        return 0;
    default:
        return -1;
    }
}

int CompressionBlockType::fromString(CompressionBlockType::Value* result,
                                     const bslstl::StringRef&     string)
{
    if (bdlb::String::areEqualCaseless(string, "UNDEFINED")) {
        *result = e_UNDEFINED;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "NONE")) {
        *result = e_RAW;
        return 0;
    }
    if (bdlb::String::areEqualCaseless(string, "BER")) {
        *result = e_RLE;
        return 0;
    }

    return -1;
}

const char* CompressionBlockType::toString(CompressionBlockType::Value value)
{
    switch (value) {
    case e_UNDEFINED: {
        return "UNDEFINED";
    } break;
    case e_RAW: {
        return "RAW";
    } break;
    case e_RLE: {
        return "RLE";
    } break;
    }

    BSLS_ASSERT(!"invalid enumerator");
    return 0;
}

bsl::ostream& CompressionBlockType::print(bsl::ostream&               stream,
                                          CompressionBlockType::Value value)
{
    return stream << toString(value);
}

bsl::ostream& operator<<(bsl::ostream& stream, CompressionBlockType::Value rhs)
{
    return CompressionBlockType::print(stream, rhs);
}

ntsa::Error CompressionBlock::encode(bsl::size_t* numBytesEncoded,
                                     bdlbb::Blob* destination)
{
    BSLMF_ASSERT(sizeof(*this) == 4);

    ntcs::BlobUtil::append(destination, this, sizeof(*this));
    *numBytesEncoded += sizeof(*this);

    return ntsa::Error();
}

ntsa::Error CompressionBlock::decode(bsl::size_t* numBytesDecoded,
                                     const void*  source,
                                     bsl::size_t  size)
{
    BSLMF_ASSERT(sizeof(*this) == 4);

    if (size < sizeof(*this)) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bsl::memcpy(this, source, sizeof(*this));
    *numBytesDecoded += sizeof(*this);

    if (d_flags != 1 && d_flags != 2) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error CompressionBlock::decode(bsl::size_t*        numBytesDecoded,
                                     const bsl::uint8_t* sourceBegin,
                                     const bsl::uint8_t* sourceEnd)
{
    return this->decode(numBytesDecoded, sourceBegin, sourceEnd - sourceBegin);
}

ntsa::Error CompressionBlock::decode(bsl::size_t*    numBytesDecoded,
                                     bsl::streambuf* source)
{
    bsl::uint8_t buffer[sizeof(*this)];

    bsl::streamsize numAvailable = source->in_avail();
    if (numAvailable < 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }
    else if (numAvailable < static_cast<bsl::streamsize>(sizeof(*this))) {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }

    bsl::streamsize n =
        source->sgetn(reinterpret_cast<char*>(buffer), sizeof(*this));

    if (static_cast<bsl::size_t>(n) != sizeof(*this)) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    *numBytesDecoded = sizeof(*this);

    return ntsa::Error();
}

ntsa::Error CompressionBlock::decode(bsl::size_t*       numBytesDecoded,
                                     const bdlbb::Blob& source)
{
    bsl::size_t numBytesAvailable = ntcs::BlobUtil::size(source);

    if (numBytesAvailable > sizeof(*this)) {
        const bdlbb::BlobBuffer& buffer = source.buffer(0);

        const bsl::uint8_t* bufferData =
            reinterpret_cast<const bsl::uint8_t*>(buffer.data());

        bsl::size_t bufferSize;
        if (source.numDataBuffers() == 1) {
            bufferSize =
                static_cast<bsl::size_t>(source.lastDataBufferLength());
        }
        else {
            bufferSize = static_cast<bsl::size_t>(buffer.size());
        }

        if (bufferSize >= sizeof(*this)) {
            return this->decode(numBytesDecoded, bufferData, bufferSize);
        }
        else {
            bdlbb::InBlobStreamBuf isb(&source);
            return this->decode(numBytesDecoded, &isb);
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_WOULD_BLOCK);
    }
}

bool CompressionBlock::equals(const CompressionBlock& other) const
{
    return d_length == other.d_length && d_literal == other.d_literal &&
           d_flags == other.d_flags;
}

bool CompressionBlock::less(const CompressionBlock& other) const
{
    if (d_length < other.d_length) {
        return true;
    }

    if (other.d_length < d_length) {
        return false;
    }

    if (d_literal < other.d_literal) {
        return true;
    }

    if (other.d_literal < d_literal) {
        return false;
    }

    return d_flags < other.d_flags;
}

bsl::ostream& CompressionBlock::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    ntcd::CompressionBlockType::Value type = this->type();

    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    printer.printAttribute("type", type);
    printer.printAttribute("length", d_length);

    if (type == ntcd::CompressionBlockType::e_RLE) {
        if (bdlb::CharType::isPrint(static_cast<char>(d_literal))) {
            printer.printAttribute("literal", static_cast<char>(d_literal));
        }
        else {
            printer.printAttribute("literal", d_literal);
        }
    }

    printer.end();
    return stream;
}

CompressionEncoderRle::CompressionEncoderRle(
    const ntca::CompressionConfig& configuration,
    bslma::Allocator*              basicAllocator)
: d_frameHeader()
, d_frameHeaderPosition(0)
, d_frameContentBytesTotal(0)
, d_frameContentCrc(ntcd::CompressionCrc32::k_NULL_CRC32C)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CompressionEncoderRle::~CompressionEncoderRle()
{
}

ntsa::Error CompressionEncoderRle::deflateBegin(
    ntca::DeflateContext*       context,
    bdlbb::Blob*                result,
    const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    NTCI_LOG_STREAM_DEBUG << "Deflate begin: options = " << options
                          << NTCI_LOG_STREAM_END;

    d_frameHeaderPosition = static_cast<bsl::size_t>(result->length());

    d_frameHeader.reset();
    d_frameContentBytesTotal = 0;
    d_frameContentCrc        = ntcd::CompressionCrc32::k_NULL_CRC32C;

    bsl::size_t frameHeaderBytesEncoded = 0;
    error = d_frameHeader.encode(&frameHeaderBytesEncoded, result);
    if (error) {
        return error;
    }

    context->setBytesWritten(
        context->bytesWritten() + frameHeaderBytesEncoded);

    return ntsa::Error();
}

ntsa::Error CompressionEncoderRle::deflateNext(
    ntca::DeflateContext*       context,
    bdlbb::Blob*                result,
    const bsl::uint8_t*         data,
    bsl::size_t                 size,
    const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    if (size == 0) {
        return ntsa::Error();
    }

    const bsl::uint8_t* begin    = data;
    const bsl::uint8_t* current  = begin;
    const bsl::uint8_t* mark     = 0;
    const bsl::uint8_t* previous = 0;
    const bsl::uint8_t* end      = begin + size;

    enum Mode { k_MODE_DEFAULT, k_MODE_SAME, k_MODE_DIFFERENT };

    Mode mode = k_MODE_DEFAULT;

    while (current != end) {
        if (mark == 0) {
            mark = current;
        }
        else {
            if (mode == k_MODE_DEFAULT) {
                if (*current == *mark) {
                    mode = k_MODE_SAME;
                }
                else {
                    mode = k_MODE_DIFFERENT;
                }
            }
            else if (mode == k_MODE_SAME) {
                if (*current != *previous) {
                    BSLS_ASSERT(*previous == *mark);

                    ntcd::CompressionBlock block;
                    block.setType(ntcd::CompressionBlockType::e_RLE);
                    block.setLength(current - mark);
                    block.setLiteral(*previous);

                    NTCI_LOG_STREAM_DEBUG << "Deflated " << block
                                          << NTCI_LOG_STREAM_END;

                    bsl::size_t blockBytesEncoded = 0;
                    error = block.encode(&blockBytesEncoded, result);
                    if (error) {
                        return error;
                    }

                    d_frameContentBytesTotal += blockBytesEncoded;

                    context->setBytesWritten(
                        context->bytesWritten() + blockBytesEncoded);

                    mode = k_MODE_DEFAULT;
                    mark = current;
                }
            }
            else {
                if (*current == *previous) {
                    ntcd::CompressionBlock block;
                    block.setType(ntcd::CompressionBlockType::e_RAW);
                    block.setLength(previous - mark);

                    NTCI_LOG_STREAM_DEBUG
                        << "Deflated " << block << " "
                        << bsl::string_view((const char*)(mark),
                                            block.length())
                        << NTCI_LOG_STREAM_END;

                    bsl::size_t blockBytesEncoded = 0;
                    error = block.encode(&blockBytesEncoded, result);
                    if (error) {
                        return error;
                    }

                    d_frameContentBytesTotal += blockBytesEncoded;
                    
                    context->setBytesWritten(
                        context->bytesWritten() + blockBytesEncoded);

                    ntcs::BlobUtil::append(result, mark, block.length());

                    d_frameContentBytesTotal += block.length();
                    
                    context->setBytesWritten(
                        context->bytesWritten() + block.length());

                    mode = k_MODE_SAME;
                    mark = previous;
                }
            }
        }

        previous = current++;
    }

    BSLS_ASSERT(current == end);

    if (mode == k_MODE_SAME) {
        ntcd::CompressionBlock block;
        block.setType(ntcd::CompressionBlockType::e_RLE);
        block.setLength(current - mark);
        block.setLiteral(*previous);

        NTCI_LOG_STREAM_DEBUG << "Deflated " << block << NTCI_LOG_STREAM_END;

        bsl::size_t blockBytesEncoded = 0;
        error = block.encode(&blockBytesEncoded, result);
        if (error) {
            return error;
        }

        d_frameContentBytesTotal += blockBytesEncoded;
        context->setBytesWritten(context->bytesWritten() + blockBytesEncoded);
    }
    else {
        ntcd::CompressionBlock block;
        block.setType(ntcd::CompressionBlockType::e_RAW);
        block.setLength(current - mark);

        NTCI_LOG_STREAM_DEBUG << "Deflated " << block << NTCI_LOG_STREAM_END;

        bsl::size_t blockBytesEncoded = 0;
        block.encode(&blockBytesEncoded, result);

        d_frameContentBytesTotal += blockBytesEncoded;
        context->setBytesWritten(context->bytesWritten() + blockBytesEncoded);

        ntcs::BlobUtil::append(result, mark, block.length());

        d_frameContentBytesTotal += block.length();            
        context->setBytesWritten(context->bytesWritten() + block.length());
    }

    context->setBytesRead(context->bytesRead() + size);

    d_frameContentCrc = 
        bdlde::Crc32c::calculate(data, size, d_frameContentCrc);

    return ntsa::Error();
}

ntsa::Error CompressionEncoderRle::deflateEnd(
    ntca::DeflateContext*       context,
    bdlbb::Blob*                result,
    const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::size_t frameHeaderBytesEncoded = 0;
    bsl::size_t frameFooterBytesEncoded = 0;

    context->setChecksum(d_frameContentCrc);

    d_frameHeader.setLength(d_frameContentBytesTotal);
    d_frameHeader.setChecksum(d_frameContentCrc);

    error = d_frameHeader.encode(&frameHeaderBytesEncoded, 
                                 result, 
                                 d_frameHeaderPosition);
    if (error) {
        return error;
    }

    ntcd::CompressionFrameFooter frameFooter;
    frameFooter.setChecksum(d_frameContentCrc);

    error = frameFooter.encode(&frameFooterBytesEncoded, result);
    if (error) {
        return error;
    }

    context->setBytesWritten(
        context->bytesWritten() + frameFooterBytesEncoded);

    NTCI_LOG_STREAM_DEBUG << "Deflate end: context = " << *context
                          << " header = " << d_frameHeader << " footer = " 
                          << frameFooter
                          << NTCI_LOG_STREAM_END;

    d_frameHeader.reset();
    d_frameHeaderPosition    = 0;
    d_frameContentBytesTotal = 0;
    d_frameContentCrc        = ntcd::CompressionCrc32::k_NULL_CRC32C;

    return ntsa::Error();
}

ntsa::Error CompressionDecoderRle::process(ntca::InflateContext*       context,
                                           bdlbb::Blob*                result,
                                           const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    ntsa::Error error;

    bsl::size_t numBytesWritten = 0;

    while (true) {
        if (d_state == e_WANT_FRAME_HEADER) {
            bsl::size_t numBytesDecoded = 0;
            error = d_frameHeader.decode(&numBytesDecoded, d_input);
            if (error) {
                if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                    break;
                }
                else {
                    return this->fail(error);
                }
            }

            d_frameContentBytesNeeded = d_frameHeader.length();

            ntcs::BlobUtil::pop(&d_input, numBytesDecoded);

            if (d_frameContentBytesNeeded > 0) {
                d_state = e_WANT_BLOCK;
            }
            else {
                d_state = e_WANT_FRAME_FOOTER;
            }

            continue;
        }
        else if (d_state == e_WANT_BLOCK) {
            ntcd::CompressionBlock block;

            bsl::size_t numBytesDecoded = 0;
            error = block.decode(&numBytesDecoded, d_input);
            if (error) {
                if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                    break;
                }
                else {
                    return this->fail(error);
                }
            }

            ntcs::BlobUtil::pop(&d_input, numBytesDecoded);
            d_frameContentBytesNeeded -= numBytesDecoded;

            if (block.type() == ntcd::CompressionBlockType::e_RAW) {
                if (block.length() > 0) {
                    bdlbb::BlobUtil::append(result,
                                            d_input,
                                            0,
                                            static_cast<int>(block.length()));

                    d_frameContentCrc =
                        ntcd::CompressionCrc32::calculate(d_input,
                                                          block.length());

                    ntcs::BlobUtil::pop(&d_input, block.length());
                    d_frameContentBytesNeeded -= block.length();

                    numBytesWritten += block.length();
                }
            }
            else if (block.type() == ntcd::CompressionBlockType::e_RLE) {
                if (block.length() > 0) {
                    d_expansion.clear();
                    d_expansion.resize(block.length(), block.literal());

                    ntcs::BlobUtil::append(result,
                                           &d_expansion[0],
                                           d_expansion.size());

                    d_frameContentCrc =
                        ntcd::CompressionCrc32::calculate(&d_expansion[0],
                                                          d_expansion.size());

                    d_expansion.clear();

                    numBytesWritten += block.length();
                }
            }
            else {
                return this->fail(ntsa::Error(ntsa::Error::e_INVALID));
            }

            if (d_frameContentBytesNeeded > 0) {
                d_state = e_WANT_BLOCK;
            }
            else {
                d_state = e_WANT_FRAME_FOOTER;
            }

            continue;
        }
        else if (d_state == e_WANT_FRAME_FOOTER) {
            ntcd::CompressionFrameFooter footer;

            bsl::size_t numBytesDecoded = 0;
            error = footer.decode(&numBytesDecoded, d_input);
            if (error) {
                if (error == ntsa::Error(ntsa::Error::e_WOULD_BLOCK)) {
                    break;
                }
                else {
                    return this->fail(error);
                }
            }

            ntcs::BlobUtil::pop(&d_input, numBytesDecoded);

            if (d_frameContentCrc != d_frameHeader.checksum()) {
                NTCI_LOG_ERROR("Failed to inflate message: "
                               "the calculated checksum %u "
                               "does not match the expected checksum %u",
                               d_frameContentCrc,
                               d_frameHeader.checksum());

                return this->fail(ntsa::Error(ntsa::Error::e_INVALID));
            }

            d_state = e_WANT_FRAME_HEADER;

            d_frameHeader.reset();
            d_frameContentBytesNeeded = 0;
            d_frameContentCrc         = ntcd::CompressionCrc32::k_NULL_CRC32C;
            continue;
        }
        else if (d_state == e_ERROR) {
            if (d_error) {
                return d_error;
            }
            else {
                return this->fail(ntsa::Error(ntsa::Error::e_INVALID));
            }
        }
        else {
            return this->fail(ntsa::Error(ntsa::Error::e_INVALID));
        }
    }

    context->setBytesWritten(context->bytesWritten() + numBytesWritten);

    return ntsa::Error();
}

ntsa::Error CompressionDecoderRle::fail(ntsa::Error error)
{
    d_state = e_ERROR;
    d_error = error;

    d_expansion.clear();
    d_frameHeader.reset();
    d_frameContentBytesNeeded = 0;
    d_frameContentCrc         = ntcd::CompressionCrc32::k_NULL_CRC32C;

    return error;
}

CompressionDecoderRle::CompressionDecoderRle(
    const ntca::CompressionConfig& configuration,
    bslma::Allocator*              basicAllocator)
: d_state(e_WANT_FRAME_HEADER)
, d_input(basicAllocator)
, d_expansion(basicAllocator)
, d_frameHeader()
, d_frameContentBytesNeeded(0)
, d_frameContentCrc(ntcd::CompressionCrc32::k_NULL_CRC32C)
, d_config(configuration)
, d_error()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CompressionDecoderRle::~CompressionDecoderRle()
{
}

ntsa::Error CompressionDecoderRle::inflateBegin(
    ntca::InflateContext*       context,
    bdlbb::Blob*                result,
    const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    context->setPosition(static_cast<bsl::size_t>(result->length()));

    if (d_state != e_WANT_FRAME_HEADER) {
        if (d_error) {
            return d_error;
        }
        else {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }
    }

    return ntsa::Error();
}

ntsa::Error CompressionDecoderRle::inflateNext(
    ntca::InflateContext*       context,
    bdlbb::Blob*                result,
    const bsl::uint8_t*         data,
    bsl::size_t                 size,
    const ntca::InflateOptions& options)
{
    ntsa::Error error;

    ntcs::BlobUtil::append(&d_input, data, size);
    context->setBytesRead(context->bytesRead() + size);

    error = this->process(context, result, options);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error CompressionDecoderRle::inflateNext(
    ntca::InflateContext*       context,
    bdlbb::Blob*                result,
    const bdlbb::Blob&          data,
    const ntca::InflateOptions& options)
{
    ntsa::Error error;

    bdlbb::BlobUtil::append(&d_input, data);
    context->setBytesRead(context->bytesRead() +
                          static_cast<bsl::size_t>(data.length()));

    error = this->process(context, result, options);
    if (error) {
        return error;
    }

    return ntsa::Error();
}

ntsa::Error CompressionDecoderRle::inflateEnd(
    ntca::InflateContext*       context,
    bdlbb::Blob*                result,
    const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

CompressionEncoderLz4::CompressionEncoderLz4(
    const ntca::CompressionConfig& configuration,
    bslma::Allocator*              basicAllocator)
: d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CompressionEncoderLz4::~CompressionEncoderLz4()
{
}

ntsa::Error CompressionEncoderLz4::deflateBegin(
    ntca::DeflateContext*       context,
    bdlbb::Blob*                result,
    const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Deflate begin" << NTCI_LOG_STREAM_END;

    context->setPosition(static_cast<bsl::size_t>(result->length()));

    return ntsa::Error();
}

ntsa::Error CompressionEncoderLz4::deflateNext(
    ntca::DeflateContext*       context,
    bdlbb::Blob*                result,
    const bsl::uint8_t*         data,
    bsl::size_t                 size,
    const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(data);
    NTCCFG_WARNING_UNUSED(size);
    NTCCFG_WARNING_UNUSED(options);

    if (size == 0) {
        return ntsa::Error();
    }

    std::size_t numBytesWritten = 0;

    const unsigned int flags = 6;

    bsl::size_t bounds = static_cast<bsl::size_t>(lz4x_bounds(size, flags));

    bsl::vector<bsl::uint8_t> temp;
    temp.resize(bounds);

    numBytesWritten = lz4x_encode(&temp[0],
                                  static_cast<unsigned int>(temp.size()),
                                  data,
                                  static_cast<unsigned int>(size),
                                  flags);

    ntcs::BlobUtil::append(result, &temp[0], numBytesWritten);

    context->setBytesRead(context->bytesRead() + size);
    context->setBytesWritten(context->bytesWritten() + numBytesWritten);

// MRM
#if 0
    bsl::uint32_t checksum =
        Compression::checksum(context->checksum(), data, size);

    context->setChecksum(checksum);
#endif

    return ntsa::Error();
}

ntsa::Error CompressionEncoderLz4::deflateEnd(
    ntca::DeflateContext*       context,
    bdlbb::Blob*                result,
    const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    NTCI_LOG_CONTEXT();

    NTCI_LOG_STREAM_DEBUG << "Deflate end " << *context << NTCI_LOG_STREAM_END;

    return ntsa::Error();
}

CompressionDecoderLz4::CompressionDecoderLz4(
    const ntca::CompressionConfig& configuration,
    bslma::Allocator*              basicAllocator)
: d_state(e_WANT_FRAME_HEADER)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

CompressionDecoderLz4::~CompressionDecoderLz4()
{
}

ntsa::Error CompressionDecoderLz4::inflateBegin(
    ntca::InflateContext*       context,
    bdlbb::Blob*                result,
    const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    context->setPosition(static_cast<bsl::size_t>(result->length()));

    return ntsa::Error();
}

ntsa::Error CompressionDecoderLz4::inflateNext(
    ntca::InflateContext*       context,
    bdlbb::Blob*                result,
    const bsl::uint8_t*         data,
    bsl::size_t                 size,
    const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(data);
    NTCCFG_WARNING_UNUSED(size);
    NTCCFG_WARNING_UNUSED(options);

    bsl::size_t numBytesWritten = 0;

    bsl::vector<bsl::uint8_t> temp;
    temp.resize(1024 * 1024 * 16);

    numBytesWritten = lz4x_decode(&temp[0],
                                  static_cast<unsigned int>(temp.size()),
                                  data,
                                  static_cast<unsigned int>(size));

    ntcs::BlobUtil::append(result, &temp[0], numBytesWritten);

    context->setBytesRead(context->bytesRead() + size);
    context->setBytesWritten(context->bytesWritten() + numBytesWritten);

    return ntsa::Error();
}

ntsa::Error CompressionDecoderLz4::inflateNext(
    ntca::InflateContext*       context,
    bdlbb::Blob*                result,
    const bdlbb::Blob&          data,
    const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(data);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error CompressionDecoderLz4::inflateEnd(
    ntca::InflateContext*       context,
    bdlbb::Blob*                result,
    const ntca::InflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(result);
    NTCCFG_WARNING_UNUSED(options);

    return ntsa::Error();
}

ntsa::Error Compression::deflateBegin(ntca::DeflateContext*       context,
                                      bdlbb::Blob*                result,
                                      const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    if (d_config.type().value() == ntca::CompressionType::e_RLE) {
        error = d_rleEncoder.deflateBegin(context, result, options);
        if (error) {
            return error;
        }
    }
    else if (d_config.type().value() == ntca::CompressionType::e_LZ4) {
        error = d_lz4Encoder.deflateBegin(context, result, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateNext(ntca::DeflateContext*       context,
                                     bdlbb::Blob*                result,
                                     const bsl::uint8_t*         data,
                                     bsl::size_t                 size,
                                     const ntca::DeflateOptions& options)
{
    NTCCFG_WARNING_UNUSED(options);

    ntsa::Error error;

    if (d_config.type().value() == ntca::CompressionType::e_RLE) {
        error = d_rleEncoder.deflateNext(context, result, data, size, options);
        if (error) {
            return error;
        }
    }
    else if (d_config.type().value() == ntca::CompressionType::e_LZ4) {
        error = d_lz4Encoder.deflateNext(context, result, data, size, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Compression::deflateEnd(ntca::DeflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntca::DeflateOptions& options)
{
    ntsa::Error error;

    if (d_config.type().value() == ntca::CompressionType::e_RLE) {
        error = d_rleEncoder.deflateEnd(context, result, options);
        if (error) {
            return error;
        }
    }
    else if (d_config.type().value() == ntca::CompressionType::e_LZ4) {
        error = d_lz4Encoder.deflateEnd(context, result, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateBegin(ntca::InflateContext*       context,
                                      bdlbb::Blob*                result,
                                      const ntca::InflateOptions& options)
{
    ntsa::Error error;

    if (d_config.type().value() == ntca::CompressionType::e_RLE) {
        error = d_rleDecoder.inflateBegin(context, result, options);
        if (error) {
            return error;
        }
    }
    else if (d_config.type().value() == ntca::CompressionType::e_LZ4) {
        error = d_lz4Decoder.inflateBegin(context, result, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateNext(ntca::InflateContext*       context,
                                     bdlbb::Blob*                result,
                                     const bsl::uint8_t*         data,
                                     bsl::size_t                 size,
                                     const ntca::InflateOptions& options)
{
    ntsa::Error error;

    if (d_config.type().value() == ntca::CompressionType::e_RLE) {
        error = d_rleDecoder.inflateNext(context, result, data, size, options);
        if (error) {
            return error;
        }
    }
    else if (d_config.type().value() == ntca::CompressionType::e_LZ4) {
        error = d_lz4Decoder.inflateNext(context, result, data, size, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateNext(ntca::InflateContext*       context,
                                     bdlbb::Blob*                result,
                                     const bdlbb::Blob&          data,
                                     const ntca::InflateOptions& options)
{
    ntsa::Error error;

    if (d_config.type().value() == ntca::CompressionType::e_RLE) {
        error = d_rleDecoder.inflateNext(context, result, data, options);
        if (error) {
            return error;
        }
    }
    else if (d_config.type().value() == ntca::CompressionType::e_LZ4) {
        error = d_lz4Decoder.inflateNext(context, result, data, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

ntsa::Error Compression::inflateEnd(ntca::InflateContext*       context,
                                    bdlbb::Blob*                result,
                                    const ntca::InflateOptions& options)
{
    ntsa::Error error;

    if (d_config.type().value() == ntca::CompressionType::e_RLE) {
        error = d_rleDecoder.inflateEnd(context, result, options);
        if (error) {
            return error;
        }
    }
    else if (d_config.type().value() == ntca::CompressionType::e_LZ4) {
        error = d_lz4Decoder.inflateEnd(context, result, options);
        if (error) {
            return error;
        }
    }
    else {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    return ntsa::Error();
}

Compression::Compression(const ntca::CompressionConfig&         configuration,
                         const bsl::shared_ptr<ntci::DataPool>& dataPool,
                         bslma::Allocator*                      basicAllocator)
: d_rleEncoder(configuration, basicAllocator)
, d_rleDecoder(configuration, basicAllocator)
, d_lz4Encoder(configuration, basicAllocator)
, d_lz4Decoder(configuration, basicAllocator)
, d_dataPool_sp(dataPool)
, d_config(configuration)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    NTCCFG_WARNING_UNUSED(d_allocator_p);

    if (d_config.type().isNull()) {
        d_config.setType(ntca::CompressionType::e_RLE);
    }

    if (d_config.goal().isNull()) {
        d_config.setGoal(ntca::CompressionGoal::e_BALANCED);
    }
}

Compression::~Compression()
{
}

}  // close package namespace
}  // close enterprise namespace
