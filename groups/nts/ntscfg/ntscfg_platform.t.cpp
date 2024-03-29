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

#include <ntscfg_platform.h>

#include <bsl_cstdlib.h>

int testCase1()
{
    return 0;
}

int main(int argc, char** argv)
{
    int testCase = 0;
    if (argc > 1) {
        testCase = bsl::atoi(argv[1]);
    }

    if (testCase == 0 || testCase == 1) {
        return testCase1();
    }
    else {
        return -1;
    }

    return 0;
}
