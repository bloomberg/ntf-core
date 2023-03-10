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

#include <ntcd_datautil.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcd_datautil_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntcd {

char DataUtil::generateByte(bsl::size_t position, bsl::size_t dataset)
{
    struct {
        const char* source;
        bsl::size_t length;
    } DATA[] = {
        {"abcdefghijklmnopqrstuvwxyz", 26},
        {"ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26}
    };

    enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

    dataset = dataset % NUM_DATA;
    return DATA[dataset].source[position % DATA[dataset].length];
}

void DataUtil::generateData(bsl::string* result,
                            bsl::size_t  size,
                            bsl::size_t  offset,
                            bsl::size_t  dataset)
{
    result->clear();
    result->resize(size);

    for (bsl::size_t i = offset; i < offset + size; ++i) {
        (*result)[i] = generateByte(offset + i, dataset);
    }
}

void DataUtil::generateData(bdlbb::Blob* result,
                            bsl::size_t  size,
                            bsl::size_t  offset,
                            bsl::size_t  dataset)
{
    result->removeAll();
    result->setLength(static_cast<int>(size));

    bsl::size_t k = 0;

    for (int i = 0; i < result->numDataBuffers(); ++i) {
        const bdlbb::BlobBuffer& buffer = result->buffer(i);

        bsl::size_t numBytesToWrite = i == result->numDataBuffers() - 1
                                          ? result->lastDataBufferLength()
                                          : buffer.size();

        for (bsl::size_t j = 0; j < numBytesToWrite; ++j) {
            buffer.data()[j] = generateByte(offset + k, dataset);
            ++k;
        }
    }
}

bsl::string DataUtil::formatMicroseconds(double value)
{
    bsl::stringstream ss;
    ss << bsl::setprecision(2);
    ss << bsl::fixed;

    if (value < 1000) {
        ss << value << "us";
        return ss.str();
    }

    value /= 1000;

    if (value < 1000) {
        ss << value << "ms";
        return ss.str();
    }

    value /= 1000;

    ss << value << "s";
    return ss.str();
}

}  // close package namespace
}  // close enterprise namespace
