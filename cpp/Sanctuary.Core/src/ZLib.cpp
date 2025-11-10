#include "Sanctuary/Core/ZLib.h"
#include <zlib.h>
#include <cstring>
#include <bit>

namespace Sanctuary::Core::IO {

int ZLib::compress(
    std::span<const std::byte> input,
    std::span<std::byte> output,
    bool prefixLength
) noexcept {
    if (output.size() < 12) {
        return -1;
    }

    size_t outputOffset = 0;

    if (prefixLength) {
        output[0] = static_cast<std::byte>(0xA1);
        output[1] = static_cast<std::byte>(0xB2);
        output[2] = static_cast<std::byte>(0xC3);
        output[3] = static_cast<std::byte>(0xD4);

        // Write input length in big-endian
        uint32_t length = static_cast<uint32_t>(input.size());
        if constexpr (std::endian::native == std::endian::little) {
            length = std::byteswap(length);
        }
        std::memcpy(output.data() + 4, &length, sizeof(length));

        outputOffset = 8;
    }

    z_stream stream{};
    stream.next_in = reinterpret_cast<const Bytef*>(input.data());
    stream.avail_in = static_cast<uInt>(input.size());
    stream.next_out = reinterpret_cast<Bytef*>(output.data() + outputOffset);
    stream.avail_out = static_cast<uInt>(output.size() - outputOffset);

    if (deflateInit(&stream, Z_DEFAULT_COMPRESSION) != Z_OK) {
        return -1;
    }

    int result = deflate(&stream, Z_FINISH);
    deflateEnd(&stream);

    if (result != Z_STREAM_END) {
        return -1;
    }

    return static_cast<int>(stream.total_out + outputOffset);
}

int ZLib::decompress(
    std::span<const std::byte> input,
    std::span<std::byte> output
) noexcept {
    if (input.empty()) {
        return -1;
    }

    int prefixedLength = getPrefixedLength(input);

    if (prefixedLength >= 0) {
        if (output.size() < static_cast<size_t>(prefixedLength)) {
            return -1;
        }
        input = input.subspan(8);
    }

    z_stream stream{};
    stream.next_in = reinterpret_cast<const Bytef*>(input.data());
    stream.avail_in = static_cast<uInt>(input.size());
    stream.next_out = reinterpret_cast<Bytef*>(output.data());
    stream.avail_out = static_cast<uInt>(output.size());

    if (inflateInit(&stream) != Z_OK) {
        return -1;
    }

    int result = inflate(&stream, Z_FINISH);
    inflateEnd(&stream);

    if (result != Z_STREAM_END) {
        return -1;
    }

    return static_cast<int>(stream.total_out);
}

int ZLib::getPrefixedLength(std::span<const std::byte> input) noexcept {
    if (input.size() <= 8) {
        return -1;
    }

    uint32_t magic;
    std::memcpy(&magic, input.data(), sizeof(magic));

    if constexpr (std::endian::native == std::endian::little) {
        magic = std::byteswap(magic);
    }

    if (magic != MAGIC) {
        return -1;
    }

    int32_t length;
    std::memcpy(&length, input.data() + 4, sizeof(length));

    if constexpr (std::endian::native == std::endian::little) {
        length = std::byteswap(length);
    }

    return length;
}

} // namespace Sanctuary::Core::IO
