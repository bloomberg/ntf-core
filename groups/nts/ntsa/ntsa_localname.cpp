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

#include <ntsa_localname.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_localname_cpp, "$Id$ $CSID$")

#include <bdlb_guid.h>
#include <bdlb_guidutil.h>

#include <bslim_printer.h>
#include <bslmf_assert.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_sstream.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <windows.h>
#endif

namespace BloombergLP {
namespace ntsa {

LocalName::LocalName()
: d_size(0)
, d_abstract(0)
, d_unused(0)
{
    BSLMF_ASSERT(sizeof(LocalName) == sizeof d_path + 1 + 1 + 1);
    BSLMF_ASSERT(sizeof(LocalName) == 96);
}

LocalName::LocalName(const LocalName& other)
{
    bsl::memcpy(this, &other, sizeof(LocalName));
}

LocalName::~LocalName()
{
}

LocalName& LocalName::operator=(const LocalName& other)
{
    if (this != &other) {
        bsl::memcpy(this, &other, sizeof(LocalName));
    }

    return *this;
}

void LocalName::reset()
{
    d_size     = 0;
    d_abstract = 0;
    d_unused   = 0;
}

ntsa::Error LocalName::setAbstract()
{
#if defined(BSLS_PLATFORM_OS_LINUX)
    d_abstract = 1;
    return ntsa::Error();
#else
    d_abstract = 0;
    return ntsa::Error(ENOTSUP);
#endif
}

ntsa::Error LocalName::setPersistent()
{
    d_abstract = 0;
    return ntsa::Error();
}

ntsa::Error LocalName::setUnnamed()
{
    d_size = 0;
    return ntsa::Error();
}

ntsa::Error LocalName::setValue(const bslstl::StringRef& value)
{
    bsl::size_t size = value.size();

    if (size > ntsa::LocalName::k_MAX_PATH_LENGTH) {
        size = ntsa::LocalName::k_MAX_PATH_LENGTH;
    }

    bsl::memcpy(d_path, value.data(), size);

    d_path[size] = 0;
    d_size       = static_cast<bsl::uint8_t>(size);

    return ntsa::Error();
}

bslstl::StringRef LocalName::value() const
{
    return bslstl::StringRef(d_path, d_size);
}

bool LocalName::isAbstract() const
{
    return static_cast<bool>(d_abstract);
}

bool LocalName::isPersistent() const
{
    return !static_cast<bool>(d_abstract);
}

bool LocalName::isAbsolute() const
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    if (d_size > 0) {
        return d_path[0] == '/';
    }
    else {
        return false;
    }

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    // clang-format off
    if (d_size > 0) {
        if (d_path[0] == '/' || d_path[0] == '\\') {
            return true;
        }
        else {
            if (d_size > 3) {
                if (((d_path[0] >= 'A' && d_path[0] <= 'Z') ||
                     (d_path[0] >= 'a' && d_path[0] <= 'z')) 
                     &&
                     (d_path[1] == ':')
                     &&
                     (d_path[2] == '/' || d_path[2] == '\\')) {
                    return true;
                }
                else {
                    return false;
                }
            }
            else {
                return false;
            }
        }
    }
    else {
        return false;
    }
    // clang-format on

#else
#error Not implemented
#endif
}

bool LocalName::isRelative() const
{
    return !this->isAbsolute();
}

bool LocalName::isUnnamed() const
{
    return d_size == 0;
}

bool LocalName::equals(const LocalName& other) const
{
    if (d_size != other.d_size) {
        return false;
    }

    const char* first1 = d_path;
    const char* last1  = d_path + d_size;

    const char* first2 = other.d_path;
    const char* last2  = other.d_path + other.d_size;

    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        char ch1 = *first1;
        char ch2 = *first2;

        if (ch1 != ch2) {
            return false;
        }
    }

    return true;
}

bool LocalName::less(const LocalName& other) const
{
    const char* first1 = d_path;
    const char* last1  = d_path + d_size;

    const char* first2 = other.d_path;
    const char* last2  = other.d_path + other.d_size;

    for (; first1 != last1 && first2 != last2; ++first1, ++first2) {
        char ch1 = *first1;
        char ch2 = *first2;

        if (ch1 < ch2) {
            return true;
        }

        if (ch1 > ch2) {
            return false;
        }
    }

    return (first1 == last1 && first2 != last2);
}

bsl::ostream& LocalName::print(bsl::ostream& stream,
                               int           level,
                               int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start(true);

    if (isUnnamed()) {
        stream << "(unnamed)";
    }
    else {
        stream << bslstl::StringRef(d_path, d_size);
    }

    printer.end(true);

    return stream;
}

ntsa::LocalName LocalName::generateUnique()
{
#if defined(BSLS_PLATFORM_OS_UNIX)

    ntsa::LocalName localName;

#if defined(BSLS_PLATFORM_OS_LINUX)
    localName.setAbstract();
#endif

    {
        bdlb::Guid guid = bdlb::GuidUtil::generate();

        const char* tmp = bsl::getenv("TMPDIR");
        if (tmp == 0) {
            tmp = "/tmp";
        }

        bsl::stringstream ss;
        ss << tmp << "/ntf-" << guid;

        localName.setValue(ss.str());
    }

    return localName;

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

    ntsa::LocalName localName;

    {
        bdlb::Guid guid = bdlb::GuidUtil::generate();

        bsl::stringstream ss;

        const char* tmp = std::getenv("TMPDIR");
        if (tmp != 0) {
            ss << tmp << "/ntf-" << guid;
        }
        else {
            char  buffer[MAX_PATH + 1];
            DWORD rc = GetTempPathA(static_cast<DWORD>(sizeof buffer), buffer);
            if (rc <= 0 || rc >= static_cast<DWORD>(sizeof buffer)) {
                ss << "C:\\Windows\\Temp\\ntf-" << guid;
            }
            else {
                ss << buffer;

                if (buffer[rc - 1] != '/' && buffer[rc - 1] != '\\') {
                    ss << '\\';
                }

                ss << "ntf-" << guid;
            }
        }

        localName.setValue(ss.str());
    }

    return localName;

#else
#error Not implemented
#endif
}

}  // close package namespace
}  // close enterprise namespace
