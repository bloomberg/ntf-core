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

#include <ntca_encryptionsecret.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionsecret_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

namespace BloombergLP {
namespace ntca {

EncryptionSecret::EncryptionSecret(bslma::Allocator* basicAllocator)
: d_data(basicAllocator)
{
}

EncryptionSecret::EncryptionSecret(const EncryptionSecret& original,
                                   bslma::Allocator*       basicAllocator)
: d_data(original.d_data, basicAllocator)
{
}

EncryptionSecret::~EncryptionSecret()
{
    this->reset();
}

EncryptionSecret& EncryptionSecret::operator=(const EncryptionSecret& other)
{
    if (this != &other) {
        d_data = other.d_data;
    }

    return *this;
}

void EncryptionSecret::reset()
{
    if (!d_data.empty()) {
        bsl::memset(&d_data.front(), 0, d_data.size());
    }

    d_data.clear();
}

void EncryptionSecret::append(bsl::int8_t value)
{
    d_data.push_back(static_cast<bsl::uint8_t>(value));
}

void EncryptionSecret::append(bsl::uint8_t value)
{
    d_data.push_back(value);
}

void EncryptionSecret::append(const void* data, bsl::size_t size)
{
    d_data.insert(d_data.end(),
                  reinterpret_cast<const bsl::uint8_t*>(data),
                  reinterpret_cast<const bsl::uint8_t*>(data) + size);
}

bsl::size_t EncryptionSecret::copy(void*       destination,
                                   bsl::size_t capacity) const
{
    if (!d_data.empty()) {
        bsl::size_t numToWrite = d_data.size();
        if (numToWrite > capacity) {
            numToWrite = capacity;
        }

        bsl::memcpy(destination, &d_data.front(), numToWrite);

        return numToWrite;
    }
    else {
        return 0;
    }
}

const void* EncryptionSecret::data() const
{
    if (!d_data.empty()) {
        return &d_data.front();
    }
    else {
        return 0;
    }
}

bsl::size_t EncryptionSecret::size() const
{
    return d_data.size();
}

bool EncryptionSecret::empty() const
{
    return d_data.empty();
}

bool EncryptionSecret::equals(const EncryptionSecret& other) const
{
    return d_data == other.d_data;
}

bool EncryptionSecret::less(const EncryptionSecret& other) const
{
    return d_data < other.d_data;
}

bsl::ostream& EncryptionSecret::print(bsl::ostream& stream,
                                      int           level,
                                      int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_data.empty()) {
        printer.printAttribute("data", "<secret>");
    }

    printer.end();
    return stream;
}

bsl::ostream& operator<<(bsl::ostream& stream, const EncryptionSecret& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionSecret& lhs, const EncryptionSecret& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionSecret& lhs, const EncryptionSecret& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionSecret& lhs, const EncryptionSecret& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
