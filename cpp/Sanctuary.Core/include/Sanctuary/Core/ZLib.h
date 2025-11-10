#pragma once

#include <span>
#include <optional>
#include <cstdint>
#include <array>

namespace Sanctuary::Core::IO {

class ZLib {
public:
    static constexpr uint32_t MAGIC = 0xA1B2C3D4;

    // Compress data with optional length prefix
    // Returns number of bytes written, or -1 on error
    [[nodiscard]] static int compress(
        std::span<const std::byte> input,
        std::span<std::byte> output,
        bool prefixLength = false
    ) noexcept;

    // Decompress data (handles prefixed length automatically)
    // Returns number of bytes written, or -1 on error
    [[nodiscard]] static int decompress(
        std::span<const std::byte> input,
        std::span<std::byte> output
    ) noexcept;

private:
    [[nodiscard]] static int getPrefixedLength(std::span<const std::byte> input) noexcept;
};

} // namespace Sanctuary::Core::IO
