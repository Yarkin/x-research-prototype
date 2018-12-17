#pragma once

#include "xbasic/xns_macro.h"

#include <cstdint>

NS_BEG2(top, openssl)

enum class xenum_crypto_key_type : std::uint8_t
{
    private_key = 0,
    public_key  = 1
};

using xcrypto_key_t = xenum_crypto_key_type;

NS_END2
