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
#include <bsl_cstring.h>

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

int Platform::exit()
{
    ntsf::System::exit();
    return 0;
}

}  // close package namespace
}  // close enterprise namespace
