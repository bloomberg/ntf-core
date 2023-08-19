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

#include <ntcs_proactordetachcontext.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_roactordetachcontext_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntcs {

ProactorDetachContext::ProactorDetachContext()
: d_processCounter()
, d_detachState(e_DETACH_NOT_REQUIRED)
{
}

unsigned int ProactorDetachContext::processCounter() const
{
    return d_processCounter.load();
}

unsigned int ProactorDetachContext::incrementProcessCounter()
{
    return ++d_processCounter;
}

unsigned int ProactorDetachContext::decrementProcessCounter()
{
    return --d_processCounter;
}

bool ProactorDetachContext::noDetach() const
{
    return d_detachState.load() == e_DETACH_NOT_REQUIRED;
}

bool ProactorDetachContext::trySetDetachScheduled()
{
    unsigned int val =
        d_detachState.testAndSwap(e_DETACH_REQUIRED, e_DETACH_SCHEDULED);
    return val == e_DETACH_REQUIRED;
}

bool ProactorDetachContext::trySetDetachRequired()
{
    unsigned int val =
        d_detachState.testAndSwap(e_DETACH_NOT_REQUIRED, e_DETACH_REQUIRED);
    return val == e_DETACH_NOT_REQUIRED;
}

}  // close package namespace
}  // close enterprise namespace
