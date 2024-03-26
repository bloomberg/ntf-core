// Copyright 2023 Bloomberg Finance L.P.
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

#ifndef INCLUDED_NTSU_ZEROCOPYUTIL
#define INCLUDED_NTSU_ZEROCOPYUTIL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

namespace BloombergLP {
namespace ntsu {

/// @internal @brief
/// Redefine types and constants used for Linux zero-copy in a portable way,
/// independant of any operating system headers and content of those headers
/// that may or may not be present on the build machine.
///
/// @details
/// The following caveats apply to zero-copy support:
///
/// @li Zero-copy is only supported for TCP and UDP sockets.

/// @li Zero-copy for TCP sockets is only supported for Linux kernel versions
/// greater than or equal to 4.14.0.
///
/// @li Zero-copy for UDP sockets is only supported for Linux kernel versions
/// greater than or equal to 5.0.0.
///
/// @li To reduce the complexity in the implementation detected proper support,
/// zero-copy is limited to only Linux kernel versions greater than or equal
/// to 5.0.0. Technically zero-copy is supported for TCP sockets for Linux
/// kernel versions greater than or equal to 4.14.0.
///
/// @ingroup module_ntsu
struct ZeroCopyUtil {
    enum {  //copied from include/asm-generic/socket.h
        e_SO_ZEROCOPY = 60
    };

    enum {  // copied from include/linux/socket.h
        e_MSG_ZEROCOPY = 0x4000000
    };

    enum {  // copied from include/linux/errqueue.h
        e_SO_EE_ORIGIN_ZEROCOPY      = 5,
        e_SO_EE_CODE_ZEROCOPY_COPIED = 1
    };
};

}  // close package namespace
}  // close enterprise namespace

#endif
