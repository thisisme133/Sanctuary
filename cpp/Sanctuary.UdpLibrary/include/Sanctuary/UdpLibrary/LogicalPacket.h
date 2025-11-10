#pragma once

#include <cstdint>
#include <span>
#include <memory>
#include <vector>

namespace Sanctuary::UdpLibrary {

// Base class for logical packets
class LogicalPacket {
public:
    virtual ~LogicalPacket() = default;

    virtual std::span<uint8_t> get_data_ptr() = 0;
    virtual std::span<const uint8_t> get_data_ptr() const = 0;
    virtual int get_data_len() const noexcept = 0;
    virtual void set_data_len(int len) noexcept = 0;

    virtual bool is_internal_packet() const noexcept {
        return false;
    }
};

// Simple non-pooled logical packet
class SimpleLogicalPacket : public LogicalPacket {
    std::vector<uint8_t> data_;
    int data_len_;

public:
    explicit SimpleLogicalPacket(int size) : data_(size), data_len_(0) {}

    std::span<uint8_t> get_data_ptr() override {
        return std::span{data_.data(), static_cast<size_t>(data_len_)};
    }

    std::span<const uint8_t> get_data_ptr() const override {
        return std::span{data_.data(), static_cast<size_t>(data_len_)};
    }

    int get_data_len() const noexcept override {
        return data_len_;
    }

    void set_data_len(int len) noexcept override {
        data_len_ = len;
    }

    void set_data(std::span<const uint8_t> d1, std::span<const uint8_t> d2 = {}) {
        data_len_ = d1.size() + d2.size();
        std::copy(d1.begin(), d1.end(), data_.begin());
        if (!d2.empty()) {
            std::copy(d2.begin(), d2.end(), data_.begin() + d1.size());
        }
    }
};

// Pooled logical packet
class PooledLogicalPacket : public LogicalPacket {
    std::vector<uint8_t> data_;
    int data_len_;

public:
    explicit PooledLogicalPacket(int size) : data_(size), data_len_(0) {}

    std::span<uint8_t> get_data_ptr() override {
        return std::span{data_.data(), static_cast<size_t>(data_len_)};
    }

    std::span<const uint8_t> get_data_ptr() const override {
        return std::span{data_.data(), static_cast<size_t>(data_len_)};
    }

    int get_data_len() const noexcept override {
        return data_len_;
    }

    void set_data_len(int len) noexcept override {
        data_len_ = len;
    }

    void set_data(std::span<const uint8_t> d1, std::span<const uint8_t> d2 = {}) {
        data_len_ = d1.size() + d2.size();
        std::copy(d1.begin(), d1.end(), data_.begin());
        if (!d2.empty()) {
            std::copy(d2.begin(), d2.end(), data_.begin() + d1.size());
        }
    }
};

// Fixed size logical packet
class FixedLogicalPacket : public LogicalPacket {
    uint8_t* data_;
    int capacity_;
    int data_len_;

public:
    FixedLogicalPacket(uint8_t* buffer, int capacity)
        : data_(buffer), capacity_(capacity), data_len_(0) {}

    std::span<uint8_t> get_data_ptr() override {
        return std::span{data_, static_cast<size_t>(data_len_)};
    }

    std::span<const uint8_t> get_data_ptr() const override {
        return std::span{data_, static_cast<size_t>(data_len_)};
    }

    int get_data_len() const noexcept override {
        return data_len_;
    }

    void set_data_len(int len) noexcept override {
        data_len_ = len;
    }
};

} // namespace Sanctuary::UdpLibrary
