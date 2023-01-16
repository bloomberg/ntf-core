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

#include <ntsscm_version.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsscm_version_cpp, "$Id$ $CSID$")

#include <bdlb_numericparseutil.h>
#include <bslscm_patchversion.h>
#include <bsl_cstring.h>
#include <bsl_string_view.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif

namespace BloombergLP {
namespace ntsscm {

#define STRINGIFY2(a) #a
#define STRINGIFY(a) STRINGIFY2(a)

#define NTSSCM_VERSION_STRING                                                 \
    "BLP_LIB_BDE_NTS_" STRINGIFY(NTS_VERSION_MAJOR) "." STRINGIFY(            \
        NTS_VERSION_MINOR) "." STRINGIFY(NTS_VERSION_PATCH)

const char* Version::s_ident = "$Id: " NTSSCM_VERSION_STRING " $";
const char* Version::s_what  = "@(#)" NTSSCM_VERSION_STRING;

const char* Version::NTSSCM_S_VERSION    = NTSSCM_VERSION_STRING;
const char* Version::s_dependencies      = "";
const char* Version::s_buildInfo         = "";
const char* Version::s_timestamp         = "";
const char* Version::s_sourceControlInfo = "";

#if defined(BSLS_PLATFORM_OS_UNIX)

int Version::systemVersion(int* major, int* minor, int* patch, int* build)
{
    int rc;

    *major = 0;
    *minor = 0;
    *patch = 0;
    *build = 0;

    utsname u;
    bsl::memset(&u, 0, sizeof u);

    rc = uname(&u);
    if (rc < 0) {
        return rc;
    }

    bsl::string_view sysname(u.sysname, bsl::strlen(u.sysname));
    bsl::string_view release(u.release, bsl::strlen(u.release));
    bsl::string_view version(u.version, bsl::strlen(u.version));

#if defined(BSLS_PLATFORM_OS_AIX)

    rc = bdlb::NumericParseUtil::parseInt(major, version);
    if (rc != 0) {
        return rc;
    }

    rc = bdlb::NumericParseUtil::parseInt(minor, release);
    if (rc != 0) {
        return rc;
    }

    return 0;

#elif defined(BSLS_PLATFORM_OS_DARWIN)

    bsl::string_view input = release;
    bsl::string_view remainder;

    rc = bdlb::NumericParseUtil::parseInt(major, &remainder, input);
    if (rc != 0) {
        return rc;
    }

    if (!remainder.empty()) {
        if (remainder[0] != '.') {
            return 1;
        }

        input = remainder.substr(1);

        rc = bdlb::NumericParseUtil::parseInt(minor, &remainder, input);
        if (rc != 0) {
            return rc;
        }

        if (!remainder.empty()) {
            if (remainder[0] != '.') {
                return 1;
            }

            input = remainder.substr(1);

            rc = bdlb::NumericParseUtil::parseInt(patch, &remainder, input);
            if (rc != 0) {
                return rc;
            }
        }
    }

    return 0;

#elif defined(BSLS_PLATFORM_OS_LINUX)

    bsl::string_view input = release;
    bsl::string_view remainder;

    rc = bdlb::NumericParseUtil::parseInt(major, &remainder, input);
    if (rc != 0) {
        return rc;
    }

    if (!remainder.empty()) {
        if (remainder[0] != '.') {
            return 1;
        }

        input = remainder.substr(1);

        rc = bdlb::NumericParseUtil::parseInt(minor, &remainder, input);
        if (rc != 0) {
            return rc;
        }

        if (!remainder.empty()) {
            if (remainder[0] != '.') {
                return 1;
            }

            input = remainder.substr(1);

            rc = bdlb::NumericParseUtil::parseInt(patch, &remainder, input);
            if (rc != 0) {
                return rc;
            }
        }
    }

    return 0;

#elif defined(BSLS_PLATFORM_OS_SOLARIS)

    bsl::string_view input = version;
    bsl::string_view remainder;

    rc = bdlb::NumericParseUtil::parseInt(major, &remainder, input);
    if (rc != 0) {
        return rc;
    }

    if (!remainder.empty()) {
        if (remainder[0] != '.') {
            return 1;
        }

        input = remainder.substr(1);

        rc = bdlb::NumericParseUtil::parseInt(minor, &remainder, input);
        if (rc != 0) {
            return rc;
        }

        if (!remainder.empty()) {
            if (remainder[0] != '.') {
                return 1;
            }

            input = remainder.substr(1);

            rc = bdlb::NumericParseUtil::parseInt(patch, &remainder, input);
            if (rc != 0) {
                return rc;
            }
        }
    }

    return 0;

#else
    return -1;
#endif
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

int Version::systemVersion(int* major, int* minor, int* patch, int* build)
{
    *major = 0;
    *minor = 0;
    *patch = 0;
    *build = 0;

    OSVERSIONINFO info;
    bsl::memset(&info, 0, sizeof info);

    info.dwOSVersionInfoSize = sizeof info;

    BOOL getVersionExSuccess = GetVersionEx(&info);
    if (!getVersionExSuccess) {
        return -1;
    }

    *major = static_cast<int>(info.dwMajorVersion);
    *minor = static_cast<int>(info.dwMinorVersion);
    *patch = 0;

    *build = static_cast<int>(info.dwBuildNumber);

    return 0;
}

#else
#error Not implemented
#endif

}  // close namespace ntsscm
}  // close namespace BloombergLP
