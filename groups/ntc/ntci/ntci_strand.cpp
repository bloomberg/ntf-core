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

#include <ntci_strand.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_strand_cpp, "$Id$ $CSID$")

#include <bslmt_threadutil.h>

namespace BloombergLP {
namespace ntci {

/// Describe the process-wide strand state.
class StrandState
{
  public:
    /// Create the process-wide state necessary to manage strands.
    StrandState();

    /// Destroy the process-wide state necessary to manage strands.
    ~StrandState();

    /// The process-wide strand thread-local key.
    bslmt::ThreadUtil::Key d_key;

    /// The null strand indicating the unknown or unspecified strand.
    bsl::shared_ptr<ntci::Strand> d_null_sp;

    /// The global strand state.
    static StrandState s_global;

  private:
    StrandState(const StrandState&);
    StrandState& operator=(const StrandState&);
};

StrandState StrandState::s_global;

StrandState::StrandState()
{
    int rc = bslmt::ThreadUtil::createKey(&StrandState::s_global.d_key, 0);
    BSLS_ASSERT_OPT(rc == 0);
}

StrandState::~StrandState()
{
    // int rc = bslmt::ThreadUtil::deleteKey(StrandState::s_global.d_key);
    // BSLS_ASSERT_OPT(rc == 0);
}

Strand::~Strand()
{
}

const bsl::shared_ptr<ntci::Strand>& Strand::unspecified()
{
    return StrandState::s_global.d_null_sp;
}

const bsl::shared_ptr<ntci::Strand>& Strand::unknown()
{
    return StrandState::s_global.d_null_sp;
}

ntci::Strand* Strand::setThreadLocal(ntci::Strand* strand)
{
    int rc;

    ntci::Strand* previous = reinterpret_cast<ntci::Strand*>(
        bslmt::ThreadUtil::getSpecific(StrandState::s_global.d_key));

    rc = bslmt::ThreadUtil::setSpecific(
        StrandState::s_global.d_key,
        const_cast<const void*>(static_cast<void*>(strand)));
    BSLS_ASSERT_OPT(rc == 0);

    return previous;
}

ntci::Strand* Strand::getThreadLocal()
{
    ntci::Strand* current = reinterpret_cast<ntci::Strand*>(
        bslmt::ThreadUtil::getSpecific(StrandState::s_global.d_key));

    return current;
}

}  // close package namespace
}  // close enterprise namespace
