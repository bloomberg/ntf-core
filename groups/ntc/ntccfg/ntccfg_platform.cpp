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

#include <ntccfg_platform.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntccfg_platform_cpp, "$Id$ $CSID$")

#include <ntsf_system.h>
#include <bsls_log.h>
#include <bsl_cstdlib.h>
#include <bsl_cstdio.h>
#include <bsl_cstring.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <unistd.h>
#include <dlfcn.h>
extern char** environ;
#elif defined(BSLS_PLATFORM_OS_WINDOWS)
#include <direct.h>
#include <windows.h>
#include <libloaderapi.h>
#else
#error Not implemented
#endif

// Set to 1 to automatically initialize the library during C++ static
// initialization.
#define NTCCFG_PLATFORM_AUTO_INITIALIZE 0

namespace BloombergLP {
namespace ntccfg {

namespace {

#if NTCCFG_PLATFORM_AUTO_INITIALIZE

struct Initializer {
    // This struct defines a guard class to automatically initialize and
    // cleanup the necessary library state and platform conditions.

    Initializer();
    // Initialize the library state.

    ~Initializer();
    // Clean up the library state.
};

// The static object to automatically initialize and clean up the library
// state.
Initializer s_initializer;

Initializer::Initializer()
{
    ntccfg::Platform::initialize();
}

Initializer::~Initializer()
{
    ntccfg::Platform::exit();
}

#endif

}  // close unnamed namespace

int Platform::initialize()
{
    ntsa::Error error = ntsf::System::initialize();
    return error.number();
}

int Platform::ignore(ntscfg::Signal::Value signal)
{
    ntsa::Error error = ntsf::System::ignore(signal);
    return error.number();
}

#if defined(BSLS_PLATFORM_OS_UNIX)

int Platform::setEnvironmentVariable(const bsl::string& name,
                                     const bsl::string& value)
{
    return ::setenv(name.c_str(), value.c_str(), 1);
}

int Platform::getEnvironmentVariable(bsl::string*       result,
                                     const bsl::string& name)
{
    const char* value = ::getenv(name.c_str());
    if (value == 0) {
        return ENOENT;
    }

    result->assign(value);
    return 0;
}

int Platform::setWorkingDirectory(const bsl::string& value)
{
    return ::chdir(value.c_str());
}

int Platform::getWorkingDirectory(bsl::string* result)
{
    char temp[PATH_MAX];
    if (temp != ::getcwd(temp, sizeof temp)) {
        return errno;
    }

    result->assign(temp);
    return 0;
}


void* Platform::getDynamicObjectHandle()
{
    void* handle = dlopen(0, RTLD_LAZY);
    if (handle == 0) {
        bsl::fprintf(
            stderr, "%s: %s\n", "Failed to open handle to self", dlerror());
        return 0;
    }

    return handle;
}

void* Platform::getDynamicObjectHandle(const bsl::string& path)
{
    void* handle = dlopen(path.c_str(), RTLD_LAZY);
    if (handle == 0) {
        bsl::fprintf(
            stderr, "%s: %s\n", "Failed to open handle to self", dlerror());
        return 0;
    }

    return handle;
}

void* Platform::getDynamicObjectFunction(void* module, const bsl::string& name)
{
    void* function = dlsym(module, name.c_str());
    if (function == 0) {
        bsl::fprintf(
            stderr, "%s: %s\n", "Failed to open function", dlerror());
        return 0;
    }

    return function;
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

int Platform::setEnvironmentVariable(const bsl::string& name,
                                     const bsl::string& value)
{
    DWORD rc = SetEnvironmentVariable(name.c_str(), value.c_str());
    if (rc == FALSE) {
        return static_cast<int>(GetLastError());
    }

    return 0;
}

int Platform::getEnvironmentVariable(bsl::string*       result,
                                     const bsl::string& name)
{
    DWORD rc;

    rc = GetEnvironmentVariable(name.c_str(), 0, 0);
    if (rc == 0) {
        return static_cast<int>(GetLastError());
    }

    if (rc <= 0) {
        return ERROR_NOT_ENOUGH_MEMORY;
    }

    result->resize(static_cast<bsl::size_t>(rc - 1));

    if (rc > 1) {
        rc = GetEnvironmentVariable(name.c_str(),
                                    result->data(),
                                    static_cast<DWORD>(result->size() + 1));
        if (rc == 0) {
            return static_cast<int>(GetLastError());
        }

        if (rc < 0) {
            return ERROR_NOT_ENOUGH_MEMORY;
        }
    }

    return 0;
}

int Platform::setWorkingDirectory(const bsl::string& value)
{
    int rc = _chdir(value.c_str());
    if (rc != 0) {
        return static_cast<int>(GetLastError());
    }

    return 0;
}

int Platform::getWorkingDirectory(bsl::string* result)
{
    char temp[MAX_PATH];
    if (temp != _getcwd(temp, sizeof temp)) {
        return static_cast<int>(GetLastError());
    }

    result->assign(temp);
    return 0;
}

void* Platform::getDynamicObjectHandle()
{
    return 0;
}

void* Platform::getDynamicObjectHandle(const bsl::string& path)
{
    (void)(path);

    return 0;
}

void* Platform::getDynamicObjectFunction(void* module, const bsl::string& name)
{
    (void)(module);
    (void)(name);

    return 0;
}

#else
#error Not implemented
#endif

int Platform::exit()
{
    ntsf::System::exit();
    return 0;
}

}  // close package namespace
}  // close enterprise namespace
