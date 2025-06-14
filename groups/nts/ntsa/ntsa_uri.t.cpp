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
    BALL_LOG_SET_CLASS_CATEGORY("NTSA.URI.TEST");

  public:
    // Verify a URI profile using default schemes.
    static void verifyProfileImplicit();

    // Verify a URI profile using default schemes interpreted as application
    //  schemes.
    static void verifyProfileImplicitApplication();

    // Verify a URI profile using application schemes.
    static void verifyProfileExplicitApplication();

    // Verify a URI profile using secure application schemes.
    static void verifyProfileExplicitApplicationSecurity();

    // Verify normalization of URIs using a profile.
    static void verifyProfileNormalize();

    // TODO
    static void verifyCase1();

    // TODO
    static void verifyCase2();

    // TODO
    static void verifyParseProfile();

    // TODO
    static void verifyCase3();

    // TODO
    static void verifyCase4();

    // TODO
    static void verifyCase5();

    // TODO
    static void verifyDump();
};

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyProfileImplicit)
{
    ntsa::Error error;

    ntsa::UriProfile profile(NTSCFG_TEST_ALLOCATOR);

    error = profile.registerImplicit();
    NTSCFG_TEST_OK(error);

    typedef ntsa::TransportProtocol TP;
    typedef ntsa::TransportDomain   TD;
    typedef ntsa::TransportMode     TM;

    struct Data {
        const char*                    scheme;
        const char*                    canonicalScheme;
        ntsa::TransportProtocol::Value transportProtocol;
        ntsa::TransportDomain::Value   transportDomain;
        ntsa::TransportMode::Value     transportMode;
    };

    // clang-format off
    const Data k_DATA[] = {
        { "",               "tcp",   TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
        { "tcp",            "tcp",   TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
        { "tcp+ipv4",       "tcp",   TP::e_TCP,   TD::e_IPV4,      TM::e_STREAM   },
        { "tcp+ipv6",       "tcp",   TP::e_TCP,   TD::e_IPV6,      TM::e_STREAM   },
        { "local",          "local", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
        { "unix",           "local", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
        { "udp",            "udp",   TP::e_UDP,   TD::e_UNDEFINED, TM::e_DATAGRAM },
        { "udp+ipv4",       "udp",   TP::e_UDP,   TD::e_IPV4,      TM::e_DATAGRAM },
        { "udp+ipv6",       "udp",   TP::e_UDP,   TD::e_IPV6,      TM::e_DATAGRAM },
        { "local+dgram",    "local", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "local+datagram", "local", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "unix+dgram",     "local", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "unix+datagram",  "local", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM }
    };
    // clang-format on

    const bsl::size_t k_DATA_COUNT = sizeof(k_DATA) / sizeof(k_DATA[0]);

    for (bsl::size_t i = 0; i < k_DATA_COUNT; ++i) {
        Data data = k_DATA[i];

        bsl::string          canonicalScheme;
        ntsa::TransportSuite transportSuite;

        error = profile.parseScheme(&canonicalScheme, 
                                    &transportSuite, 
                                    data.scheme);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG << "Scheme = " 
                       << data.scheme 
                       << " canonicalScheme = " 
                       << canonicalScheme 
                       << " transportSuite = " 
                       << transportSuite 
                       << BALL_LOG_END;

        NTSCFG_TEST_EQ(canonicalScheme, data.canonicalScheme);
        NTSCFG_TEST_EQ(transportSuite.transportSecurity(), 
                       ntsa::TransportSecurity::e_UNDEFINED);
        NTSCFG_TEST_EQ(transportSuite.transportProtocol(), 
                       data.transportProtocol);
        NTSCFG_TEST_EQ(transportSuite.transportDomain(), 
                       data.transportDomain);
        NTSCFG_TEST_EQ(transportSuite.transportMode(), 
                       data.transportMode);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyProfileImplicitApplication)
{
    ntsa::Error error;

    ntsa::UriProfile profile(NTSCFG_TEST_ALLOCATOR);

    error = profile.registerImplicit("http");
    NTSCFG_TEST_OK(error);

    typedef ntsa::TransportProtocol TP;
    typedef ntsa::TransportDomain   TD;
    typedef ntsa::TransportMode     TM;

    struct Data {
        const char*                    scheme;
        const char*                    canonicalScheme;
        ntsa::TransportProtocol::Value transportProtocol;
        ntsa::TransportDomain::Value   transportDomain;
        ntsa::TransportMode::Value     transportMode;
    };

    // clang-format off
    const Data k_DATA[] = {
        { "",               "http", TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
        { "tcp",            "http", TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
        { "tcp+ipv4",       "http", TP::e_TCP,   TD::e_IPV4,      TM::e_STREAM   },
        { "tcp+ipv6",       "http", TP::e_TCP,   TD::e_IPV6,      TM::e_STREAM   },
        { "local",          "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
        { "unix",           "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
        { "udp",            "http", TP::e_UDP,   TD::e_UNDEFINED, TM::e_DATAGRAM },
        { "udp+ipv4",       "http", TP::e_UDP,   TD::e_IPV4,      TM::e_DATAGRAM },
        { "udp+ipv6",       "http", TP::e_UDP,   TD::e_IPV6,      TM::e_DATAGRAM },
        { "local+dgram",    "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "local+datagram", "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "unix+dgram",     "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "unix+datagram",  "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM }
    };
    // clang-format on

    const bsl::size_t k_DATA_COUNT = sizeof(k_DATA) / sizeof(k_DATA[0]);

    for (bsl::size_t i = 0; i < k_DATA_COUNT; ++i) {
        Data data = k_DATA[i];

        bsl::string          canonicalScheme;
        ntsa::TransportSuite transportSuite;

        error = profile.parseScheme(&canonicalScheme, 
                                    &transportSuite, 
                                    data.scheme);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG << "Scheme = " 
                       << data.scheme 
                       << " canonicalScheme = " 
                       << canonicalScheme 
                       << " transportSuite = " 
                       << transportSuite 
                       << BALL_LOG_END;

        NTSCFG_TEST_EQ(canonicalScheme, data.canonicalScheme);
        NTSCFG_TEST_EQ(transportSuite.transportSecurity(), 
                       ntsa::TransportSecurity::e_UNDEFINED);
        NTSCFG_TEST_EQ(transportSuite.transportProtocol(), 
                       data.transportProtocol);
        NTSCFG_TEST_EQ(transportSuite.transportDomain(), 
                       data.transportDomain);
        NTSCFG_TEST_EQ(transportSuite.transportMode(), 
                       data.transportMode);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyProfileExplicitApplication)
{
    ntsa::Error error;

    ntsa::UriProfile profile(NTSCFG_TEST_ALLOCATOR);

    error = profile.registerExplicit("http");
    NTSCFG_TEST_OK(error);

    typedef ntsa::TransportProtocol TP;
    typedef ntsa::TransportDomain   TD;
    typedef ntsa::TransportMode     TM;

    struct Data {
        const char*                    scheme;
        const char*                    canonicalScheme;
        ntsa::TransportProtocol::Value transportProtocol;
        ntsa::TransportDomain::Value   transportDomain;
        ntsa::TransportMode::Value     transportMode;
    };

    // clang-format off
    const Data k_DATA[] = {
        { "http",                "http", TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
        { "http+tcp",            "http", TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
        { "http+tcp+ipv4",       "http", TP::e_TCP,   TD::e_IPV4,      TM::e_STREAM   },
        { "http+tcp+ipv6",       "http", TP::e_TCP,   TD::e_IPV6,      TM::e_STREAM   },
        { "http+local",          "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
        { "http+unix",           "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
        { "http+udp",            "http", TP::e_UDP,   TD::e_UNDEFINED, TM::e_DATAGRAM },
        { "http+udp+ipv4",       "http", TP::e_UDP,   TD::e_IPV4,      TM::e_DATAGRAM },
        { "http+udp+ipv6",       "http", TP::e_UDP,   TD::e_IPV6,      TM::e_DATAGRAM },
        { "http+local+dgram",    "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "http+local+datagram", "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "http+unix+dgram",     "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "http+unix+datagram",  "http", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM }
    };
    // clang-format on

    const bsl::size_t k_DATA_COUNT = sizeof(k_DATA) / sizeof(k_DATA[0]);

    for (bsl::size_t i = 0; i < k_DATA_COUNT; ++i) {
        Data data = k_DATA[i];

        bsl::string          canonicalScheme;
        ntsa::TransportSuite transportSuite;

        error = profile.parseScheme(&canonicalScheme, 
                                    &transportSuite, 
                                    data.scheme);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG << "Scheme = " 
                       << data.scheme 
                       << " canonicalScheme = " 
                       << canonicalScheme 
                       << " transportSuite = " 
                       << transportSuite 
                       << BALL_LOG_END;

        NTSCFG_TEST_EQ(canonicalScheme, data.canonicalScheme);
        NTSCFG_TEST_EQ(transportSuite.transportSecurity(), 
                       ntsa::TransportSecurity::e_UNDEFINED);
        NTSCFG_TEST_EQ(transportSuite.transportProtocol(), 
                       data.transportProtocol);
        NTSCFG_TEST_EQ(transportSuite.transportDomain(), 
                       data.transportDomain);
        NTSCFG_TEST_EQ(transportSuite.transportMode(), 
                       data.transportMode);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyProfileExplicitApplicationSecurity)
{
    ntsa::Error error;

    ntsa::UriProfile profile(NTSCFG_TEST_ALLOCATOR);

    error = profile.registerExplicit("https", 
                                     ntsa::TransportSecurity::e_TLS);
    NTSCFG_TEST_OK(error);

    typedef ntsa::TransportProtocol TP;
    typedef ntsa::TransportDomain   TD;
    typedef ntsa::TransportMode     TM;

    struct Data {
        const char*                    scheme;
        const char*                    canonicalScheme;
        ntsa::TransportProtocol::Value transportProtocol;
        ntsa::TransportDomain::Value   transportDomain;
        ntsa::TransportMode::Value     transportMode;
    };

    // clang-format off
    const Data k_DATA[] = {
        { "https",                "https", TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
        { "https+tcp",            "https", TP::e_TCP,   TD::e_UNDEFINED, TM::e_STREAM   },
        { "https+tcp+ipv4",       "https", TP::e_TCP,   TD::e_IPV4,      TM::e_STREAM   },
        { "https+tcp+ipv6",       "https", TP::e_TCP,   TD::e_IPV6,      TM::e_STREAM   },
        { "https+local",          "https", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
        { "https+unix",           "https", TP::e_LOCAL, TD::e_LOCAL,     TM::e_STREAM   },
        { "https+udp",            "https", TP::e_UDP,   TD::e_UNDEFINED, TM::e_DATAGRAM },
        { "https+udp+ipv4",       "https", TP::e_UDP,   TD::e_IPV4,      TM::e_DATAGRAM },
        { "https+udp+ipv6",       "https", TP::e_UDP,   TD::e_IPV6,      TM::e_DATAGRAM },
        { "https+local+dgram",    "https", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "https+local+datagram", "https", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "https+unix+dgram",     "https", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM },
        { "https+unix+datagram",  "https", TP::e_LOCAL, TD::e_LOCAL,     TM::e_DATAGRAM }
    };
    // clang-format on

    const bsl::size_t k_DATA_COUNT = sizeof(k_DATA) / sizeof(k_DATA[0]);

    for (bsl::size_t i = 0; i < k_DATA_COUNT; ++i) {
        Data data = k_DATA[i];

        bsl::string          canonicalScheme;
        ntsa::TransportSuite transportSuite;

        error = profile.parseScheme(&canonicalScheme, 
                                    &transportSuite, 
                                    data.scheme);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG << "Scheme = " 
                       << data.scheme 
                       << " canonicalScheme = " 
                       << canonicalScheme 
                       << " transportSuite = " 
                       << transportSuite 
                       << BALL_LOG_END;

        NTSCFG_TEST_EQ(canonicalScheme, data.canonicalScheme);
        NTSCFG_TEST_EQ(transportSuite.transportSecurity(), 
                       ntsa::TransportSecurity::e_TLS);
        NTSCFG_TEST_EQ(transportSuite.transportProtocol(), 
                       data.transportProtocol);
        NTSCFG_TEST_EQ(transportSuite.transportDomain(), 
                       data.transportDomain);
        NTSCFG_TEST_EQ(transportSuite.transportMode(), 
                       data.transportMode);
    }
}

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyProfileNormalize)
{
    ntsa::Error error;

    ntsa::UriProfile profile(NTSCFG_TEST_ALLOCATOR);

    error = profile.registerImplicit("http");
    NTSCFG_TEST_OK(error);

    error = profile.registerExplicit("http");
    NTSCFG_TEST_OK(error);

    {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        uri.setHost(ntsa::Ipv4Address::loopback());
        uri.setPort(12345);

        bsl::string text = uri.text();

        error = profile.normalize(&uri);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG_BLOCK {
            uri.dump(text, BALL_LOG_OUTPUT_STREAM);
        }
    }

    {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        uri.setHost("example.com");
        uri.setPort(12345);

        bsl::string text = uri.text();

        error = profile.normalize(&uri);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG_BLOCK {
            uri.dump(text, BALL_LOG_OUTPUT_STREAM);
        }
    }

    {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        ntsa::LocalName localName;
        localName.setValue("/path/to/socket");

        uri.setEndpoint(ntsa::Endpoint(localName));

        bsl::string text = uri.text();

        error = profile.normalize(&uri);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG_BLOCK {
            uri.dump(text, BALL_LOG_OUTPUT_STREAM);
        }
    }

    {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        uri.setScheme("tcp");
        uri.setHost(ntsa::Ipv4Address::loopback());
        uri.setPort(12345);

        bsl::string text = uri.text();

        error = profile.normalize(&uri);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG_BLOCK {
            uri.dump(text, BALL_LOG_OUTPUT_STREAM);
        }
    }

    {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        uri.setScheme("tcp");
        uri.setHost("example.com");
        uri.setPort(12345);

        bsl::string text = uri.text();

        error = profile.normalize(&uri);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG_BLOCK {
            uri.dump(text, BALL_LOG_OUTPUT_STREAM);
        }
    }

    {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        uri.setScheme("udp");
        uri.setHost(ntsa::Ipv4Address::loopback());
        uri.setPort(12345);

        bsl::string text = uri.text();

        error = profile.normalize(&uri);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG_BLOCK {
            uri.dump(text, BALL_LOG_OUTPUT_STREAM);
        }
    }

    {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        uri.setScheme("udp");
        uri.setHost("example.com");
        uri.setPort(12345);

        bsl::string text = uri.text();

        error = profile.normalize(&uri);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG_BLOCK {
            uri.dump(text, BALL_LOG_OUTPUT_STREAM);
        }
    }

    {
        ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

        ntsa::LocalName localName;
        localName.setValue("/path/to/socket");

        uri.setScheme("unix");
        uri.setEndpoint(ntsa::Endpoint(localName));

        bsl::string text = uri.text();

        error = profile.normalize(&uri);
        NTSCFG_TEST_OK(error);

        BALL_LOG_DEBUG_BLOCK {
            uri.dump(text, BALL_LOG_OUTPUT_STREAM);
        }
    }
}

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

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyParseProfile)
{
    ntsa::Error error;

    ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);

    ntsa::UriProfile profile;
    error = profile.registerImplicit("http");
    NTSCFG_TEST_OK(error);

    bool valid = false;

    valid = uri.parse("tcp://127.0.0.1:12345", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("udp://127.0.0.1:12345", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("tcp://[::1]:12345", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("udp://[::1]:12345", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("unix://[/path/to/socket]", profile);
    NTSCFG_TEST_TRUE(valid);
    
    valid = uri.parse("unix+datagram://[/path/to/socket]", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("unix+datagram://[/path/to/socket]/path/to/resource", 
                      profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("tcp://127.0.0.1:12345?foo", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("tcp://127.0.0.1:12345?foo=bar", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("tcp://127.0.0.1:12345?foo=bar&baz", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("tcp://127.0.0.1:12345?foo=bar&baz=qux", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("127.0.0.1:12345", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("[::1]:12345", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("[/path/to/socket]", profile);
    NTSCFG_TEST_TRUE(valid);

    valid = uri.parse("/path/to/socket", profile);
    NTSCFG_TEST_TRUE(valid);
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

NTSCFG_TEST_FUNCTION(ntsa::UriTest::verifyDump)
{
    ntsa::Error error;
    bool        valid = false;

    ntsa::UriProfile profile(NTSCFG_TEST_ALLOCATOR);

    error = profile.registerImplicit("rdp");
    NTSCFG_TEST_OK(error);

    error = profile.registerExplicit("rdp");
    NTSCFG_TEST_OK(error);

    bslstl::StringRef text = "udp://example.com?foo=bar#anchor";

    ntsa::Uri uri(NTSCFG_TEST_ALLOCATOR);
    valid = uri.parse(text, profile);
    NTSCFG_TEST_TRUE(valid);

    BALL_LOG_DEBUG_BLOCK
    {
        uri.dump(text, BALL_LOG_OUTPUT_STREAM);
    }
}

}  // close namespace ntsa
}  // close namespace BloombergLP
