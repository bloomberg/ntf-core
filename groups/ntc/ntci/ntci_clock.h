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

#ifndef INCLUDED_NTCI_CLOCK
#define INCLUDED_NTCI_CLOCK

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>

namespace BloombergLP {
namespace ntci {

/// Provide an interface to return the current time.
///
/// @par Thread Safety
/// This class is thread safe.
class Clock
{
  public:
    /// Destroy this object.
    virtual ~Clock();

    /// Return the current elapsed time since the Unix epoch.
    virtual bsls::TimeInterval currentTime() const = 0;
};

}  // end namespace ntci
}  // end namespace BloombergLP
#endif
