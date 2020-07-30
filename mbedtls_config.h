/**
 * \file config-mini-tls1_1.h
 *
 * \brief Minimal configuration for TLS 1.1 (RFC 4346)
 */
/*
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
/*
 * Minimal configuration for TLS 1.1 (RFC 4346), implementing only the
 * required ciphersuite: MBEDTLS_TLS_RSA_WITH_3DES_EDE_CBC_SHA
 *
 * See README.txt for usage instructions.
 */

#ifndef MBEDTLS_CONFIG_H
#define MBEDTLS_CONFIG_H

#define CHAR_BIT 8

#define MBEDTLS_HAVE_ASM
//#define MBEDTLS_HAVE_TIME

#define MBEDTLS_REMOVE_ARC4_CIPHERSUITES
#define MBEDTLS_ECP_DP_SECP256R1_ENABLED
//#define MBEDTLS_ECDSA_DETERMINISTIC
#define MBEDTLS_NO_PLATFORM_ENTROPY
#define MBEDTLS_ECP_NO_INTERNAL_RNG
//#define MBEDTLS_AES_FEWER_TABLES

//#define MBEDTLS_AES_C
#define MBEDTLS_ASN1_PARSE_C
#define MBEDTLS_ASN1_WRITE_C
#define MBEDTLS_BASE64_C
#define MBEDTLS_BIGNUM_C
//#define MBEDTLS_CIPHER_C
//#define MBEDTLS_CMAC_C
#define MBEDTLS_ECDSA_C
//#define MBEDTLS_ECDSA_C
#define MBEDTLS_ECP_C
//#define MBEDTLS_ERROR_C
//#define MBEDTLS_HMAC_DRBG_C
#define MBEDTLS_MD_C
#define MBEDTLS_OID_C
#define MBEDTLS_PEM_PARSE_C
#define MBEDTLS_PK_C
#define MBEDTLS_PK_PARSE_C
//#define MBEDTLS_PLATFORM_C
#define MBEDTLS_SHA256_C
#define MBEDTLS_SHA256_SMALLER

#undef MBEDTLS_GCM_C
#undef MBEDTLS_CHACHA20_C
#undef MBEDTLS_CHACHAPOLY_C
#undef MBEDTLS_POLY1305_C
#undef MBEDTLS_USE_PSA_CRYPTO


#endif /* MBEDTLS_CONFIG_H */
