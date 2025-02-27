/*
 * Copyright (C) 2020-2021 Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *   3. Neither the name of Intel Corporation nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef EHSM_RPOVIDER_H
#define EHSM_PROVIDER_H

#include <string>
#include <stdint.h>

#include "sgx_ukey_exchange.h"
#include "sgx_tkey_exchange.h"
#include "sample_ra_msg.h"

using namespace std;

#define _T(x) x

#define ENCLAVE_PATH "libenclave-ehsm-core.signed.so"

#define EH_ENCRYPT_MAX_SIZE (6*1024)

#define EH_AAD_MAX_SIZE (8*1024)

#define EH_QUOTE_MAX_SIZE (8*1024)

#define EH_CMK_MAX_SIZE (4*1024)

#define EH_DATA_KEY_MAX_SIZE 1024

#define EH_AES_GCM_IV_SIZE  12
#define EH_AES_GCM_MAC_SIZE 16

#define RSA_OAEP_2048_SHA_256_MAX_ENCRYPTION_SIZE       190
//#define RSA_2048_OAEP_SHA_1_MAX_ENCRYPTION_SIZE       214

#define RSA_OAEP_3072_SHA_256_MAX_ENCRYPTION_SIZE       318
//#define RSA_3072_OAEP_SHA_1_MAX_ENCRYPTION_SIZE       342

#define SM2PKE_MAX_ENCRYPTION_SIZE                      6047

#define RSA_OAEP_3072_CIPHER_LENGTH       384
#define RSA_OAEP_3072_SIGNATURE_SIZE      384
#define RSA_OAEP_3072_DIGEST_SIZE         256

typedef enum {
    EH_OK                           = 0,
    EH_KEYSPEC_INVALID              = -1,
    EH_DEVICE_MEMORY                = -2,
    EH_DEVICE_ERROR                 = -3,
    EH_GENERAL_ERROR                = -4,
    EH_FUNCTION_FAILED              = -5,
    EH_ARGUMENTS_BAD                = -6,
    EH_LA_SETUP_ERROR               = -7,
    EH_LA_EXCHANGE_MSG_ERROR        = -8,
    EH_LA_CLOSE_ERROR               = -9,
} ehsm_status_t;

typedef enum  {
    EH_INTERNAL_KEY,
    EXTERNAL_KEY,
} ehsm_keyorigin_t;

typedef enum {
    EH_AES_GCM_128 = 0,
    EH_AES_GCM_256 =1,
    EH_RSA_2048 = 2,
    EH_RSA_3072 = 3,
    EH_EC_P256 = 4,
    EH_EC_P512 = 5,
    EH_EC_SM2 =6,
    EH_SM4 =7,
} ehsm_keyspec_t;

typedef enum {
    ENCRYPT_DECRYPT = 0,
    SIGN_VERIFY = 1,
} ehsm_keypurpose_t;

#pragma pack(push,1)

typedef struct {
    uint32_t keyspec;
    uint32_t origin;
    uint32_t purpose;
    uint32_t apiversion;
    uint8_t  descrption[16];
    uint8_t  createdate[8];
} ehsm_keymetadata_t;

typedef struct {
    ehsm_keymetadata_t  metadata;
    uint32_t            keybloblen;
    uint8_t             *keyblob;
} ehsm_keyblob_t;

typedef struct {
    uint32_t    datalen;
    uint8_t     *data;
} ehsm_data_t;

#pragma pack(pop)

#ifndef SAFE_FREE
#define SAFE_FREE(ptr) {if (NULL != (ptr)) {free(ptr); (ptr) = NULL;}}
#endif


namespace EHsmProvider
{

ehsm_status_t Initialize();

void Finalize();

/*
Description:

Create a customer master key with the following metadatas
.keyspec;
    -EH_AES_GCM_128,
    -EH_AES_GCM_256,
    -EH_RSA_2048,
    -EH_RSA_3072,
    -EH_EC_P256,
    -EH_EC_P512,
    -EH_EC_SM2,
    -EH_SM4,
.origin;
    -EH_INTERNAL_KEY (generated from the eHSM inside)
    -EXTERNAL_KEY (generated by the customer and want to import into the eHSM),
.purpose;
    -ENCRYPT_DECRYPT,
    -SIGN_VERIFY,
.apiversion;
    -Reserved
.descrption;
    -Reserved
.createdate
    -Reserved

Note: the CMK will be wrapped by the DK(DomainKey)
*/
ehsm_status_t CreateKey(ehsm_keyblob_t *cmk);

/*
Description:
Encrypt an arbitrary set of bytes using the CMK.(only support symmetric types)

Input:
cmk -- A symmetric cmk,
plaintext -- the datas of the plaintext
aad -- some extra datas input by the user, which could help to to ensure data integrity, and not
be included in the cipherblobs

Output:
ciphertext -- the data of the ciphertext


Note: Only data of 6KB or less can be encrypted
*/
ehsm_status_t Encrypt(ehsm_keyblob_t *cmk,
        ehsm_data_t *plaintext,
        ehsm_data_t *aad,
        ehsm_data_t *ciphertext);

/*
Description:
Decrypts ciphertext using the CMK.(only support symmetric types)

Input:
cmk -- A symmetric cmk
ciphertext -- the data of the ciphertext
aad -- some extra datas input by the user, which could help to to ensure data integrity


Output:
plaintext -- the datas of the plaintext
*/
ehsm_status_t Decrypt(ehsm_keyblob_t *cmk,
        ehsm_data_t *ciphertext,
        ehsm_data_t *aad,
        ehsm_data_t *plaintext);

/*
Description:
Encrypt an arbitrary set of bytes using the CMK.(only support asymmetric types)

Input:
cmk -- An asymmetric cmk,
plaintext -- the datas of the plaintext

Output:
ciphertext -- the data of the ciphertext


Note:
the data size is limited decided by the keyspec:
RSA_OAEP_2048_SHA_256_MAX_ENCRYPTION_SIZE       190
RSA_2048_OAEP_SHA_1_MAX_ENCRYPTION_SIZE       214

RSA_OAEP_3072_SHA_256_MAX_ENCRYPTION_SIZE       318
RSA_3072_OAEP_SHA_1_MAX_ENCRYPTION_SIZE       342

SM2PKE_MAX_ENCRYPTION_SIZE                      6047
*/
ehsm_status_t AsymmetricEncrypt(ehsm_keyblob_t *cmk,
    ehsm_data_t *plaintext,
    ehsm_data_t *ciphertext);

/*
Description:
Decrypt an arbitrary set of bytes using the CMK.(only support asymmetric types)

Input:
cmk -- An asymmetric cmk,
ciphertext -- the data of the ciphertext

Output:
plaintext -- the datas of the plaintext


Note:
the data size is limited decided by the keyspec:
RSA_OAEP_2048_SHA_256_MAX_ENCRYPTION_SIZE       190
RSA_2048_OAEP_SHA_1_MAX_ENCRYPTION_SIZE       214

RSA_OAEP_3072_SHA_256_MAX_ENCRYPTION_SIZE       318
RSA_3072_OAEP_SHA_1_MAX_ENCRYPTION_SIZE       342

SM2PKE_MAX_ENCRYPTION_SIZE                      6047
*/

ehsm_status_t AsymmetricDecrypt(ehsm_keyblob_t *cmk,
        ehsm_data_t *ciphertext,
        ehsm_data_t *plaintext);
/*
Description:
Generates a random data key that is used to locally encrypt data.

the datakey will be wrapped by the specified CMK(only support asymmetric keyspec),
and it will return the plaintext and ciphertext of the data key.

You can use the plaintext of the data key to locally encrypt your data without using KMS
and store the encrypted data together with the ciphertext of the data key, then clear the
plaintext data from memory as soon as possible.

when you want to obtain the plaintext of datakey again, you can call the Decrypt with the
cmk to get the plaintext data.


Input:
cmk -- A symmetric cmk,
aad -- some extra datas input by the user, which could help to to ensure data integrity
plaintext -- the datas of the plaintext


Output:
ciphertext -- the data of the ciphertext
*/
ehsm_status_t GenerateDataKey(ehsm_keyblob_t *cmk,
        ehsm_data_t *aad,
        ehsm_data_t *plaintext,
        ehsm_data_t *ciphertext);


/*
Description:
The same as GenerateDataKey, but doesn’t return plaintext of generated DataKey.
*/
ehsm_status_t GenerateDataKeyWithoutPlaintext(ehsm_keyblob_t *cmk,
        ehsm_data_t *aad,
        ehsm_data_t *plaintext,
        ehsm_data_t *ciphertext);


/*
Description:
ehsm-core enclave will decrypt user-supplied ciphertextblob with specified CMK to get the
plaintext of DataKey, then use the user-supplied Public key to encrypt this DataKey
(aka ExportedDataKey). This ExportedDataKey (ciphertext) will be returned to caller.

Input:
cmk -- A symmetric cmk,
ukey -- An asymmetric key,
aad -- some extra datas input by the user, which could help to to ensure data integrity
olddatakey -- the ciphertext of the datakey wrapped by the cmk

Output:
newdatakey -- the ciphertext of the datakey wrapped by the ukey
*/
ehsm_status_t ExportDataKey(ehsm_keyblob_t *cmk,
        ehsm_keyblob_t *ukey,
        ehsm_data_t *aad,
        ehsm_data_t *olddatakey,
        ehsm_data_t *newdatakey);

/*
Description:
Performs sign operation using the cmk(only support asymmetric keyspec).

Input:
cmk -- An asymmetric cmk,
digest -- the hash of datas want to be signed.


Output:
signature -- the signature of the digest signed by the cmk
*/
ehsm_status_t Sign(ehsm_keyblob_t *cmk,
           ehsm_data_t *digest,
           ehsm_data_t *signature);

/*
Description:
Performs verify operation using the cmk(only support asymmetric keyspec).

Input:
cmk -- An asymmetric cmk,
digest -- the hash of datas want to be signed.
signature -- the signature of the digest signed by the cmk

Output:
result -- true/false
*/
ehsm_status_t Verify(ehsm_keyblob_t *cmk,
             ehsm_data_t *digest,
             ehsm_data_t *signature,
             bool* result);

/*
Description:
Performs quote generation and return the quote.

Input/Output:
quote -- the quote for the target encalve.
*/
ehsm_status_t GenerateQuote(ehsm_data_t *quote);

/*
Description:
Performs quote verification and return the result.

Input:
quote -- the quote need to be verified.

Output:
result -- verification result with type sgx_ql_qv_result_t.
*/
ehsm_status_t VerifyQuote(ehsm_data_t *quote,
            const char *mr_signer,
            const char *mr_enclave,
            sgx_ql_qv_result_t *result);

/*
Description:
Obtain a valid appid and apikey

Output:
appid -- an uuid
apikey -- 32bit random number
*/
ehsm_status_t Enroll(ehsm_data_t *appid, ehsm_data_t *apikey);

/*
Description:
Generate a 32-bit random character and use sharedKey encrypt character

Output:
apikey -- 32-bit random character
cipherkey -- encrypted character
*/
ehsm_status_t generate_apikey(ehsm_data_t *apikey, ehsm_data_t *cipherapikey);

/*
Description:
is used to get the remote attestation and key exchange

Output:
msg1 -- the msg1 has g_a
*/
ehsm_status_t ra_get_msg1(sgx_ra_msg1_t *msg1);

/*
Description:
is used to get the remote attestation and key exchange

Output:
msg3 -- the msg3 has ga || QUOTE(SHA256(ga|gb|VK))  || CMACSMK(ga || QUOTE(SHA256(ga|gb|VK)))
*/
ehsm_status_t ra_get_msg3(sgx_ra_msg2_t *p_msg2, uint32_t msg2_size, sgx_ra_msg3_t **p_msg3, uint32_t p_msg3_size);

ehsm_status_t verify_att_result_msg(sample_ra_att_result_msg_t *p_att_result_msg);
}
#endif
