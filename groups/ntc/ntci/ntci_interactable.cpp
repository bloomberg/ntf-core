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

#include <ntci_interactable.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_interactable_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

Interactable::~Interactable()
{
}

ntsa::Error Interactable::interact(bsl::string*       output,
                                   const bsl::string& command)
{
    NTCCFG_WARNING_UNUSED(command);

    output->clear();

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

InteractableRegistry::~InteractableRegistry()
{
}

}  // close package namespace
}  // close enterprise namespace
