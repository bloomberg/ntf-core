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

#include <ntsa_temporary.h>

#include <ntscfg_test.h>

#include <bdls_filesystemutil.h>
#include <bdls_pathutil.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_fstream.h>

using namespace BloombergLP;

//=============================================================================
//                                 TEST PLAN
//-----------------------------------------------------------------------------
//                                 Overview
//                                 --------
//
//-----------------------------------------------------------------------------

// [ 1]
//-----------------------------------------------------------------------------
// [ 1]
//-----------------------------------------------------------------------------

NTSCFG_TEST_CASE(1)
{
    // Concern: A temporary directory is automatically created and removed
    // by a temporary directory guard.

    ntscfg::TestAllocator ta;
    {
        bsl::string directoryPath;
        {
            ntsa::TemporaryDirectory tempDirectory;
            directoryPath = tempDirectory.path();

            BSLS_LOG_DEBUG("Create temporary directory: %s",
                           directoryPath.c_str());

            bsl::string filePath = tempDirectory.path();
            bdls::PathUtil::appendRaw(&filePath, "file.txt");

            bsl::ofstream ofs(filePath.c_str());
            BSLS_ASSERT(ofs);

            ofs << "Hello, world!" << bsl::endl;
        }

        BSLS_ASSERT(!bdls::FilesystemUtil::exists(directoryPath));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern: A temporary file is automatically created and removed
    // by a temporary file guard.

    ntscfg::TestAllocator ta;
    {
        bsl::string filePath;
        {
            ntsa::TemporaryFile tempFile;
            filePath = tempFile.path();

            BSLS_LOG_DEBUG("Create temporary file: %s", filePath.c_str());

            bsl::ofstream ofs(filePath.c_str());
            BSLS_ASSERT(ofs);

            ofs << "Hello, world!";
        }

        BSLS_ASSERT(!bdls::FilesystemUtil::exists(filePath));
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
}
NTSCFG_TEST_DRIVER_END;
