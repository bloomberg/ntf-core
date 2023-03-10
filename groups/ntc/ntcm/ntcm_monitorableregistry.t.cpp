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

#include <ntcm_monitorableregistry.h>

#include <ntccfg_test.h>

#include <bslma_testallocator.h>

#include <bsl_cstdlib.h>
#include <bsl_iostream.h>
#include <bsl_sstream.h>
#include <bsl_stdexcept.h>

using namespace BloombergLP;
using namespace ntcm;

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

//=============================================================================
//                      STANDARD BDE ASSERT TEST MACRO
//-----------------------------------------------------------------------------
static int testStatus = 0;

static void aSsErT(int c, const char* s, int i)
{
    if (c) {
        bsl::cout << "Error " << __FILE__ << "(" << i << "): " << s
                  << "    (failed)" << bsl::endl;
        if (0 <= testStatus && testStatus <= 100)
            ++testStatus;
    }
}

#define ASSERT(X)                                                             \
    {                                                                         \
        aSsErT(!(X), #X, __LINE__);                                           \
    }

//=============================================================================
//                  STANDARD BDE LOOP-ASSERT TEST MACROS
//-----------------------------------------------------------------------------
#define LOOP_ASSERT(I, X)                                                     \
    {                                                                         \
        if (!(X)) {                                                           \
            bsl::cout << #I << ": " << I << "\n";                             \
            aSsErT(1, #X, __LINE__);                                          \
        }                                                                     \
    }

#define LOOP2_ASSERT(I, J, X)                                                 \
    {                                                                         \
        if (!(X)) {                                                           \
            bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\n";  \
            aSsErT(1, #X, __LINE__);                                          \
        }                                                                     \
    }

#define LOOP3_ASSERT(I, J, K, X)                                              \
    {                                                                         \
        if (!(X)) {                                                           \
            bsl::cout << #I << ": " << I << "\t" << #J << ": " << J << "\t"   \
                      << #K << ": " << K << "\n";                             \
            aSsErT(1, #X, __LINE__);                                          \
        }                                                                     \
    }

//=============================================================================
//                  SEMI-STANDARD TEST OUTPUT MACROS
//-----------------------------------------------------------------------------
#define P(X) bsl::cout << #X " = " << (X) << bsl::endl;
#define Q(X) bsl::cout << "<| " #X " |>" << bsl::endl;
#define P_(X) bsl::cout << #X " = " << (X) << ", " << bsl::flush;
#define L_ __LINE__
#define NL "\n"
#define T_() bsl::cout << "    " << bsl::flush;

//=============================================================================
//                  GLOBAL TYPEDEFS/CONSTANTS FOR TESTING
//-----------------------------------------------------------------------------

static int verbose             = 0;
static int veryVerbose         = 0;
static int veryVeryVerbose     = 0;
static int veryVeryVeryVerbose = 0;

//=============================================================================
//                        HELPER FUNCTIONS AND CLASSES
//-----------------------------------------------------------------------------

//=============================================================================
//                              MAIN PROGRAM
//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    int test            = argc > 1 ? bsl::atoi(argv[1]) : 0;
    verbose             = (argc > 2);
    veryVerbose         = (argc > 3);
    veryVeryVerbose     = (argc > 4);
    veryVeryVeryVerbose = (argc > 5);
    bsl::cout << "TEST " << __FILE__ << " CASE " << test << bsl::endl;
    ;

    switch (test) {
    case 0:  // Zero is always the leading case.
    case 1: {
        // TESTING USAGE EXAMPLE
        //
        // Concerns:
        //   The usage example shown in the component-level documentation
        //   compiles and executes as expected.
        //
        // Plan:
        //   Implement the test exactly as shown in the example.

        ntccfg::TestAllocator ta;
        {
            ntcm::MonitorableRegistry monitorableRegistry(&ta);
        }
        ASSERT(0 == ta.numBlocksInUse());
    } break;
    default: {
        bsl::cerr << "WARNING: CASE `" << test << "' NOT FOUND." << bsl::endl;
        testStatus = -1;
    }
    }

    if (testStatus > 0) {
        bsl::cerr << "Error, non-zero test status = " << testStatus << "."
                  << bsl::endl;
    }
    return testStatus;
}
