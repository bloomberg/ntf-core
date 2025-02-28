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

#ifndef INCLUDED_NTCTLC_PLUGIN
#define INCLUDED_NTCTLC_PLUGIN

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntccfg_config.h>
#include <ntccfg_platform.h>
#include <ntci_compression.h>
#include <ntci_compressiondriver.h>
#include <ntcscm_version.h>
#include <bsl_memory.h>

namespace BloombergLP {
namespace ntctlc {

/// Provide a facility to inject the 'zlib', 'zstd', and 'lz4' compression
/// libraries into NTC.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctlc
struct Plugin {
    /// Initialize this plugin and register support for compression in NTC
    /// using the 'zlib', 'zstd', and 'lz4' third-party libraries. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the global allocator is used.
    static void initialize(bslma::Allocator* basicAllocator = 0);

    /// Load into the specified 'result' the compression driver implemented
    /// using 'zlib', 'zstd', and 'lz4' third-party libraries.
    static void load(bsl::shared_ptr<ntci::CompressionDriver>* result);

    /// Deregister support for compression in NTF using the 'zlib', 'zstd', and
    /// 'lz4' third-party libraries and clean up all resources required by this
    /// plugin.
    static void exit();
};

/// Provide a a scoped guard to automatically initialize and clean up the NTC
/// plugin provided by this library.
///
/// @par Thread Safety
/// This class is thread safe.
///
/// @ingroup module_ntctlc
class PluginGuard
{
  private:
    PluginGuard(const PluginGuard&) BSLS_KEYWORD_DELETED;
    PluginGuard& operator=(const PluginGuard&) BSLS_KEYWORD_DELETED;

  public:
    /// Initialize this plugin and register support for compression in NTC
    /// using the 'zlib', 'zstd', and 'lz4' third-party libraries. Optionally
    /// specify a 'basicAllocator' used to supply memory. If 'basicAllocator'
    /// is 0, the global allocator is used.
    explicit PluginGuard(bslma::Allocator* allocator = 0);

    /// Deregister support for compression in NTC using the 'zlib', 'zstd', and
    /// 'lz4' third-party libraries and clean up all resources required by this
    /// plugin.
    ~PluginGuard();
};

/// Initialize this plugin and register support for TLS in NTF using the
/// 'openssl' third-party library.
#define NTF_REGISTER_PLUGIN_GZIP_ZLIB_ZSTD_LZ4()                              \
    BloombergLP::ntctlc::PluginGuard ntfPluginGzipZlibZstdLz4

}  // close namespace ntctlc
}  // close namespace BloombergLP
#endif
