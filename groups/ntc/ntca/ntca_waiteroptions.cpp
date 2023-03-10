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

#include <ntca_waiteroptions.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntca_waiteroptions_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntca {

WaiterOptions::WaiterOptions(bslma::Allocator* basicAllocator)
: d_metricName(basicAllocator)
, d_threadHandle()
, d_threadIndex()
{
}

WaiterOptions::WaiterOptions(const WaiterOptions& original,
                             bslma::Allocator*    basicAllocator)
: d_metricName(original.d_metricName, basicAllocator)
, d_threadHandle(original.d_threadHandle)
, d_threadIndex(original.d_threadIndex)
{
}

WaiterOptions::~WaiterOptions()
{
}

WaiterOptions& WaiterOptions::operator=(const WaiterOptions& other)
{
    if (this != &other) {
        d_metricName   = other.d_metricName;
        d_threadHandle = other.d_threadHandle;
        d_threadIndex  = other.d_threadIndex;
    }

    return *this;
}

void WaiterOptions::setMetricName(const bsl::string& metricName)
{
    d_metricName = metricName;
}

void WaiterOptions::setThreadHandle(bslmt::ThreadUtil::Handle threadHandle)
{
    d_threadHandle = threadHandle;
}

void WaiterOptions::setThreadIndex(bsl::size_t threadIndex)
{
    d_threadIndex = threadIndex;
}

const bsl::string& WaiterOptions::metricName() const
{
    return d_metricName;
}

bslmt::ThreadUtil::Handle WaiterOptions::threadHandle() const
{
    return d_threadHandle;
}

const bdlb::NullableValue<bsl::size_t>& WaiterOptions::threadIndex() const
{
    return d_threadIndex;
}

}  // close package namespace
}  // close enterprise namespace
