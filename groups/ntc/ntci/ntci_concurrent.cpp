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

#include <ntci_concurrent.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_concurrent_cpp, "$Id$ $CSID$")

#if NTC_BUILD_WITH_COROUTINES
namespace BloombergLP {
namespace ntci {

void Concurrent::initialize()
{
}

void Concurrent::exit()
{
}

}  // close namespace ntci
}  // close namespace BloombergLP
#endif
