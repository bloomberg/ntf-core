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

#include <ntca_encryptionkey.h>

#include <ntccfg_test.h>
#include <bdlsb_fixedmeminstreambuf.h>
#include <bdlsb_memoutstreambuf.h>
#include <bslma_allocator.h>
#include <bslma_default.h>
#include <bsls_assert.h>
#include <bsl_iomanip.h>
#include <bsl_iostream.h>

using namespace BloombergLP;

namespace test {

const bsl::uint8_t k_RSA[1190] = {
    0x30, 0x82, 0x04, 0xA2, 0x02, 0x01, 0x00, 0x02, 0x82, 0x01, 0x01, 0x00,
    0xB8, 0x57, 0x64, 0x2B, 0xB7, 0x9E, 0x49, 0x4D, 0x32, 0x9C, 0x89, 0x1C,
    0xFA, 0x79, 0x32, 0xCE, 0x78, 0xA1, 0xB8, 0xA0, 0x46, 0xF6, 0xB0, 0x72,
    0x2D, 0xCA, 0x10, 0x28, 0x0C, 0x44, 0x6C, 0x43, 0xAD, 0x1D, 0xF2, 0xDD,
    0x88, 0xF1, 0x83, 0x32, 0x09, 0x1D, 0xCB, 0xA6, 0x14, 0xDF, 0xB4, 0x41,
    0xF7, 0x50, 0x6A, 0xF7, 0x81, 0xC3, 0xAF, 0xC8, 0x7E, 0x42, 0xF0, 0x3F,
    0x30, 0xD9, 0x2F, 0xEF, 0x1A, 0x67, 0xBB, 0x46, 0x84, 0x97, 0x1E, 0xA5,
    0x0C, 0xF8, 0x2D, 0xED, 0x39, 0xE5, 0x53, 0x19, 0x6B, 0x95, 0x0E, 0x46,
    0x82, 0xF5, 0xC5, 0x56, 0x88, 0xB8, 0x51, 0x5C, 0x60, 0x60, 0x2E, 0x20,
    0x66, 0x2F, 0x56, 0x51, 0xC8, 0xEE, 0x20, 0xA4, 0x8A, 0x0A, 0x4F, 0x54,
    0x0D, 0xDA, 0xF7, 0x63, 0x69, 0xE0, 0xE2, 0x30, 0xDD, 0xBC, 0x18, 0x88,
    0x9C, 0x01, 0x15, 0x8D, 0x90, 0xE7, 0x10, 0xF1, 0xF7, 0x09, 0xBF, 0x9E,
    0xD0, 0x07, 0xCB, 0xFB, 0x92, 0xE3, 0x40, 0x26, 0xA4, 0xCB, 0x68, 0xCD,
    0x39, 0x48, 0xC0, 0xA5, 0x31, 0xB2, 0x1D, 0x9A, 0xBC, 0xBB, 0x87, 0x0D,
    0xD7, 0x02, 0x6E, 0x16, 0x80, 0xD1, 0x01, 0x1B, 0x60, 0xDC, 0x67, 0x7F,
    0x4B, 0xA2, 0x51, 0x2C, 0xDB, 0xE7, 0x4D, 0x95, 0xFC, 0x25, 0x37, 0x19,
    0x1A, 0x51, 0x2A, 0x4E, 0x64, 0x1C, 0x1C, 0x7A, 0xA7, 0x54, 0x4B, 0x6F,
    0xEB, 0xD1, 0x12, 0xE9, 0x0E, 0x3B, 0xD8, 0xF3, 0x98, 0xF1, 0xCF, 0xDB,
    0x78, 0x95, 0x7E, 0x12, 0x9E, 0xCD, 0xB6, 0xDF, 0x23, 0xC5, 0xA4, 0x73,
    0x4D, 0x3B, 0x27, 0xB1, 0xF0, 0xF4, 0x23, 0x69, 0x95, 0x1C, 0x8C, 0x82,
    0x03, 0x36, 0x21, 0x5C, 0x76, 0xA2, 0xB2, 0xD9, 0x2C, 0xD1, 0x77, 0xDE,
    0xFE, 0x37, 0x2B, 0xD2, 0x6A, 0x35, 0x0D, 0xF2, 0x87, 0x06, 0xC6, 0xFE,
    0x20, 0xDD, 0x06, 0x49, 0x02, 0x03, 0x01, 0x00, 0x01, 0x02, 0x82, 0x01,
    0x00, 0x74, 0x88, 0x9A, 0x84, 0xA0, 0x81, 0x69, 0xE9, 0x69, 0x73, 0x57,
    0xA2, 0x80, 0x1D, 0x1A, 0x27, 0x38, 0x4F, 0x53, 0xFD, 0xCE, 0x9E, 0xA8,
    0x90, 0xA5, 0xAD, 0xB5, 0x64, 0xFB, 0xCC, 0x4D, 0x7C, 0xC2, 0xF5, 0xC9,
    0xD0, 0x8E, 0x7F, 0xCE, 0xE6, 0x54, 0xA9, 0x07, 0x52, 0xA8, 0xA3, 0x7A,
    0xBA, 0xB0, 0xF3, 0x9F, 0x8B, 0x73, 0xF7, 0x17, 0x2A, 0xF1, 0xD6, 0xB3,
    0x35, 0xDF, 0x55, 0x8D, 0x5D, 0xE4, 0x1D, 0x6B, 0x60, 0x7A, 0xF5, 0x65,
    0xA4, 0x05, 0x7C, 0xE6, 0xDB, 0xAE, 0xDB, 0xF0, 0x1A, 0x83, 0xCE, 0x28,
    0xC6, 0x1E, 0x87, 0xEC, 0x06, 0x76, 0x4B, 0xBE, 0xF3, 0x5C, 0xE1, 0xB0,
    0x87, 0x3E, 0xD1, 0xF2, 0x09, 0xA5, 0xB2, 0x13, 0xE4, 0xB0, 0x77, 0x30,
    0x0A, 0xFC, 0xEF, 0x4D, 0xB1, 0x04, 0xFF, 0xEB, 0x3D, 0xE5, 0x0E, 0xC1,
    0x9B, 0xAA, 0xB3, 0x6C, 0xC2, 0x9F, 0xB5, 0xD3, 0x19, 0x70, 0x7B, 0xAD,
    0x5D, 0xCB, 0xE5, 0xF5, 0x8C, 0x06, 0xA3, 0x54, 0xB4, 0x94, 0xA4, 0xE2,
    0x02, 0xBC, 0x3D, 0xCF, 0x5A, 0x22, 0xCF, 0xDB, 0xE3, 0x2C, 0xA5, 0xED,
    0x9D, 0x59, 0xE2, 0xC2, 0xBE, 0x0D, 0x32, 0xEB, 0x55, 0xE4, 0x68, 0xC2,
    0xF5, 0x3A, 0x7D, 0x94, 0xE5, 0xEA, 0x52, 0x12, 0x47, 0x7E, 0x3A, 0x17,
    0x8D, 0x2B, 0x6C, 0xC6, 0xF2, 0xB7, 0x4B, 0xF4, 0xDF, 0x23, 0x35, 0xBC,
    0x34, 0xCD, 0xA5, 0x86, 0xD1, 0x25, 0xD6, 0x32, 0xA7, 0xD4, 0x44, 0x66,
    0xA2, 0xF5, 0x81, 0xB7, 0xFE, 0x64, 0x22, 0x49, 0xDB, 0xD5, 0xB3, 0x40,
    0x86, 0xA4, 0xFE, 0x52, 0x6B, 0xD5, 0xB9, 0x2A, 0x04, 0x14, 0xC4, 0xEA,
    0x4E, 0x55, 0xE2, 0x4F, 0x74, 0xB9, 0xCE, 0xA8, 0xD0, 0x6A, 0xDE, 0x1D,
    0xAC, 0x1A, 0xCB, 0x03, 0x17, 0x58, 0x62, 0xA5, 0xF9, 0x2D, 0xA2, 0x2C,
    0xB2, 0x14, 0x21, 0x49, 0xB9, 0x02, 0x81, 0x81, 0x00, 0xE5, 0x19, 0x2C,
    0xE5, 0xD5, 0x9D, 0x8D, 0x1D, 0x10, 0x52, 0xFC, 0xD8, 0x63, 0x56, 0xCF,
    0x61, 0x5E, 0x8E, 0x7E, 0x7D, 0x46, 0xD1, 0x0B, 0x2C, 0x9C, 0x02, 0x12,
    0xFF, 0x10, 0x74, 0xA2, 0xE5, 0x1F, 0x2D, 0x5C, 0xFD, 0xB8, 0x51, 0x06,
    0x5F, 0x10, 0x79, 0xC8, 0x33, 0x18, 0x8E, 0x6C, 0x9D, 0x49, 0x9F, 0x0F,
    0x6F, 0xA8, 0x04, 0x16, 0x37, 0x8A, 0x0A, 0x52, 0x4F, 0xC8, 0xAA, 0xF9,
    0x53, 0xA7, 0xE3, 0x1F, 0x1C, 0xBB, 0x5C, 0x41, 0x69, 0x60, 0xF2, 0xFB,
    0x4B, 0xF5, 0x3D, 0x5F, 0xEF, 0x4E, 0x7E, 0x2D, 0xBE, 0x45, 0x2B, 0x25,
    0x66, 0x81, 0x82, 0x8F, 0xE5, 0x47, 0xF6, 0x2F, 0x16, 0xBC, 0x11, 0xD4,
    0x96, 0x9E, 0xAE, 0xB2, 0x64, 0x88, 0x14, 0xAB, 0xEF, 0xA9, 0x07, 0x8B,
    0x4E, 0xAE, 0xF3, 0x0D, 0x01, 0xC1, 0x93, 0x64, 0xCC, 0x81, 0xCB, 0x06,
    0x44, 0xD4, 0xCD, 0xF0, 0xD7, 0x02, 0x81, 0x81, 0x00, 0xCD, 0xFC, 0xCB,
    0xDB, 0x95, 0xEE, 0x9A, 0xF3, 0x3E, 0xF4, 0xB0, 0x8D, 0x7B, 0xF6, 0x64,
    0xB4, 0x1F, 0xA2, 0x2C, 0xC0, 0x7A, 0x78, 0x15, 0xB6, 0x09, 0x6F, 0x4D,
    0x74, 0x86, 0x11, 0x71, 0x12, 0x69, 0x81, 0x61, 0x80, 0x0B, 0x25, 0x7D,
    0x70, 0x47, 0x4D, 0x2B, 0x24, 0x99, 0xF5, 0xAD, 0x05, 0x33, 0x62, 0xB0,
    0x33, 0x25, 0xD7, 0x5D, 0x47, 0x12, 0x6E, 0x59, 0xFF, 0x6A, 0xF8, 0x90,
    0xAA, 0x0D, 0x44, 0x65, 0x21, 0x87, 0x89, 0x24, 0xB1, 0xEE, 0xA9, 0x15,
    0x65, 0x48, 0xC3, 0x1E, 0x39, 0x49, 0x29, 0xD7, 0xDC, 0x97, 0xDE, 0xEF,
    0x40, 0x2B, 0xF9, 0x1B, 0x3A, 0x56, 0x94, 0xD6, 0x52, 0x95, 0x5C, 0xBB,
    0xAE, 0x57, 0x8F, 0xB1, 0x5C, 0xD7, 0x50, 0x15, 0x79, 0xCF, 0xA0, 0xB1,
    0x9D, 0x1F, 0x48, 0xE1, 0x05, 0x9A, 0xBD, 0xAC, 0xAF, 0x1C, 0xC1, 0x4B,
    0x1F, 0x25, 0xF3, 0x3D, 0xDF, 0x02, 0x81, 0x80, 0x10, 0x6A, 0x75, 0x65,
    0x22, 0xA1, 0x5F, 0x73, 0x2D, 0xB0, 0x44, 0x9D, 0xDC, 0xC0, 0x63, 0xAB,
    0xBE, 0x72, 0xC7, 0x6D, 0x82, 0x9D, 0x88, 0x07, 0x72, 0x46, 0x53, 0xC6,
    0x23, 0x0F, 0xB8, 0x7C, 0x62, 0x77, 0x85, 0xAA, 0x48, 0x82, 0xC5, 0xCA,
    0x1A, 0x0C, 0x1F, 0xE4, 0x49, 0x66, 0x2B, 0x32, 0x3A, 0x0C, 0x43, 0xB6,
    0xE4, 0x7B, 0x26, 0x7C, 0x64, 0x4F, 0xC9, 0x06, 0x1E, 0x82, 0xE1, 0xCA,
    0xB4, 0xC4, 0xF9, 0x7D, 0xBD, 0x8F, 0x26, 0xC0, 0xBE, 0x49, 0xAA, 0xE8,
    0x0E, 0x72, 0x8F, 0x65, 0x3E, 0x62, 0x9C, 0x34, 0x7E, 0x10, 0x51, 0x65,
    0xBE, 0xF1, 0xF4, 0x0D, 0xA1, 0xD2, 0x6A, 0x1D, 0x5E, 0xEC, 0x88, 0x27,
    0x22, 0xC7, 0xB1, 0xD0, 0xB2, 0x4F, 0x88, 0xE2, 0x6B, 0x62, 0x37, 0x01,
    0x34, 0x58, 0x06, 0x1E, 0x92, 0x18, 0x55, 0xA8, 0xB1, 0xC4, 0x32, 0x76,
    0xCC, 0x28, 0xD5, 0x91, 0x02, 0x81, 0x80, 0x65, 0xA6, 0x6B, 0x9F, 0x58,
    0x14, 0xE0, 0x68, 0x4B, 0x58, 0x0A, 0xCC, 0x37, 0x31, 0x37, 0x2E, 0xE0,
    0x31, 0x72, 0xAE, 0x25, 0x81, 0x9F, 0x0D, 0x2F, 0x08, 0xBB, 0x0B, 0x56,
    0xA1, 0xC2, 0xCF, 0xD8, 0x3A, 0xDD, 0xB7, 0x87, 0x48, 0x9F, 0xE7, 0x49,
    0x78, 0x5E, 0x62, 0xF8, 0x8B, 0x11, 0x2F, 0xF0, 0xD4, 0xB1, 0xA8, 0xFB,
    0xF2, 0x94, 0x78, 0x5F, 0x33, 0x0F, 0xBC, 0x88, 0xB8, 0x53, 0x03, 0x67,
    0xFA, 0xA9, 0x81, 0x6B, 0x56, 0x15, 0x42, 0xAD, 0x48, 0xC9, 0x63, 0x2B,
    0x0E, 0x06, 0x9B, 0x34, 0x6C, 0xAE, 0xB6, 0x90, 0x47, 0x96, 0x7F, 0x6F,
    0x73, 0xC5, 0xA8, 0xFB, 0x53, 0x05, 0x94, 0xC9, 0x5A, 0x72, 0xCF, 0xAD,
    0x2C, 0x1E, 0xD5, 0x76, 0xF7, 0x71, 0x31, 0xA4, 0x71, 0x63, 0x54, 0xEB,
    0x3C, 0x56, 0xB7, 0x22, 0x4E, 0x99, 0xA9, 0xEC, 0xA1, 0xC2, 0x85, 0x83,
    0x7F, 0xCE, 0xB7, 0x02, 0x81, 0x80, 0x67, 0xF3, 0x2D, 0xE5, 0xAA, 0x67,
    0x99, 0x93, 0x9B, 0x00, 0x28, 0x14, 0x65, 0xBB, 0x76, 0xF8, 0xD6, 0x33,
    0x1E, 0x74, 0x8E, 0x94, 0x27, 0x0F, 0xCF, 0x62, 0xCA, 0xDD, 0xB3, 0x3E,
    0xC1, 0xB7, 0x94, 0xFC, 0x2F, 0x33, 0xF6, 0x1F, 0x8B, 0xE3, 0x51, 0x2E,
    0x4B, 0x97, 0x84, 0xCC, 0x56, 0x90, 0x81, 0x89, 0xDE, 0x33, 0xC5, 0xB6,
    0xE3, 0x35, 0x4E, 0xCA, 0xE2, 0x31, 0xA1, 0x1D, 0xFF, 0xD8, 0x38, 0x26,
    0x02, 0xAA, 0xE0, 0x9F, 0xD9, 0x4F, 0xB3, 0x7F, 0x54, 0x6A, 0xEE, 0x1A,
    0xB0, 0x63, 0x0E, 0x57, 0x6A, 0x7C, 0x34, 0x6A, 0x07, 0xD6, 0xEB, 0xB9,
    0x8E, 0x2E, 0x48, 0x1D, 0xA4, 0x42, 0x96, 0x5F, 0x75, 0x94, 0x58, 0x16,
    0xBF, 0xFF, 0xA3, 0xAA, 0x6E, 0x33, 0x0D, 0x3C, 0x6C, 0xA3, 0xA4, 0x3C,
    0xC8, 0x01, 0xAB, 0xDB, 0xF3, 0x50, 0xAC, 0x64, 0x65, 0x98, 0xD5, 0x12,
    0x92, 0x46};

const unsigned char k_NIST_P256[121] = {
    0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0x1F, 0xEE, 0x58, 0x14,
    0xF4, 0x28, 0x25, 0xF3, 0xB3, 0x2F, 0xA8, 0x40, 0x9C, 0xEE, 0x2D,
    0xB5, 0xFB, 0xC5, 0xB2, 0xBF, 0x71, 0x03, 0xD5, 0x26, 0x3B, 0x05,
    0xC0, 0xFD, 0xB6, 0x3A, 0x10, 0x26, 0xA0, 0x0A, 0x06, 0x08, 0x2A,
    0x86, 0x48, 0xCE, 0x3D, 0x03, 0x01, 0x07, 0xA1, 0x44, 0x03, 0x42,
    0x00, 0x04, 0x77, 0xAA, 0xE6, 0x57, 0x1B, 0x27, 0x9E, 0xB9, 0x41,
    0x9B, 0x4E, 0x68, 0xEA, 0xD6, 0xEA, 0xB0, 0x7F, 0x15, 0xF8, 0x93,
    0x3D, 0xF4, 0xFC, 0x36, 0xA3, 0xE1, 0x36, 0xB8, 0x5E, 0x4B, 0xAB,
    0x0D, 0xF4, 0x9A, 0x55, 0xB0, 0x12, 0xB4, 0xBE, 0x66, 0x81, 0xBE,
    0x8B, 0x7F, 0xF6, 0x3A, 0x94, 0xF3, 0x40, 0xA4, 0x00, 0xA3, 0xC8,
    0xCE, 0x31, 0x8B, 0x34, 0xEA, 0xCE, 0x65, 0x17, 0x87, 0x9E, 0xF0};

const unsigned char k_NIST_P384[167] = {
    0x30, 0x81, 0xA4, 0x02, 0x01, 0x01, 0x04, 0x30, 0xA2, 0x4B, 0x87, 0xF2,
    0x2C, 0x79, 0xE5, 0xA7, 0x5B, 0x7B, 0xE4, 0x84, 0xC2, 0xD2, 0x08, 0xB8,
    0x4C, 0xD9, 0xE1, 0x74, 0xA8, 0x83, 0x7B, 0xAC, 0xE8, 0xD3, 0xF7, 0xF7,
    0xF7, 0x48, 0xAE, 0x16, 0x84, 0xF2, 0x5A, 0x79, 0xBE, 0xAA, 0x76, 0x6D,
    0x43, 0xA1, 0x33, 0x51, 0x76, 0xE3, 0x23, 0x26, 0xA0, 0x07, 0x06, 0x05,
    0x2B, 0x81, 0x04, 0x00, 0x22, 0xA1, 0x64, 0x03, 0x62, 0x00, 0x04, 0xC1,
    0x1C, 0x84, 0x3F, 0xE4, 0x50, 0xA5, 0xC8, 0xDD, 0x15, 0xD1, 0x57, 0x8A,
    0xBF, 0xA9, 0xD4, 0x09, 0xD3, 0x5E, 0xA3, 0xD6, 0x5D, 0x50, 0x35, 0x8A,
    0x66, 0x2C, 0xF6, 0x25, 0x79, 0x4C, 0x73, 0x91, 0x61, 0x22, 0x5B, 0x85,
    0x8C, 0xA6, 0x1E, 0xC4, 0xED, 0x89, 0xE7, 0x05, 0x64, 0x91, 0x99, 0x72,
    0xAB, 0x76, 0xAC, 0x7C, 0xF8, 0x00, 0x40, 0x26, 0xDB, 0xD4, 0xAB, 0x27,
    0x52, 0x5F, 0x64, 0x28, 0xC2, 0x84, 0xC3, 0x6F, 0x13, 0xF8, 0x54, 0xA5,
    0x10, 0xBB, 0x64, 0xAC, 0x00, 0x4A, 0xDF, 0xD7, 0xE8, 0x5D, 0x32, 0x2F,
    0x63, 0xAB, 0x3B, 0x3D, 0xB9, 0x67, 0x32, 0x73, 0x69, 0x4B, 0x25};

const unsigned char k_NIST_P521[223] = {
    0x30, 0x81, 0xDC, 0x02, 0x01, 0x01, 0x04, 0x42, 0x00, 0xB0, 0x37, 0x80,
    0xB6, 0x8E, 0xE9, 0x66, 0xAB, 0x78, 0x6A, 0x86, 0xBE, 0xEB, 0xC4, 0x04,
    0xA8, 0xEE, 0xD9, 0xA4, 0x35, 0x53, 0x1F, 0x91, 0x0B, 0x62, 0x60, 0x1E,
    0xE5, 0xE7, 0x59, 0xF1, 0x23, 0x7C, 0x57, 0x31, 0x32, 0xD9, 0xFA, 0x0A,
    0x4F, 0xA1, 0xC6, 0xA8, 0xEB, 0x9F, 0x8A, 0x4F, 0x06, 0x28, 0x4B, 0x63,
    0xB4, 0xB8, 0x6B, 0xB6, 0x1A, 0xF7, 0xD0, 0x9F, 0xCB, 0x99, 0x53, 0xD8,
    0x3D, 0x94, 0xA0, 0x07, 0x06, 0x05, 0x2B, 0x81, 0x04, 0x00, 0x23, 0xA1,
    0x81, 0x89, 0x03, 0x81, 0x86, 0x00, 0x04, 0x00, 0x6D, 0x2E, 0xE7, 0x00,
    0xBB, 0xCD, 0xEE, 0x60, 0xF6, 0x5E, 0x6C, 0x9A, 0x5A, 0xD7, 0x28, 0x10,
    0xED, 0xF1, 0x19, 0xB8, 0x98, 0x9D, 0xA9, 0x16, 0xC4, 0x9C, 0xF2, 0xAD,
    0xF9, 0xED, 0x54, 0xAE, 0x5B, 0x43, 0x59, 0x43, 0xD1, 0x39, 0xEC, 0x84,
    0x18, 0x09, 0x97, 0x51, 0x30, 0xD6, 0x06, 0x17, 0x91, 0x48, 0x3E, 0x33,
    0xBB, 0x92, 0x7F, 0x2D, 0x60, 0x4E, 0xCF, 0x5F, 0x46, 0x3A, 0x08, 0x4E,
    0xBB, 0x01, 0x89, 0xD2, 0xA3, 0x1F, 0x5C, 0xEB, 0x3D, 0xCD, 0xFC, 0x20,
    0x95, 0x77, 0x3F, 0x1D, 0x73, 0x22, 0x3B, 0x9D, 0x39, 0x6A, 0xCE, 0x54,
    0x9F, 0x24, 0xDD, 0x59, 0xEF, 0x0A, 0x83, 0xA4, 0x15, 0xFC, 0x9F, 0xE0,
    0x03, 0x42, 0x34, 0xAF, 0x5E, 0x31, 0xF7, 0x7E, 0x67, 0xEF, 0xE5, 0xC9,
    0xFF, 0x10, 0x81, 0x92, 0xB9, 0x18, 0xC2, 0xA0, 0x7A, 0xDE, 0x6F, 0xDF,
    0x9C, 0x8E, 0xED, 0x18, 0x93, 0xC1, 0xC1};

const unsigned char k_ED25519[48] = {
    0x30, 0x2E, 0x02, 0x01, 0x00, 0x30, 0x05, 0x06, 0x03, 0x2B, 0x65, 0x70,
    0x04, 0x22, 0x04, 0x20, 0xBF, 0x75, 0x92, 0x5C, 0xE9, 0xA8, 0x23, 0x34,
    0x61, 0x50, 0xC2, 0x4A, 0xE6, 0xAE, 0x57, 0x79, 0x9F, 0xBA, 0x5A, 0x6D,
    0x89, 0xD9, 0x6A, 0x3B, 0x12, 0x2F, 0x9C, 0xD8, 0x2D, 0x57, 0xA9, 0x37};

const unsigned char k_ED448[73] = {
    0x30, 0x47, 0x02, 0x01, 0x00, 0x30, 0x05, 0x06, 0x03, 0x2B, 0x65,
    0x71, 0x04, 0x3B, 0x04, 0x39, 0x2F, 0x2C, 0x5B, 0xFE, 0xCE, 0x77,
    0x92, 0xE3, 0x59, 0x85, 0xE5, 0x23, 0x9F, 0xEB, 0xD5, 0x44, 0x77,
    0x19, 0x97, 0xED, 0xE7, 0x00, 0x38, 0x22, 0xCE, 0x0E, 0x6F, 0xD7,
    0xDC, 0x14, 0x67, 0xA4, 0x26, 0x08, 0x21, 0xDD, 0x5B, 0xD5, 0x7A,
    0xB3, 0x1C, 0x6B, 0x57, 0xB7, 0xE7, 0x5E, 0xB8, 0xD0, 0x6D, 0x24,
    0xEE, 0x7D, 0x97, 0xB7, 0xB6, 0x83, 0x23};

}  // close namespace 'test'

NTCCFG_TEST_CASE(1)
{
    // Concern: Encode/decode RSA keys in the RSAPrivateKey ASN.1 format.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        NTCCFG_TEST_LOG_DEBUG
            << "Decoding: "
            << bdlb::PrintStringSingleLineHexDumper((const char*)test::k_RSA,
                                                    sizeof test::k_RSA)
            << NTCCFG_TEST_LOG_END;

        ntca::EncryptionKeyRsaValuePrivate rsa(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_RSA),
                sizeof test::k_RSA);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = rsa.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << rsa << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);
        {
            ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

            error = rsa.encode(&encoder);
            NTCCFG_TEST_OK(error);

            rc = osb.pubsync();
            NTCCFG_TEST_EQ(rc, 0);

            NTCCFG_TEST_EQ(osb.length(), sizeof test::k_RSA);

            rc = bsl::memcmp(osb.data(), test::k_RSA, sizeof test::k_RSA);
            NTCCFG_TEST_EQ(rc, 0);
        }

        ntca::EncryptionKey key(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_RSA),
                sizeof test::k_RSA);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = key.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << key << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_TRUE(key.isRsa());
        NTCCFG_TEST_EQ(key.rsa(), rsa);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(2)
{
    // Concern: Encode/decode elliptic curve keys using the NIST P-256 profile
    // in the ECPrivateKey ASN.1 format.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        NTCCFG_TEST_LOG_DEBUG << "Decoding: "
                              << bdlb::PrintStringSingleLineHexDumper(
                                     (const char*)test::k_NIST_P256,
                                     sizeof test::k_NIST_P256)
                              << NTCCFG_TEST_LOG_END;

        ntca::EncryptionKeyEllipticCurveValuePrivate ellipticCurve(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_NIST_P256),
                sizeof test::k_NIST_P256);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = ellipticCurve.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << ellipticCurve
                              << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);
        {
            ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

            error = ellipticCurve.encode(&encoder);
            NTCCFG_TEST_OK(error);

            rc = osb.pubsync();
            NTCCFG_TEST_EQ(rc, 0);

            NTCCFG_TEST_EQ(osb.length(), sizeof test::k_NIST_P256);

            rc = bsl::memcmp(osb.data(),
                             test::k_NIST_P256,
                             sizeof test::k_NIST_P256);

            if (rc != 0) {
                for (bsl::size_t i = 0; i < sizeof test::k_NIST_P256; ++i) {
                    bsl::uint8_t e = test::k_NIST_P256[i];
                    bsl::uint8_t f = static_cast<bsl::uint8_t>(osb.data()[i]);

                    if (f != e) {
                        NTCCFG_TEST_LOG_ERROR << "Mismatch at byte " << i
                                              << ":"
                                              << "\nE: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(e)
                                              << "\nF: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(f)
                                              << NTCCFG_TEST_LOG_END;
                    }
                }
            }

            NTCCFG_TEST_EQ(rc, 0);
        }

        ntca::EncryptionKey key(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_NIST_P256),
                sizeof test::k_NIST_P256);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = key.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << key << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_TRUE(key.isEllipticCurve());
        NTCCFG_TEST_EQ(key.ellipticCurve(), ellipticCurve);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(3)
{
    // Concern: Encode/decode elliptic curve keys using the NIST P-384 profile
    // in the ECPrivateKey ASN.1 format.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        NTCCFG_TEST_LOG_DEBUG << "Decoding: "
                              << bdlb::PrintStringSingleLineHexDumper(
                                     (const char*)test::k_NIST_P384,
                                     sizeof test::k_NIST_P384)
                              << NTCCFG_TEST_LOG_END;

        ntca::EncryptionKeyEllipticCurveValuePrivate ellipticCurve(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_NIST_P384),
                sizeof test::k_NIST_P384);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = ellipticCurve.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << ellipticCurve
                              << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);
        {
            ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

            error = ellipticCurve.encode(&encoder);
            NTCCFG_TEST_OK(error);

            rc = osb.pubsync();
            NTCCFG_TEST_EQ(rc, 0);

            NTCCFG_TEST_EQ(osb.length(), sizeof test::k_NIST_P384);

            rc = bsl::memcmp(osb.data(),
                             test::k_NIST_P384,
                             sizeof test::k_NIST_P384);

            if (rc != 0) {
                for (bsl::size_t i = 0; i < sizeof test::k_NIST_P384; ++i) {
                    bsl::uint8_t e = test::k_NIST_P384[i];
                    bsl::uint8_t f = static_cast<bsl::uint8_t>(osb.data()[i]);

                    if (f != e) {
                        NTCCFG_TEST_LOG_ERROR << "Mismatch at byte " << i
                                              << ":"
                                              << "\nE: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(e)
                                              << "\nF: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(f)
                                              << NTCCFG_TEST_LOG_END;
                    }
                }
            }

            NTCCFG_TEST_EQ(rc, 0);
        }

        ntca::EncryptionKey key(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_NIST_P384),
                sizeof test::k_NIST_P384);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = key.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << key << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_TRUE(key.isEllipticCurve());
        NTCCFG_TEST_EQ(key.ellipticCurve(), ellipticCurve);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(4)
{
    // Concern: Encode/decode elliptic curve keys using the NIST P-521 profile
    // in the ECPrivateKey ASN.1 format.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        NTCCFG_TEST_LOG_DEBUG << "Decoding: "
                              << bdlb::PrintStringSingleLineHexDumper(
                                     (const char*)test::k_NIST_P521,
                                     sizeof test::k_NIST_P521)
                              << NTCCFG_TEST_LOG_END;

        ntca::EncryptionKeyEllipticCurveValuePrivate ellipticCurve(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_NIST_P521),
                sizeof test::k_NIST_P521);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = ellipticCurve.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << ellipticCurve
                              << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);
        {
            ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

            error = ellipticCurve.encode(&encoder);
            NTCCFG_TEST_OK(error);

            rc = osb.pubsync();
            NTCCFG_TEST_EQ(rc, 0);

            NTCCFG_TEST_EQ(osb.length(), sizeof test::k_NIST_P521);

            rc = bsl::memcmp(osb.data(),
                             test::k_NIST_P521,
                             sizeof test::k_NIST_P521);

            if (rc != 0) {
                for (bsl::size_t i = 0; i < sizeof test::k_NIST_P521; ++i) {
                    bsl::uint8_t e = test::k_NIST_P521[i];
                    bsl::uint8_t f = static_cast<bsl::uint8_t>(osb.data()[i]);

                    if (f != e) {
                        NTCCFG_TEST_LOG_ERROR << "Mismatch at byte " << i
                                              << ":"
                                              << "\nE: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(e)
                                              << "\nF: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(f)
                                              << NTCCFG_TEST_LOG_END;
                    }
                }
            }

            NTCCFG_TEST_EQ(rc, 0);
        }

        ntca::EncryptionKey key(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_NIST_P521),
                sizeof test::k_NIST_P521);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = key.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << key << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_TRUE(key.isEllipticCurve());
        NTCCFG_TEST_EQ(key.ellipticCurve(), ellipticCurve);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(5)
{
    // Concern: Encode/decode Edwards Curve ED25519 in the PrivateKeyInfo ASN.1
    // format.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        NTCCFG_TEST_LOG_DEBUG << "Decoding: "
                              << bdlb::PrintStringSingleLineHexDumper(
                                     (const char*)test::k_ED25519,
                                     sizeof test::k_ED25519)
                              << NTCCFG_TEST_LOG_END;

        ntca::EncryptionKeyInfoPrivate privateKeyInfo(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_ED25519),
                sizeof test::k_ED25519);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = privateKeyInfo.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << privateKeyInfo
                              << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);
        {
            ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

            error = privateKeyInfo.encode(&encoder);
            NTCCFG_TEST_OK(error);

            rc = osb.pubsync();
            NTCCFG_TEST_EQ(rc, 0);

            NTCCFG_TEST_EQ(osb.length(), sizeof test::k_ED25519);

            rc = bsl::memcmp(osb.data(),
                             test::k_ED25519,
                             sizeof test::k_ED25519);

            if (rc != 0) {
                for (bsl::size_t i = 0; i < sizeof test::k_ED25519; ++i) {
                    bsl::uint8_t e = test::k_ED25519[i];
                    bsl::uint8_t f = static_cast<bsl::uint8_t>(osb.data()[i]);

                    if (f != e) {
                        NTCCFG_TEST_LOG_ERROR << "Mismatch at byte " << i
                                              << ":"
                                              << "\nE: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(e)
                                              << "\nF: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(f)
                                              << NTCCFG_TEST_LOG_END;
                    }
                }
            }

            NTCCFG_TEST_EQ(rc, 0);
        }

        ntca::EncryptionKey key(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_ED25519),
                sizeof test::k_ED25519);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = key.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << key << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_TRUE(key.isInfo());
        NTCCFG_TEST_EQ(key.info(), privateKeyInfo);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_CASE(6)
{
    // Concern: Encode/decode Edwards Curve ED448 in the PrivateKeyInfo ASN.1
    // format.

    ntccfg::TestAllocator ta;
    {
        ntsa::Error error;
        int         rc;

        NTCCFG_TEST_LOG_DEBUG
            << "Decoding: "
            << bdlb::PrintStringSingleLineHexDumper((const char*)test::k_ED448,
                                                    sizeof test::k_ED448)
            << NTCCFG_TEST_LOG_END;

        ntca::EncryptionKeyInfoPrivate privateKeyInfo(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_ED448),
                sizeof test::k_ED448);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = privateKeyInfo.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << privateKeyInfo
                              << NTCCFG_TEST_LOG_END;

        bdlsb::MemOutStreamBuf osb(&ta);
        {
            ntsa::AbstractSyntaxEncoder encoder(&osb, &ta);

            error = privateKeyInfo.encode(&encoder);
            NTCCFG_TEST_OK(error);

            rc = osb.pubsync();
            NTCCFG_TEST_EQ(rc, 0);

            NTCCFG_TEST_EQ(osb.length(), sizeof test::k_ED448);

            rc = bsl::memcmp(osb.data(), test::k_ED448, sizeof test::k_ED448);

            if (rc != 0) {
                for (bsl::size_t i = 0; i < sizeof test::k_ED448; ++i) {
                    bsl::uint8_t e = test::k_ED448[i];
                    bsl::uint8_t f = static_cast<bsl::uint8_t>(osb.data()[i]);

                    if (f != e) {
                        NTCCFG_TEST_LOG_ERROR << "Mismatch at byte " << i
                                              << ":"
                                              << "\nE: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(e)
                                              << "\nF: " << bsl::hex
                                              << static_cast<bsl::uint32_t>(f)
                                              << NTCCFG_TEST_LOG_END;
                    }
                }
            }

            NTCCFG_TEST_EQ(rc, 0);
        }

        ntca::EncryptionKey key(&ta);
        {
            bdlsb::FixedMemInStreamBuf isb(
                reinterpret_cast<const char*>(test::k_ED448),
                sizeof test::k_ED448);

            ntsa::AbstractSyntaxDecoder decoder(&isb, &ta);

            error = key.decode(&decoder);
            NTCCFG_TEST_OK(error);
        }

        NTCCFG_TEST_LOG_DEBUG << "Key = " << key << NTCCFG_TEST_LOG_END;

        NTCCFG_TEST_TRUE(key.isInfo());
        NTCCFG_TEST_EQ(key.info(), privateKeyInfo);
    }
    NTCCFG_TEST_ASSERT(ta.numBlocksInUse() == 0);
}

NTCCFG_TEST_DRIVER
{
    NTCCFG_TEST_REGISTER(1);
    NTCCFG_TEST_REGISTER(2);
    NTCCFG_TEST_REGISTER(3);
    NTCCFG_TEST_REGISTER(4);
    NTCCFG_TEST_REGISTER(5);
    NTCCFG_TEST_REGISTER(6);
}
NTCCFG_TEST_DRIVER_END;
