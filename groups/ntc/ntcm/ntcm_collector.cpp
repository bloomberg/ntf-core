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

#include <ntcm_collector.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcm_collector_cpp, "$Id$ $CSID$")

#include <bdld_manageddatum.h>
#include <bdlma_bufferedsequentialallocator.h>
#include <bdlt_currenttime.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bsls_assert.h>
#include <bsls_timeinterval.h>

#include <bsl_vector.h>

namespace BloombergLP {
namespace ntcm {

namespace {

// The number of pools with which to configure a multipool allocator such
// that the smallest pool allocates 8 bytes and the largest pool allocates 1K.
const int POOLS_UP_TO_1K = 8;

}  // close unnamed namespace

Collector::Collector(const LoadCallback& loadCallback,
                     bslma::Allocator*   basicAllocator)
: d_mutex()
, d_publishers(basicAllocator)
, d_memoryPools(POOLS_UP_TO_1K, basicAllocator)
, d_loader(bsl::allocator_arg, basicAllocator, loadCallback)
, d_config(basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Collector::Collector(const ntca::MonitorableCollectorConfig& configuration,
                     const LoadCallback&                     loadCallback,
                     bslma::Allocator*                       basicAllocator)
: d_mutex()
, d_publishers(basicAllocator)
, d_memoryPools(POOLS_UP_TO_1K, basicAllocator)
, d_loader(bsl::allocator_arg, basicAllocator, loadCallback)
, d_config(configuration, basicAllocator)
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

Collector::~Collector()
{
}

void Collector::registerPublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher)
{
    LockGuard guard(&d_mutex);
    d_publishers.insert(publisher);
}

void Collector::deregisterPublisher(
    const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher)
{
    LockGuard guard(&d_mutex);
    d_publishers.erase(publisher);
}

void Collector::start()
{
}

void Collector::stop()
{
}

void Collector::collect()
{
    char                               arena[1024];
    bdlma::BufferedSequentialAllocator sequentialAllocator(arena,
                                                           sizeof arena,
                                                           &d_memoryPools);

    typedef bsl::vector<bsl::shared_ptr<ntci::MonitorablePublisher> >
                    PublisherVector;
    PublisherVector publishers(&sequentialAllocator);
    {
        LockGuard guard(&d_mutex);

        publishers.reserve(d_publishers.size());
        publishers.assign(d_publishers.begin(), d_publishers.end());
    }

    typedef bsl::vector<bsl::shared_ptr<ntci::Monitorable> > MonitorableVector;
    MonitorableVector monitorables(&sequentialAllocator);

    d_loader(&monitorables);

    bsls::TimeInterval now = bdlt::CurrentTime::now();

    for (MonitorableVector::const_iterator it = monitorables.begin();
         it != monitorables.end();
         ++it)
    {
        const bsl::shared_ptr<ntci::Monitorable>& monitorable = *it;

        const bool final = it == monitorables.end() - 1;

        bdld::ManagedDatum statistics(&sequentialAllocator);
        monitorable->getStats(&statistics);

        if (!statistics.datum().isArray()) {
            continue;
        }

        for (PublisherVector::const_iterator jt = publishers.begin();
             jt != publishers.end();
             ++jt)
        {
            const bsl::shared_ptr<ntci::MonitorablePublisher>& publisher = *jt;
            publisher->publish(monitorable, statistics.datum(), now, final);
        }
    }
}

const ntca::MonitorableCollectorConfig& Collector::configuration() const
{
    return d_config;
}

}  // close package namespace
}  // close enterprise namespace
