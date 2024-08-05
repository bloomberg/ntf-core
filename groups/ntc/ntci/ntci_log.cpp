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

#include <ntci_log.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntci_log_cpp, "$Id$ $CSID$")

#include <ntccfg_tune.h>
#include <bdlsb_fixedmemoutstreambuf.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bslmt_lockguard.h>
#include <bslmt_mutex.h>
#include <bslmt_threadutil.h>
#include <bsls_assert.h>
#include <bsls_spinlock.h>
#include <bsl_cstdarg.h>
#include <bsl_cstdio.h>
#include <bsl_cstdlib.h>
#include <bsl_cstring.h>
#include <bsl_limits.h>
#include <bsl_sstream.h>

#define NTCI_LOG_UNSET_CONTEXT 0

// Define to 1 to use 'vsprintf'. Uncomment, undefine or set to zero to use
// the custom formatting functions.
#define NTCI_LOG_VSPRINTF 1

namespace BloombergLP {
namespace ntci {

namespace {

bslmt::ThreadUtil::Key s_key;

enum { k_MAX_LOG_RECORDS = 256 };

struct LogRecord {
    LogRecord()
    : d_severity(bsls::LogSeverity::e_TRACE)
    , d_file(0)
    , d_line(0)
    , d_length(0)
    {
        d_buffer[0] = 0;
    }

    bsls::LogSeverity::Enum d_severity;
    const char*             d_file;
    int                     d_line;
    char                    d_buffer[2048];
    int                     d_length;
};

struct LogJournal {
    explicit LogJournal(bslma::Allocator* basicAllocator = 0)
    : d_mutex()
    , d_records(basicAllocator)
    , d_position(0)
    , d_allocator_p(bslma::Default::allocator(basicAllocator))
    {
        d_records.resize(k_MAX_LOG_RECORDS);
    }

    /// Define a type alias for a mutex.
    typedef ntccfg::Mutex Mutex;

    /// Define a type alias for a mutex lock guard.
    typedef ntccfg::LockGuard LockGuard;

    Mutex                  d_mutex;
    bsl::vector<LogRecord> d_records;
    int                    d_position;
    bslma::Allocator*      d_allocator_p;
};

bsls::AtomicBool s_journalEnabled;
bsls::SpinLock   s_journalLock = BSLS_SPINLOCK_UNLOCKED;
LogJournal*      s_journal_p;

void flushLogJournalLocked()
{
    for (int i = 0; i < s_journal_p->d_position; ++i) {
        BSLS_ASSERT(s_journal_p->d_position < k_MAX_LOG_RECORDS);
        const LogRecord* record = &s_journal_p->d_records[i];
        bsls::Log::logMessage(record->d_severity,
                              record->d_file,
                              record->d_line,
                              record->d_buffer);
    }

    s_journal_p->d_position = 0;
}

void destroyKey(void* key)
{
    if (key) {
        typedef ntci::LogContext Type;

        Type* logContext = reinterpret_cast<Type*>(key);
        logContext->~Type();

        bsl::free(key);
    }
}

struct Initializer {
    Initializer()
    {
        int rc = bslmt::ThreadUtil::createKey(&s_key, &destroyKey);
        BSLS_ASSERT_OPT(rc == 0);

        bool journal;
        if (ntccfg::Tune::configure(&journal, "NTC_LOG_JOURNAL")) {
            ntci::Log::initialize(journal);
        }
    }

    ~Initializer()
    {
        ntci::Log::exit();

        // int rc = bslmt::ThreadUtil::deleteKey(s_key);
        // BSLS_ASSERT_OPT(rc == 0);
    }
} s_initializer;

// enum PrintFieldType

enum PrintFieldType {
    // Enumerates the printed types.

    // The print type is not defined.
    PRINT_FIELD_TYPE_UNDEFINED = 0,

    // The print type is 'int8_t'.
    PRINT_FIELD_TYPE_INT8 = 1,

    // The print type is 'uint8_t'.
    PRINT_FIELD_TYPE_UINT8 = 2,

    // The print type is 'int16_t'.
    PRINT_FIELD_TYPE_INT16 = 3,

    // The print type is 'uint16_t'.
    PRINT_FIELD_TYPE_UINT16 = 4,

    // The print type is 'int32_t'.
    PRINT_FIELD_TYPE_INT32 = 5,

    // The print type is 'uint32_t'.
    PRINT_FIELD_TYPE_UINT32 = 6,

    // The print type is 'int64_t'.
    PRINT_FIELD_TYPE_INT64 = 7,

    // The print type is 'uint64_t'.
    PRINT_FIELD_TYPE_UINT64 = 8,

    // The print type is 'const void*'.
    PRINT_FIELD_TYPE_ADDRESS = 9,

    // The print type is 'float'.
    PRINT_FIELD_TYPE_FLOAT = 10,

    // The print type is 'double'.
    PRINT_FIELD_TYPE_DOUBLE = 11,

    // The print type is long double.
    PRINT_FIELD_TYPE_LONG_DOUBLE = 12,

    // The print type is 'bool'.
    PRINT_FIELD_TYPE_BOOL = 13,

    // The print type is 'char'.
    PRINT_FIELD_TYPE_CHAR = 14,

    // The print type is 'wchar_t'.
    PRINT_FIELD_TYPE_WIDE_CHAR = 15,

    // The print type is 'const char*'.
    PRINT_FIELD_TYPE_STRING = 16,

    // The print type is 'const wchar_t*'.
    PRINT_FIELD_TYPE_WIDE_STRING = 17
};

// Left justify the output within the field width. The corresponding flag
// code is '-'.
#define PRINT_FIELD_FLAG_JUSTIFY_LEFT (1U << 1)

// Preceed the result with a plus or minus sign, even for positive numbers. By
// default, only negative numbers are preceded with a minus sign. The
// corresponding flag code is '+'.
#define PRINT_FIELD_FLAG_SIGN (1U << 2)

// If no sign is going to be written, a blank space is inserted before the
// value. The corresponding flag code is ' '.
#define PRINT_FIELD_FLAG_SPACE (1U << 3)

// Used with o, x or X specifiers the value is preceeded with 0, 0x or 0X
// respectively for values different than zero.
//
// Note that this is the same flag as PRINT_FIELD_FLAG_DECIMAL_POINT, the flag
// meaning is overloaded depending on the formatted type. The corresponding
// flag code is '#'.
#define PRINT_FIELD_FLAG_BASE (1U << 4)

// Used with a, A, e, E, f, F, g or G it forces the written output to contain
// a decimal point even if no more digits follow. By default, if no digits
// follow, no decimal point is written.
//
// Note that this is the same flag as PRINT_FIELD_FLAG_BASE, the flag meaning
// is overloaded depending on the formatted type The corresponding flag code
// is '#'.
#define PRINT_FIELD_FLAG_DECIMAL_POINT (1U << 4)

// Left-pads the number with zeroes instead of spaces when padding is
// specified. The corresponding flag code is '0'.
#define PRINT_FIELD_FLAG_ZERO (1U << 5)

// The integer or exponent of a decimal has the thousands grouping separator
// applied.
#define PRINT_FIELD_FLAG_APOSTROPHE (1U << 6)

// Left-pads the number with zeroes up to the maximum number of digits
// necessary to represent the integral type in its base, e.g. print 0x00FF
// instead of 0xFF when printing an unsigned 16-bit integer in base-16.
//
// The standard '0' flag combined with the a width format:
//
//     print("%020zx", (size_t)(15));
//
// Prints:
//
//     "0x0000000000000000000F"
//
// Even on 32-bit platorms. That is, the representation is always left-padded
// with zeroes to the explicit width in the format. The width and the number of
// prepended zeroes are combined into the same concept. However,
//
//     print("%@20zx", (size_t)(15));
//
// Prints on 32-bit platforms:
//
//     "            0x0000000F"
//
// And on 64-bit platforms:
//
//     "    0x000000000000000F"
//
// The corresponding flag code is '@'. Note that this flag is a custom
// extension to the standard 'printf' format.
#define PRINT_FIELD_FLAG_CANONICAL (1U << 7)

// Print, for example, 4K instead of 4096. The corresponding flag code is '~'.
// Note that this flag is a custom extension to the standard 'printf' format.
#define PRINT_FIELD_FLAG_APPROXIMATION (1U << 8)

// enum PrintFieldLength

enum PrintFieldLength {
    // Enumerates the type length modifier.

    // The length is the default for the specifier.
    PRINT_FIELD_LENGTH_DEFAULT = 0,

    // The integer specifier in the set [di] is a 'signed char', in the
    // set [uoxXn] is an 'unsigned char', and equal to 'n' is a 'signed char*'.
    // The corresponding length code is 'hh'.
    PRINT_FIELD_LENGTH_CHAR = 1,

    // The integer specifier in the set [di] is a 'short', in the set [uoxXn]
    // is an 'unsigned short', and equal to 'n' is a 'short*'. The
    // corresponding length code is 'h'.
    PRINT_FIELD_LENGTH_SHORT = 2,

    // The integer specifier in the set [di] is a 'long', in the set [uoxXn]
    // is an 'unsigned long', equal to 'c' is a 'wint_t', equal to 's' is a
    // 'wchar_t*', and equal to 'n' is a 'long*'. The corresponding length code
    // is 'l'.
    PRINT_FIELD_LENGTH_LONG = 3,

    // The integer specifier in the set [di] is a 'long long', in the set
    // [uoxXn] is an 'unsigned long long', and equal to 'n' is a 'long*'. The
    // corresponding length code is 'll'.
    PRINT_FIELD_LENGTH_LONG_LONG = 4,

    // The integer specifier in the set [di] is a 'intmax_t', in the set
    // [uoxXn] is an 'uintmax_t', and equal to 'n' is a 'intmax_t*'. The
    // corresponding length code is 'j'.
    PRINT_FIELD_LENGTH_INTMAX_T = 5,

    // The integer specifier in the set [di] is a 'size_t', in the set [uoxXn]
    // is an 'size_t', and equal to 'n' is a 'size_t*'. The corresponding
    // length code is 'z'.
    PRINT_FIELD_LENGTH_SIZE_T = 6,

    // The integer specifier in the set [di] is a 'ptrdiff_t', in the set
    // [uoxXn] is an 'ptrdiff_t', and equal to 'n' is a 'ptrdiff_t*'. The
    // corresponding length code is 't'.
    PRINT_FIELD_LENGTH_PTRDIFF_T = 7,

    // The floating point specifier in the set [fFeEgGaA] is a 'long double'.
    // The corresponding length code is 'L'.
    PRINT_FIELD_LENGTH_LONG_DOUBLE = 8
};

// enum PrintFieldSpecifier

enum PrintFieldSpecifier {
    // Enumerates the print format type specifier.

    // Print a signed integral number in decimal notation. The corresponding
    // specifier code is 'd' or 'i'.
    PRINT_FIELD_SPECIFIER_INTEGER_DECIMAL_SIGNED = 0,

    // Print an unsigned integral number in decimal notation. The corresponding
    // specifier code is 'u'.
    PRINT_FIELD_SPECIFIER_INTEGER_DECIMAL_UNSIGNED = 1,

    // Print an integral number in binary notation. The corresponding specifier
    // code is 'b'.
    PRINT_FIELD_SPECIFIER_INTEGER_BINARY = 2,

    // Print an integral number in octal notation. The corresponding specifier
    // code is 'o'.
    PRINT_FIELD_SPECIFIER_INTEGER_OCTAL = 3,

    // Print an integral number in hexadecimal notation using lowercase
    // characters. The corresponding specifier code is 'x'.
    PRINT_FIELD_SPECIFIER_INTEGER_HEXADECIMAL_LOWERCASE = 4,

    // Print an integral number in hexadecimal notation using uppercase
    // characters. The corresponding specifier code is 'X'.
    PRINT_FIELD_SPECIFIER_INTEGER_HEXADECIMAL_UPPERCASE = 5,

    // Print a floating point in decimal notation using lowercase characters.
    // The corresponding specifier code is 'f'.
    PRINT_FIELD_SPECIFIER_FLOATING_POINT_DECIMAL_LOWERCASE = 6,

    // Print a floating point number in decimal notation using uppercase
    // characters. The corresponding specifier code is 'F'.
    PRINT_FIELD_SPECIFIER_FLOATING_POINT_DECIMAL_UPPERCASE = 7,

    // Print a floating point number in scientific notation using lowercase
    // characters. The corresponding specifier code is 'e'.
    PRINT_FIELD_SPECIFIER_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE = 8,

    // Print a floating point number in scientific notation using uppercase
    // characters. The corresponding specifier code is 'E'.
    PRINT_FIELD_SPECIFIER_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE = 9,

    // Print a floating point number using the shortest representation of
    // either a decimal notation or a scientific notation using lowercase
    // characters. The corresponding specifier code is 'g', shortest of either
    // 'e' or 'f'.
    PRINT_FIELD_SPECIFIER_FLOATING_POINT_SHORTEST_REP_LOWERCASE = 10,

    // Print a floating point number using the shortest representation of
    // either a decimal notation or a scientific notation using uppercase
    // characters. The corresponding specifier code is 'G', shortest of either
    // 'E' or 'F'.
    PRINT_FIELD_SPECIFIER_FLOATING_POINT_SHORTEST_REP_UPPERCASE = 11,

    // Print a floating point number in hexadecimal notation using lowercase
    // characters. The corresponding specifier code is 'a'.
    PRINT_FIELD_SPECIFIER_FLOATING_POINT_HEXADECIMAL_LOWERCASE = 12,

    // Print a floating point number in hexadecimal notation using uppercase
    // characters. The corresponding specifier code is 'A'.
    PRINT_FIELD_SPECIFIER_FLOATING_POINT_HEXADECIMAL_UPPERCASE = 13,

    // Print a single character. The corresponding specifier code is 'c'.
    PRINT_FIELD_SPECIFIER_CHARACTER = 14,

    // Print a null-terminated sequence of characters. The corresponding
    // specifier code is 's'.
    PRINT_FIELD_SPECIFIER_STRING = 15,

    // Print a memory address. The corresponding specifier code is 'p'.
    PRINT_FIELD_SPECIFIER_POINTER = 16,

    // Do not print any but store the current number of bytes written to the
    // parameter. The corresponding specifier code is 'n'.
    PRINT_FIELD_SPECIFIER_OUTPUT = 17
};

/// Describes a print field.
struct PrintField {
    PrintField()
    : flags(0)
    , width()
    , precision()
    , length(PRINT_FIELD_LENGTH_DEFAULT)
    , specifier(PRINT_FIELD_SPECIFIER_INTEGER_DECIMAL_SIGNED)
    , base(0)
    , places(0)
    , uppercase(false)
    , type(PRINT_FIELD_TYPE_UNDEFINED)
    {
    }

    // The flags.
    bsl::uint32_t flags;

    // The minimum width of the formatted field, if any.
    bdlb::NullableValue<bsl::size_t> width;

    // The precision, if any.
    bdlb::NullableValue<bsl::size_t> precision;

    // The type length modifier.
    PrintFieldLength length;

    // The type specifier.
    PrintFieldSpecifier specifier;

    // The numeric base, either 10, 8, or 16, for decimal, octal, or
    // hexademical, respectively.
    bsl::uint32_t base;

    // The maximum number of digits to print a "full" representation of a
    // number in the base, excluding the base indicator (e.g. '0x'). For
    // example, an unsigned 16-bit integer in a hexadecimal base representation
    // requires 4 places, "FFFF". This value is zero for non-integral types.
    bsl::uint32_t places;

    // The flag indicating the alphabetical characters in the hexadecimal
    // or scientific notation should be printed in uppercase rather than
    // lowercase.
    bool uppercase;

    // The type, computed from the specifier and length.
    PrintFieldType type;
};

/// Describes the context in which formatted values were printed.
struct PrintContext {
    PrintContext()
    : size(0)
    {
    }

    // The number of bytes written.
    bsl::size_t size;
};

/// Describes the options to print formatted values.
struct PrintOptions {
    PrintOptions()
    : check_underflow(false)
    , check_overflow(false)
    , max_width((bsl::size_t)(-1))
    , max_length((bsl::size_t)(-1))
    , boolalpha(false)
    {
    }

    // Flag that indicates each value should be checked for underflow.
    bool check_underflow;

    // The flag that indicates each value should be checked for overflow.
    bool check_overflow;

    // The maximum width of any formatted field. The default is unbounded.
    bsl::size_t max_width;

    // The maximum number of characters allowed to be written to the output
    // device. The default is unbounded.
    bsl::size_t max_length;

    // Print alphabetical, i.e. stringified booleans.
    bool boolalpha;
};

// The print 'data' having the 'size'.
#define PRINT_OUTPUT_DEFAULT 0

// Instead of printing the 'data' having the 'size', backspace 'size' number
// of characters.
#define PRINT_OUTPUT_BACKSPACE (1 << 1)

// Defines a type alias for a function to print a sequence of characters to an
// output device.
typedef ntsa::Error (*PrintOutput)(void*         closure,
                                   PrintContext* context,
                                   const char*   data,
                                   bsl::size_t   size,
                                   bsl::size_t   flags);

/// Provide functions for printing formatted output.
struct PrintUtil {
    // CLASS FUNCTIONS

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printInt8(PrintOutput         output,
                                 void*               closure,
                                 PrintContext*       context,
                                 const PrintOptions* options,
                                 const PrintField*   field,
                                 bsl::int8_t         value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printUint8(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  bsl::uint8_t        value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printInt16(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  bsl::int16_t        value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printUint16(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   bsl::uint16_t       value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printInt32(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  bsl::int32_t        value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printUint32(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   bsl::uint32_t       value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printInt64(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  bsl::int64_t        value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printUint64(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   bsl::uint64_t       value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printAddress(PrintOutput         output,
                                    void*               closure,
                                    PrintContext*       context,
                                    const PrintOptions* options,
                                    const PrintField*   field,
                                    const void*         value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printFloat(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  float               value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printDouble(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   double              value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printLongDouble(PrintOutput         output,
                                       void*               closure,
                                       PrintContext*       context,
                                       const PrintOptions* options,
                                       const PrintField*   field,
                                       long double         value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printBool(PrintOutput         output,
                                 void*               closure,
                                 PrintContext*       context,
                                 const PrintOptions* options,
                                 const PrintField*   field,
                                 bool                value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printChar(PrintOutput         output,
                                 void*               closure,
                                 PrintContext*       context,
                                 const PrintOptions* options,
                                 const PrintField*   field,
                                 char                value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printWideChar(PrintOutput         output,
                                     void*               closure,
                                     PrintContext*       context,
                                     const PrintOptions* options,
                                     const PrintField*   field,
                                     wchar_t             value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printString(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   const char*         value);

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printWideString(PrintOutput         output,
                                       void*               closure,
                                       PrintContext*       context,
                                       const PrintOptions* options,
                                       const PrintField*   field,
                                       const wchar_t*      value);

// TODO: This function is currently unused.
#if 0
    static ntsa::Error printVariable(PrintOutput         output,
                                     void*               closure,
                                     PrintContext*       context,
                                     const PrintOptions* options,
                                     const char*         format,
                                     ...);
        // Write the specified 'value' to the specified 'output' device
        // according to the specified 'format'. Return the error.
#endif

    /// Write the specified 'value' to the specified 'output' device
    /// according to the specified 'format'. Return the error.
    static ntsa::Error printVariableArgs(PrintOutput         output,
                                         void*               closure,
                                         PrintContext*       context,
                                         const PrintOptions* options,
                                         const char*         format,
                                         bsl::va_list        args);

// TODO: This function is currently unused.
#if 0
    // Write a variable number of args in the specified 'format' to
    // the specified 'output' device. Return the number of bytes written.
    static size_t print(PrintOutput output,
                void*      closure,
                const char*    format,
                ...);
#endif

    /// Write the specified variable number of 'args' in the specified
    /// 'format' to the specified 'output' device. Return the number of
    /// bytes written.
    static size_t printArgs(PrintOutput  output,
                            void*        closure,
                            const char*  format,
                            bsl::va_list args);
};

// Defines a type alias for the type that small integer types are promoted
// to when calling variable-argument functions.
//
// For example, calling 'var_arg' with the wrong type results in the warning:
//
// warning: 'int8_t' {aka 'signed char'} is promoted to 'int' when
// passed through '...' (int8_t)(va_arg(args, int8_t)));
// so you should pass 'int' not 'int8_t' {aka 'signed char'} to 'va_arg'
typedef int print_varags_t;

// The maximum number of places required to represent an unsigned 8-bit integer
// in a binary represenation.
#define PRINT_FIELD_TYPE_UINT8_MAX_DIGITS_IN_BASE_2 8

// The maximum number of places required to represent an unsigned 8-bit integer
// in an octal represenation.
#define PRINT_FIELD_TYPE_UINT8_MAX_DIGITS_IN_BASE_8 3

// The maximum number of places required to represent an unsigned 8-bit integer
// in a decimal represenation.
#define PRINT_FIELD_TYPE_UINT8_MAX_DIGITS_IN_BASE_10 3

// The maximum number of places required to represent an unsigned 8-bit integer
// in a hexadecimal represenation.
#define PRINT_FIELD_TYPE_UINT8_MAX_DIGITS_IN_BASE_16 2

// The maximum number of places required to represent an unsigned 16-bit
// integer in a binary represenation.
#define PRINT_FIELD_TYPE_UINT16_MAX_DIGITS_IN_BASE_2 16

// The maximum number of places required to represent an unsigned 16-bit
// integer in an octal represenation.
#define PRINT_FIELD_TYPE_UINT16_MAX_DIGITS_IN_BASE_8 6

// The maximum number of places required to represent an unsigned 16-bit
// integer in decimal represenation.
#define PRINT_FIELD_TYPE_UINT16_MAX_DIGITS_IN_BASE_10 5

// The maximum number of places required to represent an unsigned 16-bit
// integer in a hexadecimal represenation.
#define PRINT_FIELD_TYPE_UINT16_MAX_DIGITS_IN_BASE_16 4

// The maximum number of places required to represent an unsigned 32-bit
// integer in a binary represenation.
#define PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_2 32

// The maximum number of places required to represent an unsigned 32-bit
// integer in an octal represenation.
#define PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_8 11

// The maximum number of places required to represent an unsigned 32-bit
// integer in a decimal represenation.
#define PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_10 10

// The maximum number of places required to represent an unsigned 32-bit
// integer in a hexadecimal represenation.
#define PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_16 8

// The maximum number of places required to represent an unsigned 64-bit
// integer in a binary represenation.
#define PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_2 64

// The maximum number of places required to represent an unsigned 64-bit
// integer in an octal represenation.
#define PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_8 22

// The maximum number of places required to represent an unsigned 64-bit
// integer in a decimal represenation.
#define PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_10 20

// The maximum number of places required to represent an unsigned 64-bit
// integer in a hexadecimal represenation.
#define PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_16 16

// The default precision for a type 'char'.
#define PRINT_FIELD_DEFAULT_PRECISION_CHAR 1

// The default precision for a type 'unsigned char'.
#define PRINT_FIELD_DEFAULT_PRECISION_UNSIGNED_CHAR 1

const char* PRINT_UNITS[9] = {"", "K", "M", "G", "T", "P", "E", "Z", "Y"};

/// Provide low-level printing functions for the implementation
/// of higher-level print utilities. This struct is thread safe.
struct PrintImpl {
    /// Print padding consisting of spaces to format a field having the
    /// specified 'length' into the specified 'width'. Return the error.
    static ntsa::Error printPaddingSpaces(PrintOutput         output,
                                          void*               closure,
                                          PrintContext*       context,
                                          const PrintOptions* options,
                                          bsl::size_t         length,
                                          bsl::size_t         width);

    /// Print padding consisting of zeroes to format a field having the
    /// specified 'length' into the specified 'width'. Return the error.
    static ntsa::Error printPaddingZeroes(PrintOutput         output,
                                          void*               closure,
                                          PrintContext*       context,
                                          const PrintOptions* options,
                                          bsl::size_t         length,
                                          bsl::size_t         width);

    /// Print the specified 'field' containing the string representation of
    /// the specified 32-bit unsigned integer 'value' to the specified
    /// 'output' given the specified 'closure' according to the specified
    /// 'options'. Load into the specified 'context' the results of the
    /// operation. If the specified 'negative' flag is true, treat 'value'
    /// as the negation of the desired printed value. Return the error.
    static ntsa::Error printInteger32(PrintOutput         output,
                                      void*               closure,
                                      PrintContext*       context,
                                      const PrintOptions* options,
                                      const PrintField*   field,
                                      bsl::uint32_t       value,
                                      bool                negative);

    /// Print the specified 'field' containing the string representation of
    /// the specified 64-bit unsigned integer 'value' to the specified
    /// 'output' given the specified 'closure' according to the specified
    /// 'options'. Load into the specified 'context' the results of the
    /// operation. If the specified 'negative' flag is true, treat 'value'
    /// as the negation of the desired printed value. Return the error.
    static ntsa::Error printInteger64(PrintOutput         output,
                                      void*               closure,
                                      PrintContext*       context,
                                      const PrintOptions* options,
                                      const PrintField*   field,
                                      bsl::uint64_t       value,
                                      bool                negative);
};

// Print padding consisting of spaces to format a field having the specified
// 'length' into the specified 'width'.
ntsa::Error PrintImpl::printPaddingSpaces(PrintOutput         output,
                                          void*               closure,
                                          PrintContext*       context,
                                          const PrintOptions* options,
                                          bsl::size_t         length,
                                          bsl::size_t         width)
{
    NTCCFG_WARNING_UNUSED(options);

    if (width > length) {
        char padding[2] = {' ', 0};

        bsl::size_t count = width - length;

        if (output) {
            for (bsl::size_t i = 0; i < count; ++i) {
                ntsa::Error error =
                    output(closure, context, padding, 1, PRINT_OUTPUT_DEFAULT);
                if (error) {
                    return error;
                }
            }
        }

        return ntsa::Error();
    }
    else {
        return ntsa::Error();
    }
}

// Print padding consisting of zeroes to format a field having the specified
// 'length' into the specified 'width'.
ntsa::Error PrintImpl::printPaddingZeroes(PrintOutput         output,
                                          void*               closure,
                                          PrintContext*       context,
                                          const PrintOptions* options,
                                          bsl::size_t         length,
                                          bsl::size_t         width)
{
    NTCCFG_WARNING_UNUSED(options);

    if (width > length) {
        char padding[2] = {'0', 0};

        bsl::size_t count = width - length;

        if (output) {
            for (bsl::size_t i = 0; i < count; ++i) {
                ntsa::Error error =
                    output(closure, context, padding, 1, PRINT_OUTPUT_DEFAULT);
                if (error) {
                    return error;
                }
            }
        }

        return ntsa::Error();
    }
    else {
        return ntsa::Error();
    }
}

// Print the specified 'field' containing the string representation of the
// specified 32-bit unsigned integer 'value' to the specified 'output' given
// the specified 'closure' according to the specified 'options'. Load into
// the specified 'context' the results of the operation. If the specified
// 'negative' flag is true, treat 'value' as the negation of the desired
// printed value. Return the error.
ntsa::Error PrintImpl::printInteger32(PrintOutput         output,
                                      void*               closure,
                                      PrintContext*       context,
                                      const PrintOptions* options,
                                      const PrintField*   field,
                                      bsl::uint32_t       value,
                                      bool                negative)
{
    ntsa::Error error;

    // Get the field with, if any.

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    // Get the base.

    bsl::uint32_t base = field->base;
    if (base != 2 && base != 8 && base == 10 && base == 16) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Approximate the value, if requested.

    bsl::size_t unit = 0;

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_APPROXIMATION)) {
        while (value >= 1024 &&
               unit < sizeof(PRINT_UNITS) / sizeof(PRINT_UNITS[0]))
        {
            value /= 1024;
            ++unit;
        }
    }

    // Allocate a buffer sufficient to store the string representation of
    // 'value' in any base, including the longest "hash" (or base indicator,
    // e.g. '0x') plus the null terminator. The longest representation is in
    // base-2, and longest hash is a tie between base-2 ('0b') and base-16
    // ('0x').

    char buffer[PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_2 + 2 + 1];

    char* current = buffer;
    char* end     = buffer + (sizeof(buffer) - 1);

    // Print the reverse of the unit, if the value is approximated and its
    // original value is greater than 1024.

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_APPROXIMATION) &&
        unit > 0)
    {
        if (PRINT_UNITS[unit][1] != 0) {
            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = PRINT_UNITS[unit][1];
        }

        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *current++ = PRINT_UNITS[unit][0];
    }

    // Stringify the number, reversed such that the least significant digit
    // is in the lowest memory address. The buffer will be reversed when
    // specified to the output function.

    while (true) {
        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
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

    // Pad with zeroes up to the maximum number of digits that is canonical
    // for the type represented in the base, if specified in the format.
    // Note that this format flag is a custom extension to the standard
    // 'printf' format.

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_CANONICAL)) {
        bsl::size_t length = current - buffer;
        if (field->places > length) {
            bsl::size_t count = field->places - length;
            for (bsl::size_t i = 0; i < count; ++i) {
                if (current >= end) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                *current++ = '0';
            }
        }
    }

    // Print the reverse of the base indicator, if specified in the format.

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_BASE)) {
        if (field->base == 2) {
            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = 'b';

            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = '0';
        }
        else if (field->base == 8) {
            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = '0';
        }
        else if (field->base == 16) {
            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = 'x';

            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = '0';
        }
    }

    // Handle 'negative', or the space flag or the sign flag.

    if (negative) {
        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *current++ = '-';
    }
    else if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_SIGN)) {
        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *current++ = '+';
    }
    else if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_SPACE)) {
        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *current++ = ' ';
    }

    // Null terminate the buffer for cosmetic reasons to aid debugging.

    *current = 0;

    // Print padding to justify right within the field width, if required.

    size_t length = current - buffer;

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, PRINT_FIELD_FLAG_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_ZERO)) {
            error = printPaddingZeroes(output,
                                       closure,
                                       context,
                                       options,
                                       length,
                                       width);
            if (error) {
                return error;
            }
        }
        else {
            error = printPaddingSpaces(output,
                                       closure,
                                       context,
                                       options,
                                       length,
                                       width);
            if (error) {
                return error;
            }
        }
    }

    // Print the reverse of the buffer.

    if (length > 0) {
        if (output) {
            bsl::size_t index = length - 1;
            while (true) {
                char ch = buffer[index];

                error = output(closure, context, &ch, 1, PRINT_OUTPUT_DEFAULT);
                if (error) {
                    return error;
                }

                if (index == 0) {
                    break;
                }

                --index;
            }
        }
    }

    // Print padding to justify left within the field width, if required.

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_ZERO)) {
            error = printPaddingZeroes(output,
                                       closure,
                                       context,
                                       options,
                                       length,
                                       width);
            if (error) {
                return error;
            }
        }
        else {
            error = printPaddingSpaces(output,
                                       closure,
                                       context,
                                       options,
                                       length,
                                       width);
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

// Print the specified 'field' containing the string representation of the
// specified 64-bit unsigned integer 'value' to the specified 'output' given
// the specified 'closure' according to the specified 'options'. Load into
// the specified 'context' the results of the operation. If the specified
// 'negative' flag is true, treat 'value' as the negation of the desired
// printed value. Return the error.
ntsa::Error PrintImpl::printInteger64(PrintOutput         output,
                                      void*               closure,
                                      PrintContext*       context,
                                      const PrintOptions* options,
                                      const PrintField*   field,
                                      bsl::uint64_t       value,
                                      bool                negative)
{
    ntsa::Error error;

    // Get the field width, if any.

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    // Get the base.

    bsl::uint64_t base = field->base;
    if (base != 2 && base != 8 && base == 10 && base == 16) {
        return ntsa::Error(ntsa::Error::e_INVALID);
    }

    // Approximate the value, if requested.

    bsl::size_t unit = 0;

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_APPROXIMATION)) {
        while (value >= 1024 &&
               unit < sizeof(PRINT_UNITS) / sizeof(PRINT_UNITS[0]))
        {
            value /= 1024;
            ++unit;
        }
    }

    // Allocate a buffer sufficient to store the string representation of
    // 'value' in any base, including the longest "hash" (or base indicator,
    // e.g. '0x') plus the null terminator. The longest representation is in
    // base-2, and longest hash is a tie between base-2 ('0b') and base-16
    // ('0x').

    char buffer[PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_2 + 2 + 1];

    char* current = buffer;
    char* end     = buffer + (sizeof(buffer) - 1);

    // Print the reverse of the unit, if the value is approximated and its
    // original value is greater than 1024.

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_APPROXIMATION) &&
        unit > 0)
    {
        if (PRINT_UNITS[unit][1] != 0) {
            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = PRINT_UNITS[unit][1];
        }

        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *current++ = PRINT_UNITS[unit][0];
    }

    // Stringify the number, reversed such that the least significant digit
    // is in the lowest memory address. The buffer will be reversed when
    // specified to the output function.

    while (true) {
        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
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

    // Pad with zeroes up to the maximum number of digits that is canonical
    // for the type represented in the base, if specified in the format.
    // Note that this format flag is a custom extension to the standard
    // 'printf' format.

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_CANONICAL)) {
        bsl::size_t length = current - buffer;
        if (field->places > length) {
            bsl::size_t count = field->places - length;
            for (bsl::size_t i = 0; i < count; ++i) {
                if (current >= end) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                *current++ = '0';
            }
        }
    }

    // Print the reverse of the base indicator, if specified int the format.

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_BASE)) {
        if (field->base == 2) {
            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = 'b';

            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = '0';
        }
        else if (field->base == 8) {
            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = '0';
        }
        else if (field->base == 16) {
            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = 'x';

            if (current >= end) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            *current++ = '0';
        }
    }

    // Handle 'negative', or the space flag or the sign flag.

    if (negative) {
        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *current++ = '-';
    }
    else if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_SIGN)) {
        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *current++ = '+';
    }
    else if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_SPACE)) {
        if (current >= end) {
            return ntsa::Error(ntsa::Error::e_INVALID);
        }

        *current++ = ' ';
    }

    // Null terminate the buffer for cosmetic reasons to aid debugging.

    *current = 0;

    // Print padding to justify right within the field width, if required.

    bsl::size_t length = current - buffer;

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, PRINT_FIELD_FLAG_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_ZERO)) {
            error = printPaddingZeroes(output,
                                       closure,
                                       context,
                                       options,
                                       length,
                                       width);
            if (error) {
                return error;
            }
        }
        else {
            error = printPaddingSpaces(output,
                                       closure,
                                       context,
                                       options,
                                       length,
                                       width);
            if (error) {
                return error;
            }
        }
    }

    // Print the reverse of the buffer.

    if (length > 0) {
        if (output) {
            size_t index = length - 1;
            while (true) {
                char ch = buffer[index];

                error = output(closure, context, &ch, 1, PRINT_OUTPUT_DEFAULT);
                if (error) {
                    return error;
                }

                if (index == 0) {
                    break;
                }

                --index;
            }
        }
    }

    // Print padding to justify left within the field width, if required.

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_JUSTIFY_LEFT)) {
        if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_ZERO)) {
            error = printPaddingZeroes(output,
                                       closure,
                                       context,
                                       options,
                                       length,
                                       width);
            if (error) {
                return error;
            }
        }
        else {
            error = printPaddingSpaces(output,
                                       closure,
                                       context,
                                       options,
                                       length,
                                       width);
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printInt8(PrintOutput         output,
                                 void*               closure,
                                 PrintContext*       context,
                                 const PrintOptions* options,
                                 const PrintField*   field,
                                 bsl::int8_t         value)
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

    return PrintImpl::printInteger32(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     quantity,
                                     negative);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printUint8(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  bsl::uint8_t        value)
{
    return PrintImpl::printInteger32(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     value,
                                     false);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printInt16(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  bsl::int16_t        value)
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

    return PrintImpl::printInteger32(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     quantity,
                                     negative);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printUint16(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   bsl::uint16_t       value)
{
    return PrintImpl::printInteger32(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     value,
                                     false);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printInt32(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  bsl::int32_t        value)
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

    return PrintImpl::printInteger32(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     quantity,
                                     negative);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printUint32(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   bsl::uint32_t       value)
{
    return PrintImpl::printInteger32(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     value,
                                     false);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printInt64(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  bsl::int64_t        value)
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

    return PrintImpl::printInteger64(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     quantity,
                                     negative);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printUint64(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   bsl::uint64_t       value)
{
    return PrintImpl::printInteger64(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     value,
                                     false);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printAddress(PrintOutput         output,
                                    void*               closure,
                                    PrintContext*       context,
                                    const PrintOptions* options,
                                    const PrintField*   field,
                                    const void*         value)
{
#if defined(BSLS_PLATFORM_CPU_32_BIT)
    uint32_t quantity = (uint32_t)(value);
    return PrintImpl::printInteger32(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     quantity,
                                     false);
#else
    uint64_t quantity = (uint64_t)(value);
    return PrintImpl::printInteger64(output,
                                     closure,
                                     context,
                                     options,
                                     field,
                                     quantity,
                                     false);
#endif
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printFloat(PrintOutput         output,
                                  void*               closure,
                                  PrintContext*       context,
                                  const PrintOptions* options,
                                  const PrintField*   field,
                                  float               value)
{
    NTCCFG_WARNING_UNUSED(output);
    NTCCFG_WARNING_UNUSED(closure);
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(field);
    NTCCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printDouble(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   double              value)
{
    NTCCFG_WARNING_UNUSED(output);
    NTCCFG_WARNING_UNUSED(closure);
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(field);
    NTCCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printLongDouble(PrintOutput         output,
                                       void*               closure,
                                       PrintContext*       context,
                                       const PrintOptions* options,
                                       const PrintField*   field,
                                       long double         value)
{
    NTCCFG_WARNING_UNUSED(output);
    NTCCFG_WARNING_UNUSED(closure);
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(field);
    NTCCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printBool(PrintOutput         output,
                                 void*               closure,
                                 PrintContext*       context,
                                 const PrintOptions* options,
                                 const PrintField*   field,
                                 bool                value)
{
    if (value) {
        if (options->boolalpha) {
            return PrintUtil::printString(output,
                                          closure,
                                          context,
                                          options,
                                          field,
                                          "true");
        }
        else {
            return PrintImpl::printInteger32(output,
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
            return PrintUtil::printString(output,
                                          closure,
                                          context,
                                          options,
                                          field,
                                          "false");
        }
        else {
            return PrintImpl::printInteger32(output,
                                             closure,
                                             context,
                                             options,
                                             field,
                                             0,
                                             false);
        }
    }
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printChar(PrintOutput         output,
                                 void*               closure,
                                 PrintContext*       context,
                                 const PrintOptions* options,
                                 const PrintField*   field,
                                 char                value)
{
    ntsa::Error error;

    bsl::size_t width = 0;
    if (!field->width.isNull()) {
        width = field->width.value();
    }

    bsl::size_t precision = PRINT_FIELD_DEFAULT_PRECISION_CHAR;
    if (!field->precision.isNull()) {
        precision = field->precision.value();
    }

    bsl::size_t length = 1;
    if (precision < length) {
        length = precision;
    }

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, PRINT_FIELD_FLAG_JUSTIFY_LEFT)) {
        error = PrintImpl::printPaddingSpaces(output,
                                              closure,
                                              context,
                                              options,
                                              length,
                                              width);
        if (error) {
            return error;
        }
    }

    if (output) {
        error =
            output(closure, context, &value, precision, PRINT_OUTPUT_DEFAULT);
        if (error) {
            return error;
        }
    }

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_JUSTIFY_LEFT)) {
        error = PrintImpl::printPaddingSpaces(output,
                                              closure,
                                              context,
                                              options,
                                              length,
                                              width);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printWideChar(PrintOutput         output,
                                     void*               closure,
                                     PrintContext*       context,
                                     const PrintOptions* options,
                                     const PrintField*   field,
                                     wchar_t             value)
{
    NTCCFG_WARNING_UNUSED(output);
    NTCCFG_WARNING_UNUSED(closure);
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(field);
    NTCCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printString(PrintOutput         output,
                                   void*               closure,
                                   PrintContext*       context,
                                   const PrintOptions* options,
                                   const PrintField*   field,
                                   const char*         value)
{
    ntsa::Error error;

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

    if (NTCCFG_BIT_IS_NOT_SET(field->flags, PRINT_FIELD_FLAG_JUSTIFY_LEFT)) {
        error = PrintImpl::printPaddingSpaces(output,
                                              closure,
                                              context,
                                              options,
                                              length,
                                              width);
        if (error) {
            return error;
        }
    }

    if (output) {
        error = output(closure, context, value, length, PRINT_OUTPUT_DEFAULT);
        if (error) {
            return error;
        }
    }

    if (NTCCFG_BIT_IS_SET(field->flags, PRINT_FIELD_FLAG_JUSTIFY_LEFT)) {
        error = PrintImpl::printPaddingSpaces(output,
                                              closure,
                                              context,
                                              options,
                                              length,
                                              width);
        if (error) {
            return error;
        }
    }

    return ntsa::Error();
}

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printWideString(PrintOutput         output,
                                       void*               closure,
                                       PrintContext*       context,
                                       const PrintOptions* options,
                                       const PrintField*   field,
                                       const wchar_t*      value)
{
    NTCCFG_WARNING_UNUSED(output);
    NTCCFG_WARNING_UNUSED(closure);
    NTCCFG_WARNING_UNUSED(context);
    NTCCFG_WARNING_UNUSED(options);
    NTCCFG_WARNING_UNUSED(field);
    NTCCFG_WARNING_UNUSED(value);

    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
}

// TODO: This function is currently unused.
#if 0
// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printVariable(PrintOutput         output,
                                     void*               closure,
                                     PrintContext*       context,
                                     const PrintOptions* options,
                                     const char*         format,
                                     ...)
{
    ntsa::Error error;

    bsl::va_list args;
    va_start(args, format);

    error =
        PrintUtil::printVariableArgs(
            output, closure, context, options, format, args);

    va_end(args);

    return error;
}
#endif

// Write the specified 'value' to the specified 'output' device according to
// the specified 'format'. Return the error.
ntsa::Error PrintUtil::printVariableArgs(PrintOutput         output,
                                         void*               closure,
                                         PrintContext*       context,
                                         const PrintOptions* options,
                                         const char*         format,
                                         bsl::va_list        args)
{
    ntsa::Error error;

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
                    error = output(closure,
                                   context,
                                   data,
                                   size,
                                   PRINT_OUTPUT_DEFAULT);
                    if (error) {
                        return error;
                    }
                }
            }

            ++current;

            ch = *current;
            if (ch == 0) {
                return ntsa::Error(ntsa::Error::e_INVALID);
            }

            if (ch == '%') {
                if (output) {
                    error =
                        output(closure, context, &ch, 1, PRINT_OUTPUT_DEFAULT);
                    if (error) {
                        return error;
                    }
                }

                ++current;
            }
            else {
                PrintField field;

                // [flags][width][.precision][length]specifier

                ntsa::Error error;

                char ch;

                // Parse flags.

                ch = *current;
                if (ch == 0) {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                while (true) {
                    if (ch == '-') {
                        field.flags |= PRINT_FIELD_FLAG_JUSTIFY_LEFT;
                    }
                    else if (ch == '+') {
                        field.flags |= PRINT_FIELD_FLAG_SIGN;
                    }
                    else if (ch == ' ') {
                        field.flags |= PRINT_FIELD_FLAG_SPACE;
                    }
                    else if (ch == '#') {
                        field.flags |= (PRINT_FIELD_FLAG_BASE |
                                        PRINT_FIELD_FLAG_DECIMAL_POINT);
                    }
                    else if (ch == '0') {
                        field.flags |= PRINT_FIELD_FLAG_ZERO;
                    }
                    else if (ch == '\'') {
                        field.flags |= PRINT_FIELD_FLAG_APOSTROPHE;
                    }
                    else if (ch == '@') {
                        field.flags |= PRINT_FIELD_FLAG_CANONICAL;
                    }
                    else if (ch == '~') {
                        field.flags |= PRINT_FIELD_FLAG_APPROXIMATION;
                    }
                    else {
                        break;
                    }

                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                // Parse the width.

                if (ch == '*') {
                    int signed_width = (int)(va_arg(args, int));
                    if (signed_width < 0) {
                        field.width.makeValue((size_t)(-signed_width));
                        NTCCFG_BIT_SET(&field.flags,
                                       PRINT_FIELD_FLAG_JUSTIFY_LEFT);
                    }
                    else {
                        field.width.makeValue((size_t)(signed_width));
                        NTCCFG_BIT_CLEAR(&field.flags,
                                         PRINT_FIELD_FLAG_JUSTIFY_LEFT);
                    }

                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (ch >= '1' && ch <= '9') {
                    bsl::uint32_t width = 0;

                    do {
                        width = width * 10 + (uint32_t)(ch - '0');

                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }
                    } while (ch >= '0' && ch <= '9');

                    field.width.makeValue(width);
                }

                // Parse the precision.

                if (ch == '.') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    if (ch == '*') {
                        int signed_precision = (int)(va_arg(args, int));
                        if (signed_precision < 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }
                        else {
                            field.precision.makeValue(
                                (size_t)(signed_precision));
                        }

                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
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
                                return ntsa::Error(ntsa::Error::e_INVALID);
                            }
                        } while (ch >= '0' && ch <= '9');

                        field.precision.makeValue(precision);
                    }
                    else {
                        field.precision.makeValue(0);
                    }
                }

                // Parse the length of the data type.

                if (ch == 'h') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    if (ch == 'h') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }

                        field.length = PRINT_FIELD_LENGTH_CHAR;
                    }
                    else {
                        field.length = PRINT_FIELD_LENGTH_SHORT;
                    }
                }
                else if (ch == 'l') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    if (ch == 'l') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }

                        field.length = PRINT_FIELD_LENGTH_LONG_LONG;
                    }
                    else {
                        field.length = PRINT_FIELD_LENGTH_LONG;
                    }
                }
                else if (ch == 'j') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    field.length = PRINT_FIELD_LENGTH_INTMAX_T;
                }
                else if (ch == 'z') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    field.length = PRINT_FIELD_LENGTH_SIZE_T;
                }
                else if (ch == 't') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    field.length = PRINT_FIELD_LENGTH_PTRDIFF_T;
                }
                else if (ch == 'L') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    field.length = PRINT_FIELD_LENGTH_LONG_DOUBLE;
                }
                else if (ch == 'I') {
                    ++current;

                    ch = *current;
                    if (ch == 0) {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }

                    if (ch == '8') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }

                        field.length = PRINT_FIELD_LENGTH_CHAR;
                    }
                    else if (ch == '1') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }

                        if (ch == '6') {
                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return ntsa::Error(ntsa::Error::e_INVALID);
                            }

                            field.length = PRINT_FIELD_LENGTH_SHORT;
                        }
                        else {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }
                    }
                    else if (ch == '3') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }

                        if (ch == '2') {
                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return ntsa::Error(ntsa::Error::e_INVALID);
                            }

                            field.length = PRINT_FIELD_LENGTH_DEFAULT;
                        }
                        else {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }
                    }
                    else if (ch == '6') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }

                        if (ch == '4') {
                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return ntsa::Error(ntsa::Error::e_INVALID);
                            }

                            field.length = PRINT_FIELD_LENGTH_LONG_LONG;
                        }
                        else {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }
                    }
                    else if (ch == 'R') {
                        ++current;

                        ch = *current;
                        if (ch == 0) {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }

                        if (ch == 'G') {
                            ++current;

                            ch = *current;
                            if (ch == 0) {
                                return ntsa::Error(ntsa::Error::e_INVALID);
                            }

                            field.length = PRINT_FIELD_LENGTH_SIZE_T;
                        }
                        else {
                            return ntsa::Error(ntsa::Error::e_INVALID);
                        }
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                // Parse specifier.

                if (ch == 'd' || ch == 'i') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_INTEGER_DECIMAL_SIGNED;
                    NTCCFG_BIT_CLEAR(&field.flags, PRINT_FIELD_FLAG_BASE);
                    field.base = 10;
                }
                else if (ch == 'u') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_INTEGER_DECIMAL_UNSIGNED;
                    NTCCFG_BIT_CLEAR(&field.flags, PRINT_FIELD_FLAG_BASE);
                    field.base = 10;
                }
                else if (ch == 'b') {
                    field.specifier = PRINT_FIELD_SPECIFIER_INTEGER_BINARY;
                    field.base      = 2;
                }
                else if (ch == 'o') {
                    field.specifier = PRINT_FIELD_SPECIFIER_INTEGER_OCTAL;
                    field.base      = 8;
                }
                else if (ch == 'x') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_INTEGER_HEXADECIMAL_LOWERCASE;
                    field.base = 16;
                }
                else if (ch == 'X') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_INTEGER_HEXADECIMAL_UPPERCASE;
                    field.base      = 16;
                    field.uppercase = true;
                }
                else if (ch == 'f') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_DECIMAL_LOWERCASE;
                }
                else if (ch == 'F') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_DECIMAL_UPPERCASE;
                    field.uppercase = true;
                }
                else if (ch == 'e') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE;
                }
                else if (ch == 'E') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE;
                    field.uppercase = true;
                }
                else if (ch == 'g') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_SHORTEST_REP_LOWERCASE;
                }
                else if (ch == 'G') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_SHORTEST_REP_UPPERCASE;
                    field.uppercase = true;
                }
                else if (ch == 'a') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_HEXADECIMAL_LOWERCASE;
                    field.base = 16;
                }
                else if (ch == 'A') {
                    field.specifier =
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_HEXADECIMAL_UPPERCASE;
                    field.base      = 16;
                    field.uppercase = false;
                }
                else if (ch == 'c') {
                    field.specifier = PRINT_FIELD_SPECIFIER_CHARACTER;
                }
                else if (ch == 's') {
                    field.specifier = PRINT_FIELD_SPECIFIER_STRING;
                }
                else if (ch == 'p') {
                    field.specifier = PRINT_FIELD_SPECIFIER_POINTER;
                    field.base      = 16;
                    // field.uppercase = true;
                    NTCCFG_BIT_SET(&field.flags, PRINT_FIELD_FLAG_BASE);
                    // NTCCFG_BIT_SET(&field.flags,
                    //                PRINT_FIELD_FLAG_CANONICAL);
                }
                else if (ch == 'n') {
                    field.specifier = PRINT_FIELD_SPECIFIER_OUTPUT;
                }
                else {
                    return ntsa::Error(ntsa::Error::e_INVALID);
                }

                ++current;

                // Compute the type from the specifier and the length.

                if (field.specifier ==
                    PRINT_FIELD_SPECIFIER_INTEGER_DECIMAL_SIGNED)
                {
                    if (field.length == PRINT_FIELD_LENGTH_DEFAULT) {
                        field.type = PRINT_FIELD_TYPE_INT32;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_CHAR) {
                        field.type = PRINT_FIELD_TYPE_INT8;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_SHORT) {
                        field.type = PRINT_FIELD_TYPE_INT16;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_LONG) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = PRINT_FIELD_TYPE_INT32;
#else
                        field.type = PRINT_FIELD_TYPE_INT64;
#endif
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_LONG_LONG) {
                        field.type = PRINT_FIELD_TYPE_INT64;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_INTMAX_T) {
                        field.type = PRINT_FIELD_TYPE_INT64;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_SIZE_T) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = PRINT_FIELD_TYPE_UINT32;
#else
                        field.type = PRINT_FIELD_TYPE_UINT64;
#endif
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_PTRDIFF_T) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = PRINT_FIELD_TYPE_INT32;
#else
                        field.type = PRINT_FIELD_TYPE_INT64;
#endif
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_INTEGER_DECIMAL_UNSIGNED ||
                    field.specifier == PRINT_FIELD_SPECIFIER_INTEGER_BINARY ||
                    field.specifier == PRINT_FIELD_SPECIFIER_INTEGER_OCTAL ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_INTEGER_HEXADECIMAL_LOWERCASE ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_INTEGER_HEXADECIMAL_UPPERCASE)
                {
                    if (field.length == PRINT_FIELD_LENGTH_DEFAULT) {
                        field.type = PRINT_FIELD_TYPE_UINT32;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_CHAR) {
                        field.type = PRINT_FIELD_TYPE_UINT8;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_SHORT) {
                        field.type = PRINT_FIELD_TYPE_UINT16;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_LONG) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = PRINT_FIELD_TYPE_UINT32;
#else
                        field.type = PRINT_FIELD_TYPE_UINT64;
#endif
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_LONG_LONG) {
                        field.type = PRINT_FIELD_TYPE_UINT64;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_INTMAX_T) {
                        field.type = PRINT_FIELD_TYPE_UINT64;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_SIZE_T) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = PRINT_FIELD_TYPE_UINT32;
#else
                        field.type = PRINT_FIELD_TYPE_UINT64;
#endif
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_PTRDIFF_T) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                        field.type = PRINT_FIELD_TYPE_INT32;
#else
                        field.type = PRINT_FIELD_TYPE_INT64;
#endif
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (field.specifier == PRINT_FIELD_SPECIFIER_CHARACTER) {
                    if (field.length == PRINT_FIELD_LENGTH_DEFAULT) {
                        field.type = PRINT_FIELD_TYPE_CHAR;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_LONG) {
                        field.type = PRINT_FIELD_TYPE_WIDE_CHAR;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (field.specifier == PRINT_FIELD_SPECIFIER_STRING) {
                    if (field.length == PRINT_FIELD_LENGTH_DEFAULT) {
                        field.type = PRINT_FIELD_TYPE_STRING;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_LONG) {
                        field.type = PRINT_FIELD_TYPE_WIDE_STRING;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (field.specifier == PRINT_FIELD_SPECIFIER_POINTER) {
                    field.type = PRINT_FIELD_TYPE_ADDRESS;
                }
                else if (
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_DECIMAL_LOWERCASE ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_DECIMAL_UPPERCASE ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_SCIENTIFIC_NOTATION_LOWERCASE ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_SCIENTIFIC_NOTATION_UPPERCASE ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_SHORTEST_REP_LOWERCASE ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_SHORTEST_REP_UPPERCASE ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_HEXADECIMAL_LOWERCASE ||
                    field.specifier ==
                        PRINT_FIELD_SPECIFIER_FLOATING_POINT_HEXADECIMAL_UPPERCASE)
                {
                    if (field.length == PRINT_FIELD_LENGTH_DEFAULT) {
                        field.type = PRINT_FIELD_TYPE_DOUBLE;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_LONG_DOUBLE) {
                        field.type = PRINT_FIELD_TYPE_LONG_DOUBLE;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (field.specifier == PRINT_FIELD_SPECIFIER_OUTPUT) {
                    return ntsa::Error(ntsa::Error::e_NOT_IMPLEMENTED);
                }
                else {
                    if (field.length == PRINT_FIELD_LENGTH_DEFAULT) {
                        field.type = PRINT_FIELD_TYPE_DOUBLE;
                    }
                    else if (field.length == PRINT_FIELD_LENGTH_LONG_DOUBLE) {
                        field.type = PRINT_FIELD_TYPE_LONG_DOUBLE;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }

                // Compute the places from the the type.

                if (field.type == PRINT_FIELD_TYPE_INT8 ||
                    field.type == PRINT_FIELD_TYPE_UINT8)
                {
                    if (field.base == 2) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT8_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT8_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT8_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT8_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (field.type == PRINT_FIELD_TYPE_INT16 ||
                         field.type == PRINT_FIELD_TYPE_UINT16)
                {
                    if (field.base == 2) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT16_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT16_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT16_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT16_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (field.type == PRINT_FIELD_TYPE_INT32 ||
                         field.type == PRINT_FIELD_TYPE_UINT32)
                {
                    if (field.base == 2) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (field.type == PRINT_FIELD_TYPE_INT64 ||
                         field.type == PRINT_FIELD_TYPE_UINT64)
                {
                    if (field.base == 2) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
                }
                else if (field.type == PRINT_FIELD_TYPE_ADDRESS) {
#if defined(BSLS_PLATFORM_CPU_32_BIT)
                    if (field.base == 2) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT32_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
#else
                    if (field.base == 2) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_2;
                    }
                    else if (field.base == 8) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_8;
                    }
                    else if (field.base == 10) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_10;
                    }
                    else if (field.base == 16) {
                        field.places =
                            PRINT_FIELD_TYPE_UINT64_MAX_DIGITS_IN_BASE_16;
                    }
                    else {
                        return ntsa::Error(ntsa::Error::e_INVALID);
                    }
#endif
                }

                // Print the type.

                switch (field.type) {
                case PRINT_FIELD_TYPE_UNDEFINED:
                    error = ntsa::Error(ntsa::Error::e_INVALID);
                    break;
                case PRINT_FIELD_TYPE_INT8:
                    error = PrintUtil::printInt8(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::int8_t)(va_arg(args, print_varags_t)));
                    break;
                case PRINT_FIELD_TYPE_UINT8:
                    error = PrintUtil::printUint8(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::uint8_t)(va_arg(args, print_varags_t)));
                    break;
                case PRINT_FIELD_TYPE_INT16:
                    error = PrintUtil::printInt16(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::int16_t)(va_arg(args, print_varags_t)));
                    break;
                case PRINT_FIELD_TYPE_UINT16:
                    error = PrintUtil::printUint16(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::uint16_t)(va_arg(args, print_varags_t)));
                    break;
                case PRINT_FIELD_TYPE_INT32:
                    error = PrintUtil::printInt32(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::int32_t)(va_arg(args, bsl::int32_t)));
                    break;
                case PRINT_FIELD_TYPE_UINT32:
                    error = PrintUtil::printUint32(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::uint32_t)(va_arg(args, bsl::uint32_t)));
                    break;
                case PRINT_FIELD_TYPE_INT64:
                    error = PrintUtil::printInt64(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::int64_t)(va_arg(args, bsl::int64_t)));
                    break;
                case PRINT_FIELD_TYPE_UINT64:
                    error = PrintUtil::printUint64(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (bsl::uint64_t)(va_arg(args, bsl::uint64_t)));
                    break;
                case PRINT_FIELD_TYPE_ADDRESS:
                    error = PrintUtil::printAddress(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (const void*)(va_arg(args, const void*)));
                    break;
                case PRINT_FIELD_TYPE_FLOAT:
                    error =
                        PrintUtil::printFloat(output,
                                              closure,
                                              context,
                                              options,
                                              &field,
                                              (float)(va_arg(args, double)));
                    break;
                case PRINT_FIELD_TYPE_DOUBLE:
                    error =
                        PrintUtil::printDouble(output,
                                               closure,
                                               context,
                                               options,
                                               &field,
                                               (double)(va_arg(args, double)));
                    break;
                case PRINT_FIELD_TYPE_LONG_DOUBLE:
                    error = PrintUtil::printLongDouble(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (long double)(va_arg(args, long double)));
                    break;
                case PRINT_FIELD_TYPE_BOOL:
                    error = PrintUtil::printBool(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (char)(va_arg(args, print_varags_t)));
                    break;
                case PRINT_FIELD_TYPE_CHAR:
                    error = PrintUtil::printChar(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (char)(va_arg(args, print_varags_t)));
                    break;
                case PRINT_FIELD_TYPE_WIDE_CHAR:
                    error = PrintUtil::printWideChar(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (wchar_t)(va_arg(args, print_varags_t)));
                    break;
                case PRINT_FIELD_TYPE_STRING:
                    error = PrintUtil::printString(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (const char*)(va_arg(args, const char*)));
                    break;
                case PRINT_FIELD_TYPE_WIDE_STRING:
                    error = PrintUtil::printWideString(
                        output,
                        closure,
                        context,
                        options,
                        &field,
                        (const wchar_t*)(va_arg(args, const wchar_t*)));
                    break;
                }

                if (error) {
                    return error;
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
            error = output(closure, context, data, size, PRINT_OUTPUT_DEFAULT);
            if (error) {
                return error;
            }
        }
    }

    return ntsa::Error();
}

// TODO: This function is currently unused.
#if 0
// Write a variable number of args in the specified 'format' to
// the specified 'output' device. Return the number of bytes written.
size_t PrintUtil::print(
    PrintOutput output, void* closure, const char* format, ...)
{
    size_t result;

    bsl::va_list args;
    va_start(args, format);

    result = print_args(output, closure, format, args);

    va_end(args);

    return result;
}
#endif

// Write the specified variable number of 'args' in the specified 'format' to
// the specified 'output' device. Return the number of bytes written.
size_t PrintUtil::printArgs(PrintOutput  output,
                            void*        closure,
                            const char*  format,
                            bsl::va_list args)
{
    PrintContext context;
    PrintOptions options;

    ntsa::Error error = PrintUtil::printVariableArgs(output,
                                                     closure,
                                                     &context,
                                                     &options,
                                                     format,
                                                     args);
    if (error) {
        // TODO: Set errno or something similar to help the standard
        // implementation.
        return context.size;
    }

    return context.size;
}

}  // close unnamed namespace

ntci::LogContext* LogContext::getThreadLocal()
{
    ntci::LogContext* current = reinterpret_cast<ntci::LogContext*>(
        bslmt::ThreadUtil::getSpecific(s_key));

    if (current == 0) {
        void* arena = bsl::malloc(sizeof(ntci::LogContext));
        current     = new (arena) ntci::LogContext();
        int rc      = bslmt::ThreadUtil::setSpecific(
            s_key,
            const_cast<const void*>(static_cast<void*>(current)));
        BSLS_ASSERT_OPT(rc == 0);
    }

    return current;
}

void Log::initialize(bool journal)
{
    bsls::SpinLockGuard guard(&s_journalLock);

    if (journal) {
        void* arena = bsl::malloc(sizeof(LogJournal));
        s_journal_p =
            new (arena) LogJournal(bslma::Default::globalAllocator());

        bsl::atexit(&Log::exit);
    }

    s_journalEnabled = journal;
}

void Log::write(const ntci::LogContext* logContext,
                bsls::LogSeverity::Enum severity,
                const char*             file,
                int                     line,
                const char*             format,
                ...)
{
    bsl::va_list args;
    va_start(args, format);

    Log::writeArgs(logContext, severity, file, line, format, args);

    va_end(args);
}

void Log::writeArgs(const ntci::LogContext* logContext,
                    bsls::LogSeverity::Enum severity,
                    const char*             file,
                    int                     line,
                    const char*             format,
                    bsl::va_list            args)
{
    if (s_journalEnabled) {
        bsls::SpinLockGuard guard(&s_journalLock);

        LogRecord* record = &s_journal_p->d_records[s_journal_p->d_position];

        record->d_severity = severity;
        record->d_file     = file;
        record->d_line     = line;

        bsl::size_t n = 0;
        if (logContext) {
            n = ntci::LogUtil::formatContext(record->d_buffer,
                                             sizeof record->d_buffer,
                                             *logContext);
        }

#if NTCI_LOG_UNSET_CONTEXT
        if (n == 0) {
            const char UNSET_BANNER[] = "****** NTC UNSET CONTEXT ******: ";
            bsl::memcpy(record->d_buffer,
                        UNSET_BANNER,
                        sizeof UNSET_BANNER - 1);
            n = sizeof UNSET_BANNER - 1;
        }
#endif

#if NTCI_LOG_VSPRINTF

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY)
        int rc = bsl::vsnprintf(record->d_buffer + n,
                                (sizeof record->d_buffer) - n,
                                format,
                                args);
        BSLS_ASSERT(rc >= 0);
        if (rc > static_cast<int>((sizeof record->d_buffer) - n - 1)) {
            record->d_buffer[(sizeof record->d_buffer) - 1] = 0;
        }
#else
        // TODO: Prevent buffer overflows.
        vsprintf(record->d_buffer + n, format, args);
#endif

#else

        LogUtil::formatBufferArgs(record->d_buffer + n,
                                  (sizeof record->d_buffer) - n,
                                  format,
                                  args);

#endif

        ++s_journal_p->d_position;

        if (s_journal_p->d_position == k_MAX_LOG_RECORDS) {
            flushLogJournalLocked();
            BSLS_ASSERT(s_journal_p->d_position == 0);
        }
    }
    else {
        char buffer[4096];

        bsl::size_t n = 0;
        if (logContext) {
            n = ntci::LogUtil::formatContext(buffer,
                                             sizeof buffer,
                                             *logContext);
        }

#if NTCI_LOG_UNSET_CONTEXT
        if (n == 0) {
            const char UNSET_BANNER[] = "****** NTC UNSET CONTEXT ******: ";
            bsl::memcpy(buffer, UNSET_BANNER, sizeof UNSET_BANNER - 1);
            n = sizeof UNSET_BANNER - 1;
        }
#endif

#if NTCI_LOG_VSPRINTF

#if defined(BSLS_LIBRARYFEATURES_HAS_C99_LIBRARY)

        int rc = bsl::vsnprintf(buffer + n, (sizeof buffer) - n, format, args);
        BSLS_ASSERT(rc >= 0);
        if (rc > static_cast<int>((sizeof buffer) - n - 1)) {
            buffer[(sizeof buffer) - 1] = 0;
        }
#else
        // TODO: Prevent buffer overflows.
        vsprintf(buffer + n, format, args);
#endif

#else

        LogUtil::formatBufferArgs(buffer + n,
                                  (sizeof buffer) - n,
                                  format,
                                  args);

#endif

        bsls::Log::logMessage(severity, file, line, buffer);
    }
}

void Log::writeRaw(const ntci::LogContext* logContext,
                   bsls::LogSeverity::Enum severity,
                   const char*             file,
                   int                     line,
                   const char*             data,
                   bsl::size_t             size)
{
    if (s_journalEnabled) {
        bsls::SpinLockGuard guard(&s_journalLock);

        LogRecord* record = &s_journal_p->d_records[s_journal_p->d_position];

        record->d_severity = severity;
        record->d_file     = file;
        record->d_line     = line;

        char buffer[1024];

        bsl::size_t n = 0;
        if (logContext) {
            n = ntci::LogUtil::formatContext(buffer,
                                             sizeof buffer,
                                             *logContext);
        }

#if NTCI_LOG_UNSET_CONTEXT
        if (n == 0) {
            const char UNSET_BANNER[] = "****** NTC UNSET CONTEXT ******: ";
            bsl::memcpy(buffer, UNSET_BANNER, sizeof UNSET_BANNER - 1);
            n = sizeof UNSET_BANNER - 1;
        }
#endif

        bdlsb::MemOutStreamBuf sb(bslma::Default::globalAllocator());

        sb.sputn(buffer, n);
        sb.sputn(data, size);
        sb.sputc('\0');
        sb.pubsync();

        if (sb.length() < sizeof record->d_buffer - 1) {
            bsl::memcpy(record->d_buffer, sb.data(), sb.length());
            record->d_buffer[sb.length()] = 0;
        }
        else {
            bsl::memcpy(record->d_buffer,
                        sb.data(),
                        sizeof record->d_buffer - 1);
            record->d_buffer[sizeof record->d_buffer - 1] = 0;
        }

        ++s_journal_p->d_position;

        if (s_journal_p->d_position == k_MAX_LOG_RECORDS) {
            flushLogJournalLocked();
            BSLS_ASSERT(s_journal_p->d_position == 0);
        }
    }
    else {
        char buffer[1024];

        bsl::size_t n = 0;
        if (logContext) {
            n = ntci::LogUtil::formatContext(buffer,
                                             sizeof buffer,
                                             *logContext);
        }

#if NTCI_LOG_UNSET_CONTEXT
        if (n == 0) {
            const char UNSET_BANNER[] = "****** NTC UNSET CONTEXT ******: ";
            bsl::memcpy(buffer, UNSET_BANNER, sizeof UNSET_BANNER - 1);
            n = sizeof UNSET_BANNER - 1;
        }
#endif

        bdlsb::MemOutStreamBuf sb(bslma::Default::globalAllocator());

        sb.sputn(buffer, n);
        sb.sputn(data, size);
        sb.sputc('\0');
        sb.pubsync();

        bsls::Log::logMessage(severity, file, line, sb.data());
    }
}

void Log::flush()
{
    if (s_journalEnabled) {
        bsls::SpinLockGuard guard(&s_journalLock);
        flushLogJournalLocked();
        BSLS_ASSERT(s_journal_p->d_position == 0);
    }
}

void Log::exit()
{
    if (s_journalEnabled) {
        bsls::SpinLockGuard guard(&s_journalLock);
        if (s_journal_p) {
            flushLogJournalLocked();
            BSLS_ASSERT(s_journal_p->d_position == 0);

            typedef LogJournal Type;
            s_journal_p->~Type();

            bsl::free(s_journal_p);
            s_journal_p = 0;
        }
    }
}

bsl::size_t LogUtil::formatContext(char*                   destination,
                                   bsl::size_t             destinationCapacity,
                                   const ntci::LogContext& logContext)
{
    bdlsb::FixedMemOutStreamBuf osb(destination, destinationCapacity);

    {
        bsl::ostream os(&osb);

        bool empty = true;

        if (logContext.d_owner) {
            if (!empty) {
                os << ' ';
            }
            os << "transport '" << logContext.d_owner << "'";
            empty = false;
        }

        if (!logContext.d_monitorIndex.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "monitor " << logContext.d_monitorIndex.value();
            empty = false;
        }

        if (!logContext.d_threadIndex.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "thread " << logContext.d_threadIndex.value();
            empty = false;
        }

        if (!logContext.d_sourceId.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "source " << logContext.d_sourceId.value();
            empty = false;
        }

        if (!logContext.d_channelId.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "channel " << logContext.d_channelId.value();
            empty = false;
        }

        if (!logContext.d_sessionHandle.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "session " << logContext.d_sessionHandle.value();
            empty = false;
        }

        if (!logContext.d_descriptorHandle.isNull()) {
            if (!empty) {
                os << ' ';
            }
            os << "descriptor " << logContext.d_descriptorHandle.value();
            empty = false;
        }

        if (!logContext.d_sourceEndpoint.isNull()) {
            if (!logContext.d_sourceEndpoint.value().isUndefined()) {
                if (!empty) {
                    os << ' ';
                }
                os << "at " << logContext.d_sourceEndpoint.value();
                empty = false;
            }
        }

        if (!logContext.d_remoteEndpoint.isNull()) {
            if (!logContext.d_remoteEndpoint.value().isUndefined()) {
                if (!empty) {
                    os << ' ';
                }
                os << "to " << logContext.d_remoteEndpoint.value();
                empty = false;
            }
        }

        if (!empty) {
            os << ": ";
        }
    }

    osb.pubsync();

    bsl::size_t n = static_cast<bsl::size_t>(osb.length());

    if (n == destinationCapacity) {
        --n;
    }

    destination[0] = static_cast<char>(toupper(destination[0]));
    destination[n] = 0;

    return n;
}

// Copy to the specified 'destination' having the specified 'capacity' the
// specified 'format' string replacing the escaped characters with the
// values in their respective variable number of arguments. Return the number
// of characters written.
bsl::size_t LogUtil::formatBuffer(char*       destination,
                                  bsl::size_t capacity,
                                  const char* format,
                                  ...)
{
    bsl::va_list args;
    va_start(args, format);

    bsl::size_t result =
        LogUtil::formatBufferArgs(destination, capacity, format, args);

    va_end(args);

    return result;
}

namespace {

struct FormatBuffer {
    char*       data;
    bsl::size_t size;
    bsl::size_t capacity;
};

struct FormatBufferUtil {
    static ntsa::Error append(FormatBuffer* buffer,
                              PrintContext* context,
                              const char*   data,
                              bsl::size_t   size,
                              bsl::size_t   flags);
};

ntsa::Error FormatBufferUtil::append(FormatBuffer* buffer,
                                     PrintContext* context,
                                     const char*   data,
                                     bsl::size_t   size,
                                     bsl::size_t   flags)
{
    NTCCFG_WARNING_UNUSED(flags);

    if (buffer->size + size >= buffer->capacity) {
        return ntsa::Error(ntsa::Error::e_LIMIT);
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

    return ntsa::Error();
}

}  // close unnamed namespace

// Copy to the specified 'destination' having the specified 'capacity' the
// specified 'format' string replacing the escaped characters with the
// values in their respective variable number of arguments. Return the
// number of characters written.
bsl::size_t LogUtil::formatBufferArgs(char*        destination,
                                      size_t       capacity,
                                      const char*  format,
                                      bsl::va_list args)
{
    FormatBuffer buffer;
    buffer.data     = destination;
    buffer.size     = 0;
    buffer.capacity = capacity;

    bsl::size_t size =
        PrintUtil::printArgs((PrintOutput)(&FormatBufferUtil::append),
                             &buffer,
                             format,
                             args);

    BSLS_ASSERT(size < capacity);
    destination[size] = 0;

    return size;
}

}  // close package namespace
}  // close enterprise namespace
