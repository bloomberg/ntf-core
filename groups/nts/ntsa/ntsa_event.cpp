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

#include <ntsa_event.h>

#include <bsls_ident.h>
BSLS_IDENT_RCSID(ntsa_event_cpp, "$Id$ $CSID$")

#include <bslim_printer.h>

namespace BloombergLP {
namespace ntsa {

bool Event::equals(const Event& other) const
{
    return (d_handle == other.d_handle && d_state == other.d_state &&
            d_bytesReadable == other.d_bytesReadable &&
            d_bytesWritable == other.d_bytesWritable &&
            d_backlog == other.d_backlog && d_error == other.d_error);
}

bool Event::less(const Event& other) const
{
    if (d_handle < other.d_handle) {
        return true;
    }

    if (other.d_handle < d_handle) {
        return false;
    }

    if (d_state < other.d_state) {
        return true;
    }

    if (other.d_state < d_state) {
        return false;
    }

    if (d_bytesReadable < other.d_bytesReadable) {
        return true;
    }

    if (other.d_bytesReadable < d_bytesReadable) {
        return false;
    }

    if (d_bytesWritable < other.d_bytesWritable) {
        return true;
    }

    if (other.d_bytesWritable < d_bytesWritable) {
        return false;
    }

    if (d_backlog < other.d_backlog) {
        return true;
    }

    if (other.d_backlog < d_backlog) {
        return false;
    }

    return d_error < other.d_error;
}

bsl::ostream& Event::print(bsl::ostream& stream,
                           int           level,
                           int           spacesPerLevel) const
{
    bslim::Printer printer(&stream, level, spacesPerLevel);
    printer.start();

    if (d_handle != ntsa::k_INVALID_HANDLE) {
        printer.printAttribute("handle", d_handle);
    }
    else {
        printer.printAttribute("handle", "INVALID");
    }

    if (d_state != 0) {
        bsl::string stateDescription;
        bsl::size_t numFlags = 0;

        if ((d_state & (1 << e_READABLE)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("READABLE", 8);
            ++numFlags;
        }

        if ((d_state & (1 << e_WRITABLE)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("WRITABLE", 8);
            ++numFlags;
        }

        if ((d_state & (1 << e_ERROR)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("ERROR", 5);
            ++numFlags;
        }

        if ((d_state & (1 << e_SHUTDOWN)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("SHUTDOWN", 8);
            ++numFlags;
        }

        if ((d_state & (1 << e_DISCONNECTED)) != 0) {
            if (numFlags > 0) {
                stateDescription.append(1, ' ');
            }

            stateDescription.append("DISCONNECTED", 12);
            ++numFlags;
        }

        printer.printAttribute("state", stateDescription);
    }

    if (!d_bytesReadable.isNull()) {
        printer.printAttribute("bytesReadable", d_bytesReadable);
    }

    if (!d_bytesWritable.isNull()) {
        printer.printAttribute("bytesWritable", d_bytesWritable);
    }

    if (!d_backlog.isNull()) {
        printer.printAttribute("backlog", d_backlog);
    }

    if (d_error) {
        printer.printAttribute("error", d_error);
    }

    printer.end();
    return stream;
}

}  // close package namespace
}  // close enterprise namespace
