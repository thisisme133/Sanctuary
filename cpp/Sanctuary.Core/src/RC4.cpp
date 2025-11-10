#include "Sanctuary/Core/RC4.h"
#include <stdexcept>
#include <algorithm>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

namespace Sanctuary::Core::Cryptography {

std::vector<std::byte> RC4::base64Decode(std::string_view encoded) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bmem = BIO_new_mem_buf(encoded.data(), encoded.size());
    bmem = BIO_push(b64, bmem);
    BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);

    std::vector<std::byte> output(encoded.size());
    int decodedLength = BIO_read(bmem, output.data(), output.size());

    BIO_free_all(bmem);

    if (decodedLength < 0) {
        throw std::runtime_error("Base64 decode failed");
    }

    output.resize(decodedLength);
    return output;
}

RC4::RC4(std::string_view base64Key) {
    auto keyBytes = base64Decode(base64Key);

    // Initialize state
    for (size_t i = 0; i < STATE_LENGTH; ++i) {
        engineState_[i] = static_cast<std::byte>(i);
    }

    size_t i1 = 0;
    size_t i2 = 0;

    for (size_t i = 0; i < STATE_LENGTH; ++i) {
        i2 = ((std::to_integer<uint8_t>(keyBytes[i1]) & 0xff) +
              std::to_integer<uint8_t>(engineState_[i]) + i2) & 0xff;

        std::swap(engineState_[i], engineState_[i2]);

        i1 = (i1 + 1) % keyBytes.size();
    }
}

void RC4::apply(std::span<std::byte> data) noexcept {
    for (auto& byte : data) {
        x_ = (x_ + 1) & 0xff;
        y_ = (std::to_integer<uint8_t>(engineState_[x_]) + y_) & 0xff;

        auto sx = engineState_[x_];
        auto sy = engineState_[y_];

        engineState_[x_] = sy;
        engineState_[y_] = sx;

        byte ^= engineState_[(std::to_integer<uint8_t>(sx) + std::to_integer<uint8_t>(sy)) & 0xff];
    }
}

} // namespace Sanctuary::Core::Cryptography
