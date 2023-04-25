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

#ifndef INCLUDED_NTSA_TEMPORARY
#define INCLUDED_NTSA_TEMPORARY

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntscfg_platform.h>
#include <ntsscm_version.h>
#include <ntsa_error.h>
#include <bdls_filesystemutil.h>
#include <bsl_string.h>

namespace BloombergLP {
namespace ntsa {

/// Provide a temporary directory.
///
/// @details
/// This class provides a guard to create a directory in the effective
/// temporary directory for the current process whose name is randomly assigned
/// to guarantee no collisions with other directories or files in the effective
/// temporary directory. The guarded directory, and all its contents, are
/// automatically removed when an object of this class is destroyed destroyed.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example
/// This example shows how to create a temporary directory which is
/// automatically deleted by a 'ntsa::TemporaryDirectory' guard.
///
///     bsl::string directoryPath;
///     {
///         ntsa::TemporaryDirectory tempDirectory;
///         directoryPath = tempDirectory.path();
///
///         bsl::string filePath = tempDirectory.path();
///         bdls::PathUtil::appendRaw(&filePath, "file.txt");
///
///         bsl::ofstream ofs(filePath.c_str());
///         BSLS_ASSERT(ofs);
///
///         ofs << "Hello, world!" << bsl::endl;
///     }
///
///     BSLS_ASSERT(!bdls::FilesystemUtil::exists(directoryPath));
///
/// @ingroup module_ntsa_system
class TemporaryDirectory {
    bsl::string d_path;
    bool        d_keep;

  private:
    // NOT IMPLEMENTED
    TemporaryDirectory(const TemporaryDirectory&) BSLS_KEYWORD_DELETED;
    TemporaryDirectory& operator=(const TemporaryDirectory&) BSLS_KEYWORD_DELETED;

  public:
    /// Create a new directory in the temporary directory that is removed
    /// when this object is destroyed. Optionally specify a 'basicAllocator'
    /// used to supply memory. If 'basicAllocator' is 0, the currently
    /// installed default allocator is used.
    explicit TemporaryDirectory(bslma::Allocator *basicAllocator = 0);
        
    /// Destroy this object.
    ~TemporaryDirectory();

    /// Do not remove the directory and all its contents upon the 
    /// destruction of this object.
    void keep();
        
    /// Return the path to the directory.
    const bsl::string& path() const; 
};

/// Provide a temporary file.
///
/// @details
/// This class provides a guard to create a file in the effective temporary
/// directory for the current process that is automatically removed when an
/// object of this class is destroyed.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @par Usage Example
/// This example shows how to create a temporary file which is automatically
/// deleted by a 'ntsa::TemporaryFile' guard.
///
///     bsl::string filePath;
///     {
///         ntsa::TemporaryFile tempFile;
///         filePath = tempFile.path();
///
///         bsl::ofstream ofs(filePath.c_str());
///         BSLS_ASSERT(ofs);
///
///         ofs << "Hello, world!";
///     }
///
///     BSLS_ASSERT(!bdls::FilesystemUtil::exists(filePath));
///
/// @ingroup module_ntsa_system
class TemporaryFile {
    bsl::string d_path;
    bool        d_keep;

  private:
    TemporaryFile(const TemporaryFile&) BSLS_KEYWORD_DELETED;
    TemporaryFile& operator=(const TemporaryFile&) BSLS_KEYWORD_DELETED;

  public:
    // CREATORS
    explicit TemporaryFile(bslma::Allocator *basicAllocator = 0);
        // Create a new file in the temporary directory that is removed when
        // this object is destroyed. Optionally specify a 'basicAllocator' used
        // to supply memory. If 'basicAllocator' is 0, the currently installed
        // default allocator is used.

    TemporaryFile(ntsa::TemporaryDirectory *tempDirectory, 
             bslma::Allocator         *basicAllocator = 0);
        // Create a new file in the specified 'tempDirectory' that is removed
        // when this object is destroyed. Optionally specify a 'basicAllocator'
        // used to supply memory. If 'basicAllocator' is 0, the currently 
        // installed default allocator is used.

    TemporaryFile(ntsa::TemporaryDirectory *tempDirectory, 
             const bsl::string&        filename, 
             bslma::Allocator         *basicAllocator = 0);
        // Create a new file in the specified 'tempDirectory' having the 
        // specified 'filename' that is removed when this object is destroyed.
        // Optionally specify a 'basicAllocator' used to supply memory. If 
        // 'basicAllocator' is 0, the currently installed default allocator is
        // used.

    ~TemporaryFile();
        // Destroy this object.

    // MANIPULATORS
    void keep();
        // Do not remove the file upon the destruction of this object.

    ntsa::Error write(const bsl::string& content);
        // Write the specified 'content' to the file.

    // ACCESSORS
    const bsl::string& path() const;
        // Return the path to the file.
};

}  // close package namespace
}  // close enterprise namespace
#endif
