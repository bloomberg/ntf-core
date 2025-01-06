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

#ifndef INCLUDED_NTCA_WAITEROPTIONS
#define INCLUDED_NTCA_WAITEROPTIONS

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bslmt_threadutil.h>
#include <bsl_memory.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntca {

/// Describe the configuration of a thread driving a reactor or proactor.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b metricName:
/// The metric name of the thread driving the reactor or proactor. If the
/// metric name is not explicitly set, the metric name is derived from the
/// metric name of the reactor or proactor.
///
/// @li @b threadHandle:
/// The handle of the thread driving the reactor or proactor. If the thread
/// handle is not explicitly set, the thread handle is the handle to the thread
/// that registers the waiter.
///
/// @li @b threadIndex:
/// The index of the thread driving the reactor or proactor. If the thread
/// index is not explicitly set, the thread index is the index to the thread
/// that registers the waiter.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_ntci_runtime
class WaiterOptions
{
    bsl::string                      d_metricName;
    bslmt::ThreadUtil::Handle        d_threadHandle;
    bdlb::NullableValue<bsl::size_t> d_threadIndex;

  public:
    /// Create new waiter options. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit WaiterOptions(bslma::Allocator* basicAllocator = 0);

    /// Create new waiter options having the same value as the specified
    /// 'original' object. Optionally specify a 'basicAllocator' used to
    /// supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    WaiterOptions(const WaiterOptions& original,
                  bslma::Allocator*    basicAllocator = 0);

    /// Destroy this object.
    ~WaiterOptions();

    /// Assign the value of the specified 'other' object to this object.
    /// Return a reference to this modifiable object.
    WaiterOptions& operator=(const WaiterOptions& other);

    /// Set the metric name to the specified 'metricName'. If the metric
    /// name is not explicitly set, the metric name is derived from the
    /// metric name of the reactor or proactor.
    void setMetricName(const bsl::string& metricName);

    /// Set the thread handle to the specified 'threadHandle'. If the thread
    /// handle is not explicitly set, the thread handle is the handle to
    /// the thread that registers the waiter.
    void setThreadHandle(bslmt::ThreadUtil::Handle threadHandle);

    /// Set the thread index to the specified 'threadIndex'. If the thread
    /// index is not explicitly set, the thread index is the index to
    /// the thread that registers the waiter.
    void setThreadIndex(bsl::size_t threadIndex);

    /// Return the metric name.
    const bsl::string& metricName() const;

    /// Return the thread handle.
    bslmt::ThreadUtil::Handle threadHandle() const;

    /// Return the thread index.
    const bdlb::NullableValue<bsl::size_t>& threadIndex() const;
};

}  // close package namespace
}  // close enterprise namespace
#endif
