#pragma once

#include "xbasic/xns_macro.h"

#include <openssl/ossl_typ.h>

#include <memory>


NS_BEG1(std)

#define XOPENSSL_DECLARE_DEFAULT_DELETE(EVP_TYPE)   \
    template <>                                     \
    struct default_delete<EVP_TYPE>                 \
    {                                               \
        void                                        \
        operator()(EVP_TYPE * ptr) const noexcept;  \
    }

XOPENSSL_DECLARE_DEFAULT_DELETE(EVP_CIPHER_CTX);
XOPENSSL_DECLARE_DEFAULT_DELETE(EVP_PKEY_CTX);
XOPENSSL_DECLARE_DEFAULT_DELETE(EVP_PKEY);

NS_END1
