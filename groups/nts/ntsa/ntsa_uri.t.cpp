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

#include <ntsa_uri.h>

#include <ntscfg_test.h>

#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iostream.h>

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

// clang-format off
struct {
    int         d_success;
    const char *d_output;
} DATA[] = {

    {1, "scheme://host"},

    {1, "scheme://host?k1=p1"},
    {1, "scheme://host?k1=p1&k2=p2"},
    {1, "scheme://host?k1=p1&k2=p2&k3=p3"},

    {1, "scheme://host#anchor"},

    {1, "scheme://host?k1=p1#anchor"},
    {1, "scheme://host?k1=p1&k2=p2#anchor"},
    {1, "scheme://host?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://host:12345"},

    {1, "scheme://host:12345?k1=p1"},
    {1, "scheme://host:12345?k1=p1&k2=p2"},
    {1, "scheme://host:12345?k1=p1&k2=p2&k3=p3"},

    {1, "scheme://host:12345#anchor"},

    {1, "scheme://host:12345?k1=p1#anchor"},
    {1, "scheme://host:12345?k1=p1&k2=p2#anchor"},
    {1, "scheme://host:12345?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://host:12345/path"},
    {1, "scheme://host:12345/path#anchor"},

    {1, "scheme://host:12345/path?k1=p1#anchor"},
    {1, "scheme://host:12345/path?k1=p1&k2=p2#anchor"},
    {1, "scheme://host:12345/path?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://host:12345/path/to"},
    {1, "scheme://host:12345/path/to#anchor"},

    {1, "scheme://host:12345/path/to?k1=p1#anchor"},
    {1, "scheme://host:12345/path/to?k1=p1&k2=p2#anchor"},
    {1, "scheme://host:12345/path/to?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://host:12345/path/to/resource"},
    {1, "scheme://host:12345/path/to/resource#anchor"},

    {1, "scheme://host:12345/path/to/resource?k1=p1#anchor"},
    {1, "scheme://host:12345/path/to/resource?k1=p1&k2=p2#anchor"},
    {1, "scheme://host:12345/path/to/resource?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://user@host"},

    {1, "scheme://user@host?k1=p1"},
    {1, "scheme://user@host?k1=p1&k2=p2"},
    {1, "scheme://user@host?k1=p1&k2=p2&k3=p3"},

    {1, "scheme://user@host#anchor"},

    {1, "scheme://user@host?k1=p1#anchor"},
    {1, "scheme://user@host?k1=p1&k2=p2#anchor"},
    {1, "scheme://user@host?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://user@host/path"},
    {1, "scheme://user@host/path#anchor"},

    {1, "scheme://user@host/path?k1=p1#anchor"},
    {1, "scheme://user@host/path?k1=p1&k2=p2#anchor"},
    {1, "scheme://user@host/path?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://user@host/path/to"},
    {1, "scheme://user@host/path/to#anchor"},

    {1, "scheme://user@host/path/to?k1=p1#anchor"},
    {1, "scheme://user@host/path/to?k1=p1&k2=p2#anchor"},
    {1, "scheme://user@host/path/to?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://user@host/path/to/resource"},
    {1, "scheme://user@host/path/to/resource#anchor"},

    {1, "scheme://user@host/path/to/resource?k1=p1#anchor"},
    {1, "scheme://user@host/path/to/resource?k1=p1&k2=p2#anchor"},
    {1, "scheme://user@host/path/to/resource?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://user@host:12345"},

    {1, "scheme://user@host:12345?k1=p1"},
    {1, "scheme://user@host:12345?k1=p1&k2=p2"},
    {1, "scheme://user@host:12345?k1=p1&k2=p2&k3=p3"},

    {1, "scheme://user@host:12345#anchor"},

    {1, "scheme://user@host:12345?k1=p1#anchor"},
    {1, "scheme://user@host:12345?k1=p1&k2=p2#anchor"},
    {1, "scheme://user@host:12345?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://user@host:12345/path"},
    {1, "scheme://user@host:12345/path#anchor"},

    {1, "scheme://user@host:12345/path?k1=p1#anchor"},
    {1, "scheme://user@host:12345/path?k1=p1&k2=p2#anchor"},
    {1, "scheme://user@host:12345/path?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://user@host:12345/path/to"},
    {1, "scheme://user@host:12345/path/to#anchor"},

    {1, "scheme://user@host:12345/path/to?k1=p1#anchor"},
    {1, "scheme://user@host:12345/path/to?k1=p1&k2=p2#anchor"},
    {1, "scheme://user@host:12345/path/to?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme://user@host:12345/path/to/resource"},
    {1, "scheme://user@host:12345/path/to/resource#anchor"},

    {1, "scheme://user@host:12345/path/to/resource?k1=p1#anchor"},
    {1, "scheme://user@host:12345/path/to/resource?k1=p1&k2=p2#anchor"},
    {1, "scheme://user@host:12345/path/to/resource?k1=p1&k2=p2&k3=p3#anchor"},

    // This is treated by the parser as <host:port>.
#if 0
    {1, "scheme:path"},
    {1, "scheme:path#anchor"},

    {1, "scheme:path?k1=p1"},
    {1, "scheme:path?k1=p1&k2=p2"},
    {1, "scheme:path?k1=p1&k2=p2&k3=p3"},

    {1, "scheme:path?k1=p1#anchor"},
    {1, "scheme:path?k1=p1&k2=p2#anchor"},
    {1, "scheme:path?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme:path/to"},
    {1, "scheme:path/to#anchor"},

    {1, "scheme:path/to?k1=p1"},
    {1, "scheme:path/to?k1=p1&k2=p2"},
    {1, "scheme:path/to?k1=p1&k2=p2&k3=p3"},

    {1, "scheme:path/to?k1=p1#anchor"},
    {1, "scheme:path/to?k1=p1&k2=p2#anchor"},
    {1, "scheme:path/to?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme:path/to/resource"},
    {1, "scheme:path/to/resource#anchor"},

    {1, "scheme:path/to/resource?k1=p1"},
    {1, "scheme:path/to/resource?k1=p1&k2=p2"},
    {1, "scheme:path/to/resource?k1=p1&k2=p2&k3=p3"},

    {1, "scheme:path/to/resource?k1=p1#anchor"},
    {1, "scheme:path/to/resource?k1=p1&k2=p2#anchor"},
    {1, "scheme:path/to/resource?k1=p1&k2=p2&k3=p3#anchor"},
#endif

    {1, "scheme:///path"},
    {1, "scheme:///path#anchor"},

    {1, "scheme:///path?k1=p1"},
    {1, "scheme:///path?k1=p1&k2=p2"},
    {1, "scheme:///path?k1=p1&k2=p2&k3=p3"},

    {1, "scheme:///path?k1=p1#anchor"},
    {1, "scheme:///path?k1=p1&k2=p2#anchor"},
    {1, "scheme:///path?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme:///path/to"},
    {1, "scheme:///path/to#anchor"},

    {1, "scheme:///path/to?k1=p1"},
    {1, "scheme:///path/to?k1=p1&k2=p2"},
    {1, "scheme:///path/to?k1=p1&k2=p2&k3=p3"},

    {1, "scheme:///path/to?k1=p1#anchor"},
    {1, "scheme:///path/to?k1=p1&k2=p2#anchor"},
    {1, "scheme:///path/to?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "scheme:///path/to/resource"},
    {1, "scheme:///path/to/resource#anchor"},

    {1, "scheme:///path/to/resource?k1=p1"},
    {1, "scheme:///path/to/resource?k1=p1&k2=p2"},
    {1, "scheme:///path/to/resource?k1=p1&k2=p2&k3=p3"},

    {1, "scheme:///path/to/resource?k1=p1#anchor"},
    {1, "scheme:///path/to/resource?k1=p1&k2=p2#anchor"},
    {1, "scheme:///path/to/resource?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "path"},
    {1, "path#anchor"},

    {1, "path?k1=p1"},
    {1, "path?k1=p1&k2=p2"},
    {1, "path?k1=p1&k2=p2&k3=p3"},

    {1, "path?k1=p1#anchor"},
    {1, "path?k1=p1&k2=p2#anchor"},
    {1, "path?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "path/to"},
    {1, "path/to#anchor"},

    {1, "path/to?k1=p1"},
    {1, "path/to?k1=p1&k2=p2"},
    {1, "path/to?k1=p1&k2=p2&k3=p3"},

    {1, "path/to?k1=p1#anchor"},
    {1, "path/to?k1=p1&k2=p2#anchor"},
    {1, "path/to?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "path/to/resource"},
    {1, "path/to/resource#anchor"},

    {1, "path/to/resource?k1=p1"},
    {1, "path/to/resource?k1=p1&k2=p2"},
    {1, "path/to/resource?k1=p1&k2=p2&k3=p3"},

    {1, "path/to/resource?k1=p1#anchor"},
    {1, "path/to/resource?k1=p1&k2=p2#anchor"},
    {1, "path/to/resource?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "/path"},
    {1, "/path#anchor"},

    {1, "/path?k1=p1"},
    {1, "/path?k1=p1&k2=p2"},
    {1, "/path?k1=p1&k2=p2&k3=p3"},

    {1, "/path?k1=p1#anchor"},
    {1, "/path?k1=p1&k2=p2#anchor"},
    {1, "/path?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "/path/to"},
    {1, "/path/to#anchor"},

    {1, "/path/to?k1=p1"},
    {1, "/path/to?k1=p1&k2=p2"},
    {1, "/path/to?k1=p1&k2=p2&k3=p3"},

    {1, "/path/to?k1=p1#anchor"},
    {1, "/path/to?k1=p1&k2=p2#anchor"},
    {1, "/path/to?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "/path/to/resource"},
    {1, "/path/to/resource#anchor"},

    {1, "/path/to/resource?k1=p1"},
    {1, "/path/to/resource?k1=p1&k2=p2"},
    {1, "/path/to/resource?k1=p1&k2=p2&k3=p3"},

    {1, "/path/to/resource?k1=p1#anchor"},
    {1, "/path/to/resource?k1=p1&k2=p2#anchor"},
    {1, "/path/to/resource?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "host.domain/path"},
    {1, "host.domain/path#anchor"},

    {1, "host.domain/path?k1=p1"},
    {1, "host.domain/path?k1=p1&k2=p2"},
    {1, "host.domain/path?k1=p1&k2=p2&k3=p3"},

    {1, "host.domain/path?k1=p1#anchor"},
    {1, "host.domain/path?k1=p1&k2=p2#anchor"},
    {1, "host.domain/path?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "host.domain/path/to"},
    {1, "host.domain/path/to#anchor"},

    {1, "host.domain/path/to?k1=p1"},
    {1, "host.domain/path/to?k1=p1&k2=p2"},
    {1, "host.domain/path/to?k1=p1&k2=p2&k3=p3"},

    {1, "host.domain/path/to?k1=p1#anchor"},
    {1, "host.domain/path/to?k1=p1&k2=p2#anchor"},
    {1, "host.domain/path/to?k1=p1&k2=p2&k3=p3#anchor"},

    {1, "host.domain/path/to/resource"},
    {1, "host.domain/path/to/resource#anchor"},

    {1, "host.domain/path/to/resource?k1=p1"},
    {1, "host.domain/path/to/resource?k1=p1&k2=p2"},
    {1, "host.domain/path/to/resource?k1=p1&k2=p2&k3=p3"},

    {1, "host.domain/path/to/resource?k1=p1#anchor"},
    {1, "host.domain/path/to/resource?k1=p1&k2=p2#anchor"},
    {1, "host.domain/path/to/resource?k1=p1&k2=p2&k3=p3#anchor"},

    { 1, "host.domain:12345/path" },
    { 1, "host.domain:12345/path#anchor" },

    { 1, "host.domain:12345/path?k1=p1" },
    { 1, "host.domain:12345/path?k1=p1&k2=p2" },
    { 1, "host.domain:12345/path?k1=p1&k2=p2&k3=p3" },

    { 1, "host.domain:12345/path?k1=p1#anchor" },
    { 1, "host.domain:12345/path?k1=p1&k2=p2#anchor" },
    { 1, "host.domain:12345/path?k1=p1&k2=p2&k3=p3#anchor" },

    { 1, "host.domain:12345/path/to" },
    { 1, "host.domain:12345/path/to#anchor" },

    { 1, "host.domain:12345/path/to?k1=p1" },
    { 1, "host.domain:12345/path/to?k1=p1&k2=p2" },
    { 1, "host.domain:12345/path/to?k1=p1&k2=p2&k3=p3" },

    { 1, "host.domain:12345/path/to?k1=p1#anchor" },
    { 1, "host.domain:12345/path/to?k1=p1&k2=p2#anchor" },
    { 1, "host.domain:12345/path/to?k1=p1&k2=p2&k3=p3#anchor" },

    { 1, "host.domain:12345/path/to/resource" },
    { 1, "host.domain:12345/path/to/resource#anchor" },

    { 1, "host.domain:12345/path/to/resource?k1=p1" },
    { 1, "host.domain:12345/path/to/resource?k1=p1&k2=p2" },
    { 1, "host.domain:12345/path/to/resource?k1=p1&k2=p2&k3=p3" },

    { 1, "host.domain:12345/path/to/resource?k1=p1#anchor" },
    { 1, "host.domain:12345/path/to/resource?k1=p1&k2=p2#anchor" },
    { 1, "host.domain:12345/path/to/resource?k1=p1&k2=p2&k3=p3#anchor" }
};
// clang-format on

enum { NUM_DATA = sizeof(DATA) / sizeof(*DATA) };

NTSCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        for (int i = 0; i < NUM_DATA; ++i) {
            ntsa::Uri uri(&ta);

            if (NTSCFG_TEST_VERBOSITY) {
                bsl::cout << "--" << bsl::endl;
                bsl::cout << "URI: " << DATA[i].d_output << bsl::endl;
            }

            bool isValid = uri.parse(DATA[i].d_output);
            if (!DATA[i].d_success) {
                NTSCFG_TEST_FALSE(isValid);

                if (NTSCFG_TEST_VERBOSITY) {
                    bsl::cout << "URI: "
                              << "INVALID" << bsl::endl;
                }
            }
            else {
                NTSCFG_TEST_TRUE(isValid);

                if (NTSCFG_TEST_VERBOSITY) {
                    bsl::cout << "URI: " << uri << bsl::endl;
                }

                bsl::string uri_string = uri.text();

                if (NTSCFG_TEST_VERBOSITY) {
                    bsl::cout << "URI: " << uri_string << bsl::endl;
                }

                NTSCFG_TEST_EQ(uri_string, bsl::string(DATA[i].d_output));
            }
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntscfg::TestAllocator ta;
    {
        ntsa::Error error;

        {
            const bsl::string text = "localhost";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_TRUE(uri.scheme().isNull());

            NTSCFG_TEST_FALSE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.authority().value().host().isNull());

            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().isDomainName());
            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().domainName().equals(
                    "localhost"));

            NTSCFG_TEST_TRUE(uri.authority().value().port().isNull());
        }

        {
            const bsl::string text = "localhost:80";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_TRUE(uri.scheme().isNull());

            NTSCFG_TEST_FALSE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.authority().value().host().isNull());

            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().isDomainName());
            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().domainName().equals(
                    "localhost"));

            NTSCFG_TEST_FALSE(uri.authority().value().port().isNull());
            NTSCFG_TEST_EQ(uri.authority().value().port().value(), 80);
        }

        {
            const bsl::string text = "tcp://localhost";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_FALSE(uri.scheme().isNull());
            NTSCFG_TEST_EQ(uri.scheme().value(), "tcp");

            NTSCFG_TEST_FALSE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.authority().value().host().isNull());

            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().isDomainName());
            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().domainName().equals(
                    "localhost"));

            NTSCFG_TEST_TRUE(uri.authority().value().port().isNull());
        }

        {
            const bsl::string text = "tcp://localhost:80";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_FALSE(uri.scheme().isNull());
            NTSCFG_TEST_EQ(uri.scheme().value(), "tcp");

            NTSCFG_TEST_FALSE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.authority().value().host().isNull());

            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().isDomainName());
            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().domainName().equals(
                    "localhost"));

            NTSCFG_TEST_FALSE(uri.authority().value().port().isNull());
            NTSCFG_TEST_EQ(uri.authority().value().port().value(), 80);
        }

        {
            const bsl::string text = "udp://localhost";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_FALSE(uri.scheme().isNull());
            NTSCFG_TEST_EQ(uri.scheme().value(), "udp");

            NTSCFG_TEST_FALSE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.authority().value().host().isNull());

            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().isDomainName());
            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().domainName().equals(
                    "localhost"));

            NTSCFG_TEST_TRUE(uri.authority().value().port().isNull());
        }

        {
            const bsl::string text = "udp://localhost:80";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_FALSE(uri.scheme().isNull());
            NTSCFG_TEST_EQ(uri.scheme().value(), "udp");

            NTSCFG_TEST_FALSE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.authority().value().host().isNull());

            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().isDomainName());
            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().domainName().equals(
                    "localhost"));

            NTSCFG_TEST_FALSE(uri.authority().value().port().isNull());
            NTSCFG_TEST_EQ(uri.authority().value().port().value(), 80);
        }

        {
            const bsl::string text = "unix://localhost";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_FALSE(uri.scheme().isNull());
            NTSCFG_TEST_EQ(uri.scheme().value(), "unix");

            NTSCFG_TEST_FALSE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.authority().value().host().isNull());

            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().isDomainName());
            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().domainName().equals(
                    "localhost"));

            NTSCFG_TEST_TRUE(uri.authority().value().port().isNull());
        }

        {
            const bsl::string text = "unix://localhost:80";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_FALSE(uri.scheme().isNull());
            NTSCFG_TEST_EQ(uri.scheme().value(), "unix");

            NTSCFG_TEST_FALSE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.authority().value().host().isNull());

            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().isDomainName());
            NTSCFG_TEST_TRUE(
                uri.authority().value().host().value().domainName().equals(
                    "localhost"));

            NTSCFG_TEST_FALSE(uri.authority().value().port().isNull());
            NTSCFG_TEST_EQ(uri.authority().value().port().value(), 80);
        }

        {
            const bsl::string text = "/path";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_TRUE(uri.scheme().isNull());

            NTSCFG_TEST_TRUE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.path().isNull());
            NTSCFG_TEST_EQ(uri.path().value(), "/path");
        }

        {
            const bsl::string text = "/path/to/resource";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_TRUE(uri.scheme().isNull());

            NTSCFG_TEST_TRUE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.path().isNull());
            NTSCFG_TEST_EQ(uri.path().value(), "/path/to/resource");
        }

        {
            const bsl::string text = "unix:///path";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_FALSE(uri.scheme().isNull());
            NTSCFG_TEST_EQ(uri.scheme().value(), "unix");

            NTSCFG_TEST_TRUE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.path().isNull());
            NTSCFG_TEST_EQ(uri.path().value(), "/path");
        }

        {
            const bsl::string text = "unix:///path/to/resource";

            ntsa::Uri uri(&ta);

            bool parseResult = uri.parse(text);
            NTSCFG_TEST_TRUE(parseResult);

            NTSCFG_TEST_FALSE(uri.scheme().isNull());
            NTSCFG_TEST_EQ(uri.scheme().value(), "unix");

            NTSCFG_TEST_TRUE(uri.authority().isNull());

            NTSCFG_TEST_FALSE(uri.path().isNull());
            NTSCFG_TEST_EQ(uri.path().value(), "/path/to/resource");
        }
    }
    NTSCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTSCFG_TEST_DRIVER
{
    NTSCFG_TEST_REGISTER(1);
    NTSCFG_TEST_REGISTER(2);
}
NTSCFG_TEST_DRIVER_END;
