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

#include <ntscfg_test.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_interest_t_cpp, "$Id$ $CSID$")

#include <ntsa_interest.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Interest'.
class InterestTest
{
  public:
    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

  private:
    // Ensure the specified 'socket' is not found in the specified
    // 'interestSet'.
    static void ensureNotFound(const ntsa::InterestSet& interestSet,
                               ntsa::Handle             socket);

    // Ensure the specified 'socket' is found in the specified 'interestSet'
    // but there is interest in neither readability nor writability.
    static void ensureWantNone(const ntsa::InterestSet& interestSet,
                               ntsa::Handle             socket);

    // Ensure the specified 'socket' is found in the specified 'interestSet'
    // with interest in readability but not writability.
    static void ensureWantReadable(const ntsa::InterestSet& interestSet,
                                   ntsa::Handle             socket);

    // Ensure the specified 'socket' is found in the specified 'interestSet'
    // with interest in writability but not readability.
    static void ensureWantWritable(const ntsa::InterestSet& interestSet,
                                   ntsa::Handle             socket);

    // Ensure the specified 'socket' is found in the specified 'interestSet' with
    // interest in both readability and writability.
    static void ensureWantBoth(const ntsa::InterestSet& interestSet,
                               ntsa::Handle             socket);
};

NTSCFG_TEST_FUNCTION(ntsa::InterestTest::verifyCase1)
{
    // Concern: ntsa::Interest stores the readable and writable states and
    // correctly reports whether there is interest in readability, writability,
    // either, both, or neither.

    const ntsa::Handle k_SOCKET         = 10;
    const ntsa::Handle k_SOCKET_INVALID = ntsa::k_INVALID_HANDLE;

    NTSCFG_TEST_NE(k_SOCKET, k_SOCKET_INVALID);

    ntsa::Interest interest;

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET_INVALID);
    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_FALSE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_TRUE(interest.wantNone());

    interest.setHandle(k_SOCKET);

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_FALSE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_TRUE(interest.wantNone());

    interest.showReadable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    interest.hideReadable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_FALSE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_TRUE(interest.wantNone());

    interest.showWritable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_TRUE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    interest.hideWritable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_FALSE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_TRUE(interest.wantNone());

    interest.showReadable();
    interest.showWritable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(interest.wantReadable());
    NTSCFG_TEST_TRUE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_TRUE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    interest.hideReadable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_TRUE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    interest.showReadable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(interest.wantReadable());
    NTSCFG_TEST_TRUE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_TRUE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    interest.hideWritable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    interest.showWritable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(interest.wantReadable());
    NTSCFG_TEST_TRUE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_TRUE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    interest.hideReadable();
    interest.hideWritable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_FALSE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_TRUE(interest.wantNone());

    interest.showReadable();
    interest.showWritable();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET);
    NTSCFG_TEST_TRUE(interest.wantReadable());
    NTSCFG_TEST_TRUE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_TRUE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    interest.reset();

    NTSCFG_TEST_EQ(interest.handle(), k_SOCKET_INVALID);
    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_FALSE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_TRUE(interest.wantNone());
}

NTSCFG_TEST_FUNCTION(ntsa::InterestTest::verifyCase2)
{
    ntsa::Error error;

    const ntsa::Handle k_SOCKET_MIN = 1;
    const ntsa::Handle k_SOCKET_MAX = 32;

    // Create an interest set.

    ntsa::InterestSet interestSet(NTSCFG_TEST_ALLOCATOR);

    // Ensure the interest set is initially empty.

    NTSCFG_TEST_TRUE(interestSet.empty());

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        InterestTest::ensureNotFound(interestSet, i);
    }

    // Attach each socket to the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.attach(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantNone(interestSet, j);
            }
            else {
                InterestTest::ensureNotFound(interestSet, j);
            }
        }
    }

    // Gain interest in readability of each socket in the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantReadable(interestSet, j);
            }
            else {
                InterestTest::ensureWantNone(interestSet, j);
            }
        }
    }

    // Lose interest in readability of each socket in the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantNone(interestSet, j);
            }
            else {
                InterestTest::ensureWantReadable(interestSet, j);
            }
        }
    }

    // Gain interest in writability of each socket in the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantWritable(interestSet, j);
            }
            else {
                InterestTest::ensureWantNone(interestSet, j);
            }
        }
    }

    // Lose interest in writability of each socket in the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantNone(interestSet, j);
            }
            else {
                InterestTest::ensureWantWritable(interestSet, j);
            }
        }
    }

    // Gain interest in readability then writability of each socket in
    // the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantReadable(interestSet, j);
            }
            else {
                InterestTest::ensureWantNone(interestSet, j);
            }
        }
    }

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantBoth(interestSet, j);
            }
            else {
                InterestTest::ensureWantReadable(interestSet, j);
            }
        }
    }

    // Lose interest in readability then writability of each socket in the
    // interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantWritable(interestSet, j);
            }
            else {
                InterestTest::ensureWantBoth(interestSet, j);
            }
        }
    }

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantNone(interestSet, j);
            }
            else {
                InterestTest::ensureWantWritable(interestSet, j);
            }
        }
    }

    // Gain interest in readability then writability of each socket in
    // the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantReadable(interestSet, j);
            }
            else {
                InterestTest::ensureWantNone(interestSet, j);
            }
        }
    }

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantBoth(interestSet, j);
            }
            else {
                InterestTest::ensureWantReadable(interestSet, j);
            }
        }
    }

    // Lose interest in writability then readability of each socket in the
    // interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantReadable(interestSet, j);
            }
            else {
                InterestTest::ensureWantBoth(interestSet, j);
            }
        }
    }

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantNone(interestSet, j);
            }
            else {
                InterestTest::ensureWantReadable(interestSet, j);
            }
        }
    }

    // Gain interest in writability then readability of each socket in
    // the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantWritable(interestSet, j);
            }
            else {
                InterestTest::ensureWantNone(interestSet, j);
            }
        }
    }

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantBoth(interestSet, j);
            }
            else {
                InterestTest::ensureWantWritable(interestSet, j);
            }
        }
    }

    // Lose interest in writability then readability of each socket in the
    // interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantReadable(interestSet, j);
            }
            else {
                InterestTest::ensureWantBoth(interestSet, j);
            }
        }
    }

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantNone(interestSet, j);
            }
            else {
                InterestTest::ensureWantReadable(interestSet, j);
            }
        }
    }

    // Gain interest in writability then readability of each socket in
    // the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantWritable(interestSet, j);
            }
            else {
                InterestTest::ensureWantNone(interestSet, j);
            }
        }
    }

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.showReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantBoth(interestSet, j);
            }
            else {
                InterestTest::ensureWantWritable(interestSet, j);
            }
        }
    }

    // Lose interest in readability then writability of each socket in the
    // interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideReadable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantWritable(interestSet, j);
            }
            else {
                InterestTest::ensureWantBoth(interestSet, j);
            }
        }
    }

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.hideWritable(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureWantNone(interestSet, j);
            }
            else {
                InterestTest::ensureWantWritable(interestSet, j);
            }
        }
    }

    // Detach each socket to the interest set.

    for (ntsa::Handle i = k_SOCKET_MIN; i <= k_SOCKET_MAX; ++i) {
        error = interestSet.detach(i);
        NTSCFG_TEST_OK(error);

        for (ntsa::Handle j = k_SOCKET_MIN; j <= k_SOCKET_MAX; ++j) {
            if (j <= i) {
                InterestTest::ensureNotFound(interestSet, j);
            }
            else {
                InterestTest::ensureWantNone(interestSet, j);
            }
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::InterestTest::verifyCase3)
{
    // Concern: Sockets are not automatically attached to the interest set
    // if interest is attempted to be gained on an un-attached socket.

    ntsa::Error error;

    const ntsa::Handle k_SOCKET_A = 10;
    const ntsa::Handle k_SOCKET_X = 1000;

    ntsa::InterestSet interestSet(NTSCFG_TEST_ALLOCATOR);

    error = interestSet.attach(k_SOCKET_A);
    NTSCFG_TEST_OK(error);

    error = interestSet.showReadable(k_SOCKET_A);
    NTSCFG_TEST_OK(error);

    error = interestSet.showWritable(k_SOCKET_A);
    NTSCFG_TEST_OK(error);

    error = interestSet.showReadable(k_SOCKET_X);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));

    error = interestSet.showWritable(k_SOCKET_X);
    NTSCFG_TEST_EQ(error, ntsa::Error(ntsa::Error::e_INVALID));
}

NTSCFG_TEST_FUNCTION(ntsa::InterestTest::verifyCase4)
{
    // Concern: The interest set is iteratable.

    const ntsa::Handle k_SOCKET_A = 10;
    const ntsa::Handle k_SOCKET_B = 100;
    const ntsa::Handle k_SOCKET_C = 1000;
    const ntsa::Handle k_SOCKET_D = 10000;

    ntsa::InterestSet interestSet(NTSCFG_TEST_ALLOCATOR);

    interestSet.attach(k_SOCKET_A);
    interestSet.attach(k_SOCKET_B);
    interestSet.attach(k_SOCKET_C);
    interestSet.attach(k_SOCKET_D);

    interestSet.showReadable(k_SOCKET_B);
    interestSet.showWritable(k_SOCKET_C);
    interestSet.showReadable(k_SOCKET_D);
    interestSet.showWritable(k_SOCKET_D);

    NTSCFG_TEST_LOG_DEBUG << "Interest set = " << interestSet
                          << NTSCFG_TEST_LOG_END;

    typedef bsl::vector<ntsa::Interest> InterestVector;
    InterestVector                      interestVector(NTSCFG_TEST_ALLOCATOR);

    for (ntsa::InterestSet::const_iterator it = interestSet.cbegin();
         it != interestSet.cend();
         ++it)
    {
        const ntsa::Interest& interest = *it;

        NTSCFG_TEST_LOG_DEBUG << "Interest = " << *it << NTSCFG_TEST_LOG_END;

        interestVector.push_back(interest);
    }

    NTSCFG_TEST_EQ(interestVector.size(), 4);

    NTSCFG_TEST_EQ(interestVector[0].handle(), k_SOCKET_A);
    NTSCFG_TEST_TRUE(interestVector[0].wantNone());

    NTSCFG_TEST_EQ(interestVector[1].handle(), k_SOCKET_B);
    NTSCFG_TEST_TRUE(interestVector[1].wantReadable());

    NTSCFG_TEST_EQ(interestVector[2].handle(), k_SOCKET_C);
    NTSCFG_TEST_TRUE(interestVector[2].wantWritable());

    NTSCFG_TEST_EQ(interestVector[3].handle(), k_SOCKET_D);
    NTSCFG_TEST_TRUE(interestVector[3].wantBoth());
}

void InterestTest::ensureNotFound(const ntsa::InterestSet& interestSet,
                                  ntsa::Handle             socket)
{
    NTSCFG_TEST_FALSE(interestSet.contains(socket));

    ntsa::Interest interest;
    bool           found = interestSet.find(&interest, socket);
    NTSCFG_TEST_FALSE(found);
}

void InterestTest::ensureWantNone(const ntsa::InterestSet& interestSet,
                                  ntsa::Handle             socket)
{
    NTSCFG_TEST_TRUE(interestSet.contains(socket));

    ntsa::Interest interest;
    bool           found = interestSet.find(&interest, socket);

    NTSCFG_TEST_TRUE(found);
    NTSCFG_TEST_EQ(interest.handle(), socket);

    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_FALSE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_TRUE(interest.wantNone());

    NTSCFG_TEST_FALSE(interestSet.wantReadable(socket));
    NTSCFG_TEST_FALSE(interestSet.wantWritable(socket));
    NTSCFG_TEST_FALSE(interestSet.wantAny(socket));
    NTSCFG_TEST_FALSE(interestSet.wantBoth(socket));
    NTSCFG_TEST_TRUE(interestSet.wantNone(socket));
}

void InterestTest::ensureWantReadable(const ntsa::InterestSet& interestSet,
                                      ntsa::Handle             socket)
{
    NTSCFG_TEST_TRUE(interestSet.contains(socket));

    ntsa::Interest interest;
    bool           found = interestSet.find(&interest, socket);

    NTSCFG_TEST_TRUE(found);
    NTSCFG_TEST_EQ(interest.handle(), socket);

    NTSCFG_TEST_TRUE(interest.wantReadable());
    NTSCFG_TEST_FALSE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    NTSCFG_TEST_TRUE(interestSet.wantReadable(socket));
    NTSCFG_TEST_FALSE(interestSet.wantWritable(socket));
    NTSCFG_TEST_TRUE(interestSet.wantAny(socket));
    NTSCFG_TEST_FALSE(interestSet.wantBoth(socket));
    NTSCFG_TEST_FALSE(interestSet.wantNone(socket));
}

void InterestTest::ensureWantWritable(const ntsa::InterestSet& interestSet,
                                      ntsa::Handle             socket)
{
    NTSCFG_TEST_TRUE(interestSet.contains(socket));

    ntsa::Interest interest;
    bool           found = interestSet.find(&interest, socket);

    NTSCFG_TEST_TRUE(found);
    NTSCFG_TEST_EQ(interest.handle(), socket);

    NTSCFG_TEST_FALSE(interest.wantReadable());
    NTSCFG_TEST_TRUE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_FALSE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    NTSCFG_TEST_FALSE(interestSet.wantReadable(socket));
    NTSCFG_TEST_TRUE(interestSet.wantWritable(socket));
    NTSCFG_TEST_TRUE(interestSet.wantAny(socket));
    NTSCFG_TEST_FALSE(interestSet.wantBoth(socket));
    NTSCFG_TEST_FALSE(interestSet.wantNone(socket));
}

void InterestTest::ensureWantBoth(const ntsa::InterestSet& interestSet,
                                  ntsa::Handle             socket)
{
    NTSCFG_TEST_TRUE(interestSet.contains(socket));

    ntsa::Interest interest;
    bool           found = interestSet.find(&interest, socket);

    NTSCFG_TEST_TRUE(found);
    NTSCFG_TEST_EQ(interest.handle(), socket);

    NTSCFG_TEST_TRUE(interest.wantReadable());
    NTSCFG_TEST_TRUE(interest.wantWritable());
    NTSCFG_TEST_TRUE(interest.wantAny());
    NTSCFG_TEST_TRUE(interest.wantBoth());
    NTSCFG_TEST_FALSE(interest.wantNone());

    NTSCFG_TEST_TRUE(interestSet.wantReadable(socket));
    NTSCFG_TEST_TRUE(interestSet.wantWritable(socket));
    NTSCFG_TEST_TRUE(interestSet.wantAny(socket));
    NTSCFG_TEST_TRUE(interestSet.wantBoth(socket));
    NTSCFG_TEST_FALSE(interestSet.wantNone(socket));
}

}  // close namespace ntsa
}  // close namespace BloombergLP
