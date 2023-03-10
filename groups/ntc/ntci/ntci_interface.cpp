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

#include <ntci_interface.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_interface_cpp, "$Id$ $CSID$")

namespace BloombergLP {
namespace ntci {

Interface::~Interface()
{
}

InterfaceStopGuard::InterfaceStopGuard(
    const bsl::shared_ptr<ntci::Interface>& interface)
: d_interface_sp(interface)
{
}

InterfaceStopGuard::~InterfaceStopGuard()
{
    if (d_interface_sp) {
        d_interface_sp->shutdown();
        d_interface_sp->linger();
    }
}

bsl::shared_ptr<ntci::Interface> InterfaceStopGuard::release()
{
    bsl::shared_ptr<ntci::Interface> result = d_interface_sp;
    d_interface_sp.reset();
    return result;
}

}  // close package namespace
}  // close enterprise namespace
