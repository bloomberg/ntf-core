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

#ifndef INCLUDED_NTCCFG_FOREACH
#define INCLUDED_NTCCFG_FOREACH

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntcscm_version.h>

/// @internal @brief
/// Generate code for a for-loop of the specified 'variable' of the
/// parameterized 'TYPE' initially assigned the specified 'begin' value and
/// incremented each loop iteration. Terminate the loop when 'variable' has the
/// same value as the specified 'end' value.
///
/// @ingroup module_ntccfg
#define NTCCFG_FOREACH(TYPE, variable, begin, end)                            \
    for (TYPE variable = (begin); variable < (end); ++variable)

/// @internal @brief
/// Generate code for a for-loop of the specified 'iterator' of the
/// parameterized 'ITERATOR_TYPE' iterating over all elements in the specified
/// 'container'.
///
/// @ingroup module_ntccfg
#define NTCCFG_FOREACH_ITERATOR(ITERATOR_TYPE, iterator, container)           \
    for (ITERATOR_TYPE iterator = (container).begin();                        \
         iterator != (container).end();                                       \
         ++iterator)

/// @internal @brief
/// Generate code for a entering a scope for a for-loop of the specified
/// 'iterator' of the parameterized 'ITERATOR_TYPE' iterating over all elements
/// in the specified 'container'.
///
/// @ingroup module_ntccfg
#define NTCCFG_FOREACH_ITERATOR_SCOPE_ENTER(ITERATOR_TYPE,                    \
                                            iterator,                         \
                                            container)                        \
    {                                                                         \
        for (ITERATOR_TYPE iterator = (container).begin();                    \
             iterator != (container).end();                                   \
             ++iterator)

/// @internal @brief
/// Generate for leaving a scope.
///
/// @ingroup module_ntccfg
#define NTCCFG_FOREACH_ITERATOR_SCOPE_LEAVE() }

#endif
