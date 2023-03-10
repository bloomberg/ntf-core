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

#include <ntccfg_tune.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntccfg_tune_cpp, "$Id$ $CSID$")

#include <bdlb_string.h>
#include <bsls_log.h>

#include <bsl_cstdlib.h>
#include <bsl_limits.h>

namespace BloombergLP {
namespace ntccfg {

namespace {

bool configureBoolean(bool* result, const char* variableName)
{
    const char* variableValue = bsl::getenv(variableName);
    if (variableValue) {
        if (bdlb::String::areEqualCaseless(variableValue, "0") ||
            bdlb::String::areEqualCaseless(variableValue, "false") ||
            bdlb::String::areEqualCaseless(variableValue, "no") ||
            bdlb::String::areEqualCaseless(variableValue, "off"))
        {
            *result = false;
            return true;
        }
        else if (bdlb::String::areEqualCaseless(variableValue, "1") ||
                 bdlb::String::areEqualCaseless(variableValue, "true") ||
                 bdlb::String::areEqualCaseless(variableValue, "yes") ||
                 bdlb::String::areEqualCaseless(variableValue, "on"))
        {
            *result = true;
            return true;
        }
        else {
            BSLS_LOG_WARN("Failed to configure %s: invalid value %s",
                          variableName,
                          variableValue);
            return false;
        }
    }
    else {
        return false;
    }
}

void configureBoolean(bool*       result,
                      const char* variableName,
                      bool        defaultValue)
{
    if (!configureBoolean(result, variableName)) {
        *result = defaultValue;
    }
}

template <typename TYPE>
bool configureSignedInteger(TYPE* result, const char* variableName)
{
    using namespace std;

    const char* variableValue = bsl::getenv(variableName);
    if (variableValue) {
        char*     end  = 0;
        long long temp = strtoll(variableValue, &end, 10);
        if (end == 0 || *end != 0) {
            BSLS_LOG_WARN("Failed to configure %s: invalid value %s",
                          variableName,
                          variableValue);
            return false;
        }
        else {
            if (temp < bsl::numeric_limits<TYPE>::min() ||
                temp > bsl::numeric_limits<TYPE>::max())
            {
                BSLS_LOG_WARN("Failed to configure %s: invalid value %s",
                              variableName,
                              variableValue);
                return false;
            }

            *result = static_cast<TYPE>(temp);
            return true;
        }
    }
    else {
        return false;
    }
}

template <typename TYPE>
void configureSignedInteger(TYPE*       result,
                            const char* variableName,
                            const TYPE& defaultValue)
{
    if (!configureSignedInteger(result, variableName)) {
        *result = defaultValue;
    }
}

template <typename TYPE>
bool configureUnsignedInteger(TYPE* result, const char* variableName)
{
    using namespace std;

    const char* variableValue = bsl::getenv(variableName);
    if (variableValue) {
        char*              end  = 0;
        unsigned long long temp = strtoull(variableValue, &end, 10);
        if (end == 0 || *end != 0) {
            BSLS_LOG_WARN("Failed to configure %s: invalid value %s",
                          variableName,
                          variableValue);
            return false;
        }
        else {
            if (temp < bsl::numeric_limits<TYPE>::min() ||
                temp > bsl::numeric_limits<TYPE>::max())
            {
                BSLS_LOG_WARN("Failed to configure %s: invalid value %s",
                              variableName,
                              variableValue);
                return false;
            }

            *result = static_cast<TYPE>(temp);
            return true;
        }
    }
    else {
        return false;
    }
}

template <typename TYPE>
void configureUnsignedInteger(TYPE*       result,
                              const char* variableName,
                              const TYPE& defaultValue)
{
    if (!configureUnsignedInteger(result, variableName)) {
        *result = defaultValue;
    }
}

template <typename TYPE>
bool configureDecimal(TYPE* result, const char* variableName)
{
    const char* variableValue = bsl::getenv(variableName);
    if (variableValue) {
        char*  end  = 0;
        double temp = bsl::strtod(variableValue, &end);
        if (end == 0 || *end != 0) {
            BSLS_LOG_WARN("Failed to configure %s: invalid value %s",
                          variableName,
                          variableValue);
            return false;
        }
        else {
            *result = static_cast<TYPE>(temp);
            return true;
        }
    }
    else {
        return false;
    }
}

template <typename TYPE>
void configureDecimal(TYPE*       result,
                      const char* variableName,
                      const TYPE& defaultValue)
{
    if (!configureDecimal(result, variableName)) {
        *result = defaultValue;
    }
}

template <typename TYPE>
bool configureCharacter(TYPE* result, const char* variableName)
{
    const char* variableValue = bsl::getenv(variableName);
    if (variableValue) {
        *result = variableValue[0];
        return true;
    }
    else {
        return false;
    }
}

template <typename TYPE>
void configureCharacter(TYPE*       result,
                        const char* variableName,
                        const TYPE& defaultValue)
{
    if (!configureCharacter(result, variableName)) {
        *result = defaultValue;
    }
}

bool configureString(bsl::string* result, const char* variableName)
{
    const char* variableValue = bsl::getenv(variableName);
    if (variableValue) {
        *result = variableValue;
        return true;
    }
    else {
        return false;
    }
}

void configureString(bsl::string*             result,
                     const char*              variableName,
                     const bslstl::StringRef& defaultValue)
{
    if (!configureString(result, variableName)) {
        *result = defaultValue;
    }
}

}  // close unnamed namespace

bool Tune::configure(bool* result, const char* name)
{
    return configureBoolean(result, name);
}

void Tune::configure(bool* result, const char* name, bool defaultValue)
{
    configureBoolean(result, name, defaultValue);
}

bool Tune::configure(char* result, const char* name)
{
    return configureCharacter(result, name);
}

void Tune::configure(char* result, const char* name, char defaultValue)
{
    configureCharacter(result, name, defaultValue);
}

bool Tune::configure(unsigned char* result, const char* name)
{
    return configureCharacter(result, name);
}

void Tune::configure(unsigned char* result,
                     const char*    name,
                     unsigned char  defaultValue)
{
    configureCharacter(result, name, defaultValue);
}

bool Tune::configure(short* result, const char* name)
{
    return configureSignedInteger(result, name);
}

void Tune::configure(short* result, const char* name, short defaultValue)
{
    configureSignedInteger(result, name, defaultValue);
}

bool Tune::configure(unsigned short* result, const char* name)
{
    return configureUnsignedInteger(result, name);
}

void Tune::configure(unsigned short* result,
                     const char*     name,
                     unsigned short  defaultValue)
{
    configureUnsignedInteger(result, name, defaultValue);
}

bool Tune::configure(int* result, const char* name)
{
    return configureSignedInteger(result, name);
}

void Tune::configure(int* result, const char* name, int defaultValue)
{
    configureSignedInteger(result, name, defaultValue);
}

bool Tune::configure(unsigned int* result, const char* name)
{
    return configureUnsignedInteger(result, name);
}

void Tune::configure(unsigned int* result,
                     const char*   name,
                     unsigned int  defaultValue)
{
    configureUnsignedInteger(result, name, defaultValue);
}

bool Tune::configure(long* result, const char* name)
{
    return configureSignedInteger(result, name);
}

void Tune::configure(long* result, const char* name, long defaultValue)
{
    configureSignedInteger(result, name, defaultValue);
}

bool Tune::configure(unsigned long* result, const char* name)
{
    return configureUnsignedInteger(result, name);
}

void Tune::configure(unsigned long* result,
                     const char*    name,
                     unsigned long  defaultValue)
{
    configureUnsignedInteger(result, name, defaultValue);
}

bool Tune::configure(long long* result, const char* name)
{
    return configureSignedInteger(result, name);
}

void Tune::configure(long long*  result,
                     const char* name,
                     long long   defaultValue)
{
    configureSignedInteger(result, name, defaultValue);
}

bool Tune::configure(unsigned long long* result, const char* name)
{
    return configureUnsignedInteger(result, name);
}

void Tune::configure(unsigned long long* result,
                     const char*         name,
                     unsigned long long  defaultValue)
{
    configureUnsignedInteger(result, name, defaultValue);
}

bool Tune::configure(float* result, const char* name)
{
    return configureDecimal(result, name);
}

void Tune::configure(float* result, const char* name, float defaultValue)
{
    configureDecimal(result, name, defaultValue);
}

bool Tune::configure(double* result, const char* name)
{
    return configureDecimal(result, name);
}

void Tune::configure(double* result, const char* name, double defaultValue)
{
    configureDecimal(result, name, defaultValue);
}

bool Tune::configure(long double* result, const char* name)
{
    return configureDecimal(result, name);
}

void Tune::configure(long double* result,
                     const char*  name,
                     long double  defaultValue)
{
    configureDecimal(result, name, defaultValue);
}

bool Tune::configure(bsl::string* result, const char* name)
{
    return configureString(result, name);
}

void Tune::configure(bsl::string*             result,
                     const char*              name,
                     const bslstl::StringRef& defaultValue)
{
    configureString(result, name, defaultValue);
}

}  // close package namespace
}  // close enterprise namespace
