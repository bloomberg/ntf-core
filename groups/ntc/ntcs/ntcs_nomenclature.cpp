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

#include <ntcs_nomenclature.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntcs_nomenclature_cpp, "$Id$ $CSID$")

#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntcs {

namespace {

bsls::AtomicInt s_anonymousReactorInstanceCount;
bsls::AtomicInt s_anonymousProactorInstanceCount;
bsls::AtomicInt s_anonymousThreadInstanceCount;
bsls::AtomicInt s_anonymousThreadPoolInstanceCount;
bsls::AtomicInt s_anonymousInterfaceInstanceCount;

}  // close unnamed namespace

bsl::string Nomenclature::createReactorName()
{
    bsl::stringstream ss;
    ss << "reactor-" << ++s_anonymousReactorInstanceCount;
    return ss.str();
}

bsl::string Nomenclature::createProactorName()
{
    bsl::stringstream ss;
    ss << "proactor-" << ++s_anonymousProactorInstanceCount;
    return ss.str();
}

bsl::string Nomenclature::createThreadName()
{
    bsl::stringstream ss;
    ss << "thread-" << ++s_anonymousThreadInstanceCount;
    return ss.str();
}

bsl::string Nomenclature::createThreadPoolName()
{
    bsl::stringstream ss;
    ss << "threadpool-" << ++s_anonymousThreadPoolInstanceCount;
    return ss.str();
}

bsl::string Nomenclature::createInterfaceName()
{
    bsl::stringstream ss;
    ss << "interface-" << ++s_anonymousInterfaceInstanceCount;
    return ss.str();
}

}  // close package namespace
}  // close enterprise namespace
