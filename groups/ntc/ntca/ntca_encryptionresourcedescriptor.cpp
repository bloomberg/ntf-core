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

#include <ntca_encryptionresourcedescriptor.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_encryptionresourcedescriptor_cpp, "$Id$ $CSID$")

#include <bdlb_print.h>
#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

EncryptionResourceDescriptor::EncryptionResourceDescriptor(
    bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

EncryptionResourceDescriptor::EncryptionResourceDescriptor(
    const EncryptionResourceDescriptor& original,
    bslma::Allocator*                   basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (d_type == e_PATH) {
        new (d_path.buffer())
            PathType(original.d_path.object(), d_allocator_p);
    }
    else if (d_type == e_DATA) {
        new (d_data.buffer())
            DataType(original.d_data.object(), d_allocator_p);
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionResourceDescriptor::~EncryptionResourceDescriptor()
{
    if (d_type == e_PATH) {
        d_path.object().~PathType();
    }
    else if (d_type == e_DATA) {
        d_data.object().~DataType();
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }
}

EncryptionResourceDescriptor& EncryptionResourceDescriptor::operator=(
    const EncryptionResourceDescriptor& other)
{
    if (this != &other) {
        if (other.d_type == e_PATH) {
            this->makePath(other.d_path.object());
        }
        else if (other.d_type == e_DATA) {
            this->makeData(other.d_data.object());
        }
        else {
            BSLS_ASSERT(other.d_type == e_UNDEFINED);
            this->reset();
        }
    }

    return *this;
}

void EncryptionResourceDescriptor::reset()
{
    if (d_type == e_PATH) {
        d_path.object().~PathType();
    }
    else if (d_type == e_DATA) {
        d_data.object().~DataType();
    }
    else {
        BSLS_ASSERT(d_type == e_UNDEFINED);
    }

    d_type = e_UNDEFINED;
}

bsl::string& EncryptionResourceDescriptor::makePath()
{
    if (d_type == e_PATH) {
        d_path.object().clear();
    }
    else {
        this->reset();
        new (d_path.buffer()) PathType(d_allocator_p);
        d_type = e_PATH;
    }

    return d_path.object();
}

bsl::string& EncryptionResourceDescriptor::makePath(const bsl::string& value)
{
    if (d_type == e_PATH) {
        d_path.object() = value;
    }
    else {
        this->reset();
        new (d_path.buffer()) PathType(value, d_allocator_p);
        d_type = e_PATH;
    }

    return d_path.object();
}

bsl::vector<char>& EncryptionResourceDescriptor::makeData()
{
    if (d_type == e_DATA) {
        d_data.object().clear();
    }
    else {
        this->reset();
        new (d_data.buffer()) DataType(d_allocator_p);
        d_type = e_DATA;
    }

    return d_data.object();
}

bsl::vector<char>& EncryptionResourceDescriptor::makeData(
    const bsl::vector<char>& value)
{
    if (d_type == e_DATA) {
        d_data.object() = value;
    }
    else {
        this->reset();
        new (d_data.buffer()) DataType(value, d_allocator_p);
        d_type = e_DATA;
    }

    return d_data.object();
}

bsl::string& EncryptionResourceDescriptor::path()
{
    BSLS_ASSERT(d_type == e_PATH);
    return d_path.object();
}

bsl::vector<char>& EncryptionResourceDescriptor::data()
{
    BSLS_ASSERT(d_type == e_DATA);
    return d_data.object();
}

const bsl::string& EncryptionResourceDescriptor::path() const
{
    BSLS_ASSERT(d_type == e_PATH);
    return d_path.object();
}

const bsl::vector<char>& EncryptionResourceDescriptor::data() const
{
    BSLS_ASSERT(d_type == e_DATA);
    return d_data.object();
}

bool EncryptionResourceDescriptor::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool EncryptionResourceDescriptor::isPath() const
{
    return d_type == e_PATH;
}

bool EncryptionResourceDescriptor::isData() const
{
    return d_type == e_DATA;
}

bool EncryptionResourceDescriptor::equals(
    const EncryptionResourceDescriptor& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_PATH) {
        return d_path.object() == other.d_path.object();
    }
    else if (d_type == e_DATA) {
        return d_data.object() == other.d_data.object();
    }

    return true;
}

bool EncryptionResourceDescriptor::less(
    const EncryptionResourceDescriptor& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (d_type == e_PATH) {
        return d_path.object() < other.d_path.object();
    }
    else if (d_type == e_DATA) {
        return d_data.object() < other.d_data.object();
    }

    return true;
}

bsl::ostream& EncryptionResourceDescriptor::print(bsl::ostream& stream,
                                                  int           level,
                                                  int spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    if (d_type == e_PATH) {
        stream << d_path.object();
    }
    else if (d_type == e_DATA) {
        stream << "<encoded>";
    }

    return stream;
}

bsl::ostream& operator<<(bsl::ostream&                       stream,
                         const EncryptionResourceDescriptor& object)
{
    return object.print(stream, 0, -1);
}

bool operator==(const EncryptionResourceDescriptor& lhs,
                const EncryptionResourceDescriptor& rhs)
{
    return lhs.equals(rhs);
}

bool operator!=(const EncryptionResourceDescriptor& lhs,
                const EncryptionResourceDescriptor& rhs)
{
    return !operator==(lhs, rhs);
}

bool operator<(const EncryptionResourceDescriptor& lhs,
               const EncryptionResourceDescriptor& rhs)
{
    return lhs.less(rhs);
}

}  // close package namespace
}  // close enterprise namespace
