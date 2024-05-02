// Copyright 2024 Bloomberg Finance L.P.
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
#ifndef INCLUDED_NTCD_DATAPOOL
#define INCLUDED_NTCD_DATAPOOL

#include <bsls_ident.h>
BSLS_IDENT("$Id: $")

#include <ntci_datapool.h>
#include <ntccfg_test.h>

namespace BloombergLP {
namespace ntcd {

NTF_MOCK_CLASS(DataPoolMock, ntci::DataPool)
NTF_MOCK_METHOD(bsl::shared_ptr<ntsa::Data>, createIncomingData)
NTF_MOCK_METHOD(bsl::shared_ptr<ntsa::Data>, createOutgoingData)
NTF_MOCK_METHOD(bsl::shared_ptr<bdlbb::Blob>, createIncomingBlob)
NTF_MOCK_METHOD(bsl::shared_ptr<bdlbb::Blob>, createOutgoingBlob)
NTF_MOCK_METHOD(void, createIncomingBlobBuffer, bdlbb::BlobBuffer*)
NTF_MOCK_METHOD(void, createOutgoingBlobBuffer, bdlbb::BlobBuffer*)
NTF_MOCK_METHOD_CONST(const bsl::shared_ptr<bdlbb::BlobBufferFactory>&,
                      incomingBlobBufferFactory)
NTF_MOCK_METHOD_CONST(const bsl::shared_ptr<bdlbb::BlobBufferFactory>&,
                      outgoingBlobBufferFactory)
NTF_MOCK_CLASS_END;

}  // close package namespace
}  // close enterprise namespace

#endif
