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

#include <ntsa_temporary.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_temporary_cpp, "$Id$ $CSID$")

#include <bdls_pathutil.h>
#include <bsls_assert.h>
#include <bsls_platform.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>

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
// clang-format on
#endif

namespace BloombergLP {
namespace ntsa {

// Provide utilities for implementing temporary directory and file guards.
// This struct is thread safe.
class TempUtil
{
  public:
    // Return the effective temporary directory defined for the user and
    // the system. The result is guaranteed to have a trailing path
    // separator, even if the definition of the environment variable does
    // not.
    static bsl::string tempDir();
};

#if defined(BSLS_PLATFORM_OS_UNIX)

bsl::string TempUtil::tempDir()
{
    const char* tmp;
    tmp = std::getenv("TMPDIR");
    if (tmp == 0) {
        tmp = std::getenv("TMP");
        if (tmp == 0) {
            tmp = std::getenv("TEMP");
            if (tmp == 0) {
                tmp = "/tmp/";
            }
        }
    }

    char buffer[PATH_MAX + 1];

    std::size_t length = std::strlen(tmp);
    BSLS_ASSERT_OPT(sizeof buffer >= length + 1);

    std::memcpy(buffer, tmp, length);
    buffer[length] = 0;

    bsl::string result(buffer);

    if (result.empty()) {
        result = ".";
    }

    if (result[result.size() - 1] != '/') {
        result.push_back('/');
    }

    return result;
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

bsl::string TempUtil::tempDir()
{
    char buffer[MAX_PATH + 1];

    const char* tmp;

    tmp = std::getenv("TMPDIR");
    if (tmp != 0) {
        std::size_t length = std::strlen(tmp);
        BSLS_ASSERT_OPT(sizeof buffer >= length + 1);

        std::memcpy(buffer, tmp, length);
        buffer[length] = 0;
    }
    else {
        DWORD rc = GetTempPathA(static_cast<DWORD>(sizeof buffer), buffer);
        BSLS_ASSERT_OPT(0 != rc);
    }

    bsl::string result(buffer);

    if (result.empty()) {
        result = ".";
    }

    if (result[result.size() - 1] != '/' && result[result.size() - 1] != '\\')
    {
        result.push_back('\\');
    }

    return result;
}

#else
#error Not implemented
#endif

TemporaryDirectory::TemporaryDirectory(bslma::Allocator* basicAllocator)
: d_path(basicAllocator)
, d_keep(false)
{
    bsl::string prefix = TempUtil::tempDir();

    int rc = bdls::FilesystemUtil::createTemporaryDirectory(&d_path, prefix);
    BSLS_ASSERT_OPT(rc == 0);
}

TemporaryDirectory::~TemporaryDirectory()
{
    if (!d_keep) {
        int rc = bdls::FilesystemUtil::remove(d_path, true);
        BSLS_ASSERT_OPT(rc == 0);
    }
}

void TemporaryDirectory::keep()
{
    d_keep = true;
}

const bsl::string& TemporaryDirectory::path() const
{
    return d_path;
}

TemporaryFile::TemporaryFile(bslma::Allocator* basicAllocator)
: d_path(basicAllocator)
, d_keep(false)
{
    bsl::string prefix = TempUtil::tempDir();

    bdls::FilesystemUtil::FileDescriptor descriptor =
        bdls::FilesystemUtil::createTemporaryFile(&d_path, prefix);

    BSLS_ASSERT_OPT(descriptor != bdls::FilesystemUtil::k_INVALID_FD);

    int rc = bdls::FilesystemUtil::close(descriptor);
    BSLS_ASSERT_OPT(rc == 0);
}

TemporaryFile::TemporaryFile(ntsa::TemporaryDirectory* tempDirectory,
                             bslma::Allocator*         basicAllocator)
: d_path(basicAllocator)
, d_keep(false)
{
    bsl::string prefix = tempDirectory->path();

#if defined(BSLS_PLATFORM_OS_UNIX)
    if (prefix[prefix.size() - 1] != '/') {
        prefix.push_back('/');
    }
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    if (prefix[prefix.size() - 1] != '\\') {
        prefix.push_back('\\');
    }
#else
#error Not implemented
#endif

    bdls::FilesystemUtil::FileDescriptor descriptor =
        bdls::FilesystemUtil::createTemporaryFile(&d_path, prefix);

    BSLS_ASSERT_OPT(descriptor != bdls::FilesystemUtil::k_INVALID_FD);

    int rc = bdls::FilesystemUtil::close(descriptor);
    BSLS_ASSERT_OPT(rc == 0);
}

TemporaryFile::TemporaryFile(ntsa::TemporaryDirectory* tempDirectory,
                             const bsl::string&        filename,
                             bslma::Allocator*         basicAllocator)
: d_path(basicAllocator)
, d_keep(false)
{
    int rc;

    d_path = tempDirectory->path();

#if defined(BSLS_PLATFORM_OS_UNIX)
    if (d_path[d_path.size() - 1] != '/') {
        d_path.push_back('/');
    }
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
    if (d_path[d_path.size() - 1] != '\\') {
        d_path.push_back('\\');
    }
#else
#error Not implemented
#endif

    rc = bdls::PathUtil::appendIfValid(&d_path, filename.c_str());
    BSLS_ASSERT_OPT(rc == 0);

    bdls::FilesystemUtil::FileDescriptor descriptor =
        bdls::FilesystemUtil::open(d_path.c_str(),
                                   bdls::FilesystemUtil::e_CREATE,
                                   bdls::FilesystemUtil::e_READ_WRITE);

    BSLS_ASSERT_OPT(descriptor != bdls::FilesystemUtil::k_INVALID_FD);

    rc = bdls::FilesystemUtil::close(descriptor);
    BSLS_ASSERT_OPT(rc == 0);
}

TemporaryFile::~TemporaryFile()
{
    if (!d_keep) {
        int rc = bdls::FilesystemUtil::remove(d_path, false);
        BSLS_ASSERT_OPT(rc == 0);
    }
}

void TemporaryFile::keep()
{
    d_keep = true;
}

ntsa::Error TemporaryFile::write(const bsl::string& content)
{
    int rc;

    bdls::FilesystemUtil::FileDescriptor descriptor =
        bdls::FilesystemUtil::open(d_path.c_str(),
                                   bdls::FilesystemUtil::e_OPEN,
                                   bdls::FilesystemUtil::e_READ_WRITE);

    BSLS_ASSERT_OPT(descriptor != bdls::FilesystemUtil::k_INVALID_FD);

    const char* p = content.c_str();
    bsl::size_t c = content.size();

    while (c != 0) {
        const bsl::size_t k_CHUNK_SIZE = 1024 * 32;
        bsl::size_t numBytesToWrite    = c < k_CHUNK_SIZE ? c : k_CHUNK_SIZE;

        rc = bdls::FilesystemUtil::write(descriptor,
                                         p,
                                         static_cast<int>(numBytesToWrite));
        if (rc < 0) {
            return ntsa::Error::last();
        }

        bsl::size_t numBytesWritten = static_cast<bsl::size_t>(rc);

        BSLS_ASSERT_OPT(c >= numBytesWritten);

        p += numBytesWritten;
        c -= numBytesWritten;
    }

    rc = bdls::FilesystemUtil::close(descriptor);
    BSLS_ASSERT_OPT(rc == 0);

    return ntsa::Error();
}

const bsl::string& TemporaryFile::path() const
{
    return d_path;
}

}  // close package namespace
}  // close enterprise namespace
