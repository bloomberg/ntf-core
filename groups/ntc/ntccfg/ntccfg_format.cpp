// Copyright 2020-2023 Bloomberg Finance L.P.
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http:///www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <ntccfg_format.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntccfg_format_cpp, "$Id$ $CSID$")

#include <ntccfg_bit.h>
#include <bdlb_nullablevalue.h>
#include <bslalg_numericformatterutil.h>

#define NTCCFG_FORMAT_FLOATING_POINT_USING_VSPRINTF 1

namespace BloombergLP {
namespace ntccfg {

const char* FORMAT_UNITS[9] = {"", "K", "M", "G", "T", "P", "E", "Z", "Y"};

/// Enumerate the format field flags.
class FormatFieldFlag
{
  public:
    /// Enumerate the format field flags.
    enum Constant {
        /// Left justify the output within the field width. The corresponding
        /// flag code is '-'.
        k_JUSTIFY_LEFT = (1U << 1),

        /// Preceed the result with a plus or minus sign, even for positive
        /// numbers. By default, only negative numbers are preceded with a
        /// minus sign. The corresponding flag code is '+'.
        k_SIGN = (1U << 2),

        /// If no sign is going to be written, a blank space is inserted before
        /// the value. The corresponding flag code is ' '.
        k_SPACE = (1U << 3),

        /// Used with o, x or X specifiers the value is preceeded with 0, 0x or
        /// 0X respectively for values different than zero.
        ///
        /// Note that this is the same flag as k_DECIMAL_POINT, the flag
        /// meaning is overloaded depending on the formatted type. The
        /// corresponding flag code is '#'.
        k_BASE = (1U << 4),

        /// Used with a, A, e, E, f, F, g or G it forces the written output to
        /// contain a decimal point even if no more digits follow. By default,
        /// if no digits follow, no decimal point is written.
        ///
        /// Note that this is the same flag as k_BASE, the flag meaning is
        /// overloaded depending on the formatted type The corresponding flag
        /// code is '#'.
        k_DECIMAL_POINT = (1U << 4),

        /// Left-pads the number with zeroes instead of spaces when padding is
        /// specified. The corresponding flag code is '0'.
        k_ZERO = (1U << 5),

        /// The integer or exponent of a decimal has the thousands grouping
        /// separator applied.
        k_APOSTROPHE = (1U << 6),

        /// Left-pads the number with zeroes up to the maximum number of digits
        /// necessary to represent the integral type in its base, e.g. print
        /// 0x00FF instead of 0xFF when printing an unsigned 16-bit integer in
        /// base-16.
        ///
        /// The standard '0' flag combined with the a width format:
        ///
        ///     print("%020zx", (size_t)(15));
        ///
        /// Formats:
        ///
        ///     "0x0000000000000000000F"
        ///
        /// Even on 32-bit platorms. That is, the representation is always
        /// left-padded with zeroes to the explicit width in the format. The
        /// width and the number of prepended zeroes are combined into the same
        /// concept. However,
        ///
        ///     print("%@20zx", (size_t)(15));
        ///
        /// Formats on 32-bit platforms:
        ///
        ///     "            0x0000000F"
        ///
        /// And on 64-bit platforms:
        ///
        ///     "    0x000000000000000F"
        ///
        /// The corresponding flag code is '@'. Note that this flag is a custom
        /// extension to the standard 'printf' format.
        k_CANONICAL = (1U << 7),

        /// Format, for example, 4K instead of 4096. The corresponding flag
        /// code is '~'. Note that this flag is a custom extension to the
        /// standard 'printf' format.
        k_APPROXIMATION = (1U << 8)
    };
};

/// Enumerates the printed types.
class FormatType
{
  public:
    /// Enumerates the printed types.
    enum Value {
        /// The print type is not defined.
        e_UNDEFINED = 0,

        /// The print type is 'int8_t'.
        e_INT8 = 1,

        /// The print type is 'uint8_t'.
        e_UINT8 = 2,

        /// The print type is 'int16_t'.
        e_INT16 = 3,

        /// The print type is 'uint16_t'.
        e_UINT16 = 4,

        /// The print type is 'int32_t'.
        e_INT32 = 5,

        /// The print type is 'uint32_t'.
        e_UINT32 = 6,

        /// The print type is 'int64_t'.
        e_INT64 = 7,

        /// The print type is 'uint64_t'.
        e_UINT64 = 8,

        /// The print type is 'const void*'.
        e_ADDRESS = 9,

        /// The print type is 'float'.
        e_FLOAT = 10,

        /// The print type is 'double'.
        e_DOUBLE = 11,

        /// The print type is long double.
        e_LONG_DOUBLE = 12,

        /// The print type is 'bool'.
        e_BOOL = 13,

        /// The print type is 'char'.
        e_CHAR = 14,

        /// The print type is 'wchar_t'.
        e_WIDE_CHAR = 15,

        /// The print type is 'const char*'.
        e_STRING = 16,

        /// The print type is 'const wchar_t*'.
        e_WIDE_STRING = 17
    };
};

/// Enumerates the type length modifier.
class FormatLength
{
  public:
    /// Enumerates the type length modifier.
    enum Value {
        /// The length is the default for the specifier.
        e_DEFAULT = 0,

        /// The integer specifier in the set [di] is a 'signed char', in the
        /// set [uoxXn] is an 'unsigned char', and equal to 'n' is a 'signed
        /// char*'. The corresponding length code is 'hh'.
        e_CHAR = 1,

        /// The integer specifier in the set [di] is a 'short', in the set
        /// [uoxXn] is an 'unsigned short', and equal to 'n' is a 'short*'. The
        /// corresponding length code is 'h'.
        e_SHORT = 2,

        /// The integer specifier in the set [di] is a 'long', in the set
        /// [uoxXn] is an 'unsigned long', equal to 'c' is a 'wint_t', equal to
        /// 's' is a 'wchar_t*', and equal to 'n' is a 'long*'. The
        /// corresponding length code is 'l'.
        e_LONG = 3,

        /// The integer specifier in the set [di] is a 'long long', in the set
        /// [uoxXn] is an 'unsigned long long', and equal to 'n' is a 'long*'.
        /// The corresponding length code is 'll'.
        e_LONG_LONG = 4,

        /// The integer specifier in the set [di] is a 'intmax_t', in the set
        /// [uoxXn] is an 'uintmax_t', and equal to 'n' is a 'intmax_t*'. The
        /// corresponding length code is 'j'.
        e_INTMAX_T = 5,

        /// The integer specifier in the set [di] is a 'size_t', in the set
        /// [uoxXn] is an 'size_t', and equal to 'n' is a 'size_t*'. The
        /// corresponding length code is 'z'.
        e_SIZE_T = 6,

        /// The integer specifier in the set [di] is a 'ptrdiff_t', in the set
        /// [uoxXn] is an 'ptrdiff_t', and equal to 'n' is a 'ptrdiff_t*'. The
        /// corresponding length code is 't'.
        e_PTRDIFF_T = 7,

        /// The floating point specifier in the set [fFeEgGaA] is a 'long
        /// double'. The corresponding length code is 'L'.
        e_LONG_DOUBLE = 8
    };
};

/// Enumerates the print format type specifier.
class FormatSpec
{
  public:
    /// Enumerates the print format type specifier.
    enum Value {
        /// Format a signed integral number in decimal notation. The
        /// corresponding specifier code is 'd' or 'i'.
        e_INTEGER_DECIMAL_SIGNED = 0,

        /// Format an unsigned integral number in decimal notation. The
        /// corresponding specifier code is 'u'.
        e_INTEGER_DECIMAL_UNSIGNED = 1,

        /// Format an integral number in binary notation. The corresponding
        /// specifier code is 'b'.
        e_INTEGER_BINARY = 2,

        /// Format an integral number in octal notation. The corresponding
        /// specifier code is 'o'.
        e_INTEGER_OCTAL = 3,

        /// Format an integral number in hexadecimal notation using lowercase
        /// characters. The corresponding specifier code is 'x'.
        e_INTEGER_HEXADECIMAL_LOWERCASE = 4,

        /// Format an integral number in hexadecimal notation using uppercase
        /// characters. The corresponding specifier code is 'X'.
        e_INTEGER_HEXADECIMAL_UPPERCASE = 5,

        /// Format a floating point in decimal notation using lowercase
        /// characters. The corresponding specifier code is 'f'.
        e_FLOATING_POINT_DECIMAL_LOWERCASE = 6,

        /// Format a floating point number in decimal notation using uppercase
        /// characters. The corresponding specifier code is 'F'.
        e_FLOATING_POINT_DECIMAL_UPPERCASE = 7,

        /// Format a floating point number in scientific notation using
        /// lowercase characters. The corresponding specifier code is 'e'.
        e_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE = 8,

        /// Format a floating point number in scientific notation using
        /// uppercase characters. The corresponding specifier code is 'E'.
        e_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE = 9,

        /// Format a floating point number using the shortest representation of
        /// either a decimal notation or a scientific notation using lowercase
        /// characters. The corresponding specifier code is 'g', shortest of
        /// either 'e' or 'f'.
        e_FLOATING_POINT_SHORTEST_REP_LOWERCASE = 10,

        /// Format a floating point number using the shortest representation of
        /// either a decimal notation or a scientific notation using uppercase
        /// characters. The corresponding specifier code is 'G', shortest of
        /// either 'E' or 'F'.
        e_FLOATING_POINT_SHORTEST_REP_UPPERCASE = 11,

        /// Format a floating point number in hexadecimal notation using
        /// lowercase characters. The corresponding specifier code is 'a'.
        e_FLOATING_POINT_HEXADECIMAL_LOWERCASE = 12,

        /// Format a floating point number in hexadecimal notation using
        /// uppercase characters. The corresponding specifier code is 'A'.
        e_FLOATING_POINT_HEXADECIMAL_UPPERCASE = 13,

        /// Format a single character. The corresponding specifier code is 'c'.
        e_CHARACTER = 14,

        /// Format a null-terminated sequence of characters. The corresponding
        /// specifier code is 's'.
        e_STRING = 15,

        /// Format a memory address. The corresponding specifier code is 'p'.
        e_POINTER = 16,

        /// Do not print any but store the current number of bytes written to
        /// the parameter. The corresponding specifier code is 'n'.
        e_OUTPUT = 17
    };
};

/// Describes a print field.
class FormatField
{
  public:
    FormatField()
    : flags(0)
    , width()
    , precision()
    , length(FormatLength::e_DEFAULT)
    , specifier(FormatSpec::e_INTEGER_DECIMAL_SIGNED)
    , base(0)
    , places(0)
    , uppercase(false)
    , type(FormatType::e_UNDEFINED)
    {
    }

    /// The flags.
    bsl::uint32_t flags;

    /// The minimum width of the formatted field, if any.
    bdlb::NullableValue<bsl::size_t> width;

    /// The precision, if any.
    bdlb::NullableValue<bsl::size_t> precision;

    /// The type length modifier.
    FormatLength::Value length;

    /// The type specifier.
    FormatSpec::Value specifier;

    /// The numeric base, either 10, 8, or 16, for decimal, octal, or
    /// hexademical, respectively.
    bsl::uint32_t base;

    /// The maximum number of digits to print a "full" representation of a
    /// number in the base, excluding the base indicator (e.g. '0x'). For
    /// example, an unsigned 16-bit integer in a hexadecimal base
    /// representation requires 4 places, "FFFF". This value is zero for
    /// non-integral types.
    bsl::uint32_t places;

    /// The flag indicating the alphabetical characters in the hexadecimal
    /// or scientific notation should be printed in uppercase rather than
    /// lowercase.
    bool uppercase;

    /// The type, computed from the specifier and length.
    FormatType::Value type;
};

/// Describes the context in which formatted values were printed.
class FormatContext
{
  public:
    FormatContext()
    : size(0)
    {
    }

    /// The number of bytes written.
    bsl::size_t size;
};

/// Describes the options to print formatted values.
class FormatOptions
{
  public:
    FormatOptions()
    : checkUnderflow(false)
    , checkOverflow(false)
    , maxWidth((bsl::size_t)(-1))
    , maxLength((bsl::size_t)(-1))
    , boolalpha(false)
    {
    }

    /// Flag that indicates each value should be checked for underflow.
    bool checkUnderflow;

    /// The flag that indicates each value should be checked for overflow.
    bool checkOverflow;

    /// The maximum width of any formatted field. The default is unbounded.
    bsl::size_t maxWidth;

    /// The maximum number of characters allowed to be written to the output
    /// device. The default is unbounded.
    bsl::size_t maxLength;

    /// Format alphabetical, i.e. stringified booleans.
    bool boolalpha;
};

/// Enumerate the output flags.
class FormatOutputFlag
{
  public:
    /// Enumerate the output flags.
    enum Constant {
        /// The print 'data' having the 'size'.
        k_DEFAULT = 0,

        /// Instead of printing the 'data' having the 'size', backspace 'size'
        /// number of characters.
        k_BACKSPACE = (1 << 1)
    };
};

/// Defines a type alias for a function to print a sequence of characters to an
/// output device.
typedef bool (*FormatOutput)(void*          closure,
                             FormatContext* context,
                             const char*    data,
                             bsl::size_t    size,
                             bsl::size_t    flags);

/// Provide functions for printing formatted output.
class FormatUtil
{
  public:
    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printInt8(FormatOutput         output,
                          void*                closure,
                          FormatContext*       context,
                          const FormatOptions* options,
                          const FormatField*   field,
                          bsl::int8_t          value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printUint8(FormatOutput         output,
                           void*                closure,
                           FormatContext*       context,
                           const FormatOptions* options,
                           const FormatField*   field,
                           bsl::uint8_t         value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printInt16(FormatOutput         output,
                           void*                closure,
                           FormatContext*       context,
                           const FormatOptions* options,
                           const FormatField*   field,
                           bsl::int16_t         value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printUint16(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            bsl::uint16_t        value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printInt32(FormatOutput         output,
                           void*                closure,
                           FormatContext*       context,
                           const FormatOptions* options,
                           const FormatField*   field,
                           bsl::int32_t         value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printUint32(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            bsl::uint32_t        value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printInt64(FormatOutput         output,
                           void*                closure,
                           FormatContext*       context,
                           const FormatOptions* options,
                           const FormatField*   field,
                           bsl::int64_t         value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printUint64(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            bsl::uint64_t        value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printAddress(FormatOutput         output,
                             void*                closure,
                             FormatContext*       context,
                             const FormatOptions* options,
                             const FormatField*   field,
                             const void*          value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printFloat(FormatOutput         output,
                           void*                closure,
                           FormatContext*       context,
                           const FormatOptions* options,
                           const FormatField*   field,
                           float                value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printDouble(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            double               value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printLongDouble(FormatOutput         output,
                                void*                closure,
                                FormatContext*       context,
                                const FormatOptions* options,
                                const FormatField*   field,
                                long double          value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printBool(FormatOutput         output,
                          void*                closure,
                          FormatContext*       context,
                          const FormatOptions* options,
                          const FormatField*   field,
                          bool                 value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printChar(FormatOutput         output,
                          void*                closure,
                          FormatContext*       context,
                          const FormatOptions* options,
                          const FormatField*   field,
                          char                 value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printWideChar(FormatOutput         output,
                              void*                closure,
                              FormatContext*       context,
                              const FormatOptions* options,
                              const FormatField*   field,
                              wchar_t              value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printString(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            const char*          value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printWideString(FormatOutput         output,
                                void*                closure,
                                FormatContext*       context,
                                const FormatOptions* options,
                                const FormatField*   field,
                                const wchar_t*       value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static bool printVariableArgs(FormatOutput         output,
                                  void*                closure,
                                  FormatContext*       context,
                                  const FormatOptions* options,
                                  const char*          format,
                                  bsl::va_list         args);

    /// Write the specified variable number of 'args' in the specified
    /// 'format' to the specified 'output' device. Return the number of
    /// bytes written.
    static size_t printArgs(FormatOutput output,
                            void*        closure,
                            const char*  format,
                            bsl::va_list args);
};

/// Provide low-level printing functions for the implementation
/// of higher-level print utilities. This class is thread safe.
class FormatImpl
{
  public:
    /// Format padding consisting of spaces to format a field having the
    /// specified 'length' into the specified 'width'. Return the error.
    static bool printPaddingSpaces(FormatOutput         output,
                                   void*                closure,
                                   FormatContext*       context,
                                   const FormatOptions* options,
                                   bsl::size_t          length,
                                   bsl::size_t          width);

    /// Format padding consisting of zeroes to format a field having the
    /// specified 'length' into the specified 'width'. Return the error.
    static bool printPaddingZeroes(FormatOutput         output,
                                   void*                closure,
                                   FormatContext*       context,
                                   const FormatOptions* options,
                                   bsl::size_t          length,
                                   bsl::size_t          width);

    /// Format the specified 'field' containing the string representation of
    /// the specified 32-bit unsigned integer 'value' to the specified
    /// 'output' given the specified 'closure' according to the specified
    /// 'options'. Load into the specified 'context' the results of the
    /// operation. If the specified 'negative' flag is true, treat 'value'
    /// as the negation of the desired printed value. Return the error.
    static bool printInteger32(FormatOutput         output,
                               void*                closure,
                               FormatContext*       context,
                               const FormatOptions* options,
                               const FormatField*   field,
                               bsl::uint32_t        value,
                               bool                 negative);

    /// Format the specified 'field' containing the string representation of
    /// the specified 64-bit unsigned integer 'value' to the specified
    /// 'output' given the specified 'closure' according to the specified
    /// 'options'. Load into the specified 'context' the results of the
    /// operation. If the specified 'negative' flag is true, treat 'value'
    /// as the negation of the desired printed value. Return the error.
    static bool printInteger64(FormatOutput         output,
                               void*                closure,
                               FormatContext*       context,
                               const FormatOptions* options,
                               const FormatField*   field,
                               bsl::uint64_t        value,
                               bool                 negative);
};

/// Format padding consisting of spaces to format a field having the specified
/// 'length' into the specified 'width'.
bool FormatImpl::printPaddingSpaces(FormatOutput         output,
                                    void*                closure,
                                    FormatContext*       context,
                                    const FormatOptions* options,
                                    bsl::size_t          length,
                                    bsl::size_t          width)
{
    NTCCFG_WARNING_UNUSED(options);

    if (width > length) {
        char padding[2] = {' ', 0};

        bsl::size_t count = width - length;

        if (output) {
            for (bsl::size_t i = 0; i < count; ++i) {
                bool success = output(closure,
                                      context,
                                      padding,
                                      1,
                                      FormatOutputFlag::k_DEFAULT);
                if (!success) {
                    return false;
                }
            }
        }

        return true;
    }
    else {
        return true;
    }
}

/// Format padding consisting of zeroes to format a field having the specified
/// 'length' into the specified 'width'.
bool FormatImpl::printPaddingZeroes(FormatOutput         output,
                                    void*                closure,
                                    FormatContext*       context,
                                    const FormatOptions* options,
                                    bsl::size_t          length,
                                    bsl::size_t          width)
{
    NTCCFG_WARNING_UNUSED(options);

    if (width > length) {
        char padding[2] = {'0', 0};

        bsl::size_t count = width - length;

        if (output) {
            for (bsl::size_t i = 0; i < count; ++i) {
                bool success = output(closure,
                                      context,
                                      padding,
                                      1,
                                      FormatOutputFlag::k_DEFAULT);
                if (!success) {
                    return false;
                }
            }
        }

        return true;
    }
    else {
        return true;
    }
}

/// Format the specified 'field' containing the string representation of the
/// specified 32-bit unsigned integer 'value' to the specified 'output' given
/// the specified 'closure' according to the specified 'options'. Load into
/// the specified 'context' the results of the operation. If the specified
/// 'negative' flag is true, treat 'value' as the negation of the desired
/// printed value. Return the error.
bool FormatImpl::printInteger32(FormatOutput         output,
                                void*                closure,
                                FormatContext*       context,
                                const FormatOptions* options,
                                const FormatField*   field,
                                bsl::uint32_t        value,
                                bool                 negative)
{
    enum { k_UINT32_MAX_DIGITS_IN_BASE_2 = 32 };

    bool success;

    /// Get the field with, if any.

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    /// Get the base.

    bsl::uint32_t base = field->base;
    if (base != 2 && base != 8 && base == 10 && base == 16) {
        return false;
    }

    /// Approximate the value, if requested.

    bsl::size_t unit = 0;

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_APPROXIMATION)) {
        while (value >= 1024 &&
               unit < sizeof(FORMAT_UNITS) / sizeof(FORMAT_UNITS[0]))
        {
            value /= 1024;
            ++unit;
        }
    }

    /// Allocate a buffer sufficient to store the string representation of
    /// 'value' in any base, including the longest "hash" (or base indicator,
    /// e.g. '0x') plus the null terminator. The longest representation is in
    /// base-2, and longest hash is a tie between base-2 ('0b') and base-16
    /// ('0x').

    char buffer[k_UINT32_MAX_DIGITS_IN_BASE_2 + 2 + 1];

    char* current = buffer;
    char* end     = buffer + (sizeof(buffer) - 1);

    /// Format the reverse of the unit, if the value is approximated and its
    /// original value is greater than 1024.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_APPROXIMATION) &&
        unit > 0)
    {
        if (FORMAT_UNITS[unit][1] != 0) {
            if (current >= end) {
                return false;
            }

            *current++ = FORMAT_UNITS[unit][1];
        }

        if (current >= end) {
            return false;
        }

        *current++ = FORMAT_UNITS[unit][0];
    }

    /// Stringify the number, reversed such that the least significant digit
    /// is in the lowest memory address. The buffer will be reversed when
    /// specified to the output function.

    while (true) {
        if (current >= end) {
            return false;
        }

        const bsl::uint32_t digit = value % base;

        if (digit < 10) {
            *current++ = static_cast<char>('0' + digit);
        }
        else if (field->uppercase) {
            *current++ = static_cast<char>('A' + (digit - 10));
        }
        else {
            *current++ = static_cast<char>('a' + (digit - 10));
        }

        value /= base;
        if (value == 0) {
            break;
        }
    }

    /// Pad with zeroes up to the maximum number of digits that is canonical
    /// for the type represented in the base, if specified in the format.
    /// Note that this format flag is a custom extension to the standard
    /// 'printf' format.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_CANONICAL)) {
        bsl::size_t length = current - buffer;
        if (field->places > length) {
            bsl::size_t count = field->places - length;
            for (bsl::size_t i = 0; i < count; ++i) {
                if (current >= end) {
                    return false;
                }

                *current++ = '0';
            }
        }
    }

    /// Format the reverse of the base indicator, if specified in the format.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_BASE)) {
        if (field->base == 2) {
            if (current >= end) {
                return false;
            }

            *current++ = 'b';

            if (current >= end) {
                return false;
            }

            *current++ = '0';
        }
        else if (field->base == 8) {
            if (current >= end) {
                return false;
            }

            *current++ = '0';
        }
        else if (field->base == 16) {
            if (current >= end) {
                return false;
            }

            *current++ = 'x';

            if (current >= end) {
                return false;
            }

            *current++ = '0';
        }
    }

    /// Handle 'negative', or the space flag or the sign flag.

    if (negative) {
        if (current >= end) {
            return false;
        }

        *current++ = '-';
    }
    else if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_SIGN)) {
        if (current >= end) {
            return false;
        }

        *current++ = '+';
    }
    else if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_SPACE)) {
        if (current >= end) {
            return false;
        }

        *current++ = ' ';
    }

    /// Null terminate the buffer for cosmetic reasons to aid debugging.

    *current = 0;

    size_t length = current - buffer;

    /// Format padding to justify right within the field width, if required.

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_ZERO)) {
            success = printPaddingZeroes(output,
                                         closure,
                                         context,
                                         options,
                                         length,
                                         width);
            if (!success) {
                return false;
            }
        }
        else {
            success = printPaddingSpaces(output,
                                         closure,
                                         context,
                                         options,
                                         length,
                                         width);
            if (!success) {
                return false;
            }
        }
    }

    /// Format the reverse of the buffer.

    if (length > 0) {
        if (output) {
            bsl::size_t index = length - 1;
            while (true) {
                char ch = buffer[index];

                success = output(closure,
                                 context,
                                 &ch,
                                 1,
                                 FormatOutputFlag::k_DEFAULT);
                if (!success) {
                    return false;
                }

                if (index == 0) {
                    break;
                }

                --index;
            }
        }
    }

    /// Format padding to justify left within the field width, if required.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_ZERO)) {
            success = printPaddingZeroes(output,
                                         closure,
                                         context,
                                         options,
                                         length,
                                         width);
            if (!success) {
                return false;
            }
        }
        else {
            success = printPaddingSpaces(output,
                                         closure,
                                         context,
                                         options,
                                         length,
                                         width);
            if (!success) {
                return false;
            }
        }
    }

    return true;
}

/// Format the specified 'field' containing the string representation of the
/// specified 64-bit unsigned integer 'value' to the specified 'output' given
/// the specified 'closure' according to the specified 'options'. Load into
/// the specified 'context' the results of the operation. If the specified
/// 'negative' flag is true, treat 'value' as the negation of the desired
/// printed value. Return the error.
bool FormatImpl::printInteger64(FormatOutput         output,
                                void*                closure,
                                FormatContext*       context,
                                const FormatOptions* options,
                                const FormatField*   field,
                                bsl::uint64_t        value,
                                bool                 negative)
{
    enum { k_UINT64_MAX_DIGITS_IN_BASE_2 = 64 };

    bool success;

    /// Get the field width, if any.

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    /// Get the base.

    bsl::uint64_t base = field->base;
    if (base != 2 && base != 8 && base == 10 && base == 16) {
        return false;
    }

    /// Approximate the value, if requested.

    bsl::size_t unit = 0;

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_APPROXIMATION)) {
        while (value >= 1024 &&
               unit < sizeof(FORMAT_UNITS) / sizeof(FORMAT_UNITS[0]))
        {
            value /= 1024;
            ++unit;
        }
    }

    /// Allocate a buffer sufficient to store the string representation of
    /// 'value' in any base, including the longest "hash" (or base indicator,
    /// e.g. '0x') plus the null terminator. The longest representation is in
    /// base-2, and longest hash is a tie between base-2 ('0b') and base-16
    /// ('0x').

    char buffer[k_UINT64_MAX_DIGITS_IN_BASE_2 + 2 + 1];

    char* current = buffer;
    char* end     = buffer + (sizeof(buffer) - 1);

    /// Format the reverse of the unit, if the value is approximated and its
    /// original value is greater than 1024.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_APPROXIMATION) &&
        unit > 0)
    {
        if (FORMAT_UNITS[unit][1] != 0) {
            if (current >= end) {
                return false;
            }

            *current++ = FORMAT_UNITS[unit][1];
        }

        if (current >= end) {
            return false;
        }

        *current++ = FORMAT_UNITS[unit][0];
    }

    /// Stringify the number, reversed such that the least significant digit
    /// is in the lowest memory address. The buffer will be reversed when
    /// specified to the output function.

    while (true) {
        if (current >= end) {
            return false;
        }

        const bsl::uint64_t digit = value % base;

        if (digit < 10) {
            *current++ = static_cast<char>('0' + digit);
        }
        else if (field->uppercase) {
            *current++ = static_cast<char>('A' + (digit - 10));
        }
        else {
            *current++ = static_cast<char>('a' + (digit - 10));
        }

        value /= base;
        if (value == 0) {
            break;
        }
    }

    /// Pad with zeroes up to the maximum number of digits that is canonical
    /// for the type represented in the base, if specified in the format.
    /// Note that this format flag is a custom extension to the standard
    /// 'printf' format.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_CANONICAL)) {
        bsl::size_t length = current - buffer;
        if (field->places > length) {
            bsl::size_t count = field->places - length;
            for (bsl::size_t i = 0; i < count; ++i) {
                if (current >= end) {
                    return false;
                }

                *current++ = '0';
            }
        }
    }

    /// Format the reverse of the base indicator, if specified int the format.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_BASE)) {
        if (field->base == 2) {
            if (current >= end) {
                return false;
            }

            *current++ = 'b';

            if (current >= end) {
                return false;
            }

            *current++ = '0';
        }
        else if (field->base == 8) {
            if (current >= end) {
                return false;
            }

            *current++ = '0';
        }
        else if (field->base == 16) {
            if (current >= end) {
                return false;
            }

            *current++ = 'x';

            if (current >= end) {
                return false;
            }

            *current++ = '0';
        }
    }

    /// Handle 'negative', or the space flag or the sign flag.

    if (negative) {
        if (current >= end) {
            return false;
        }

        *current++ = '-';
    }
    else if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_SIGN)) {
        if (current >= end) {
            return false;
        }

        *current++ = '+';
    }
    else if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_SPACE)) {
        if (current >= end) {
            return false;
        }

        *current++ = ' ';
    }

    /// Null terminate the buffer for cosmetic reasons to aid debugging.

    *current = 0;

    bsl::size_t length = current - buffer;

    /// Format padding to justify right within the field width, if required.

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_ZERO)) {
            success = printPaddingZeroes(output,
                                         closure,
                                         context,
                                         options,
                                         length,
                                         width);
            if (!success) {
                return false;
            }
        }
        else {
            success = printPaddingSpaces(output,
                                         closure,
                                         context,
                                         options,
                                         length,
                                         width);
            if (!success) {
                return false;
            }
        }
    }

    /// Format the reverse of the buffer.

    if (length > 0) {
        if (output) {
            size_t index = length - 1;
            while (true) {
                char ch = buffer[index];

                success = output(closure,
                                 context,
                                 &ch,
                                 1,
                                 FormatOutputFlag::k_DEFAULT);
                if (!success) {
                    return false;
                }

                if (index == 0) {
                    break;
                }

                --index;
            }
        }
    }

    /// Format padding to justify left within the field width, if required.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_ZERO)) {
            success = printPaddingZeroes(output,
                                         closure,
                                         context,
                                         options,
                                         length,
                                         width);
            if (!success) {
                return false;
            }
        }
        else {
            success = printPaddingSpaces(output,
                                         closure,
                                         context,
                                         options,
                                         length,
                                         width);
            if (!success) {
                return false;
            }
        }
    }

    return true;
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printInt8(FormatOutput         output,
                           void*                closure,
                           FormatContext*       context,
                           const FormatOptions* options,
                           const FormatField*   field,
                           bsl::int8_t          value)
{
    bsl::uint32_t quantity;
    bool          negative;

    if (value >= 0) {
        quantity = value;
        negative = false;
    }
    else {
        quantity = -value;
        negative = true;
    }

    return FormatImpl::printInteger32(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      quantity,
                                      negative);
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printUint8(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            bsl::uint8_t         value)
{
    return FormatImpl::printInteger32(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      value,
                                      false);
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printInt16(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            bsl::int16_t         value)
{
    bsl::uint32_t quantity;
    bool          negative;

    if (value >= 0) {
        quantity = value;
        negative = false;
    }
    else {
        quantity = -value;
        negative = true;
    }

    return FormatImpl::printInteger32(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      quantity,
                                      negative);
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printUint16(FormatOutput         output,
                             void*                closure,
                             FormatContext*       context,
                             const FormatOptions* options,
                             const FormatField*   field,
                             bsl::uint16_t        value)
{
    return FormatImpl::printInteger32(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      value,
                                      false);
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printInt32(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            bsl::int32_t         value)
{
    bsl::uint32_t quantity;
    bool          negative;

    if (value >= 0) {
        quantity = value;
        negative = false;
    }
    else {
        quantity = -value;
        negative = true;
    }

    return FormatImpl::printInteger32(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      quantity,
                                      negative);
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printUint32(FormatOutput         output,
                             void*                closure,
                             FormatContext*       context,
                             const FormatOptions* options,
                             const FormatField*   field,
                             bsl::uint32_t        value)
{
    return FormatImpl::printInteger32(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      value,
                                      false);
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printInt64(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            bsl::int64_t         value)
{
    bsl::uint64_t quantity;
    bool          negative;

    if (value >= 0) {
        quantity = value;
        negative = false;
    }
    else {
        quantity = -value;
        negative = true;
    }

    return FormatImpl::printInteger64(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      quantity,
                                      negative);
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printUint64(FormatOutput         output,
                             void*                closure,
                             FormatContext*       context,
                             const FormatOptions* options,
                             const FormatField*   field,
                             bsl::uint64_t        value)
{
    return FormatImpl::printInteger64(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      value,
                                      false);
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printAddress(FormatOutput         output,
                              void*                closure,
                              FormatContext*       context,
                              const FormatOptions* options,
                              const FormatField*   field,
                              const void*          value)
{
#if defined(BSLS_PLATFORM_CPU_32_BIT)
    uint32_t quantity = (uint32_t)(value);
    return FormatImpl::printInteger32(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      quantity,
                                      false);
#else
    uint64_t quantity = (uint64_t)(value);
    return FormatImpl::printInteger64(output,
                                      closure,
                                      context,
                                      options,
                                      field,
                                      quantity,
                                      false);
#endif
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printFloat(FormatOutput         output,
                            void*                closure,
                            FormatContext*       context,
                            const FormatOptions* options,
                            const FormatField*   field,
                            float                value)
{
    bool success;

    /// Get the field width, if any.

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    enum {
        k_BUFFER_CAPACITY =
            bslalg::NumericFormatterUtil::ToCharsMaxLength<float>::k_VALUE
    };

    char buffer[k_BUFFER_CAPACITY];

#if NTCCFG_FORMAT_FLOATING_POINT_USING_VSPRINTF

    char code = 'f';

    if (NTCCFG_BIT_IS_SET(field->specifier,
                          FormatSpec::e_FLOATING_POINT_DECIMAL_LOWERCASE))
    {
        code = 'f';
    }
    else if (NTCCFG_BIT_IS_SET(field->specifier,
                               FormatSpec::e_FLOATING_POINT_DECIMAL_UPPERCASE))
    {
        code = 'F';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_HEXADECIMAL_LOWERCASE))
    {
        code = 'a';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_HEXADECIMAL_UPPERCASE))
    {
        code = 'A';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE))
    {
        code = 'e';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE))
    {
        code = 'E';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_SHORTEST_REP_LOWERCASE))
    {
        code = 'g';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_SHORTEST_REP_UPPERCASE))
    {
        code = 'G';
    }

    char format[5];

    if (field->precision.isNull()) {
        format[0] = '%';
        format[1] = code;
        format[2] = 0;
    }
    else {
        format[0] = '%';
        format[1] = '.';
        format[2] = static_cast<char>('0' + field->precision.value());
        format[3] = code;
        format[4] = 0;
    }

    bsl::size_t length = 0;

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY)
    int rc = bsl::snprintf(buffer, sizeof buffer, format, value);
    BSLS_ASSERT(rc >= 0);
    if (rc > static_cast<int>((sizeof buffer) - 1)) {
        buffer[(sizeof buffer) - 1] = 0;
        length                      = (sizeof buffer) - 1;
    }
    else {
        length = rc;
    }
#else
    int rc = sprintf(buffer, format, args);
    length = rc;
#endif

#else

    bslalg::NumericFormatterUtil::Format format =
        bslalg::NumericFormatterUtil::e_FIXED;

    if (NTCCFG_BIT_IS_SET(
            field->specifier,
            FormatSpec::e_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE) ||
        NTCCFG_BIT_IS_SET(
            field->specifier,
            FormatSpec::e_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE))
    {
        format = bslalg::NumericFormatterUtil::e_SCIENTIFIC;
    }

    const char* bufferEnd =
        bslalg::NumericFormatterUtil::toChars(buffer,
                                              buffer + sizeof(buffer),
                                              value,
                                              format);

    const bsl::size_t length = bufferEnd - buffer;

#endif

    /// Format padding to justify right within the field width, if required.

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_ZERO)) {
            success = FormatImpl::printPaddingZeroes(output,
                                                     closure,
                                                     context,
                                                     options,
                                                     length,
                                                     width);
            if (!success) {
                return false;
            }
        }
        else {
            success = FormatImpl::printPaddingSpaces(output,
                                                     closure,
                                                     context,
                                                     options,
                                                     length,
                                                     width);
            if (!success) {
                return false;
            }
        }
    }

    // Format the buffer.

    success =
        output(closure, context, buffer, length, FormatOutputFlag::k_DEFAULT);
    if (!success) {
        return false;
    }

    /// Format padding to justify left within the field width, if required.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_ZERO)) {
            success = FormatImpl::printPaddingZeroes(output,
                                                     closure,
                                                     context,
                                                     options,
                                                     length,
                                                     width);
            if (!success) {
                return false;
            }
        }
        else {
            success = FormatImpl::printPaddingSpaces(output,
                                                     closure,
                                                     context,
                                                     options,
                                                     length,
                                                     width);
            if (!success) {
                return false;
            }
        }
    }

    return true;
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printDouble(FormatOutput         output,
                             void*                closure,
                             FormatContext*       context,
                             const FormatOptions* options,
                             const FormatField*   field,
                             double               value)
{
    bool success;

    /// Get the field width, if any.

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    enum {
        k_BUFFER_CAPACITY =
            bslalg::NumericFormatterUtil::ToCharsMaxLength<double>::k_VALUE
    };

    char buffer[k_BUFFER_CAPACITY];

#if NTCCFG_FORMAT_FLOATING_POINT_USING_VSPRINTF

    char code = 'f';

    if (NTCCFG_BIT_IS_SET(field->specifier,
                          FormatSpec::e_FLOATING_POINT_DECIMAL_LOWERCASE))
    {
        code = 'f';
    }
    else if (NTCCFG_BIT_IS_SET(field->specifier,
                               FormatSpec::e_FLOATING_POINT_DECIMAL_UPPERCASE))
    {
        code = 'F';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_HEXADECIMAL_LOWERCASE))
    {
        code = 'a';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_HEXADECIMAL_UPPERCASE))
    {
        code = 'A';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE))
    {
        code = 'e';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE))
    {
        code = 'E';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_SHORTEST_REP_LOWERCASE))
    {
        code = 'g';
    }
    else if (NTCCFG_BIT_IS_SET(
                 field->specifier,
                 FormatSpec::e_FLOATING_POINT_SHORTEST_REP_UPPERCASE))
    {
        code = 'G';
    }

    char format[5];

    if (field->precision.isNull()) {
        format[0] = '%';
        format[1] = code;
        format[2] = 0;
    }
    else {
        format[0] = '%';
        format[1] = '.';
        format[2] = static_cast<char>('0' + field->precision.value());
        format[3] = code;
        format[4] = 0;
    }

    bsl::size_t length = 0;

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY)
    int rc = bsl::snprintf(buffer, sizeof buffer, format, value);
    BSLS_ASSERT(rc >= 0);
    if (rc > static_cast<int>((sizeof buffer) - 1)) {
        buffer[(sizeof buffer) - 1] = 0;
        length                      = (sizeof buffer) - 1;
    }
    else {
        length = rc;
    }
#else
    int rc = sprintf(buffer, format, args);
    length = rc;
#endif

#else

    bslalg::NumericFormatterUtil::Format format =
        bslalg::NumericFormatterUtil::e_FIXED;

    if (NTCCFG_BIT_IS_SET(
            field->specifier,
            FormatSpec::e_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE) ||
        NTCCFG_BIT_IS_SET(
            field->specifier,
            FormatSpec::e_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE))
    {
        format = bslalg::NumericFormatterUtil::e_SCIENTIFIC;
    }

    const char* bufferEnd =
        bslalg::NumericFormatterUtil::toChars(buffer,
                                              buffer + sizeof(buffer),
                                              value,
                                              format);

    const bsl::size_t length = bufferEnd - buffer;

#endif

    /// Format padding to justify right within the field width, if required.

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_ZERO)) {
            success = FormatImpl::printPaddingZeroes(output,
                                                     closure,
                                                     context,
                                                     options,
                                                     length,
                                                     width);
            if (!success) {
                return false;
            }
        }
        else {
            success = FormatImpl::printPaddingSpaces(output,
                                                     closure,
                                                     context,
                                                     options,
                                                     length,
                                                     width);
            if (!success) {
                return false;
            }
        }
    }

    // Format the buffer.

    success =
        output(closure, context, buffer, length, FormatOutputFlag::k_DEFAULT);
    if (!success) {
        return false;
    }

    /// Format padding to justify left within the field width, if required.

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_ZERO)) {
            success = FormatImpl::printPaddingZeroes(output,
                                                     closure,
                                                     context,
                                                     options,
                                                     length,
                                                     width);
            if (!success) {
                return false;
            }
        }
        else {
            success = FormatImpl::printPaddingSpaces(output,
                                                     closure,
                                                     context,
                                                     options,
                                                     length,
                                                     width);
            if (!success) {
                return false;
            }
        }
    }

    return true;
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printLongDouble(FormatOutput         output,
                                 void*                closure,
                                 FormatContext*       context,
                                 const FormatOptions* options,
                                 const FormatField*   field,
                                 long double          value)
{
    NTCCFG_WARNING_UNUSED(output);
    NTCCFG_WARNING_UNUSED(closure);
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(field);
    NTCCFG_WARNING_UNUSED(value);

    BSLS_ASSERT_OPT(!"Not implemented");
    return false;
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printBool(FormatOutput         output,
                           void*                closure,
                           FormatContext*       context,
                           const FormatOptions* options,
                           const FormatField*   field,
                           bool                 value)
{
    if (value) {
        if (options->boolalpha) {
            return FormatUtil::printString(output,
                                           closure,
                                           context,
                                           options,
                                           field,
                                           "true");
        }
        else {
            return FormatImpl::printInteger32(output,
                                              closure,
                                              context,
                                              options,
                                              field,
                                              1,
                                              false);
        }
    }
    else {
        if (options->boolalpha) {
            return FormatUtil::printString(output,
                                           closure,
                                           context,
                                           options,
                                           field,
                                           "false");
        }
        else {
            return FormatImpl::printInteger32(output,
                                              closure,
                                              context,
                                              options,
                                              field,
                                              0,
                                              false);
        }
    }
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printChar(FormatOutput         output,
                           void*                closure,
                           FormatContext*       context,
                           const FormatOptions* options,
                           const FormatField*   field,
                           char                 value)
{
    bool success;

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    bsl::size_t precision = 1;
    if (!field->precision.isNull()) {
        precision = field->precision.value();
    }

    bsl::size_t length = 1;
    if (precision < length) {
        length = precision;
    }

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        success = FormatImpl::printPaddingSpaces(output,
                                                 closure,
                                                 context,
                                                 options,
                                                 length,
                                                 width);
        if (!success) {
            return false;
        }
    }

    if (output) {
        success = output(closure,
                         context,
                         &value,
                         precision,
                         FormatOutputFlag::k_DEFAULT);
        if (!success) {
            return false;
        }
    }

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        success = FormatImpl::printPaddingSpaces(output,
                                                 closure,
                                                 context,
                                                 options,
                                                 length,
                                                 width);
        if (!success) {
            return false;
        }
    }

    return true;
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printWideChar(FormatOutput         output,
                               void*                closure,
                               FormatContext*       context,
                               const FormatOptions* options,
                               const FormatField*   field,
                               wchar_t              value)
{
    NTCCFG_WARNING_UNUSED(output);
    NTCCFG_WARNING_UNUSED(closure);
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(field);
    NTCCFG_WARNING_UNUSED(value);

    BSLS_ASSERT_OPT(!"Not implemented");
    return false;
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printString(FormatOutput         output,
                             void*                closure,
                             FormatContext*       context,
                             const FormatOptions* options,
                             const FormatField*   field,
                             const char*          value)
{
    bool success;

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    bsl::size_t precision = bsl::numeric_limits<bsl::size_t>::max();
    if (!field->precision.isNull()) {
        precision = field->precision.value();
    }

    const char* null_terminator = 0;
    {
        bsl::size_t count = precision;

        if (count != 0) {
            const char* position = value;
            do {
                if (*position++ == 0) {
                    null_terminator = position - 1;
                    break;
                }
            } while (--count != 0);
        }
    }

    bsl::size_t length;
    if (null_terminator != 0) {
        length = null_terminator - value;
    }
    else {
        length = precision;
    }

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        success = FormatImpl::printPaddingSpaces(output,
                                                 closure,
                                                 context,
                                                 options,
                                                 length,
                                                 width);
        if (!success) {
            return false;
        }
    }

    if (output) {
        success = output(closure,
                         context,
                         value,
                         length,
                         FormatOutputFlag::k_DEFAULT);
        if (!success) {
            return false;
        }
    }

    if (NTCCFG_BIT_IS_SET(field->flags, FormatFieldFlag::k_JUSTIFY_LEFT)) {
        success = FormatImpl::printPaddingSpaces(output,
                                                 closure,
                                                 context,
                                                 options,
                                                 length,
                                                 width);
        if (!success) {
            return false;
        }
    }

    return true;
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printWideString(FormatOutput         output,
                                 void*                closure,
                                 FormatContext*       context,
                                 const FormatOptions* options,
                                 const FormatField*   field,
                                 const wchar_t*       value)
{
    NTCCFG_WARNING_UNUSED(output);
    NTCCFG_WARNING_UNUSED(closure);
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(field);
    NTCCFG_WARNING_UNUSED(value);

    BSLS_ASSERT_OPT(!"Not implemented");
    return false;
}

/// Write the specified 'value' to the specified 'output' device according to
/// the specified 'format'. Return the error.
bool FormatUtil::printVariableArgs(FormatOutput         output,
                                   void*                closure,
                                   FormatContext*       context,
                                   const FormatOptions* options,
                                   const char*          format,
                                   bsl::va_list         args)
{
    /// Defines a type alias for the type that small integer types are promoted
    /// to when calling variable-argument functions.
    ///
    /// For example, calling 'var_arg' with the wrong type results in the warning:
    ///
    /// warning: 'int8_t' {aka 'signed char'} is promoted to 'int' when
    /// passed through '...' (int8_t)(va_arg(args, int8_t)));
    /// so you should pass 'int' not 'int8_t' {aka 'signed char'} to 'va_arg'
    typedef int DefaultIntegerType;

    enum {
        /// The maximum number of places required to represent an unsigned 8-bit integer
        /// in a binary represenation.
        k_UINT8_MAX_DIGITS_IN_BASE_2 = 8,

        /// The maximum number of places required to represent an unsigned 8-bit integer
        /// in an octal represenation.
        k_UINT8_MAX_DIGITS_IN_BASE_8 = 3,

        /// The maximum number of places required to represent an unsigned 8-bit integer
        /// in a decimal represenation.
        k_UINT8_MAX_DIGITS_IN_BASE_10 = 3,

        /// The maximum number of places required to represent an unsigned 8-bit integer
        /// in a hexadecimal represenation.
        k_UINT8_MAX_DIGITS_IN_BASE_16 = 2,

        /// The maximum number of places required to represent an unsigned 16-bit
        /// integer in a binary represenation.
        k_UINT16_MAX_DIGITS_IN_BASE_2 = 16,

        /// The maximum number of places required to represent an unsigned 16-bit
        /// integer in an octal represenation.
        k_UINT16_MAX_DIGITS_IN_BASE_8 = 6,

        /// The maximum number of places required to represent an unsigned 16-bit
        /// integer in decimal represenation.
        k_UINT16_MAX_DIGITS_IN_BASE_10 = 5,

        /// The maximum number of places required to represent an unsigned 16-bit
        /// integer in a hexadecimal represenation.
        k_UINT16_MAX_DIGITS_IN_BASE_16 = 4,

        /// The maximum number of places required to represent an unsigned 32-bit
        /// integer in a binary represenation.
        k_UINT32_MAX_DIGITS_IN_BASE_2 = 32,

        /// The maximum number of places required to represent an unsigned 32-bit
        /// integer in an octal represenation.
        k_UINT32_MAX_DIGITS_IN_BASE_8 = 11,

        /// The maximum number of places required to represent an unsigned 32-bit
        /// integer in a decimal represenation.
        k_UINT32_MAX_DIGITS_IN_BASE_10 = 10,

        /// The maximum number of places required to represent an unsigned 32-bit
        /// integer in a hexadecimal represenation.
        k_UINT32_MAX_DIGITS_IN_BASE_16 = 8,

        /// The maximum number of places required to represent an unsigned 64-bit
        /// integer in a binary represenation.
        k_UINT64_MAX_DIGITS_IN_BASE_2 = 64,

        /// The maximum number of places required to represent an unsigned 64-bit
        /// integer in an octal represenation.
        k_UINT64_MAX_DIGITS_IN_BASE_8 = 22,

        /// The maximum number of places required to represent an unsigned 64-bit
        /// integer in a decimal represenation.
        k_UINT64_MAX_DIGITS_IN_BASE_10 = 20,

        /// The maximum number of places required to represent an unsigned 64-bit
        /// integer in a hexadecimal represenation.
        k_UINT64_MAX_DIGITS_IN_BASE_16 = 16
    };

    bool success;

    const char* current = format;
    const char* mark    = 0;

    while (true) {
        char ch = *current;
        if (ch == 0) {
            break;
        }

        if (ch == '%') {
            if (mark != 0) {
                const char* data = mark;
                bsl::size_t size = current - mark;

                mark = 0;

                if (output) {
                    success = output(closure,
                                     context,
                                     data,
                                     size,
                                     FormatOutputFlag::k_DEFAULT);
                    if (!success) {
                        return false;
                    }
                }
            }

            ++current;

            ch = *current;
            if (ch == 0) {
                return false;
            }

            if (ch == '%') {
                if (output) {
                    success = output(closure,
                                     context,
                                     &ch,
                                     1,
                                     FormatOutputFlag::k_DEFAULT);
                    if (!success) {
                        return false;
                    }
                }

                ++current;
            }
            else {
                FormatField field;

                /// [flags][width][.precision][length]specifier

                bool success;

                char ch;

                /// Parse flags.

                ch = *current;
                if (ch == 0) {
                    return false;
                }

                while (true) {
                    if (ch == '-') {
                        field.flags |= FormatFieldFlag::k_JUSTIFY_LEFT;
                    }
                    else if (ch == '+') {
                        field.flags |= FormatFieldFlag::k_SIGN;
                    }
                    else if (ch == ' ') {
                        field.flags |= FormatFieldFlag::k_SPACE;
                    }
                    else if (ch == '#') {
                        field.flags |= (FormatFieldFlag::k_BASE |
                                        FormatFieldFlag::k_DECIMAL_POINT);
                    }
                    else if (ch == '0') {
                        field.flags |= FormatFieldFlag::k_ZERO;
                    }
                    else if (ch == '\'') {
                        field.flags |= FormatFieldFlag::k_APOSTROPHE;
                    }
                    else if (ch == '@') {
                        field.flags |= FormatFieldFlag::k_CANONICAL;
                    }
                    else if (ch == '~') {
                        field.flags |= FormatFieldFlag::k_APPROXIMATION;
                    }
                    else {
                        break;
                    }

                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }
                }

                /// Parse the width.

                if (ch == '*') {
                    int signed_width = (int)(va_arg(args, int));
                    if (signed_width < 0) {
                        field.width.makeValue((size_t)(-signed_width));
                        NTCCFG_BIT_SET(&field.flags,
                                       FormatFieldFlag::k_JUSTIFY_LEFT);
                    }
                    else {
                        field.width.makeValue((size_t)(signed_width));
                        NTCCFG_BIT_CLEAR(&field.flags,
                                         FormatFieldFlag::k_JUSTIFY_LEFT);
                    }

                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }
                }
                else if (ch >= '1' && ch <= '9') {
                    bsl::uint32_t width = 0;

                    do {
                        width = width * 10 + (uint32_t)(ch - '0');

                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }
                    } while (ch >= '0' && ch <= '9');

                    field.width.makeValue(width);
                }

                /// Parse the precision.

                if (ch == '.') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }

                    if (ch == '*') {
                        int signed_precision = (int)(va_arg(args, int));
                        if (signed_precision < 0) {
                            return false;
                        }
                        else {
                            field.precision.makeValue(
                                (size_t)(signed_precision));
                        }

                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }
                    }
                    else if (ch >= '1' && ch <= '9') {
                        bsl::uint32_t precision = 0;

                        do {
                            precision =
                                precision * 10 + (bsl::uint32_t)(ch - '0');

                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return false;
                            }
                        } while (ch >= '0' && ch <= '9');

                        field.precision.makeValue(precision);
                    }
                    else {
                        field.precision.makeValue(0);
                    }
                }

                /// Parse the length of the data type.

                if (ch == 'h') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }

                    if (ch == 'h') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }

                        field.length = FormatLength::e_CHAR;
                    }
                    else {
                        field.length = FormatLength::e_SHORT;
                    }
                }
                else if (ch == 'l') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }

                    if (ch == 'l') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }

                        field.length = FormatLength::e_LONG_LONG;
                    }
                    else {
                        field.length = FormatLength::e_LONG;
                    }
                }
                else if (ch == 'j') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }

                    field.length = FormatLength::e_INTMAX_T;
                }
                else if (ch == 'z') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }

                    field.length = FormatLength::e_SIZE_T;
                }
                else if (ch == 't') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }

                    field.length = FormatLength::e_PTRDIFF_T;
                }
                else if (ch == 'L') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }

                    field.length = FormatLength::e_LONG_DOUBLE;
                }
                else if (ch == 'I') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return false;
                    }

                    if (ch == '8') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }

                        field.length = FormatLength::e_CHAR;
                    }
                    else if (ch == '1') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }

                        if (ch == '6') {
                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return false;
                            }

                            field.length = FormatLength::e_SHORT;
                        }
                        else {
                            return false;
                        }
                    }
                    else if (ch == '3') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }

                        if (ch == '2') {
                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return false;
                            }

                            field.length = FormatLength::e_DEFAULT;
                        }
                        else {
                            return false;
                        }
                    }
                    else if (ch == '6') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }

                        if (ch == '4') {
                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return false;
                            }

                            field.length = FormatLength::e_LONG_LONG;
                        }
                        else {
                            return false;
                        }
                    }
                    else if (ch == 'R') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return false;
                        }

                        if (ch == 'G') {
                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return false;
                            }

                            field.length = FormatLength::e_SIZE_T;
                        }
                        else {
                            return false;
                        }
                    }
                    else {
                        return false;
                    }
                }

                /// Parse specifier.

                if (ch == 'd' || ch == 'i') {
                    field.specifier = FormatSpec::e_INTEGER_DECIMAL_SIGNED;
                    NTCCFG_BIT_CLEAR(&field.flags, FormatFieldFlag::k_BASE);
                    field.base = 10;
                }
                else if (ch == 'u') {
                    field.specifier = FormatSpec::e_INTEGER_DECIMAL_UNSIGNED;
                    NTCCFG_BIT_CLEAR(&field.flags, FormatFieldFlag::k_BASE);
                    field.base = 10;
                }
                else if (ch == 'b') {
                    field.specifier = FormatSpec::e_INTEGER_BINARY;
                    field.base      = 2;
                }
                else if (ch == 'o') {
                    field.specifier = FormatSpec::e_INTEGER_OCTAL;
                    field.base      = 8;
                }
                else if (ch == 'x') {
                    field.specifier =
                        FormatSpec::e_INTEGER_HEXADECIMAL_LOWERCASE;
                    field.base = 16;
                }
                else if (ch == 'X') {
                    field.specifier =
                        FormatSpec::e_INTEGER_HEXADECIMAL_UPPERCASE;
                    field.base      = 16;
                    field.uppercase = true;
                }
                else if (ch == 'f') {
                    field.specifier =
                        FormatSpec::e_FLOATING_POINT_DECIMAL_LOWERCASE;
                }
                else if (ch == 'F') {
                    field.specifier =
                        FormatSpec::e_FLOATING_POINT_DECIMAL_UPPERCASE;
                    field.uppercase = true;
                }
                else if (ch == 'e') {
                    field.specifier = FormatSpec::
                        e_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE;
                }
                else if (ch == 'E') {
                    field.specifier = FormatSpec::
                        e_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE;
                    field.uppercase = true;
                }
                else if (ch == 'g') {
                    field.specifier =
                        FormatSpec::e_FLOATING_POINT_SHORTEST_REP_LOWERCASE;
                }
                else if (ch == 'G') {
                    field.specifier =
                        FormatSpec::e_FLOATING_POINT_SHORTEST_REP_UPPERCASE;
                    field.uppercase = true;
                }
                else if (ch == 'a') {
                    field.specifier =
                        FormatSpec::e_FLOATING_POINT_HEXADECIMAL_LOWERCASE;
                    field.base = 16;
                }
                else if (ch == 'A') {
                    field.specifier =
                        FormatSpec::e_FLOATING_POINT_HEXADECIMAL_UPPERCASE;
                    field.base      = 16;
                    field.uppercase = false;
                }
                else if (ch == 'c') {
                    field.specifier = FormatSpec::e_CHARACTER;
                }
                else if (ch == 's') {
                    field.specifier = FormatSpec::e_STRING;
                }
                else if (ch == 'p') {
                    field.specifier = FormatSpec::e_POINTER;
                    field.base      = 16;
                    /// field.uppercase = true;
                    NTCCFG_BIT_SET(&field.flags, FormatFieldFlag::k_BASE);
                    /// NTCCFG_BIT_SET(&field.flags,
                    ///                FormatFieldFlag::k_CANONICAL);
                }
                else if (ch == 'n') {
                    field.specifier = FormatSpec::e_OUTPUT;
                }
                else {
                    return false;
                }

                ++current;

                /// Compute the type from the specifier and the length.

                if (field.specifier == FormatSpec::e_INTEGER_DECIMAL_SIGNED) {
                    if (field.length == FormatLength::e_DEFAULT) {
                        field.type = FormatType::e_INT32;
                    }
                    else if (field.length == FormatLength::e_CHAR) {
                        field.type = FormatType::e_INT8;
                    }
                    else if (field.length == FormatLength::e_SHORT) {
                        field.type = FormatType::e_INT16;
                    }
                    else if (field.length == FormatLength::e_LONG) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = FormatType::e_INT32;
#else
                        field.type = FormatType::e_INT64;
#endif
                    }
                    else if (field.length == FormatLength::e_LONG_LONG) {
                        field.type = FormatType::e_INT64;
                    }
                    else if (field.length == FormatLength::e_INTMAX_T) {
                        field.type = FormatType::e_INT64;
                    }
                    else if (field.length == FormatLength::e_SIZE_T) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = FormatType::e_UINT32;
#else
                        field.type = FormatType::e_UINT64;
#endif
                    }
                    else if (field.length == FormatLength::e_PTRDIFF_T) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = FormatType::e_INT32;
#else
                        field.type = FormatType::e_INT64;
#endif
                    }
                    else {
                        return false;
                    }
                }
                else if (field.specifier ==
                             FormatSpec::e_INTEGER_DECIMAL_UNSIGNED ||
                         field.specifier == FormatSpec::e_INTEGER_BINARY ||
                         field.specifier == FormatSpec::e_INTEGER_OCTAL ||
                         field.specifier ==
                             FormatSpec::e_INTEGER_HEXADECIMAL_LOWERCASE ||
                         field.specifier ==
                             FormatSpec::e_INTEGER_HEXADECIMAL_UPPERCASE)
                {
                    if (field.length == FormatLength::e_DEFAULT) {
                        field.type = FormatType::e_UINT32;
                    }
                    else if (field.length == FormatLength::e_CHAR) {
                        field.type = FormatType::e_UINT8;
                    }
                    else if (field.length == FormatLength::e_SHORT) {
                        field.type = FormatType::e_UINT16;
                    }
                    else if (field.length == FormatLength::e_LONG) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = FormatType::e_UINT32;
#else
                        field.type = FormatType::e_UINT64;
#endif
                    }
                    else if (field.length == FormatLength::e_LONG_LONG) {
                        field.type = FormatType::e_UINT64;
                    }
                    else if (field.length == FormatLength::e_INTMAX_T) {
                        field.type = FormatType::e_UINT64;
                    }
                    else if (field.length == FormatLength::e_SIZE_T) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = FormatType::e_UINT32;
#else
                        field.type = FormatType::e_UINT64;
#endif
                    }
                    else if (field.length == FormatLength::e_PTRDIFF_T) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = FormatType::e_INT32;
#else
                        field.type = FormatType::e_INT64;
#endif
                    }
                    else {
                        return false;
                    }
                }
                else if (field.specifier == FormatSpec::e_CHARACTER) {
                    if (field.length == FormatLength::e_DEFAULT) {
                        field.type = FormatType::e_CHAR;
                    }
                    else if (field.length == FormatLength::e_LONG) {
                        field.type = FormatType::e_WIDE_CHAR;
                    }
                    else {
                        return false;
                    }
                }
                else if (field.specifier == FormatSpec::e_STRING) {
                    if (field.length == FormatLength::e_DEFAULT) {
                        field.type = FormatType::e_STRING;
                    }
                    else if (field.length == FormatLength::e_LONG) {
                        field.type = FormatType::e_WIDE_STRING;
                    }
                    else {
                        return false;
                    }
                }
                else if (field.specifier == FormatSpec::e_POINTER) {
                    field.type = FormatType::e_ADDRESS;
                }
                else if (
                    field.specifier ==
                        FormatSpec::e_FLOATING_POINT_DECIMAL_LOWERCASE ||
                    field.specifier ==
                        FormatSpec::e_FLOATING_POINT_DECIMAL_UPPERCASE ||
                    field.specifier ==
                        FormatSpec::
                            e_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE ||
                    field.specifier ==
                        FormatSpec::
                            e_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE ||
                    field.specifier ==
                        FormatSpec::e_FLOATING_POINT_SHORTEST_REP_LOWERCASE ||
                    field.specifier ==
                        FormatSpec::e_FLOATING_POINT_SHORTEST_REP_UPPERCASE ||
                    field.specifier ==
                        FormatSpec::e_FLOATING_POINT_HEXADECIMAL_LOWERCASE ||
                    field.specifier ==
                        FormatSpec::e_FLOATING_POINT_HEXADECIMAL_UPPERCASE)
                {
                    if (field.length == FormatLength::e_DEFAULT) {
                        field.type = FormatType::e_DOUBLE;
                    }
                    else if (field.length == FormatLength::e_LONG_DOUBLE) {
                        field.type = FormatType::e_LONG_DOUBLE;
                    }
                    else {
                        return false;
                    }
                }
                else if (field.specifier == FormatSpec::e_OUTPUT) {
                    BSLS_ASSERT_OPT(!"Not implemented");
                    return false;
                }
                else {
                    if (field.length == FormatLength::e_DEFAULT) {
                        field.type = FormatType::e_DOUBLE;
                    }
                    else if (field.length == FormatLength::e_LONG_DOUBLE) {
                        field.type = FormatType::e_LONG_DOUBLE;
                    }
                    else {
                        return false;
                    }
                }

                /// Compute the places from the the type.

                if (field.type == FormatType::e_INT8 ||
                    field.type == FormatType::e_UINT8)
                {
                    if (field.base == 2) {
                        field.places = k_UINT8_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places = k_UINT8_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places = k_UINT8_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places = k_UINT8_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return false;
                    }
                }
                else if (field.type == FormatType::e_INT16 ||
                         field.type == FormatType::e_UINT16)
                {
                    if (field.base == 2) {
                        field.places = k_UINT16_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places = k_UINT16_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places = k_UINT16_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places = k_UINT16_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return false;
                    }
                }
                else if (field.type == FormatType::e_INT32 ||
                         field.type == FormatType::e_UINT32)
                {
                    if (field.base == 2) {
                        field.places = k_UINT32_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places = k_UINT32_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places = k_UINT32_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places = k_UINT32_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return false;
                    }
                }
                else if (field.type == FormatType::e_INT64 ||
                         field.type == FormatType::e_UINT64)
                {
                    if (field.base == 2) {
                        field.places = k_UINT64_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places = k_UINT64_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places = k_UINT64_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places = k_UINT64_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return false;
                    }
                }
                else if (field.type == FormatType::e_ADDRESS) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                    if (field.base == 2) {
                        field.places = k_UINT32_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places = k_UINT32_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places = k_UINT32_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places = k_UINT32_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return false;
                    }
#else
                    if (field.base == 2) {
                        field.places = k_UINT64_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places = k_UINT64_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places = k_UINT64_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places = k_UINT64_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return false;
                    }
#endif
                }

                /// Format the type.

                switch (field.type) {
                case FormatType::e_UNDEFINED:
                    success = false;
                    break;
                case FormatType::e_INT8:
                    success = FormatUtil::printInt8(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::int8_t)(va_arg(args, DefaultIntegerType)));
                    break;
                case FormatType::e_UINT8:
                    success = FormatUtil::printUint8(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::uint8_t)(va_arg(args, DefaultIntegerType)));
                    break;
                case FormatType::e_INT16:
                    success = FormatUtil::printInt16(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::int16_t)(va_arg(args, DefaultIntegerType)));
                    break;
                case FormatType::e_UINT16:
                    success = FormatUtil::printUint16(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::uint16_t)(va_arg(args, DefaultIntegerType)));
                    break;
                case FormatType::e_INT32:
                    success = FormatUtil::printInt32(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::int32_t)(va_arg(args, bsl::int32_t)));
                    break;
                case FormatType::e_UINT32:
                    success = FormatUtil::printUint32(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::uint32_t)(va_arg(args, bsl::uint32_t)));
                    break;
                case FormatType::e_INT64:
                    success = FormatUtil::printInt64(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::int64_t)(va_arg(args, bsl::int64_t)));
                    break;
                case FormatType::e_UINT64:
                    success = FormatUtil::printUint64(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::uint64_t)(va_arg(args, bsl::uint64_t)));
                    break;
                case FormatType::e_ADDRESS:
                    success = FormatUtil::printAddress(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (const void*)(va_arg(args, const void*)));
                    break;
                case FormatType::e_FLOAT:
                    success =
                        FormatUtil::printFloat(output,
                                               closure,
                                               context,
                                               options,
                                               &field,
                                               (float)(va_arg(args, double)));
                    break;
                case FormatType::e_DOUBLE:
                    success = FormatUtil::printDouble(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (double)(va_arg(args, double)));
                    break;
                case FormatType::e_LONG_DOUBLE:
                    success = FormatUtil::printLongDouble(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (long double)(va_arg(args, long double)));
                    break;
                case FormatType::e_BOOL:
                    success = FormatUtil::printBool(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (char)(va_arg(args, DefaultIntegerType)));
                    break;
                case FormatType::e_CHAR:
                    success = FormatUtil::printChar(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (char)(va_arg(args, DefaultIntegerType)));
                    break;
                case FormatType::e_WIDE_CHAR:
                    success = FormatUtil::printWideChar(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (wchar_t)(va_arg(args, DefaultIntegerType)));
                    break;
                case FormatType::e_STRING:
                    success = FormatUtil::printString(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (const char*)(va_arg(args, const char*)));
                    break;
                case FormatType::e_WIDE_STRING:
                    success = FormatUtil::printWideString(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (const wchar_t*)(va_arg(args, const wchar_t*)));
                    break;
                }

                if (!success) {
                    return false;
                }
            }
        }
        else {
            if (mark == 0) {
                mark = current;
            }

            ++current;
        }
    }

    if (mark != 0) {
        const char* data = mark;
        bsl::size_t size = current - mark;

        mark = 0;

        if (output) {
            success = output(closure,
                             context,
                             data,
                             size,
                             FormatOutputFlag::k_DEFAULT);
            if (!success) {
                return false;
            }
        }
    }

    return true;
}

/// Write the specified variable number of 'args' in the specified 'format' to
/// the specified 'output' device. Return the number of bytes written.
size_t FormatUtil::printArgs(FormatOutput output,
                             void*        closure,
                             const char*  format,
                             bsl::va_list args)
{
    FormatContext context;
    FormatOptions options;

    bool success = FormatUtil::printVariableArgs(output,
                                                 closure,
                                                 &context,
                                                 &options,
                                                 format,
                                                 args);
    if (!success) {
        /// TODO: Set errno or something similar to help the standard
        /// implementation.
        return context.size;
    }

    return context.size;
}

class FormatBuffer
{
  public:
    char*       data;
    bsl::size_t size;
    bsl::size_t capacity;
};

class FormatBufferUtil
{
  public:
    static bool append(FormatBuffer*  buffer,
                       FormatContext* context,
                       const char*    data,
                       bsl::size_t    size,
                       bsl::size_t    flags);
};

bool FormatBufferUtil::append(FormatBuffer*  buffer,
                              FormatContext* context,
                              const char*    data,
                              bsl::size_t    size,
                              bsl::size_t    flags)
{
    NTCCFG_WARNING_UNUSED(flags);

    if (buffer->size + size >= buffer->capacity) {
        return false;
    }

    if (size == 1) {
        buffer->data[buffer->size] = *data;
        ++buffer->size;
    }
    else {
        bsl::memcpy(buffer->data + buffer->size, data, size);
        buffer->size += size;
    }

    buffer->data[buffer->size]  = 0;
    context->size              += size;

    return true;
}

bsl::size_t Format::print(char*       destination,
                          bsl::size_t capacity,
                          const char* format,
                          ...)
{
    FormatBuffer buffer;
    buffer.data     = destination;
    buffer.size     = 0;
    buffer.capacity = capacity;

    bsl::va_list args;
    va_start(args, format);

    bsl::size_t size =
        FormatUtil::printArgs((FormatOutput)(&FormatBufferUtil::append),
                              &buffer,
                              format,
                              args);

    va_end(args);

    BSLS_ASSERT(size < capacity);
    destination[size] = 0;

    return size;
}

bsl::size_t Format::printArgs(char*        destination,
                              size_t       capacity,
                              const char*  format,
                              bsl::va_list args)
{
    FormatBuffer buffer;
    buffer.data     = destination;
    buffer.size     = 0;
    buffer.capacity = capacity;

    bsl::size_t size =
        FormatUtil::printArgs((FormatOutput)(&FormatBufferUtil::append),
                              &buffer,
                              format,
                              args);

    BSLS_ASSERT(size < capacity);
    destination[size] = 0;

    return size;
}

}  /// close package namespace
}  /// close enterprise namespace
