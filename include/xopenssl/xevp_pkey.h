#pragma once

#include "xopenssl/xcrypto_key_type.h"
#include "xopenssl/xevp_delete.h"

#include <openssl/evp.h>

#include <memory>
#include <string>


NS_BEG2(top, openssl)

class xtop_evp_pkey final
{
    std::unique_ptr<EVP_PKEY> m_evp_pkey{};

public:
    xtop_evp_pkey()                                  = default;
    xtop_evp_pkey(xtop_evp_pkey const &)             = delete;
    xtop_evp_pkey & operator=(xtop_evp_pkey const &) = delete;
    xtop_evp_pkey(xtop_evp_pkey &&)                  = default;
    xtop_evp_pkey & operator=(xtop_evp_pkey &&)      = default;
    ~xtop_evp_pkey()                                 = default;

    explicit
    xtop_evp_pkey(std::string const & evp_pkey_content,
                  xcrypto_key_t const key_type = xcrypto_key_t::public_key);

    explicit
    xtop_evp_pkey(std::unique_ptr<EVP_PKEY> && evp_pkey) noexcept;

    void
    swap(xtop_evp_pkey & other) noexcept;

    bool
    empty() const noexcept;

    EVP_PKEY *
    evp_pkey() const noexcept;

    std::int32_t
    size() const;
};

using xevp_pkey_t = xtop_evp_pkey;

NS_END2

NS_BEG1(std)

inline
void
swap(top::openssl::xevp_pkey_t & left,
     top::openssl::xevp_pkey_t & right) noexcept
{
    left.swap(right);
}

NS_END1
