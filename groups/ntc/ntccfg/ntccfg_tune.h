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

#ifndef INCLUDED_NTCCFG_TUNE
#define INCLUDED_NTCCFG_TUNE

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <bdlb_nullablevalue.h>
#include <bsls_types.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntccfg {

// Initialize the specified 'variable' having the specified 'name'.
#define NTCCFG_TUNE(variable, name)                                           \
    BloombergLP::ntccfg::Tune::configure(variable, #name, name##_DEFAULT)

// Get the value of the tunable identified by the specified 'name'.
#define NTCCFG_TUNE_GET(name) s_##name

// Declare and initialize the value of the tunable identified by the specified
// 'name' having the specified 'type'.
#define NTCCFG_TUNE_SET(type, name)                                           \
    type initialize_##name()                                                  \
    {                                                                         \
        type value;                                                           \
        BloombergLP::ntccfg::Tune::configure(&value, #name, name);            \
        return value;                                                         \
    }                                                                         \
                                                                              \
    type s_##name = initialize_##name()

// Set the value of the tunable identified by the specified 'name' to the
// specified 'value'.
#define NTCCFG_TUNE_SET_VALUE(name, value) s_##name = value;

/// @internal @brief
/// Provide utilities for configuring the process from the environment.
///
/// @par Thread Safety
// This class is thread safe.
///
/// @ingroup module_ntccfg
struct Tune {
  public:
    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(bool* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(bool* result, const char* name, bool defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(char* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(char* result, const char* name, char defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(unsigned char* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(unsigned char* result,
                          const char*    name,
                          unsigned char  defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(short* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(short* result, const char* name, short defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(unsigned short* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(unsigned short* result,
                          const char*     name,
                          unsigned short  defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(int* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(int* result, const char* name, int defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(unsigned int* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(unsigned int* result,
                          const char*   name,
                          unsigned int  defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(long* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(long* result, const char* name, long defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(unsigned long* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(unsigned long* result,
                          const char*    name,
                          unsigned long  defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(long long* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(long long*  result,
                          const char* name,
                          long long   defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(unsigned long long* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(unsigned long long* result,
                          const char*         name,
                          unsigned long long  defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(float* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(float* result, const char* name, float defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(double* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(double*     result,
                          const char* name,
                          double      defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(long double* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(long double* result,
                          const char*  name,
                          long double  defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    static bool configure(bsl::string* result, const char* name);

    /// Load into the specified 'result' the value of the environment
    /// variable having the specified 'name', if any, or the specified
    /// 'defaultValue' otherwise.
    static void configure(bsl::string*             result,
                          const char*              name,
                          const bslstl::StringRef& defaultValue);

    /// Load into the specified 'result' the vlaue of the environment
    /// variable having the specified 'name'. Return true if such an
    /// environment variable is defined, and false otherwise.
    template <typename TYPE>
    static bool configure(bdlb::NullableValue<TYPE>* result, const char* name);

    /// If the specified 'result' is null, load into the specified 'result'
    /// the value of the environment variable having the specified 'name',
    /// if any, or the specified 'defaultValue' otherwise.
    template <typename TYPE>
    static void configure(bdlb::NullableValue<TYPE>* result,
                          const char*                name,
                          const TYPE&                defaultValue);
};

template <typename TYPE>
bool Tune::configure(bdlb::NullableValue<TYPE>* result, const char* name)
{
    bool unset = result->isNull();
    if (unset) {
        result->makeValue();
    }

    bool found = Tune::configure(&result->value(), name);
    if (!found) {
        if (unset) {
            result->reset();
        }
    }

    return found;
}

template <typename TYPE>
void Tune::configure(bdlb::NullableValue<TYPE>* result,
                     const char*                name,
                     const TYPE&                defaultValue)
{
    if (!result->isNull()) {
        return;
    }

    bool found = Tune::configure(&result->makeValue(), name);
    if (!found) {
        *result = defaultValue;
    }
}

}  // close package namespace
}  // close enterprise namespace
#endif
