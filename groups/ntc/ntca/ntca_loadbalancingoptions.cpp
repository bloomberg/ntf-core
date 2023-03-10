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

#include <ntca_loadbalancingoptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_loadbalancingoptions_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>

namespace BloombergLP {
namespace ntca {

LoadBalancingOptions::LoadBalancingOptions()
: d_threadHandle()
, d_threadIndex()
, d_weight()
{
}

void LoadBalancingOptions::setThreadHandle(
    bslmt::ThreadUtil::Handle threadHandle)
{
    d_threadHandle = threadHandle;
}

void LoadBalancingOptions::setThreadIndex(bsl::size_t threadIndex)
{
    d_threadIndex = threadIndex;
}

void LoadBalancingOptions::setWeight(bsl::size_t weight)
{
    d_weight = weight;
}

const bdlb::NullableValue<bslmt::ThreadUtil::Handle>& LoadBalancingOptions::
    threadHandle() const
{
    return d_threadHandle;
}

const bdlb::NullableValue<bsl::size_t>& LoadBalancingOptions::threadIndex()
    const
{
    return d_threadIndex;
}

const bdlb::NullableValue<bsl::size_t>& LoadBalancingOptions::weight() const
{
    return d_weight;
}

bsl::ostream& LoadBalancingOptions::print(bsl::ostream& stream,
                                          int           level,
                                          int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (!d_threadHandle.isNull()) {
        printer.printAttribute(
            "threadHandle",
            bslmt::ThreadUtil::idAsUint64(
                bslmt::ThreadUtil::handleToId(d_threadHandle.value())));
    }

    if (!d_threadIndex.isNull()) {
        printer.printAttribute("threadIndex", d_threadIndex.value());
    }

    if (!d_weight.isNull()) {
        printer.printAttribute("weight", d_weight.value());
    }
    printer.end();
    return stream;
}

bool operator==(const LoadBalancingOptions& lhs,
                const LoadBalancingOptions& rhs)
{
    if (!lhs.threadHandle().isNull() && !rhs.threadHandle().isNull()) {
        if (!bslmt::ThreadUtil::areEqual(lhs.threadHandle().value(),
                                         rhs.threadHandle().value()))
        {
            return false;
        }
    }
    else if (lhs.threadHandle().isNull() && !rhs.threadHandle().isNull()) {
        return false;
    }
    else if (!lhs.threadHandle().isNull() && rhs.threadHandle().isNull()) {
        return false;
    }

    return lhs.threadIndex() == rhs.threadIndex() &&
           lhs.weight() == rhs.weight();
}

bool operator!=(const LoadBalancingOptions& lhs,
                const LoadBalancingOptions& rhs)
{
    return !operator==(lhs, rhs);
}

bsl::ostream& operator<<(bsl::ostream&               stream,
                         const LoadBalancingOptions& object)
{
    return object.print(stream, 0, -1);
}

}  // close package namespace
}  // close enterprise namespace
