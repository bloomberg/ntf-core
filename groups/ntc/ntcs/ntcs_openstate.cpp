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

#include <ntcs_openstate.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_openstate_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>

namespace BloombergLP {
namespace ntcs {

const char* OpenState::toString() const
{
    switch (d_value) {
    case e_DEFAULT:
        return "DEFAULT";
    case e_WAITING:
        return "WAITING";
    case e_CONNECTING:
        return "CONNECTING";
    case e_CONNECTED:
        return "CONNECTED";
    case e_CLOSED:
        return "CLOSED";
    }

    return "???";
}

}  // close package namespace
}  // close enterprise namespace
