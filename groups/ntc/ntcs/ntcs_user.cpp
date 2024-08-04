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

#include <ntcs_user.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_user_cpp, "$Id$ $CSID$")

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

User::User(bslma::Allocator* basicAllocator)
: d_dataPool_sp()
, d_resolver_sp()
, d_chronology_sp()
, d_connectionLimiter_sp()
, d_reactorMetrics_sp()
, d_proactorMetrics_sp()
, d_allocator_p(bslma::Default::allocator(basicAllocator))
{
}

User::~User()
{
}

void User::setDataPool(const bsl::shared_ptr<ntci::DataPool>& dataPool)
{
    d_dataPool_sp = dataPool;
}

void User::setResolver(const bsl::shared_ptr<ntci::Resolver>& resolver)
{
    d_resolver_sp = resolver;
}

void User::setConnectionLimiter(
    const bsl::shared_ptr<ntci::Reservation>& connectionLimiter)
{
    d_connectionLimiter_sp = connectionLimiter;
}

void User::setReactorMetrics(
    const bsl::shared_ptr<ntci::ReactorMetrics>& reactorMetrics)
{
    d_reactorMetrics_sp = reactorMetrics;
}

void User::setProactorMetrics(
    const bsl::shared_ptr<ntci::ProactorMetrics>& proactorMetrics)
{
    d_proactorMetrics_sp = proactorMetrics;
}

void User::setChronology(const bsl::shared_ptr<ntci::Chronology>& chronology)
{
    d_chronology_sp = chronology;
}

const bsl::shared_ptr<ntci::DataPool>& User::dataPool() const
{
    return d_dataPool_sp;
}

const bsl::shared_ptr<ntci::Resolver>& User::resolver() const
{
    return d_resolver_sp;
}

const bsl::shared_ptr<ntci::Chronology>& User::chronology() const
{
    return d_chronology_sp;
}

const bsl::shared_ptr<ntci::Reservation>& User::connectionLimiter() const
{
    return d_connectionLimiter_sp;
}

const bsl::shared_ptr<ntci::ReactorMetrics>& User::reactorMetrics() const
{
    return d_reactorMetrics_sp;
}

const bsl::shared_ptr<ntci::ProactorMetrics>& User::proactorMetrics() const
{
    return d_proactorMetrics_sp;
}

}  // close package namespace
}  // close enterprise namespace
