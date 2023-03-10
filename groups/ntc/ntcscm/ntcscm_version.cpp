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

#include <ntcscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcscm_version_cpp, "$Id$ $CSID$")

#include <bslscm_patchversion.h>

namespace BloombergLP {
namespace ntcscm {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define NTCSCM_VERSION_STRING                                                 \
    "BLP_LIB_BDE_NTC_" STRINGIFY(NTC_VERSION_MAJOR) "." STRINGIFY(            \
        NTC_VERSION_MINOR) "." STRINGIFY(NTC_VERSION_PATCH)

const char* Version::s_ident = "$Id: " NTCSCM_VERSION_STRING " $";
const char* Version::s_what  = "@(#)" NTCSCM_VERSION_STRING;

const char* Version::NTCSCM_S_VERSION    = NTCSCM_VERSION_STRING;
const char* Version::s_dependencies      = "";
const char* Version::s_buildInfo         = "";
const char* Version::s_timestamp         = "";
const char* Version::s_sourceControlInfo = "";

}  // close namespace ntcscm
}  // close namespace BloombergLP
