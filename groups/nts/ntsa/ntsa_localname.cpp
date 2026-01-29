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
#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>

#include <bslim_printer.h>
#include <bslmf_assert.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_sstream.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <sys/un.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <ws2def.h>
// clang-format on
#endif
#if defined(BSLS_PLATFORM_OS_WINDOWS)
#include <afunix.h>
#endif

namespace BloombergLP {
namespace ntsa {

LocalName::LocalName()
: d_size(0)
, d_abstract(false)
{
    bsl::memset(d_path, 0, k_MAX_PATH_LENGTH);
#if defined(BSLS_PLATFORM_OS_AIX)
    BSLMF_ASSERT(k_MAX_PATH_LENGTH <= (sizeof(sockaddr_un().sun_path) - 1));
#else
    BSLMF_ASSERT(k_MAX_PATH_LENGTH == (sizeof(sockaddr_un().sun_path) - 1));
#endif
}

LocalName::LocalName(bslmf::MovableRef<LocalName> original) NTSCFG_NOEXCEPT
{
    bsl::memcpy(this, &NTSCFG_MOVE_ACCESS(original), sizeof(LocalName));
    NTSCFG_MOVE_RESET(original);
}

LocalName::LocalName(const LocalName& other)
{
    bsl::memcpy(this, &other, sizeof(LocalName));
}

LocalName::~LocalName()
{
}

LocalName& LocalName::operator=(bslmf::MovableRef<LocalName> other)
    NTSCFG_NOEXCEPT
{
    if (this != &NTSCFG_MOVE_ACCESS(other)) {
        bsl::memcpy(this, &NTSCFG_MOVE_ACCESS(other), sizeof(LocalName));
        NTSCFG_MOVE_RESET(other);
    }

    return *this;
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
    bsl::memset(d_path, 0, k_MAX_PATH_LENGTH);
    d_size     = 0;
    d_abstract = false;
}

ntsa::Error LocalName::setAbstract()
{
#if defined(BSLS_PLATFORM_OS_LINUX)
    if (d_size == k_MAX_PATH_LENGTH) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }
    d_abstract = true;
    return ntsa::Error();
#else
    d_abstract = false;
    return ntsa::Error(ENOTSUP);
#endif
}

ntsa::Error LocalName::setPersistent()
{
    d_abstract = false;
    return ntsa::Error();
}

ntsa::Error LocalName::setUnnamed()
{
    d_size = 0;
    return ntsa::Error();
}

ntsa::Error LocalName::setValue(const bslstl::StringRef& value)
{
    const bsl::size_t size = value.size();

    if (size > (ntsa::LocalName::k_MAX_PATH_LENGTH -
                static_cast<unsigned>(d_abstract)))
    {
        bsl::string path = value;
        BSLS_LOG_WARN(
            "Unix domain socket name '%s' length %d exceeds maximum length %d",
            path.c_str(),
            (int)(size),
            (int)(ntsa::LocalName::k_MAX_PATH_LENGTH -
            static_cast<unsigned>(d_abstract)));
        return ntsa::Error(ntsa::Error::e_LIMIT);
    }

    bsl::memcpy(d_path, value.data(), size);

    d_size = static_cast<bsl::uint8_t>(size);

    return ntsa::Error();
}

bslstl::StringRef LocalName::value() const
{
    return bslstl::StringRef(d_path, d_size);
}

bool LocalName::isAbstract() const
{
    return d_abstract;
}

bool LocalName::isPersistent() const
{
    return !d_abstract;
}

bool LocalName::isAbsolute() const
{
    return bdls::PathUtil::isAbsolute(d_path);
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
    if (this == &other) {
        return true;
    }

    if (d_size != other.d_size) {
        return false;
    }

    if (d_abstract != other.d_abstract) {
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
    else if (isAbstract()) {
        stream << "(abstract): " << bslstl::StringRef(d_path, d_size);
    }
    else {
        stream << bslstl::StringRef(d_path, d_size);
    }

    printer.end(true);

    return stream;
}

ntsa::LocalName LocalName::generateUnique()
{
    ntsa::Error error;

    ntsa::LocalName name;
    error = ntsa::LocalName::generateUnique(&name);
    if (error) {
        bsl::abort();
    }

    return name;
}

ntsa::Error LocalName::generateUnique(ntsa::LocalName* name)
{
    return ntsa::LocalName::generateUnique(
        name, 
        ntsa::LocalName::defaultDirectory());
}

ntsa::Error LocalName::generateUnique(ntsa::LocalName*   name, 
                                      const bsl::string& directory)
{
    ntsa::Error error;
    int         rc;

    name->reset();

    bsl::string basename;
    {
        bdlb::Guid guid = bdlb::GuidUtil::generate();

        bsl::stringstream ss;
        ss << "ntf-" << guid;

        basename = ss.str();
    }

    bsl::string path = directory;
    rc = bdls::PathUtil::appendIfValid(&path, basename);
    if (rc != 0) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

#if defined(BSLS_PLATFORM_OS_LINUX)
    name->setAbstract();
#endif

    error = name->setValue(path);
    if (error) {
        name->reset();
        return error;
    }

    return ntsa::Error();
}

bsl::string LocalName::defaultDirectory()
{
    int         rc;
    bsl::string path;

    if (path.empty()) {
        const char* sockDir = bsl::getenv("SOCKDIR");
        if (sockDir != 0) {
            const bsl::size_t sockDirLength = bsl::strlen(sockDir);
            if (sockDirLength < ntsa::LocalName::k_MAX_PATH_LENGTH) {
                path = sockDir;
            }
        }
    }
    
    if (path.empty()) {
        bsl::string tempDir;
        rc = bdls::FilesystemUtil::getSystemTemporaryDirectory(&tempDir);
        if (rc == 0) {
            const bsl::size_t tempDirLength = tempDir.size();
            if (tempDirLength < ntsa::LocalName::k_MAX_PATH_LENGTH) {
                path = tempDir;
            }
        }
    }

    if (path.empty()) {
#if defined(BSLS_PLATFORM_OS_UNIX)
        path = "/tmp";
#else
        path = "C:\\Windows\\Temp";
#endif
    }

    return path;
}

const bdlat_AttributeInfo* LocalName::lookupAttributeInfo(int id)
{
    const int numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    if (id < 0 || id >= numAttributes) {
        return 0;
    }

    return &ATTRIBUTE_INFO_ARRAY[id];
}

const bdlat_AttributeInfo* LocalName::lookupAttributeInfo(const char* name,
                                                          int nameLength)
{
    const bsl::size_t numAttributes =
        sizeof(ATTRIBUTE_INFO_ARRAY) / sizeof(ATTRIBUTE_INFO_ARRAY[0]);

    for (bsl::size_t i = 0; i < numAttributes; ++i) {
        const bdlat_AttributeInfo& attributeInfo = ATTRIBUTE_INFO_ARRAY[i];
        if (attributeInfo.d_nameLength == nameLength) {
            const int compare =
                bsl::memcmp(attributeInfo.d_name_p, name, nameLength);
            if (compare == 0) {
                return &attributeInfo;
            }
        }
    }

    return 0;
}

const char LocalName::CLASS_NAME[16] = "ntsa::LocalName";

// clang-format off
const bdlat_AttributeInfo LocalName::ATTRIBUTE_INFO_ARRAY[2] =
{
    { e_ATTRIBUTE_ID_PATH,     "path",     4, "", 0 },
    { e_ATTRIBUTE_ID_ABSTRACT, "abstract", 8, "", 0 },
};
// clang-format on

}  // close package namespace
}  // close enterprise namespace
