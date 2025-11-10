#pragma once

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

namespace Sanctuary::Core::Cryptography {

class RC4 {
public:
    explicit RC4(std::string_view base64Key);

    void apply(std::span<std::byte> data) noexcept;

private:
    static constexpr size_t STATE_LENGTH = 256;

    int x_{0};
    int y_{0};
    std::array<std::byte, STATE_LENGTH> engineState_{};

    static std::vector<std::byte> base64Decode(std::string_view encoded);
};

} // namespace Sanctuary::Core::Cryptography
