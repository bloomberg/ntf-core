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
BSLS_IDENT_RCSID(ntsa_uri_t_cpp, "$Id$ $CSID$")

#include <ntsa_uri.h>

#include <balber_berdecoder.h>
#include <balber_berencoder.h>
#include <baljsn_decoder.h>
#include <baljsn_encoder.h>

using namespace BloombergLP;

namespace BloombergLP {
namespace ntsa {

// Provide tests for 'ntsa::Uri'.
class UriTest
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

    // TODO
    static void verifyCase5();
};

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyCase1)
{
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

    ntsa::Error error;

    for (int i = 0; i < NUM_DATA; ++i) {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyCase2)
{
    ntsa::Error error;

    {
        const bsl::string text = "localhost";

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        bool parseResult = uri.parse(text);
        NTSCFG_TEST_TRUE(parseResult);

        NTSCFG_TEST_TRUE(uri.scheme().isNull());

        NTSCFG_TEST_TRUE(uri.authority().isNull());

        NTSCFG_TEST_FALSE(uri.path().isNull());
        NTSCFG_TEST_EQ(uri.path().value(), "/path");
    }

    {
        const bsl::string text = "/path/to/resource";

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        bool parseResult = uri.parse(text);
        NTSCFG_TEST_TRUE(parseResult);

        NTSCFG_TEST_TRUE(uri.scheme().isNull());

        NTSCFG_TEST_TRUE(uri.authority().isNull());

        NTSCFG_TEST_FALSE(uri.path().isNull());
        NTSCFG_TEST_EQ(uri.path().value(), "/path/to/resource");
    }

    {
        const bsl::string text = "unix:///path";

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

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

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        bool parseResult = uri.parse(text);
        NTSCFG_TEST_TRUE(parseResult);

        NTSCFG_TEST_FALSE(uri.scheme().isNull());
        NTSCFG_TEST_EQ(uri.scheme().value(), "unix");

        NTSCFG_TEST_TRUE(uri.authority().isNull());

        NTSCFG_TEST_FALSE(uri.path().isNull());
        NTSCFG_TEST_EQ(uri.path().value(), "/path/to/resource");
    }
}

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyCase3)
{
    ntsa::Error error;

    {
        ntsa::Endpoint endpoint(
            ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 80));

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        error = uri.setEndpoint(endpoint);
        NTSCFG_TEST_OK(error);

        error = uri.setPath("path/to/resource");
        NTSCFG_TEST_OK(error);

        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "URI: " << uri.text() << " (endpoint: " << endpoint
                      << ")" << bsl::endl;
        }
    }

    {
        ntsa::Endpoint endpoint(
            ntsa::IpEndpoint(ntsa::Ipv4Address::loopback(), 80));

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        error = uri.setScheme("http");
        NTSCFG_TEST_OK(error);

        error = uri.setEndpoint(endpoint);
        NTSCFG_TEST_OK(error);

        error = uri.setPath("path/to/resource");
        NTSCFG_TEST_OK(error);

        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "URI: " << uri.text() << " (endpoint: " << endpoint
                      << ")" << bsl::endl;
        }
    }

    {
        ntsa::Endpoint endpoint(
            ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 80));

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        error = uri.setEndpoint(endpoint);
        NTSCFG_TEST_OK(error);

        error = uri.setPath("path/to/resource");
        NTSCFG_TEST_OK(error);

        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "URI: " << uri.text() << " (endpoint: " << endpoint
                      << ")" << bsl::endl;
        }
    }

    {
        ntsa::Endpoint endpoint(
            ntsa::IpEndpoint(ntsa::Ipv6Address::loopback(), 80));

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        error = uri.setScheme("http");
        NTSCFG_TEST_OK(error);

        error = uri.setEndpoint(endpoint);
        NTSCFG_TEST_OK(error);

        error = uri.setPath("path/to/resource");
        NTSCFG_TEST_OK(error);

        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "URI: " << uri.text() << " (endpoint: " << endpoint
                      << ")" << bsl::endl;
        }
    }

    {
        ntsa::LocalName localName;
        localName.setValue("/tmp/ntf/socket");

        ntsa::Endpoint endpoint(localName);

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        error = uri.setEndpoint(endpoint);
        NTSCFG_TEST_OK(error);

        error = uri.setPath("path/to/resource");
        NTSCFG_TEST_OK(error);

        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "URI: " << uri.text() << " (endpoint: " << endpoint
                      << ")" << bsl::endl;
        }
    }

    {
        ntsa::LocalName localName;
        localName.setValue("/tmp/ntf/socket");

        ntsa::Endpoint endpoint(localName);

        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        error = uri.setScheme("http");
        NTSCFG_TEST_OK(error);

        error = uri.setEndpoint(endpoint);
        NTSCFG_TEST_OK(error);

        error = uri.setPath("path/to/resource");
        NTSCFG_TEST_OK(error);

        if (NTSCFG_TEST_VERBOSITY) {
            bsl::cout << "URI: " << uri.text() << " (endpoint: " << endpoint
                      << ")" << bsl::endl;
        }
    }
}

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyCase4)
{
    int rc;

    ntsa::Uri e1;
    ntsa::Uri e2;

    bool valid =
        e1.parse("http://10.26.55.100:12345/path/to/resource?foo=bar#baz");
    NTSCFG_TEST_TRUE(valid);

    bdlsb::MemOutStreamBuf osb(NTSCFG_TEST_ALLOCATOR);

    balber::BerEncoder encoder(0, NTSCFG_TEST_ALLOCATOR);
    rc = encoder.encode(&osb, e1);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    NTSCFG_TEST_GT(osb.length(), 0);
    NTSCFG_TEST_NE(osb.data(), 0);

    NTSCFG_TEST_LOG_DEBUG << "Encoded:\n"
                          << bdlb::PrintStringHexDumper(
                                 osb.data(),
                                 static_cast<bsl::size_t>(osb.length()))
                          << NTSCFG_TEST_LOG_END;

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    balber::BerDecoder decoder(0, NTSCFG_TEST_ALLOCATOR);
    rc = decoder.decode(&isb, &e2);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    NTSCFG_TEST_EQ(e2, e1);
}

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyCase5)
{
    int rc;

    bsl::vector<ntsa::Uri> e1(NTSCFG_TEST_ALLOCATOR);
    bsl::vector<ntsa::Uri> e2(NTSCFG_TEST_ALLOCATOR);

    {
        ntsa::Uri element;

        bool valid = element.parse(
            "http://ntf.example.com:12345/path/to/resource?foo=bar#baz");
        NTSCFG_TEST_TRUE(valid);

        e1.push_back(element);
    }

    {
        ntsa::Uri element;

        bool valid = element.parse(
            "http://10.26.55.100:12345/path/to/resource?foo=bar#baz");
        NTSCFG_TEST_TRUE(valid);

        e1.push_back(element);
    }

    {
        ntsa::Uri element;

        ntsa::LocalName localName;
        localName.setValue("/tmp/ntf/socket");

        element.setEndpoint(ntsa::Endpoint(localName));

        e1.push_back(element);
    }

    bdlsb::MemOutStreamBuf osb;

    baljsn::Encoder encoder(NTSCFG_TEST_ALLOCATOR);
    rc = encoder.encode(&osb, e1);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    rc = osb.pubsync();
    NTSCFG_TEST_EQ(rc, 0);

    NTSCFG_TEST_GT(osb.length(), 0);
    NTSCFG_TEST_NE(osb.data(), 0);

    NTSCFG_TEST_LOG_DEBUG << "Encoded: "
                          << bsl::string_view(
                                 osb.data(),
                                 static_cast<bsl::size_t>(osb.length()))
                          << NTSCFG_TEST_LOG_END;

    bdlsb::FixedMemInStreamBuf isb(osb.data(), osb.length());

    baljsn::Decoder decoder(NTSCFG_TEST_ALLOCATOR);
    rc = decoder.decode(&isb, &e2);
    if (rc != 0) {
        NTSCFG_TEST_LOG_DEBUG << encoder.loggedMessages()
                              << NTSCFG_TEST_LOG_END;

        NTSCFG_TEST_EQ(rc, 0);
    }

    NTSCFG_TEST_EQ(e2.size(), e1.size());

    for (bsl::size_t i = 0; i < e1.size(); ++i) {
        NTSCFG_TEST_EQ(e2[i], e1[i]);
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
