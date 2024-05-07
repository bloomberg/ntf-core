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

#include <ntca_encryptioncertificate.h>

#include <ntccfg_test.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iomanip.h>
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

namespace test {

// clang-format off
const unsigned char k_USER_CERTIFICATE_ASN1[614] = {
    0x30, 0x82, 0x02, 0x62, 0x30, 0x82, 0x02, 0x08, 0xA0, 0x03,
    0x02, 0x01, 0x02, 0x02, 0x01, 0x02, 0x30, 0x0A, 0x06, 0x08,
    0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x48,
    0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C,
    0x07, 0x54, 0x45, 0x53, 0x54, 0x2E, 0x43, 0x41, 0x31, 0x15,
    0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42,
    0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x20, 0x4C,
    0x50, 0x31, 0x1D, 0x30, 0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B,
    0x0C, 0x14, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65, 0x77, 0x61,
    0x72, 0x65, 0x20, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F,
    0x72, 0x74, 0x30, 0x1E, 0x17, 0x0D, 0x32, 0x34, 0x30, 0x34,
    0x32, 0x33, 0x31, 0x34, 0x33, 0x34, 0x32, 0x31, 0x5A, 0x17,
    0x0D, 0x32, 0x35, 0x30, 0x34, 0x32, 0x33, 0x31, 0x34, 0x33,
    0x34, 0x32, 0x31, 0x5A, 0x30, 0x7C, 0x31, 0x12, 0x30, 0x10,
    0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x09, 0x54, 0x45, 0x53,
    0x54, 0x2E, 0x55, 0x53, 0x45, 0x52, 0x31, 0x15, 0x30, 0x13,
    0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42, 0x6C, 0x6F,
    0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x20, 0x4C, 0x50, 0x31,
    0x14, 0x30, 0x12, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x0B,
    0x45, 0x6E, 0x67, 0x69, 0x6E, 0x65, 0x65, 0x72, 0x69, 0x6E,
    0x67, 0x31, 0x1A, 0x30, 0x18, 0x06, 0x03, 0x55, 0x04, 0x0B,
    0x0C, 0x11, 0x50, 0x6C, 0x61, 0x74, 0x66, 0x6F, 0x72, 0x6D,
    0x20, 0x53, 0x65, 0x72, 0x76, 0x69, 0x63, 0x65, 0x73, 0x31,
    0x1D, 0x30, 0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x14,
    0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65, 0x77, 0x61, 0x72, 0x65,
    0x20, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F, 0x72, 0x74,
    0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE,
    0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D,
    0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xDF, 0x0D, 0x91,
    0xDD, 0x48, 0x1B, 0xF5, 0xE0, 0x5E, 0x93, 0x1C, 0x54, 0x20,
    0x9E, 0xAD, 0xB6, 0xA2, 0x86, 0x10, 0x3F, 0x8F, 0x29, 0x9A,
    0xB0, 0x05, 0xF9, 0x84, 0x34, 0x34, 0x7F, 0x08, 0x30, 0xCB,
    0x65, 0x62, 0x84, 0x17, 0x87, 0x72, 0x59, 0x99, 0xF0, 0x76,
    0x82, 0xD2, 0x4B, 0x56, 0xBF, 0x7E, 0x9A, 0x2D, 0xF6, 0x89,
    0xE0, 0x06, 0x05, 0x45, 0xCF, 0xD7, 0x69, 0xFF, 0x1B, 0x84,
    0x6F, 0xA3, 0x81, 0xAE, 0x30, 0x81, 0xAB, 0x30, 0x09, 0x06,
    0x03, 0x55, 0x1D, 0x13, 0x04, 0x02, 0x30, 0x00, 0x30, 0x1D,
    0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0xB0,
    0xF6, 0xEA, 0x19, 0x1E, 0xFE, 0xA6, 0xC2, 0x36, 0xF5, 0x8C,
    0x9C, 0x38, 0xD2, 0xD9, 0x04, 0x62, 0x55, 0x02, 0x6F, 0x30,
    0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18, 0x30, 0x16,
    0x80, 0x14, 0x2C, 0x27, 0xDB, 0x5A, 0x05, 0xC8, 0xA1, 0x49,
    0xFB, 0xC8, 0x1C, 0xFB, 0x40, 0x68, 0xC7, 0xA2, 0xBA, 0x42,
    0xD8, 0xDA, 0x30, 0x1C, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04,
    0x15, 0x30, 0x13, 0x82, 0x11, 0x6E, 0x74, 0x66, 0x2E, 0x62,
    0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x2E, 0x63,
    0x6F, 0x6D, 0x30, 0x1E, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04,
    0x17, 0x30, 0x15, 0x82, 0x13, 0x2A, 0x2E, 0x64, 0x65, 0x76,
    0x2E, 0x62, 0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67,
    0x2E, 0x63, 0x6F, 0x6D, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x1D,
    0x11, 0x04, 0x08, 0x30, 0x06, 0x87, 0x04, 0x0A, 0x22, 0x06,
    0x17, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x1D, 0x11, 0x04, 0x08,
    0x30, 0x06, 0x87, 0x04, 0x0A, 0x22, 0x07, 0x57, 0x30, 0x0A,
    0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02,
    0x03, 0x48, 0x00, 0x30, 0x45, 0x02, 0x20, 0x37, 0x2E, 0xA7,
    0x2E, 0xC0, 0x63, 0x26, 0x60, 0x46, 0xB7, 0x37, 0xD9, 0x19,
    0xD6, 0x60, 0x89, 0x19, 0xBA, 0x43, 0xB8, 0x61, 0x90, 0x77,
    0x5B, 0x7F, 0x6D, 0xB5, 0xD8, 0xAE, 0x7B, 0x93, 0xC2, 0x02,
    0x21, 0x00, 0xD5, 0xB2, 0x8C, 0x90, 0xA1, 0x68, 0x5D, 0xA8,
    0xB5, 0xAF, 0xA2, 0xAF, 0x30, 0x08, 0x11, 0xC4, 0xCC, 0x73,
    0x9D, 0x60, 0xB2, 0xEF, 0x9D, 0xC5, 0xA9, 0x63, 0x71, 0xB3,
    0xF4, 0x5A, 0x89, 0xF4
};

const unsigned char k_CA_CERTIFICATE_ASN1[479] = {
    0x30, 0x82, 0x01, 0xDB, 0x30, 0x82, 0x01, 0x82, 0xA0, 0x03,
    0x02, 0x01, 0x02, 0x02, 0x01, 0x01, 0x30, 0x0A, 0x06, 0x08,
    0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x30, 0x48,
    0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55, 0x04, 0x03, 0x0C,
    0x07, 0x54, 0x45, 0x53, 0x54, 0x2E, 0x43, 0x41, 0x31, 0x15,
    0x30, 0x13, 0x06, 0x03, 0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42,
    0x6C, 0x6F, 0x6F, 0x6D, 0x62, 0x65, 0x72, 0x67, 0x20, 0x4C,
    0x50, 0x31, 0x1D, 0x30, 0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B,
    0x0C, 0x14, 0x4D, 0x69, 0x64, 0x64, 0x6C, 0x65, 0x77, 0x61,
    0x72, 0x65, 0x20, 0x54, 0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F,
    0x72, 0x74, 0x30, 0x1E, 0x17, 0x0D, 0x32, 0x34, 0x30, 0x34,
    0x32, 0x32, 0x31, 0x32, 0x30, 0x33, 0x34, 0x34, 0x5A, 0x17,
    0x0D, 0x32, 0x35, 0x30, 0x34, 0x32, 0x32, 0x31, 0x32, 0x30,
    0x33, 0x34, 0x34, 0x5A, 0x30, 0x48, 0x31, 0x10, 0x30, 0x0E,
    0x06, 0x03, 0x55, 0x04, 0x03, 0x0C, 0x07, 0x54, 0x45, 0x53,
    0x54, 0x2E, 0x43, 0x41, 0x31, 0x15, 0x30, 0x13, 0x06, 0x03,
    0x55, 0x04, 0x0A, 0x0C, 0x0C, 0x42, 0x6C, 0x6F, 0x6F, 0x6D,
    0x62, 0x65, 0x72, 0x67, 0x20, 0x4C, 0x50, 0x31, 0x1D, 0x30,
    0x1B, 0x06, 0x03, 0x55, 0x04, 0x0B, 0x0C, 0x14, 0x4D, 0x69,
    0x64, 0x64, 0x6C, 0x65, 0x77, 0x61, 0x72, 0x65, 0x20, 0x54,
    0x72, 0x61, 0x6E, 0x73, 0x70, 0x6F, 0x72, 0x74, 0x30, 0x59,
    0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x02,
    0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01,
    0x07, 0x03, 0x42, 0x00, 0x04, 0xD7, 0x4E, 0x0D, 0x3B, 0xD1,
    0xA3, 0x45, 0x01, 0x79, 0xE7, 0x88, 0x57, 0x2C, 0xB1, 0x89,
    0xE6, 0xEF, 0x49, 0xE3, 0x64, 0x10, 0xA6, 0xCD, 0x3F, 0x77,
    0x98, 0x2F, 0x7B, 0xED, 0xC2, 0x18, 0xFF, 0xEE, 0xAA, 0x62,
    0xCA, 0x05, 0x66, 0x24, 0x6D, 0xA0, 0x25, 0x10, 0x81, 0x3E,
    0x8D, 0x19, 0x2D, 0x6F, 0xD5, 0x7A, 0x40, 0x18, 0xC7, 0x5B,
    0xE3, 0xB6, 0x4C, 0x34, 0xF6, 0x9D, 0x65, 0x04, 0xAA, 0xA3,
    0x5D, 0x30, 0x5B, 0x30, 0x0C, 0x06, 0x03, 0x55, 0x1D, 0x13,
    0x04, 0x05, 0x30, 0x03, 0x01, 0x01, 0xFF, 0x30, 0x0B, 0x06,
    0x03, 0x55, 0x1D, 0x0F, 0x04, 0x04, 0x03, 0x02, 0x01, 0x06,
    0x30, 0x1D, 0x06, 0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04,
    0x14, 0x2C, 0x27, 0xDB, 0x5A, 0x05, 0xC8, 0xA1, 0x49, 0xFB,
    0xC8, 0x1C, 0xFB, 0x40, 0x68, 0xC7, 0xA2, 0xBA, 0x42, 0xD8,
    0xDA, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18,
    0x30, 0x16, 0x80, 0x14, 0x2C, 0x27, 0xDB, 0x5A, 0x05, 0xC8,
    0xA1, 0x49, 0xFB, 0xC8, 0x1C, 0xFB, 0x40, 0x68, 0xC7, 0xA2,
    0xBA, 0x42, 0xD8, 0xDA, 0x30, 0x0A, 0x06, 0x08, 0x2A, 0x86,
    0x48, 0xCE, 0x3D, 0x04, 0x03, 0x02, 0x03, 0x47, 0x00, 0x30,
    0x44, 0x02, 0x20, 0x73, 0xF0, 0xCC, 0xEB, 0x8C, 0xF2, 0xF0,
    0x81, 0x00, 0x1B, 0xBA, 0xFC, 0x66, 0x64, 0x6C, 0xBC, 0x2E,
    0xF5, 0x42, 0x26, 0xFB, 0x81, 0x54, 0xB2, 0x80, 0x87, 0xD5,
    0x8C, 0x66, 0x23, 0x8A, 0x53, 0x02, 0x20, 0x7F, 0xD0, 0x87,
    0x65, 0x65, 0x9B, 0xBA, 0x39, 0x93, 0x81, 0xAA, 0x05, 0x58,
    0x7B, 0x55, 0x13, 0xDA, 0x9E, 0x78, 0x82, 0xEB, 0x89, 0xD1,
    0xD1, 0x4D, 0xB1, 0x57, 0x82, 0x35, 0x4B, 0xAF, 0x9B
};

const unsigned char k_REAL_CERTIFICATE_ASN1[1709] = {
    0x30, 0x82, 0x06, 0xA9, 0x30, 0x82, 0x05, 0x91, 0xA0, 0x03,
    0x02, 0x01, 0x02, 0x02, 0x08, 0x4C, 0x48, 0x12, 0x1D, 0x31,
    0x20, 0xE8, 0x0A, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48,
    0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x05, 0x00, 0x30, 0x81,
    0xC6, 0x31, 0x0B, 0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06,
    0x13, 0x02, 0x55, 0x53, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03,
    0x55, 0x04, 0x08, 0x13, 0x07, 0x41, 0x72, 0x69, 0x7A, 0x6F,
    0x6E, 0x61, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04,
    0x07, 0x13, 0x0A, 0x53, 0x63, 0x6F, 0x74, 0x74, 0x73, 0x64,
    0x61, 0x6C, 0x65, 0x31, 0x25, 0x30, 0x23, 0x06, 0x03, 0x55,
    0x04, 0x0A, 0x13, 0x1C, 0x53, 0x74, 0x61, 0x72, 0x66, 0x69,
    0x65, 0x6C, 0x64, 0x20, 0x54, 0x65, 0x63, 0x68, 0x6E, 0x6F,
    0x6C, 0x6F, 0x67, 0x69, 0x65, 0x73, 0x2C, 0x20, 0x49, 0x6E,
    0x63, 0x2E, 0x31, 0x33, 0x30, 0x31, 0x06, 0x03, 0x55, 0x04,
    0x0B, 0x13, 0x2A, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F,
    0x63, 0x65, 0x72, 0x74, 0x73, 0x2E, 0x73, 0x74, 0x61, 0x72,
    0x66, 0x69, 0x65, 0x6C, 0x64, 0x74, 0x65, 0x63, 0x68, 0x2E,
    0x63, 0x6F, 0x6D, 0x2F, 0x72, 0x65, 0x70, 0x6F, 0x73, 0x69,
    0x74, 0x6F, 0x72, 0x79, 0x2F, 0x31, 0x34, 0x30, 0x32, 0x06,
    0x03, 0x55, 0x04, 0x03, 0x13, 0x2B, 0x53, 0x74, 0x61, 0x72,
    0x66, 0x69, 0x65, 0x6C, 0x64, 0x20, 0x53, 0x65, 0x63, 0x75,
    0x72, 0x65, 0x20, 0x43, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69,
    0x63, 0x61, 0x74, 0x65, 0x20, 0x41, 0x75, 0x74, 0x68, 0x6F,
    0x72, 0x69, 0x74, 0x79, 0x20, 0x2D, 0x20, 0x47, 0x32, 0x30,
    0x1E, 0x17, 0x0D, 0x32, 0x33, 0x30, 0x33, 0x30, 0x31, 0x31,
    0x34, 0x30, 0x36, 0x35, 0x31, 0x5A, 0x17, 0x0D, 0x32, 0x34,
    0x30, 0x34, 0x30, 0x31, 0x31, 0x34, 0x30, 0x36, 0x35, 0x31,
    0x5A, 0x30, 0x12, 0x31, 0x10, 0x30, 0x0E, 0x06, 0x03, 0x55,
    0x04, 0x03, 0x13, 0x07, 0x75, 0x73, 0x76, 0x63, 0x2E, 0x69,
    0x6F, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0D, 0x06, 0x09, 0x2A,
    0x86, 0x48, 0x86, 0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00,
    0x03, 0x82, 0x01, 0x0F, 0x00, 0x30, 0x82, 0x01, 0x0A, 0x02,
    0x82, 0x01, 0x01, 0x00, 0x85, 0x56, 0x97, 0xE3, 0xF0, 0x51,
    0x88, 0x40, 0x66, 0x1F, 0xB5, 0x51, 0xFF, 0x0F, 0x9D, 0xC5,
    0xEE, 0x4E, 0x56, 0x80, 0x91, 0x1D, 0xBE, 0x41, 0x3F, 0xC2,
    0x37, 0x65, 0x38, 0x53, 0x0C, 0xCC, 0x71, 0x2D, 0xC9, 0xF3,
    0xF6, 0x97, 0x27, 0x02, 0x91, 0xAC, 0x26, 0x3F, 0xF7, 0x5B,
    0x48, 0xF1, 0x57, 0xAD, 0x00, 0x6C, 0xF4, 0x51, 0xCE, 0xBD,
    0xE7, 0x72, 0x34, 0x4C, 0xEB, 0xAA, 0xAA, 0x72, 0x68, 0xB4,
    0x93, 0x4D, 0x4A, 0xE5, 0xCA, 0x0E, 0xC5, 0xCA, 0x83, 0xF2,
    0x7B, 0xAC, 0xE0, 0xD9, 0xD9, 0xC1, 0x4D, 0xB9, 0x2F, 0x7E,
    0x52, 0xCA, 0x74, 0xBA, 0x50, 0xB8, 0x3C, 0xC8, 0xD2, 0xB5,
    0x19, 0x77, 0x23, 0xFB, 0x95, 0xF1, 0x90, 0x90, 0x5B, 0xE4,
    0xE9, 0xF9, 0x02, 0xAE, 0xF3, 0x18, 0x57, 0xA5, 0x34, 0x87,
    0xE6, 0xAF, 0xF5, 0xCE, 0xD2, 0x74, 0x23, 0x1E, 0xA4, 0xB6,
    0x6D, 0x17, 0x92, 0x3B, 0xF6, 0x8F, 0xCA, 0x13, 0xF2, 0x8A,
    0x22, 0x45, 0x22, 0x47, 0x47, 0x6B, 0xE2, 0x6A, 0x47, 0xD6,
    0x67, 0x24, 0xDB, 0x90, 0xAE, 0x81, 0x4E, 0xA7, 0x57, 0x7E,
    0xE2, 0xC3, 0xC6, 0x1E, 0x1A, 0x9A, 0x8E, 0x50, 0xF5, 0x6C,
    0x8E, 0x38, 0xD3, 0x21, 0x14, 0x1A, 0x1C, 0xBB, 0xD1, 0xB0,
    0xEE, 0xFA, 0xE4, 0x05, 0x62, 0xEC, 0xAB, 0x44, 0x7D, 0x12,
    0x1C, 0x36, 0x9C, 0x7C, 0x68, 0x78, 0xF5, 0x58, 0xBF, 0x99,
    0xD6, 0xAF, 0x54, 0x13, 0x1C, 0x6C, 0x2A, 0xD2, 0x99, 0x20,
    0x29, 0xDD, 0xCF, 0x70, 0xFC, 0xE3, 0xCF, 0x76, 0xE8, 0xA5,
    0x6F, 0x3C, 0x77, 0x76, 0x62, 0xB9, 0x1E, 0xAA, 0x67, 0x43,
    0x39, 0xFC, 0x1D, 0x53, 0x7C, 0x59, 0xF0, 0x5C, 0xED, 0xB5,
    0xD7, 0x9E, 0x78, 0x16, 0x06, 0x77, 0xCA, 0xC6, 0x5E, 0xF7,
    0x0A, 0xCA, 0x95, 0x23, 0x54, 0x8D, 0x3E, 0xBA, 0x80, 0x09,
    0x02, 0x03, 0x01, 0x00, 0x01, 0xA3, 0x82, 0x03, 0x4C, 0x30,
    0x82, 0x03, 0x48, 0x30, 0x0C, 0x06, 0x03, 0x55, 0x1D, 0x13,
    0x01, 0x01, 0xFF, 0x04, 0x02, 0x30, 0x00, 0x30, 0x1D, 0x06,
    0x03, 0x55, 0x1D, 0x25, 0x04, 0x16, 0x30, 0x14, 0x06, 0x08,
    0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x01, 0x06, 0x08,
    0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x03, 0x02, 0x30, 0x0E,
    0x06, 0x03, 0x55, 0x1D, 0x0F, 0x01, 0x01, 0xFF, 0x04, 0x04,
    0x03, 0x02, 0x05, 0xA0, 0x30, 0x3D, 0x06, 0x03, 0x55, 0x1D,
    0x1F, 0x04, 0x36, 0x30, 0x34, 0x30, 0x32, 0xA0, 0x30, 0xA0,
    0x2E, 0x86, 0x2C, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F,
    0x63, 0x72, 0x6C, 0x2E, 0x73, 0x74, 0x61, 0x72, 0x66, 0x69,
    0x65, 0x6C, 0x64, 0x74, 0x65, 0x63, 0x68, 0x2E, 0x63, 0x6F,
    0x6D, 0x2F, 0x73, 0x66, 0x69, 0x67, 0x32, 0x73, 0x31, 0x2D,
    0x35, 0x34, 0x36, 0x2E, 0x63, 0x72, 0x6C, 0x30, 0x63, 0x06,
    0x03, 0x55, 0x1D, 0x20, 0x04, 0x5C, 0x30, 0x5A, 0x30, 0x4E,
    0x06, 0x0B, 0x60, 0x86, 0x48, 0x01, 0x86, 0xFD, 0x6E, 0x01,
    0x07, 0x17, 0x01, 0x30, 0x3F, 0x30, 0x3D, 0x06, 0x08, 0x2B,
    0x06, 0x01, 0x05, 0x05, 0x07, 0x02, 0x01, 0x16, 0x31, 0x68,
    0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x63, 0x65, 0x72, 0x74,
    0x69, 0x66, 0x69, 0x63, 0x61, 0x74, 0x65, 0x73, 0x2E, 0x73,
    0x74, 0x61, 0x72, 0x66, 0x69, 0x65, 0x6C, 0x64, 0x74, 0x65,
    0x63, 0x68, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x72, 0x65, 0x70,
    0x6F, 0x73, 0x69, 0x74, 0x6F, 0x72, 0x79, 0x2F, 0x30, 0x08,
    0x06, 0x06, 0x67, 0x81, 0x0C, 0x01, 0x02, 0x01, 0x30, 0x81,
    0x82, 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x01,
    0x01, 0x04, 0x76, 0x30, 0x74, 0x30, 0x2A, 0x06, 0x08, 0x2B,
    0x06, 0x01, 0x05, 0x05, 0x07, 0x30, 0x01, 0x86, 0x1E, 0x68,
    0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6F, 0x63, 0x73, 0x70,
    0x2E, 0x73, 0x74, 0x61, 0x72, 0x66, 0x69, 0x65, 0x6C, 0x64,
    0x74, 0x65, 0x63, 0x68, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x30,
    0x46, 0x06, 0x08, 0x2B, 0x06, 0x01, 0x05, 0x05, 0x07, 0x30,
    0x02, 0x86, 0x3A, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F,
    0x63, 0x65, 0x72, 0x74, 0x69, 0x66, 0x69, 0x63, 0x61, 0x74,
    0x65, 0x73, 0x2E, 0x73, 0x74, 0x61, 0x72, 0x66, 0x69, 0x65,
    0x6C, 0x64, 0x74, 0x65, 0x63, 0x68, 0x2E, 0x63, 0x6F, 0x6D,
    0x2F, 0x72, 0x65, 0x70, 0x6F, 0x73, 0x69, 0x74, 0x6F, 0x72,
    0x79, 0x2F, 0x73, 0x66, 0x69, 0x67, 0x32, 0x2E, 0x63, 0x72,
    0x74, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D, 0x23, 0x04, 0x18,
    0x30, 0x16, 0x80, 0x14, 0x25, 0x45, 0x81, 0x68, 0x50, 0x26,
    0x38, 0x3D, 0x3B, 0x2D, 0x2C, 0xBE, 0xCD, 0x6A, 0xD9, 0xB6,
    0x3D, 0xB3, 0x66, 0x63, 0x30, 0x1F, 0x06, 0x03, 0x55, 0x1D,
    0x11, 0x04, 0x18, 0x30, 0x16, 0x82, 0x07, 0x75, 0x73, 0x76,
    0x63, 0x2E, 0x69, 0x6F, 0x82, 0x0B, 0x77, 0x77, 0x77, 0x2E,
    0x75, 0x73, 0x76, 0x63, 0x2E, 0x69, 0x6F, 0x30, 0x1D, 0x06,
    0x03, 0x55, 0x1D, 0x0E, 0x04, 0x16, 0x04, 0x14, 0x6E, 0x3B,
    0x56, 0x2B, 0xE8, 0x79, 0x4E, 0x68, 0x8F, 0x7D, 0x5B, 0xF9,
    0x58, 0x1D, 0xD6, 0x86, 0x7A, 0x75, 0x35, 0x07, 0x30, 0x82,
    0x01, 0x7D, 0x06, 0x0A, 0x2B, 0x06, 0x01, 0x04, 0x01, 0xD6,
    0x79, 0x02, 0x04, 0x02, 0x04, 0x82, 0x01, 0x6D, 0x04, 0x82,
    0x01, 0x69, 0x01, 0x67, 0x00, 0x76, 0x00, 0xEE, 0xCD, 0xD0,
    0x64, 0xD5, 0xDB, 0x1A, 0xCE, 0xC5, 0x5C, 0xB7, 0x9D, 0xB4,
    0xCD, 0x13, 0xA2, 0x32, 0x87, 0x46, 0x7C, 0xBC, 0xEC, 0xDE,
    0xC3, 0x51, 0x48, 0x59, 0x46, 0x71, 0x1F, 0xB5, 0x9B, 0x00,
    0x00, 0x01, 0x86, 0x9D, 0x7F, 0x59, 0xEE, 0x00, 0x00, 0x04,
    0x03, 0x00, 0x47, 0x30, 0x45, 0x02, 0x21, 0x00, 0xBE, 0x7A,
    0xE8, 0xF7, 0x09, 0x69, 0x1C, 0xFD, 0xB8, 0x68, 0x28, 0x5F,
    0xF3, 0x35, 0xA2, 0xCD, 0x09, 0x11, 0x6C, 0x8F, 0xF4, 0xF8,
    0xFE, 0x54, 0x6D, 0x67, 0x75, 0xF1, 0x8F, 0x3C, 0x0F, 0x1C,
    0x02, 0x20, 0x61, 0x37, 0x1E, 0x94, 0xBB, 0x21, 0x52, 0x4E,
    0x59, 0xED, 0xF1, 0x94, 0xEA, 0x34, 0x6F, 0xEE, 0x0C, 0x80,
    0x62, 0x27, 0xE7, 0x3B, 0x1C, 0x9C, 0xE9, 0x56, 0x05, 0xAA,
    0x8C, 0x2D, 0x61, 0x3A, 0x00, 0x75, 0x00, 0x48, 0xB0, 0xE3,
    0x6B, 0xDA, 0xA6, 0x47, 0x34, 0x0F, 0xE5, 0x6A, 0x02, 0xFA,
    0x9D, 0x30, 0xEB, 0x1C, 0x52, 0x01, 0xCB, 0x56, 0xDD, 0x2C,
    0x81, 0xD9, 0xBB, 0xBF, 0xAB, 0x39, 0xD8, 0x84, 0x73, 0x00,
    0x00, 0x01, 0x86, 0x9D, 0x7F, 0x5B, 0x74, 0x00, 0x00, 0x04,
    0x03, 0x00, 0x46, 0x30, 0x44, 0x02, 0x20, 0x6D, 0x2D, 0x83,
    0xD9, 0xFE, 0xA8, 0x95, 0xBF, 0x9A, 0x3D, 0xFB, 0x3F, 0x0D,
    0x47, 0x88, 0x32, 0xB3, 0x78, 0xB5, 0x55, 0x8E, 0x9F, 0x7F,
    0x28, 0xF2, 0xA5, 0xEA, 0xB7, 0xEA, 0xD6, 0x2E, 0x25, 0x02,
    0x20, 0x41, 0x6B, 0xC7, 0xDF, 0x82, 0x8E, 0x32, 0xD7, 0xAF,
    0x22, 0x13, 0x5F, 0x7F, 0x7E, 0xB1, 0xC1, 0x97, 0x2D, 0x2F,
    0x64, 0x6E, 0x8C, 0x96, 0xAB, 0x2D, 0x14, 0x92, 0x48, 0x2E,
    0x65, 0x2E, 0xF3, 0x00, 0x76, 0x00, 0xDA, 0xB6, 0xBF, 0x6B,
    0x3F, 0xB5, 0xB6, 0x22, 0x9F, 0x9B, 0xC2, 0xBB, 0x5C, 0x6B,
    0xE8, 0x70, 0x91, 0x71, 0x6C, 0xBB, 0x51, 0x84, 0x85, 0x34,
    0xBD, 0xA4, 0x3D, 0x30, 0x48, 0xD7, 0xFB, 0xAB, 0x00, 0x00,
    0x01, 0x86, 0x9D, 0x7F, 0x5D, 0x12, 0x00, 0x00, 0x04, 0x03,
    0x00, 0x47, 0x30, 0x45, 0x02, 0x21, 0x00, 0xF9, 0x85, 0xD1,
    0x89, 0x61, 0x8E, 0x1E, 0xD7, 0x44, 0xA7, 0xF7, 0xDB, 0x5E,
    0x50, 0x15, 0xFC, 0x93, 0x0A, 0x9F, 0x24, 0x53, 0xDA, 0x63,
    0x83, 0xEC, 0x85, 0xBB, 0x9E, 0xEC, 0x05, 0x20, 0xDC, 0x02,
    0x20, 0x59, 0xF3, 0x83, 0xF3, 0x63, 0xB2, 0xE0, 0x59, 0xE1,
    0x32, 0x44, 0xE9, 0x7A, 0x90, 0xD2, 0x8E, 0x48, 0x95, 0x72,
    0xEC, 0xB0, 0x2E, 0xCB, 0x4E, 0x7E, 0xC8, 0xC1, 0x71, 0x28,
    0xE6, 0x0C, 0xB1, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48,
    0x86, 0xF7, 0x0D, 0x01, 0x01, 0x0B, 0x05, 0x00, 0x03, 0x82,
    0x01, 0x01, 0x00, 0x21, 0x8D, 0x7D, 0x6F, 0xC8, 0x91, 0x3B,
    0x6D, 0x7B, 0x1F, 0xEF, 0x76, 0x17, 0x69, 0xFC, 0xB7, 0xB6,
    0x13, 0xBD, 0x85, 0x9F, 0x50, 0x46, 0xEB, 0x29, 0x86, 0xDB,
    0x01, 0xE5, 0x1C, 0xCB, 0xDE, 0xB6, 0xE5, 0xF9, 0x12, 0x4B,
    0xE4, 0x71, 0x3E, 0x8B, 0x21, 0xE6, 0xA9, 0xDA, 0xEF, 0x8F,
    0xC2, 0x69, 0xCB, 0xD4, 0x12, 0x72, 0xE1, 0xFF, 0x84, 0xA1,
    0x69, 0xBA, 0x96, 0x37, 0x05, 0xEF, 0xD8, 0xC5, 0x5A, 0xFD,
    0xF1, 0xC1, 0x05, 0xE9, 0xEA, 0x62, 0xF1, 0x99, 0x2B, 0x33,
    0x59, 0x98, 0x23, 0x62, 0x87, 0xA6, 0xF3, 0x7E, 0x02, 0x8D,
    0xD6, 0x0B, 0xF8, 0x25, 0x6B, 0xCC, 0x7F, 0x66, 0x73, 0xBB,
    0x94, 0x07, 0xD2, 0x94, 0x6F, 0x50, 0x18, 0xC4, 0x9B, 0x10,
    0x99, 0xFF, 0x85, 0x00, 0x50, 0xF0, 0x76, 0xC4, 0xE6, 0x89,
    0x3D, 0xF4, 0x24, 0xD6, 0xE9, 0xDC, 0x9C, 0x04, 0x31, 0xB1,
    0xBD, 0x0E, 0x60, 0x9F, 0x02, 0xFF, 0x88, 0x7E, 0x1A, 0xE5,
    0x1B, 0x98, 0xF5, 0xD0, 0x2C, 0xAD, 0x13, 0x89, 0xBD, 0xBC,
    0x0E, 0xD5, 0x8A, 0xCF, 0xB8, 0xA9, 0xA6, 0x6C, 0x80, 0x35,
    0x6D, 0xE5, 0x37, 0xEE, 0xFB, 0x0F, 0x44, 0xBF, 0x6C, 0xF8,
    0xA6, 0x1F, 0x65, 0xDD, 0x74, 0x00, 0xAD, 0x5B, 0xCC, 0x32,
    0xD9, 0xAC, 0x5A, 0xCF, 0x20, 0x4A, 0xB5, 0x67, 0x61, 0xC3,
    0x22, 0x55, 0xC9, 0xCE, 0xFA, 0xCA, 0x06, 0xBC, 0xAD, 0x08,
    0xFA, 0x98, 0xCE, 0x24, 0x8C, 0x4E, 0x09, 0xB0, 0xC4, 0x98,
    0xE5, 0x3B, 0xD7, 0xFB, 0xAB, 0xBB, 0x8B, 0xE2, 0xED, 0x87,
    0xB9, 0xAD, 0xE4, 0x1C, 0xFA, 0x65, 0x27, 0xA9, 0x81, 0xDC,
    0x49, 0xE9, 0xDE, 0xCA, 0x5E, 0x44, 0x62, 0xEF, 0x51, 0x27,
    0xB4, 0xF3, 0xA6, 0x69, 0x62, 0x2C, 0x28, 0xEE, 0x11, 0x94,
    0xC7, 0x49, 0x06, 0x4C, 0xD4, 0x2D, 0x50, 0x4D, 0x3C
};

class Indentation 
{
public:
    bsl::size_t d_numSpaces;

    explicit Indentation(bsl::size_t numSpaces) : d_numSpaces(numSpaces) {}

    friend bsl::ostream& operator<<(bsl::ostream&      stream, 
                                    const Indentation& object)
    {
        for (bsl::size_t i = 0; i < object.d_numSpaces; ++i) {
            stream << "    ";
        }

        return stream;
    }
};

class TestUtil 
{
public:
    static void decodeCertificate(bsl::streambuf*   buffer, 
                                  bslma::Allocator* allocator);

    static void decodeConstructed(ntsa::AbstractSyntaxDecoder* decoder, 
                                  bslma::Allocator*            allocator);

    static void decodeSequence(ntsa::AbstractSyntaxDecoder* decoder, 
                               bslma::Allocator*            allocator);

    static void decodePrimitive(ntsa::AbstractSyntaxDecoder* decoder, 
                                bslma::Allocator*            allocator);
};

void TestUtil::decodeCertificate(bsl::streambuf*   buffer, 
                                 bslma::Allocator* allocator)
{
    ntsa::Error error;

    ntsa::AbstractSyntaxDecoder decoder(buffer, allocator);

    error = decoder.decodeTag();
    NTCCFG_TEST_OK(error);

    NTCCFG_TEST_LOG_DEBUG << test::Indentation(decoder.depth())
                          << "Context = " 
                          << decoder.current()
                          << NTCCFG_TEST_LOG_END;

    NTCCFG_TEST_EQ(decoder.current().tagClass(), 
                   ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

    NTCCFG_TEST_EQ(decoder.current().tagType(), 
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    NTCCFG_TEST_EQ(decoder.current().tagNumber(), 
                   ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);

    TestUtil::decodeConstructed(&decoder, allocator);

    error = decoder.decodeTagComplete();
    NTCCFG_TEST_OK(error);
}

void TestUtil::decodeConstructed(ntsa::AbstractSyntaxDecoder* decoder, 
                                 bslma::Allocator*            allocator)
{
    ntsa::Error error;

    NTCCFG_TEST_EQ(decoder->current().tagType(), 
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    if (decoder->current().tagClass() ==
        ntsa::AbstractSyntaxTagClass::e_UNIVERSAL &&
        decoder->current().tagNumber() == 
        ntsa::AbstractSyntaxTagNumber::e_SEQUENCE) 
    {
        test::TestUtil::decodeSequence(decoder, allocator);
    }
    else {
        error = decoder->skip();
        NTCCFG_TEST_OK(error);

        // MRM
        // NTCCFG_TEST_EQ(decoder->current().tagNumber(), 
        //                ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);
    }
}


void TestUtil::decodeSequence(ntsa::AbstractSyntaxDecoder* decoder, 
                              bslma::Allocator*            allocator)
{
    ntsa::Error error;

    NTCCFG_TEST_EQ(decoder->current().tagClass(), 
                   ntsa::AbstractSyntaxTagClass::e_UNIVERSAL);

    NTCCFG_TEST_EQ(decoder->current().tagType(), 
                   ntsa::AbstractSyntaxTagType::e_CONSTRUCTED);

    NTCCFG_TEST_EQ(decoder->current().tagNumber(), 
                   ntsa::AbstractSyntaxTagNumber::e_SEQUENCE);

    const bsl::uint64_t endPosition = 
                        decoder->current().contentPosition() + 
                        decoder->current().contentLength().value();

    while (true) {
        const bsl::uint64_t currentPosition = decoder->position();

        if (currentPosition >= endPosition) {
            break;
        }

        error = decoder->decodeTag();
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_DEBUG << test::Indentation(decoder->depth()) 
                              << "Context = " 
                              << decoder->current()
                              << NTCCFG_TEST_LOG_END;

        if (decoder->current().tagType() == 
            ntsa::AbstractSyntaxTagType::e_CONSTRUCTED) 
        {
            test::TestUtil::decodeConstructed(decoder, allocator);
        }
        else {
            test::TestUtil::decodePrimitive(decoder, allocator);
        }

        error = decoder->decodeTagComplete();
        NTCCFG_TEST_OK(error);

    }
}

void TestUtil::decodePrimitive(ntsa::AbstractSyntaxDecoder* decoder, 
                              bslma::Allocator*            allocator)
{
    ntsa::Error error;

    if (decoder->current().tagClass() ==
        ntsa::AbstractSyntaxTagClass::e_UNIVERSAL &&
        decoder->current().tagType() ==
        ntsa::AbstractSyntaxTagType::e_PRIMITIVE)
    {

    }

    decoder->skip();

    // TODO
}

// clang-format on

}  // close namespace 'test'

NTCCFG_TEST_CASE(1)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;

        NTCCFG_TEST_LOG_DEBUG
            << "Decoding: "
            << bdlb::PrintStringSingleLineHexDumper(
                   (const char*)test::k_USER_CERTIFICATE_ASN1,
                   sizeof test::k_USER_CERTIFICATE_ASN1)
            << NTCCFG_TEST_LOG_END;

        ntca::EncryptionCertificate certificate(&ta);

        bdlsb::FixedMemInStreamBuf isb(
            reinterpret_cast<const char*>(test::k_USER_CERTIFICATE_ASN1),
            sizeof test::k_USER_CERTIFICATE_ASN1);

        test::TestUtil::decodeCertificate(&isb, &ta);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        bdlsb::FixedMemInStreamBuf isb(
            reinterpret_cast<const char*>(test::k_USER_CERTIFICATE_ASN1),
            sizeof test::k_USER_CERTIFICATE_ASN1);

        ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

        ntca::EncryptionCertificate certificate(&ta);
        error = certificate.decode(&decoder);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_DEBUG << "Certificate = " << certificate
                              << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);

        ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

        error = certificate.encode(&encoder);
        NTCCFG_TEST_OK(error);

        rc = osb.pubsync();
        NTCCFG_TEST_EQ(rc, 0);

        NTCCFG_TEST_EQ(osb.length(), sizeof test::k_USER_CERTIFICATE_ASN1);

        rc = bsl::memcmp(osb.data(), 
                         test::k_USER_CERTIFICATE_ASN1, 
                         sizeof test::k_USER_CERTIFICATE_ASN1);
        NTCCFG_TEST_EQ(rc, 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        NTCCFG_TEST_LOG_DEBUG << "Decoding: "
                              << bdlb::PrintStringSingleLineHexDumper(
                                     (const char*)test::k_CA_CERTIFICATE_ASN1,
                                     sizeof test::k_CA_CERTIFICATE_ASN1)
                              << NTCCFG_TEST_LOG_END;

        bdlsb::FixedMemInStreamBuf isb(
            reinterpret_cast<const char*>(test::k_CA_CERTIFICATE_ASN1),
            sizeof test::k_CA_CERTIFICATE_ASN1);

        ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

        ntca::EncryptionCertificate certificate(&ta);
        error = certificate.decode(&decoder);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_DEBUG << "Certificate = " << certificate
                              << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);

        ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

        error = certificate.encode(&encoder);
        NTCCFG_TEST_OK(error);

        rc = osb.pubsync();
        NTCCFG_TEST_EQ(rc, 0);

        NTCCFG_TEST_EQ(osb.length(), sizeof test::k_CA_CERTIFICATE_ASN1);

        rc = bsl::memcmp(osb.data(), 
                         test::k_CA_CERTIFICATE_ASN1, 
                         sizeof test::k_CA_CERTIFICATE_ASN1);
        NTCCFG_TEST_EQ(rc, 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern:
    // Plan:

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        NTCCFG_TEST_LOG_DEBUG
            << "Decoding: "
            << bdlb::PrintStringSingleLineHexDumper(
                   (const char*)test::k_REAL_CERTIFICATE_ASN1,
                   sizeof test::k_REAL_CERTIFICATE_ASN1)
            << NTCCFG_TEST_LOG_END;

        bdlsb::FixedMemInStreamBuf isb(
            reinterpret_cast<const char*>(test::k_REAL_CERTIFICATE_ASN1),
            sizeof test::k_REAL_CERTIFICATE_ASN1);

        ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

        ntca::EncryptionCertificate certificate(&ta);
        error = certificate.decode(&decoder);
        NTCCFG_TEST_OK(error);

        NTCCFG_TEST_LOG_DEBUG << "Certificate = " << certificate
                              << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);

        ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

        error = certificate.encode(&encoder);
        NTCCFG_TEST_OK(error);

        rc = osb.pubsync();
        NTCCFG_TEST_EQ(rc, 0);

        NTCCFG_TEST_EQ(osb.length(), sizeof test::k_REAL_CERTIFICATE_ASN1);

        rc = bsl::memcmp(osb.data(), 
                         test::k_REAL_CERTIFICATE_ASN1, 
                         sizeof test::k_REAL_CERTIFICATE_ASN1);

        if (rc != 0) {
            for (bsl::size_t i = 0; i < sizeof test::k_REAL_CERTIFICATE_ASN1; ++i)
            {
                bsl::uint8_t e = test::k_REAL_CERTIFICATE_ASN1[i];
                bsl::uint8_t f = static_cast<bsl::uint8_t>(osb.data()[i]);

                if (f != e) {
                    NTCCFG_TEST_LOG_ERROR << "Mismatch at byte " << i << ":"
                                          << "\nE: " << bsl::hex << static_cast<bsl::uint32_t>(e)
                                          << "\nF: " << bsl::hex << static_cast<bsl::uint32_t>(f)
                                          << NTCCFG_TEST_LOG_END;
                }
            }
        }

        NTCCFG_TEST_EQ(rc, 0);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
}
NTCCFG_TEST_DRIVER_END;
