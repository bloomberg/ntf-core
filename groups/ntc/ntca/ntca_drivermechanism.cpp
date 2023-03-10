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

#include <ntca_drivermechanism.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_drivermechanism_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

DriverMechanism::DriverMechanism(bslma::Allocator* basicAllocator)
: d_type(e_UNDEFINED)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

DriverMechanism::DriverMechanism(const DriverMechanism& original,
                                 bslma::Allocator*      basicAllocator)
: d_type(original.d_type)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
    if (original.isLibUv()) {
        new (d_libUv.buffer()) LibUvType(original.libUv());
    }
}

DriverMechanism::~DriverMechanism()
{
    this->reset();
}

DriverMechanism& DriverMechanism::operator=(const DriverMechanism& other)
{
    if (this == &other) {
        return *this;
    }

    this->reset();

    if (other.isLibUv()) {
        new (d_libUv.buffer()) LibUvType(other.libUv());
    }

    d_type = other.d_type;

    return *this;
}

void DriverMechanism::reset()
{
    if (isLibUv()) {
        d_libUv.object().~LibUvType();
    }

    d_type = e_UNDEFINED;
}

DriverMechanism::LibUvType& DriverMechanism::makeLibUv()
{
    if (isLibUv()) {
        d_libUv.object() = 0;
    }
    else {
        reset();
        new (d_libUv.buffer()) LibUvType();
        d_type = e_LIBUV;
    }

    return d_libUv.object();
}

DriverMechanism::LibUvType& DriverMechanism::makeLibUv(const LibUvType& loop)
{
    if (isLibUv()) {
        d_libUv.object() = loop;
    }
    else {
        reset();
        new (d_libUv.buffer()) LibUvType(loop);
        d_type = e_LIBUV;
    }

    return d_libUv.object();
}

const DriverMechanism::LibUvType& DriverMechanism::libUv() const
{
    BSLS_ASSERT(d_type == e_LIBUV);
    return d_libUv.object();
}

bool DriverMechanism::isUndefined() const
{
    return d_type == e_UNDEFINED;
}

bool DriverMechanism::isLibUv() const
{
    return d_type == e_LIBUV;
}

bool DriverMechanism::equals(const DriverMechanism& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (isLibUv()) {
        return d_libUv.object() == other.libUv();
    }

    return false;
}

bool DriverMechanism::less(const DriverMechanism& other) const
{
    if (d_type != other.d_type) {
        return false;
    }

    if (isLibUv()) {
        return d_libUv.object() < other.libUv();
    }

    return false;
}

bsl::ostream& DriverMechanism::print(bsl::ostream& stream,
                                     int           level,
                                     int           spacesPerLevel) const
{
    NTCCFG_WARNING_UNUSED(level);
    NTCCFG_WARNING_UNUSED(spacesPerLevel);

    if (isLibUv()) {
        return stream << static_cast<void*>(d_libUv.object());
    }

    return stream << "UNDEFINED";
}

bslma::Allocator* DriverMechanism::allocator() const
{
    return d_allocator_p;
}

}  // close package namespace
}  // close enterprise namespace
