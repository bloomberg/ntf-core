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

#include <bslmt_once.h>
#include <bsls_assert.h>
#include <bsls_atomic.h>
#include <bsls_atomicoperations.h>
#include <bsl_sstream.h>

namespace BloombergLP {
namespace ntcs {

/// Provide global state.
class Nomenclature::State
{
  public:
    bsls::AtomicOperations::AtomicTypes::Int d_anonymousReactorInstanceCount;
    bsls::AtomicOperations::AtomicTypes::Int d_anonymousProactorInstanceCount;
    bsls::AtomicOperations::AtomicTypes::Int d_anonymousThreadInstanceCount;
    bsls::AtomicOperations::AtomicTypes::Int d_anonymousInterfaceInstanceCount;

    static State s_global;
};

Nomenclature::State Nomenclature::State::s_global;

bsl::string Nomenclature::createReactorName()
{
    const int counter = bsls::AtomicOperations::incrementIntNv(
                            &State::s_global.d_anonymousReactorInstanceCount) -
                        1;

    bsl::stringstream ss;
    ss << "reactor-" << counter;
    return ss.str();
}

bsl::string Nomenclature::createProactorName()
{
    const int counter =
        bsls::AtomicOperations::incrementIntNv(
            &State::s_global.d_anonymousProactorInstanceCount) -
        1;

    bsl::stringstream ss;
    ss << "proactor-" << counter;
    return ss.str();
}

bsl::string Nomenclature::createThreadName()
{
    const int counter = bsls::AtomicOperations::incrementIntNv(
                            &State::s_global.d_anonymousThreadInstanceCount) -
                        1;

    bsl::stringstream ss;
    ss << "thread-" << counter;
    return ss.str();
}

bsl::string Nomenclature::createInterfaceName()
{
    const int counter =
        bsls::AtomicOperations::incrementIntNv(
            &State::s_global.d_anonymousInterfaceInstanceCount) -
        1;

    bsl::stringstream ss;
    ss << "interface-" << counter;
    return ss.str();
}

}  // close package namespace
}  // close enterprise namespace
