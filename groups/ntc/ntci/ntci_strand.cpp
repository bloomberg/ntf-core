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

namespace {

bslmt::ThreadUtil::Key        s_key;
bsl::shared_ptr<ntci::Strand> s_null_sp;

/// Provide utilities for process-wide initialization of the
/// state necessary to manage strands.
class Initializer
{
  public:
    /// Create the process-wide state necessary to manage strands.
    Initializer();

    /// Destroy the process-wide state necessary to manage strands.
    ~Initializer();
};

Initializer::Initializer()
{
    int rc = bslmt::ThreadUtil::createKey(&s_key, 0);
    BSLS_ASSERT_OPT(rc == 0);
}

Initializer::~Initializer()
{
    // int rc = bslmt::ThreadUtil::deleteKey(s_key);
    // BSLS_ASSERT_OPT(rc == 0);
}

Initializer s_initializer;

}  // close unnamed namespace

Strand::~Strand()
{
}

const bsl::shared_ptr<ntci::Strand>& Strand::unspecified()
{
    return s_null_sp;
}

const bsl::shared_ptr<ntci::Strand>& Strand::unknown()
{
    return s_null_sp;
}

ntci::Strand* Strand::setThreadLocal(ntci::Strand* strand)
{
    ntci::Strand* previous =
        reinterpret_cast<ntci::Strand*>(bslmt::ThreadUtil::getSpecific(s_key));

    int rc = bslmt::ThreadUtil::setSpecific(
        s_key,
        const_cast<const void*>(static_cast<void*>(strand)));
    BSLS_ASSERT_OPT(rc == 0);

    return previous;
}

ntci::Strand* Strand::getThreadLocal()
{
    ntci::Strand* current =
        reinterpret_cast<ntci::Strand*>(bslmt::ThreadUtil::getSpecific(s_key));

    return current;
}

}  // close package namespace
}  // close enterprise namespace
