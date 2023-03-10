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

#ifndef INCLUDED_NTCCFG_LIKELY
#define INCLUDED_NTCCFG_LIKELY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntcscm_version.h>
#include <bsls_performancehint.h>

#if NTC_BUILD_WITH_BRANCH_PREDICTION

/// Predict the following branch is likely.
/// @ingroup module_ntccfg
#define NTCCFG_LIKELY(expression)                                             \
    BSLS_PERFORMANCEHINT_PREDICT_LIKELY((bool)(expression))

/// Predict the following branch is unlikely.
/// @ingroup module_ntccfg
#define NTCCFG_UNLIKELY(expression)                                           \
    BSLS_PERFORMANCEHINT_PREDICT_UNLIKELY((bool)(expression))

#else

/// Predict the following branch is likely.
/// @ingroup module_ntccfg
#define NTCCFG_LIKELY(expression)

/// Predict the following branch is unlikely.
/// @ingroup module_ntccfg
#define NTCCFG_UNLIKELY(expression)

#endif

#endif
