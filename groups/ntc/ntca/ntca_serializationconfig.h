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

#ifndef INCLUDED_NTCA_SERIALIZATIONCONFIG
#define INCLUDED_NTCA_SERIALIZATIONCONFIG

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntca_serializationtype.h>
#include <ntccfg_platform.h>
#include <ntcscm_version.h>
#include <ntsa_error.h>
#include <bdlb_nullablevalue.h>
#include <bsl_iosfwd.h>
#include <balber_berdecoderoptions.h>
#include <balber_berencoderoptions.h>
#include <baljsn_decoderoptions.h>
#include <baljsn_encoderoptions.h>
#include <balxml_decoderoptions.h>
#include <balxml_encoderoptions.h>

namespace BloombergLP {
namespace ntca {

/// Provide a description of the result of a deflate operation.
///
/// @par Attributes
/// This class is composed of the following attributes.
///
/// @li @b bytesRead:
/// The number of bytes read from the input.
///
/// @li @b bytesWritten:
/// The number of bytes written to the output.
///
/// @li @b checksum:
/// The checksum of the bytes processed.
///
/// @par Thread Safety
/// This class is not thread safe.
///
/// @ingroup module_todo
class SerializationConfig
{
    bdlb::NullableValue<balber::BerEncoderOptions> d_berEncoderOptions;
    bdlb::NullableValue<balber::BerDecoderOptions> d_berDecoderOptions;
    bdlb::NullableValue<baljsn::EncoderOptions>    d_jsnEncoderOptions;
    bdlb::NullableValue<baljsn::DecoderOptions>    d_jsnDecoderOptions;
    bdlb::NullableValue<balxml::EncoderOptions>    d_xmlEncoderOptions;
    bdlb::NullableValue<balxml::DecoderOptions>    d_xmlDecoderOptions;

  public:
    /// Create a new serialization configuration having the default value.
    /// Optionally specify a 'basicAllocator' used to supply memory. If
    /// 'basicAllocator' is 0, the currently installed default allocator is
    /// used.
    explicit SerializationConfig(bslma::Allocator* basicAllocator = 0);

    /// Create a new serialization configuration having the same value as the
    /// specified 'original' object. Optionally specify a 'basicAllocator' used
    /// to supply memory. If 'basicAllocator' is 0, the currently installed
    /// default allocator is used.
    SerializationConfig(const SerializationConfig& original,
                        bslma::Allocator*          basicAllocator = 0);

    /// Destroy this object.
    ~SerializationConfig();

    /// Assign the value of the specified 'other' object to this object. Return
    /// a reference to this modifiable object.
    SerializationConfig& operator=(const SerializationConfig& other);

    /// Reset the value of this object to its value upon default construction.
    void reset();

    /// Set the BER encoder options to the specified 'value'.
    void setBerEncoderOptions(const balber::BerEncoderOptions& value);

    /// Set the BER decoder options to the specified 'value'.
    void setBerDecoderOptions(const balber::BerDecoderOptions& value);

    /// Set the JSON encoder options to the specified 'value'.
    void setJsonEncoderOptions(const baljsn::EncoderOptions& value);

    /// Set the JSON decoder options to the specified 'value'.
    void setJsonDecoderOptions(const baljsn::DecoderOptions& value);

    /// Set the XML encoder options to the specified 'value'.
    void setXmlEncoderOptions(const balxml::EncoderOptions& value);

    /// Set the XML decoder options to the specified 'value'.
    void setXmlDecoderOptions(const balxml::DecoderOptions& value);

    /// Return the BER encoder options.
    const bdlb::NullableValue<balber::BerEncoderOptions>& berEncoderOptions()
        const;

    /// Return the BER decoder options.
    const bdlb::NullableValue<balber::BerDecoderOptions>& berDecoderOptions()
        const;

    /// Return the JSON encoder options.
    const bdlb::NullableValue<baljsn::EncoderOptions>& jsnEncoderOptions()
        const;

    /// Return the JSON decoder options.
    const bdlb::NullableValue<baljsn::DecoderOptions>& jsnDecoderOptions()
        const;

    /// Return the XML encoder options.
    const bdlb::NullableValue<balxml::EncoderOptions>& xmlEncoderOptions()
        const;

    /// Return the XML decoder options.
    const bdlb::NullableValue<balxml::DecoderOptions>& xmlDecoderOptions()
        const;

    /// Return true if this object has the same value as the specified 'other'
    /// object, otherwise return false.
    bool equals(const SerializationConfig& other) const;

    /// Format this object to the specified output 'stream' at the optionally
    /// specified indentation 'level' and return a reference to the modifiable
    /// 'stream'.  If 'level' is specified, optionally specify
    /// 'spacesPerLevel', the number of spaces per indentation level for this
    /// and all of its nested objects.  Each line is indented by the absolute
    /// value of 'level * spacesPerLevel'.  If 'level' is negative, suppress
    /// indentation of the first line.  If 'spacesPerLevel' is negative,
    /// suppress line breaks and format the entire output on one line.  If
    /// 'stream' is initially invalid, this operation has no effect.  Note that
    /// a trailing newline is provided in multiline mode only.
    bsl::ostream& print(bsl::ostream& stream,
                        int           level          = 0,
                        int           spacesPerLevel = 4) const;

    /// This type accepts an allocator argument to its constructors and may
    /// dynamically allocate memory during its operation.
    NTSCFG_TYPE_TRAIT_ALLOCATOR_AWARE(SerializationConfig);
};

/// Write the specified 'object' to the specified 'stream'. Return
/// a modifiable reference to the 'stream'.
///
/// @related ntca::SerializationConfig
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const SerializationConfig& object);

/// Return true if the specified 'lhs' has the same value as the specified
/// 'rhs', otherwise return false.
///
/// @related ntca::SerializationConfig
bool operator==(const SerializationConfig& lhs,
                const SerializationConfig& rhs);

/// Return true if the specified 'lhs' does not have the same value as the
/// specified 'rhs', otherwise return false.
///
/// @related ntca::SerializationConfig
bool operator!=(const SerializationConfig& lhs,
                const SerializationConfig& rhs);

NTCCFG_INLINE
SerializationConfig::SerializationConfig(bslma::Allocator* basicAllocator)
: d_berEncoderOptions()
, d_berDecoderOptions()
, d_jsnEncoderOptions()
, d_jsnDecoderOptions()
, d_xmlEncoderOptions(basicAllocator)
, d_xmlDecoderOptions()
{
}

NTCCFG_INLINE
SerializationConfig::SerializationConfig(const SerializationConfig& original,
                                         bslma::Allocator* basicAllocator)
: d_berEncoderOptions(original.d_berEncoderOptions)
, d_berDecoderOptions(original.d_berDecoderOptions)
, d_jsnEncoderOptions(original.d_jsnEncoderOptions)
, d_jsnDecoderOptions(original.d_jsnDecoderOptions)
, d_xmlEncoderOptions(original.d_xmlEncoderOptions, basicAllocator)
, d_xmlDecoderOptions(original.d_xmlDecoderOptions)
{
}

NTCCFG_INLINE
SerializationConfig::~SerializationConfig()
{
}

NTCCFG_INLINE
SerializationConfig& SerializationConfig::operator=(
    const SerializationConfig& other)
{
    if (this != &other) {
        d_berEncoderOptions = other.d_berEncoderOptions;
        d_berDecoderOptions = other.d_berDecoderOptions;
        d_jsnEncoderOptions = other.d_jsnEncoderOptions;
        d_jsnDecoderOptions = other.d_jsnDecoderOptions;
        d_xmlEncoderOptions = other.d_xmlEncoderOptions;
        d_xmlDecoderOptions = other.d_xmlDecoderOptions;
    }

    return *this;
}

NTCCFG_INLINE
void SerializationConfig::reset()
{
    d_berEncoderOptions.reset();
    d_berDecoderOptions.reset();
    d_jsnEncoderOptions.reset();
    d_jsnDecoderOptions.reset();
    d_xmlEncoderOptions.reset();
    d_xmlDecoderOptions.reset();
}

NTCCFG_INLINE
void SerializationConfig::setBerEncoderOptions(
    const balber::BerEncoderOptions& value)
{
    d_berEncoderOptions = value;
}

NTCCFG_INLINE
void SerializationConfig::setBerDecoderOptions(
    const balber::BerDecoderOptions& value)
{
    d_berDecoderOptions = value;
}

NTCCFG_INLINE
void SerializationConfig::setJsonEncoderOptions(
    const baljsn::EncoderOptions& value)
{
    d_jsnEncoderOptions = value;
}

NTCCFG_INLINE
void SerializationConfig::setJsonDecoderOptions(
    const baljsn::DecoderOptions& value)
{
    d_jsnDecoderOptions = value;
}

NTCCFG_INLINE
void SerializationConfig::setXmlEncoderOptions(
    const balxml::EncoderOptions& value)
{
    d_xmlEncoderOptions = value;
}

NTCCFG_INLINE
void SerializationConfig::setXmlDecoderOptions(
    const balxml::DecoderOptions& value)
{
    d_xmlDecoderOptions = value;
}

NTCCFG_INLINE
const bdlb::NullableValue<balber::BerEncoderOptions>& SerializationConfig::
    berEncoderOptions() const
{
    return d_berEncoderOptions;
}

NTCCFG_INLINE
const bdlb::NullableValue<balber::BerDecoderOptions>& SerializationConfig::
    berDecoderOptions() const
{
    return d_berDecoderOptions;
}

NTCCFG_INLINE
const bdlb::NullableValue<baljsn::EncoderOptions>& SerializationConfig::
    jsnEncoderOptions() const
{
    return d_jsnEncoderOptions;
}

NTCCFG_INLINE
const bdlb::NullableValue<baljsn::DecoderOptions>& SerializationConfig::
    jsnDecoderOptions() const
{
    return d_jsnDecoderOptions;
}

NTCCFG_INLINE
const bdlb::NullableValue<balxml::EncoderOptions>& SerializationConfig::
    xmlEncoderOptions() const
{
    return d_xmlEncoderOptions;
}

NTCCFG_INLINE
const bdlb::NullableValue<balxml::DecoderOptions>& SerializationConfig::
    xmlDecoderOptions() const
{
    return d_xmlDecoderOptions;
}

NTCCFG_INLINE
bsl::ostream& operator<<(bsl::ostream&              stream,
                         const SerializationConfig& object)
{
    return object.print(stream, 0, -1);
}

NTCCFG_INLINE
bool operator==(const SerializationConfig& lhs, const SerializationConfig& rhs)
{
    return lhs.equals(rhs);
}

NTCCFG_INLINE
bool operator!=(const SerializationConfig& lhs, const SerializationConfig& rhs)
{
    return !operator==(lhs, rhs);
}

}  // close package namespace
}  // close enterprise namespace
#endif
