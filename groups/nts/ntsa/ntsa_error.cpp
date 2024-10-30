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

#include <ntsa_error.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_error_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>
#include <bsls_platform.h>
#include <bsl_cstring.h>
#include <bsl_sstream.h>

#if defined(BSLS_PLATFORM_OS_UNIX)
#include <errno.h>
#include <unistd.h>
#endif

#if defined(BSLS_PLATFORM_OS_WINDOWS)
#ifdef NTDDI_VERSION
#undef NTDDI_VERSION
#endif
#ifdef WINVER
#undef WINVER
#endif
#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif
#define NTDDI_VERSION 0x06000100
#define WINVER 0x0600
#define _WIN32_WINNT 0x0600
#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
// clang-format off
#include <windows.h>
#include <winerror.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
// clang-format on
#endif

namespace BloombergLP {
namespace ntsa {

/// Provide a private implementation.
class Error::Impl
{
  public:
    /// Return the string description of the specified error 'code'.
    static const char* describeErrorCode(ntsa::Error::Code code);

    /// Print the specified error 'code' to the specified 'stream'.
    static bsl::ostream& printErrorCode(bsl::ostream&     stream,
                                        ntsa::Error::Code code,
                                        int               level,
                                        int               spacesPerLevel);

    /// Print the specified error 'number' to the specified 'stream'.
    static bsl::ostream& printErrorString(bsl::ostream&       stream,
                                          ntsa::Error::Number number,
                                          int                 level,
                                          int                 spacesPerLevel);
};

const char* Error::Impl::describeErrorCode(ntsa::Error::Code code)
{
    switch (code) {
    case ntsa::Error::e_OK:
        return "OK";
    case ntsa::Error::e_UNKNOWN:
        return "UNKNOWN";
    case ntsa::Error::e_WOULD_BLOCK:
        return "WOULD_BLOCK";
    case ntsa::Error::e_INTERRUPTED:
        return "INTERRUPTED";
    case ntsa::Error::e_PENDING:
        return "PENDING";
    case ntsa::Error::e_CANCELLED:
        return "CANCELLED";
    case ntsa::Error::e_INVALID:
        return "INVALID";
    case ntsa::Error::e_EOF:
        return "EOF";
    case ntsa::Error::e_LIMIT:
        return "LIMIT";
    case ntsa::Error::e_ADDRESS_IN_USE:
        return "ADDRESS_IN_USE";
    case ntsa::Error::e_CONNECTION_TIMEOUT:
        return "CONNECTION_TIMEOUT";
    case ntsa::Error::e_CONNECTION_REFUSED:
        return "CONNECTION_REFUSED";
    case ntsa::Error::e_CONNECTION_RESET:
        return "CONNECTION_RESET";
    case ntsa::Error::e_CONNECTION_DEAD:
        return "CONNECTION_DEAD";
    case ntsa::Error::e_UNREACHABLE:
        return "UNREACHABLE";
    case ntsa::Error::e_NOT_AUTHORIZED:
        return "NOT_AUTHORIZED";
    case ntsa::Error::e_NOT_IMPLEMENTED:
        return "NOT_IMPLEMENTED";
    case ntsa::Error::e_NOT_OPEN:
        return "NOT_OPEN";
    case ntsa::Error::e_NOT_SOCKET:
        return "NOT_SOCKET";
    }

    return "???";
}

bsl::ostream& Error::Impl::printErrorCode(bsl::ostream&     stream,
                                          ntsa::Error::Code code,
                                          int               level,
                                          int               spacesPerLevel)
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    stream << describeErrorCode(code);
    return stream;
}

#if defined(BSLS_PLATFORM_OS_UNIX)

bsl::ostream& Error::Impl::printErrorString(bsl::ostream&       stream,
                                            ntsa::Error::Number number,
                                            int                 level,
                                            int                 spacesPerLevel)
{
    NTSCFG_WARNING_UNUSED(level);
    NTSCFG_WARNING_UNUSED(spacesPerLevel);

    switch (number) {
    case 0:
        return stream << "OK";
    case E2BIG:
        return stream << "Argument list too long (E2BIG)";
    case EACCES:
        return stream << "Permission denied (EACCES)";
    case EADDRINUSE:
        return stream << "Address in use (EADDRINUSE)";
    case EADDRNOTAVAIL:
        return stream << "Address not available (EADDRNOTAVAIL)";
    case EAFNOSUPPORT:
        return stream << "Address family not supported (EAFNOSUPPORT)";
    case EAGAIN:
        return stream << "Resource unavailable, try again (EAGAIN)";
    case EALREADY:
        return stream << "Connection already in progress (EALREADY)";
    case EBADF:
        return stream << "Bad file descriptor (EBADF)";
    case EBADMSG:
        return stream << "Bad message (EBADMSG)";
    case EBUSY:
        return stream << "Device or resource busy (EBUSY)";
    case ECANCELED:
        return stream << "Operation canceled (ECANCELED)";
    case ECHILD:
        return stream << "No child processes (ECHILD)";
    case ECONNABORTED:
        return stream << "Connection aborted (ECONNABORTED)";
    case ECONNREFUSED:
        return stream << "Connection refused (ECONNREFUSED)";
    case ECONNRESET:
        return stream << "Connection reset (ECONNRESET)";
    case EDEADLK:
        return stream << "Resource deadlock would occur (EDEADLK)";
    case EDESTADDRREQ:
        return stream << "Destination address required (EDESTADDRREQ)";
    case EDOM:
        return stream << "Argument out of domain of function (EDOM)";
    case EDQUOT:
        return stream << "Reserved (EDQUOT)";
    case EEXIST:
        return stream << "File exists (EEXIST)";
    case EFAULT:
        return stream << "Bad address (EFAULT)";
    case EFBIG:
        return stream << "File too large (EFBIG)";
    case EHOSTUNREACH:
        return stream << "Host is unreachable (EHOSTUNREACH)";
    case EIDRM:
        return stream << "Identifier removed (EIDRM)";
    case EILSEQ:
        return stream << "Illegal byte sequence (EILSEQ)";
    case EINPROGRESS:
        return stream << "Operation in progress (EINPROGRESS)";
    case EINTR:
        return stream << "Interrupted function (EINTR)";
    case EINVAL:
        return stream << "Invalid argument (EINVAL)";
    case EIO:
        return stream << "I/O error (EIO)";
    case EISCONN:
        return stream << "Socket is connected (EISCONN)";
    case EISDIR:
        return stream << "Is a directory (EISDIR)";
    case ELOOP:
        return stream << "Too many levels of symbolic links (ELOOP)";
    case EMFILE:
        return stream << "Too many open files (EMFILE)";
    case EMLINK:
        return stream << "Too many links (EMLINK)";
    case EMSGSIZE:
        return stream << "Message too large (EMSGSIZE)";
    case EMULTIHOP:
        return stream << "Reserved (EMULTIHOP)";
    case ENAMETOOLONG:
        return stream << "Filename too long (ENAMETOOLONG)";
    case ENETDOWN:
        return stream << "Network is down (ENETDOWN)";
    case ENETRESET:
        return stream << "Connection aborted by network (ENETRESET)";
    case ENETUNREACH:
        return stream << "Network unreachable (ENETUNREACH)";
    case ENFILE:
        return stream << "Too many files open in system (ENFILE)";
    case ENOBUFS:
        return stream << "No buffer space available (ENOBUFS)";
#if !defined(BSLS_PLATFORM_OS_FREEBSD)
    case ENODATA:
        return stream << "No message is available on read queue (ENODATA)";
#endif
    case ENODEV:
        return stream << "No such device (ENODEV)";
    case ENOENT:
        return stream << "No such file or directory (ENOENT)";
    case ENOEXEC:
        return stream << "Executable file format error (ENOEXEC)";
    case ENOLCK:
        return stream << "No locks available (ENOLCK)";
    case ENOLINK:
        return stream << "Reserved (ENOLINK)";
    case ENOMEM:
        return stream << "Not enough space (ENOMEM)";
    case ENOMSG:
        return stream << "No message of the desired type (ENOMSG)";
    case ENOPROTOOPT:
        return stream << "Protocol not available (ENOPROTOOPT)";
    case ENOSPC:
        return stream << "No space left on device (ENOSPC)";
#if !defined(BSLS_PLATFORM_OS_FREEBSD)
    case ENOSR:
        return stream << "No STREAM resources (ENOSR)";
    case ENOSTR:
        return stream << "Not a STREAM (ENOSTR)";
#endif
    case ENOSYS:
        return stream << "Function not supported (ENOSYS)";
    case ENOTCONN:
        return stream << "The socket is not connected (ENOTCONN)";
    case ENOTDIR:
        return stream << "Not a directory (ENOTDIR)";
#if !defined(BSLS_PLATFORM_OS_AIX)
    case ENOTEMPTY:
        return stream << "Directory not empty (ENOTEMPTY)";
#endif
    case ENOTSOCK:
        return stream << "Not a socket (ENOTSOCK)";
    case ENOTSUP:
        return stream << "Not supported (ENOTSUP)";
    case ENOTTY:
        return stream << "Inappropriate I/O control operation (ENOTTY)";
    case ENXIO:
        return stream << "No such device or address (ENXIO)";
#if !defined(BSLS_PLATFORM_OS_FREEBSD) && !defined(BSLS_PLATFORM_OS_LINUX)
    case EOPNOTSUPP:
        return stream << "Operation not supported on socket (EOPNOTSUPP)";
#endif
    case EOVERFLOW:
        return stream << "Value too large to be stored (EOVERFLOW)";
    case EPERM:
        return stream << "Operation not permitted (EPERM)";
    case EPIPE:
        return stream << "Broken pipe (EPIPE)";
    case EPROTO:
        return stream << "Protocol error (EPROTO)";
    case EPROTONOSUPPORT:
        return stream << "Protocol not supported (EPROTONOSUPPORT)";
    case EPROTOTYPE:
        return stream << "Protocol wrong type for socket (EPROTOTYPE)";
    case ERANGE:
        return stream << "Result too large (ERANGE)";
    case EROFS:
        return stream << "Read-only file system (EROFS)";
    case ESPIPE:
        return stream << "Invalid seek (ESPIPE)";
    case ESRCH:
        return stream << "No such process (ESRCH)";
    case ESTALE:
        return stream << "Reserved (ESTALE)";
#if !defined(BSLS_PLATFORM_OS_FREEBSD)
    case ETIME:
        return stream << "Stream ioctl() timeout (ETIME)";
#endif
    case ETIMEDOUT:
        return stream << "Connection timed out (ETIMEDOUT)";
    case ETXTBSY:
        return stream << "Text file busy (ETXTBSY)";
#if (EWOULDBLOCK != EAGAIN)
    case EWOULDBLOCK:
        stream << "Operation would block (EWOULDBLOCK)";
#endif
    case EXDEV:
        return stream << "Cross-device link (EXDEV)";
    default:
        return stream << "??? (errno " << number << ")";
    }
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

bsl::ostream& Error::Impl::printErrorString(bsl::ostream&       stream,
                                            ntsa::Error::Number number,
                                            int                 level,
                                            int                 spacesPerLevel)
{
    char buffer[512];

    DWORD length = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                                     FORMAT_MESSAGE_IGNORE_INSERTS |
                                     FORMAT_MESSAGE_MAX_WIDTH_MASK,
                                 NULL,
                                 static_cast<DWORD>(number),
                                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                 buffer,
                                 sizeof buffer,
                                 0);

    if (length == 0) {
        bsl::memset(buffer, '?', 3);
        buffer[3] = 0;
    }
    else if (length >= 2) {
        if (buffer[length - 2] == '.') {
            buffer[length - 2] = 0;
        }
    }

    stream << buffer;

    return stream;
}

#else
#error Not implemented
#endif

#if defined(BSLS_PLATFORM_OS_UNIX)

ntsa::Error::Code Error::translate(ntsa::Error::Number number)
{
    switch (number) {
    case 0:
        return ntsa::Error::e_OK;
    case E2BIG:
        return ntsa::Error::e_LIMIT;
    case EACCES:
        return ntsa::Error::e_NOT_AUTHORIZED;
    case EADDRINUSE:
        return ntsa::Error::e_ADDRESS_IN_USE;
    case EADDRNOTAVAIL:
        return ntsa::Error::e_INVALID;
    case EAFNOSUPPORT:
        return ntsa::Error::e_INVALID;
    case EAGAIN:
        return ntsa::Error::e_WOULD_BLOCK;
    case EALREADY:
        return ntsa::Error::e_INVALID;
    case EBADF:
        return ntsa::Error::e_NOT_OPEN;
    case EBADMSG:
        return ntsa::Error::e_INVALID;
    case ECANCELED:
        return ntsa::Error::e_CANCELLED;
    case ECHILD:
        return ntsa::Error::e_INVALID;
    case ECONNABORTED:
        return ntsa::Error::e_CONNECTION_DEAD;
    case ECONNREFUSED:
        return ntsa::Error::e_CONNECTION_REFUSED;
    case ECONNRESET:
        return ntsa::Error::e_CONNECTION_DEAD;
    case EDESTADDRREQ:
        return ntsa::Error::e_INVALID;
    case EDOM:
        return ntsa::Error::e_INVALID;
    case EDQUOT:
        return ntsa::Error::e_INVALID;
    case EEXIST:
        return ntsa::Error::e_INVALID;
    case EFAULT:
        return ntsa::Error::e_INVALID;
    case EFBIG:
        return ntsa::Error::e_INVALID;
    case EHOSTUNREACH:
        return ntsa::Error::e_UNREACHABLE;
    case EINPROGRESS:
        return ntsa::Error::e_PENDING;
    case EINTR:
        return ntsa::Error::e_INTERRUPTED;
    case EINVAL:
        return ntsa::Error::e_INVALID;
    case EIO:
        return ntsa::Error::e_INVALID;
    case EISCONN:
        return ntsa::Error::e_INVALID;
    case EISDIR:
        return ntsa::Error::e_INVALID;
    case ELOOP:
        return ntsa::Error::e_LIMIT;
    case EMFILE:
        return ntsa::Error::e_LIMIT;
    case EMLINK:
        return ntsa::Error::e_LIMIT;
    case EMSGSIZE:
        return ntsa::Error::e_LIMIT;
    case ENAMETOOLONG:
        return ntsa::Error::e_INVALID;
    case ENETDOWN:
        return ntsa::Error::e_UNREACHABLE;
    case ENETRESET:
        return ntsa::Error::e_CONNECTION_RESET;
    case ENETUNREACH:
        return ntsa::Error::e_UNREACHABLE;
    case ENFILE:
        return ntsa::Error::e_LIMIT;
    case ENOBUFS:
        return ntsa::Error::e_LIMIT;
#if !defined(BSLS_PLATFORM_OS_FREEBSD)
    case ENODATA:
        return ntsa::Error::e_INVALID;
#endif
    case ENODEV:
        return ntsa::Error::e_INVALID;
    case ENOENT:
        return ntsa::Error::e_EOF;
    case ENOMEM:
        return ntsa::Error::e_LIMIT;
    case ENOMSG:
        return ntsa::Error::e_INVALID;
    case ENOPROTOOPT:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case ENOSPC:
        return ntsa::Error::e_LIMIT;
#if !defined(BSLS_PLATFORM_OS_FREEBSD)
    case ENOSR:
        return ntsa::Error::e_LIMIT;
    case ENOSTR:
        return ntsa::Error::e_INVALID;
#endif
    case ENOSYS:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case ENOTCONN:
        return ntsa::Error::e_CONNECTION_DEAD;
    case ENOTDIR:
        return ntsa::Error::e_INVALID;
#if !defined(BSLS_PLATFORM_OS_AIX)
    case ENOTEMPTY:
        return ntsa::Error::e_INVALID;
#endif
    case ENOTSOCK:
        return ntsa::Error::e_NOT_SOCKET;
    case ENOTSUP:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case ENOTTY:
        return ntsa::Error::e_INVALID;
#if !defined(BSLS_PLATFORM_OS_FREEBSD) && !defined(BSLS_PLATFORM_OS_LINUX)
    case EOPNOTSUPP:
        return ntsa::Error::e_NOT_IMPLEMENTED;
#endif
    case EOVERFLOW:
        return ntsa::Error::e_INVALID;
    case EPERM:
        return ntsa::Error::e_NOT_AUTHORIZED;
    case EPIPE:
        return ntsa::Error::e_CONNECTION_DEAD;
#if !defined(BSLS_PLATFORM_OS_AIX)
    case EPROTO:
        return ntsa::Error::e_INVALID;
#endif
    case EPROTONOSUPPORT:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case EPROTOTYPE:
        return ntsa::Error::e_INVALID;
    case ERANGE:
        return ntsa::Error::e_INVALID;
    case EROFS:
        return ntsa::Error::e_INVALID;
    case ESPIPE:
        return ntsa::Error::e_INVALID;
    case ESRCH:
        return ntsa::Error::e_INVALID;
#if !defined(BSLS_PLATFORM_OS_FREEBSD)
    case ETIME:
        return ntsa::Error::e_CONNECTION_TIMEOUT;
#endif
    case ETIMEDOUT:
        return ntsa::Error::e_CONNECTION_TIMEOUT;
#if (EWOULDBLOCK != EAGAIN)
    case EWOULDBLOCK:
        return ntsa::Error::e_WOULD_BLOCK;
#endif
    default:
        return ntsa::Error::e_UNKNOWN;
    }
}

ntsa::Error::Number Error::translate(ntsa::Error::Code code)
{
    switch (code) {
    case e_OK:
        return 0;
    case e_UNKNOWN:
        return EINVAL;
    case e_WOULD_BLOCK:
        return EAGAIN;
    case e_INTERRUPTED:
        return EINTR;
    case e_PENDING:
        return EINPROGRESS;
    case e_CANCELLED:
        return ECANCELED;
    case e_INVALID:
        return EINVAL;
    case e_EOF:
        return ENOENT;
    case e_LIMIT:
        return ENOBUFS;
    case e_ADDRESS_IN_USE:
        return EADDRINUSE;
    case e_CONNECTION_TIMEOUT:
        return ETIMEDOUT;
    case e_CONNECTION_REFUSED:
        return ECONNREFUSED;
    case e_CONNECTION_RESET:
        return ECONNRESET;
    case e_CONNECTION_DEAD:
        return EPIPE;
    case e_UNREACHABLE:
        return ENETUNREACH;
    case e_NOT_AUTHORIZED:
        return EPERM;
    case e_NOT_IMPLEMENTED:
        return EPROTO;
    case e_NOT_OPEN:
        return EBADF;
    case e_NOT_SOCKET:
        return ENOTSOCK;
    default:
        return EINVAL;
    };
}

bsl::ostream& operator<<(bsl::ostream& stream, const Error& object)
{
    Error::Impl::printErrorString(stream,
                                  object.d_value.d_pair.d_number,
                                  0,
                                  -1);

    return stream;
}

ntsa::Error Error::invalid()
{
    return ntsa::Error(EINVAL);
}

ntsa::Error Error::timeout()
{
    return ntsa::Error(ETIMEDOUT);
}

ntsa::Error Error::cancelled()
{
    return ntsa::Error(ECANCELED);
}

ntsa::Error ntsa::Error::last()
{
    return ntsa::Error(errno);
}

#elif defined(BSLS_PLATFORM_OS_WINDOWS)

ntsa::Error::Code Error::translate(ntsa::Error::Number number)
{
    switch (number) {
    case 0:
        return ntsa::Error::e_OK;

    case ERROR_GRACEFUL_DISCONNECT:
        return ntsa::Error::e_EOF;
    case ERROR_NETWORK_UNREACHABLE:
        return ntsa::Error::e_UNREACHABLE;
    case ERROR_HOST_UNREACHABLE:
        return ntsa::Error::e_UNREACHABLE;
    case ERROR_PROTOCOL_UNREACHABLE:
        return ntsa::Error::e_UNREACHABLE;
    case ERROR_PORT_UNREACHABLE:
        return ntsa::Error::e_UNREACHABLE;
    case ERROR_REQUEST_ABORTED:
        return ntsa::Error::e_CANCELLED;
    case ERROR_OPERATION_ABORTED:
        return ntsa::Error::e_CANCELLED;
    case ERROR_CONNECTION_ABORTED:
        return ntsa::Error::e_CANCELLED;
    case ERROR_CONNECTION_REFUSED:
        return ntsa::Error::e_CONNECTION_REFUSED;
    case ERROR_TIMEOUT:
        return ntsa::Error::e_CONNECTION_TIMEOUT;

    case ERROR_NOT_ENOUGH_MEMORY:
        return ntsa::Error::e_LIMIT;
    case ERROR_INVALID_HANDLE:
        return ntsa::Error::e_NOT_OPEN;
    case ERROR_INVALID_PARAMETER:
        return ntsa::Error::e_INVALID;
    case ERROR_IO_INCOMPLETE:
        return ntsa::Error::e_PENDING;
    case ERROR_IO_PENDING:
        return ntsa::Error::e_PENDING;

    case ERROR_ACCESS_DENIED:
        return ntsa::Error::e_NOT_AUTHORIZED;

        // Windows error codes 64 - 65 from winerror.h.

    case ERROR_NETNAME_DELETED:
        return ntsa::Error::e_UNREACHABLE;
    case ERROR_NETWORK_ACCESS_DENIED:
        return ntsa::Error::e_UNREACHABLE;

        // Windows Sockets definitions of regular Microsoft C error codes.

    case WSAEINTR:
        return ntsa::Error::e_INTERRUPTED;

        // Windows Sockets definitions of regular Berkeley error codes.

    case WSAEWOULDBLOCK:
        return ntsa::Error::e_WOULD_BLOCK;
    case WSAEINPROGRESS:
        return ntsa::Error::e_PENDING;
    case WSAENOPROTOOPT:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case WSAEPROTONOSUPPORT:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case WSAESOCKTNOSUPPORT:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case WSAEOPNOTSUPP:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case WSAEPFNOSUPPORT:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case WSAEAFNOSUPPORT:
        return ntsa::Error::e_NOT_IMPLEMENTED;
    case WSAEADDRINUSE:
        return ntsa::Error::e_ADDRESS_IN_USE;
    case WSAENETDOWN:
        return ntsa::Error::e_UNREACHABLE;
    case WSAENETUNREACH:
        return ntsa::Error::e_UNREACHABLE;
    case WSAENETRESET:
        return ntsa::Error::e_UNREACHABLE;
    case WSAECONNABORTED:
        return ntsa::Error::e_CONNECTION_DEAD;
    case WSAECONNRESET:
        return ntsa::Error::e_CONNECTION_RESET;
    case WSAENOBUFS:
        return ntsa::Error::e_LIMIT;
    case WSAEBADF:
        return ntsa::Error::e_NOT_OPEN;
    case WSAENOTSOCK:
        return ntsa::Error::e_NOT_SOCKET;
    case WSAENOTCONN:
        return ntsa::Error::e_CONNECTION_DEAD;
    case WSAESHUTDOWN:
        return ntsa::Error::e_CONNECTION_DEAD;
    case WSAETIMEDOUT:
        return ntsa::Error::e_CONNECTION_TIMEOUT;
    case WSAECONNREFUSED:
        return ntsa::Error::e_CONNECTION_REFUSED;

    case WSAEHOSTDOWN:
        return ntsa::Error::e_UNREACHABLE;
    case WSAEHOSTUNREACH:
        return ntsa::Error::e_UNREACHABLE;
    case WSAEPROCLIM:
        return ntsa::Error::e_LIMIT;
    case WSAEUSERS:
        return ntsa::Error::e_LIMIT;
    case WSAEDQUOT:
        return ntsa::Error::e_LIMIT;
    case ENOENT:
        return ntsa::Error::e_EOF;

    case WSAEINVAL:
        return ntsa::Error::e_INVALID;

        // Winsock 2 error codes.

    case EPIPE:
        return ntsa::Error::e_CONNECTION_DEAD;

    default:
        return ntsa::Error::e_UNKNOWN;
    }
}

ntsa::Error::Number Error::translate(ntsa::Error::Code code)
{
    switch (code) {
    case e_OK:
        return 0;
    case e_UNKNOWN:
        return WSAEINVAL;
    case e_WOULD_BLOCK:
        return WSAEWOULDBLOCK;
    case e_INTERRUPTED:
        return WSAEINTR;
    case e_PENDING:
        return WSAEINPROGRESS;
    case e_CANCELLED:
        return ERROR_OPERATION_ABORTED;
    case e_INVALID:
        return WSAEINVAL;
    case e_EOF:
        return ENOENT;
    case e_LIMIT:
        return WSAENOBUFS;
    case e_ADDRESS_IN_USE:
        return WSAEADDRINUSE;
    case e_CONNECTION_TIMEOUT:
        return WSAETIMEDOUT;
    case e_CONNECTION_REFUSED:
        return WSAECONNREFUSED;
    case e_CONNECTION_RESET:
        return WSAECONNRESET;
    case e_CONNECTION_DEAD:
        return EPIPE;
    case e_UNREACHABLE:
        return WSAENETUNREACH;
    case e_NOT_AUTHORIZED:
        return ERROR_ACCESS_DENIED;
    case e_NOT_IMPLEMENTED:
        return WSAEOPNOTSUPP;
    case e_NOT_OPEN:
        return WSAEBADF;
    case e_NOT_SOCKET:
        return WSAENOTSOCK;
    default:
        return WSAEOPNOTSUPP;
    };
}

bsl::ostream& operator<<(bsl::ostream& stream, const Error& object)
{
    Error::Impl::printErrorString(
        stream, object.d_value.d_pair.d_number, 0, -1);
    return stream;
}

ntsa::Error Error::invalid()
{
    return ntsa::Error(WSAEINVAL);
}

ntsa::Error Error::timeout()
{
    return ntsa::Error(WSAETIMEDOUT);
}

ntsa::Error Error::cancelled()
{
    return ntsa::Error(ERROR_OPERATION_ABORTED);
}

ntsa::Error ntsa::Error::last()
{
    return ntsa::Error(GetLastError());
}

#else
#error Not implemented
#endif

bsl::string Error::text() const
{
    bsl::stringstream ss;
    ss << *this;
    return ss.str();
}

bsl::ostream& Error::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();
    if (d_value.d_pair.d_code != ntsa::Error::e_OK) {
        printer.printForeign(d_value.d_pair.d_code,
                             &Error::Impl::printErrorCode,
                             "code");
        printer.printAttribute("number", d_value.d_pair.d_number);
        printer.printForeign(d_value.d_pair.d_number,
                             &Error::Impl::printErrorString,
                             "description");
    }
    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
