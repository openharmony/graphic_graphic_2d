/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RS_PROFILER_MESSAGE_HELPER_H
#define RS_PROFILER_MESSAGE_HELPER_H

#include "rs_profiler_utils.h"

#include <bitset>
#include <iterator>
#include <type_traits>
#include <vector>

#ifndef RENDER_PROFILER_APPLICATION
#include <securec.h>
#endif

namespace OHOS::Rosen {

enum class PackageID {
    RS_PROFILER_HEADER,
    RS_PROFILER_BINARY,
    RS_PROFILER_RS_METRICS,
    RS_PROFILER_GFX_METRICS,
    RS_PROFILER_PREPARE,
    RS_PROFILER_PREPARE_DONE,
    RS_PROFILER_SKP_BINARY,
    RS_PROFILER_RDC_BINARY,
    RS_PROFILER_DCL_BINARY,
    RS_PROFILER_RSTREE_DUMP_JSON,
    RS_PROFILER_RSTREE_PERF_NODE_LIST,
    RS_PROFILER_RSTREE_SINGLE_NODE_PERF,
    RS_PROFILER_MSKP_FILEPATH,
    RS_PROFILER_BETAREC_FILEPATH,
    RS_PROFILER_RENDER_METRICS,
    RS_PROFILER_RS_EVENT,
};

template<typename T, typename = void>
struct HasContiguousLayout : std::false_type {};

template<typename T>
struct HasContiguousLayout<T, std::void_t<decltype(std::declval<T>().data())>> : std::true_type {};

class Packet {
public:
    enum PacketType : uint8_t {
        BINARY,
        COMMAND,
        LOG,
        UNKNOWN,
    };

    enum class Severity {
        LOG_CRITICAL,
        LOG_ERROR,
        LOG_INFO,
        LOG_DEBUG,
        LOG_TRACE,
    };

    static constexpr size_t HEADER_SIZE = sizeof(uint32_t) + sizeof(uint8_t);

    explicit Packet(PacketType type, uint32_t reserve = DEFAULT_RESERVED_SIZE);
    Packet(const Packet&) = default;
    Packet& operator=(const Packet&) = default;
    Packet(Packet&&) = default;
    Packet& operator=(Packet&&) = default;

    bool IsBinary() const;

    bool IsCommand() const;

    char* Begin();

    char* End();

    PacketType GetType() const;
    void SetType(PacketType type);
    uint32_t GetLength() const;
    uint32_t GetPayloadLength() const;

    std::vector<char> Release();

    template<typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
    [[maybe_unused]] bool Read(T& value);

    template<typename T>
    [[maybe_unused]] bool Read(T& value, size_t size);

    [[maybe_unused]] bool Read(void* value, size_t size);

    template<typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
    T Read();

    template<typename T>
    T Read(size_t size);

    template<typename T>
    [[maybe_unused]] bool Write(const T& value);

    [[maybe_unused]] bool Write(const void* value, size_t size);

private:
    void SetLength(uint32_t length);

    void InitData(PacketType type);

private:
    static constexpr size_t HEADER_TYPE_OFFSET = 0;
    static constexpr size_t HEADER_LENGTH_OFFSET = sizeof(uint8_t);
    static constexpr size_t DEFAULT_RESERVED_SIZE = 64;
    size_t readPointer_ = HEADER_SIZE;
    size_t writePointer_ = HEADER_SIZE;
    std::vector<char> data_ = { 0, 0, 0, 0, 0 };
};

template<typename T, typename>
[[maybe_unused]] inline bool Packet::Read(T& value)
{
    return Read(&value, sizeof(value));
}

template<typename T>
[[maybe_unused]] bool Packet::Read(T& value, size_t size)
{
    if constexpr (HasContiguousLayout<T>::value) {
        value.resize(size);
        return Read(value.data(), size * sizeof(typename T::value_type));
    } else {
        bool res = true;
        for (size_t i = 0; i < size; ++i) {
            typename T::value_type v {};
            res = res && Read(v);
            value.emplace(std::move(v));
        }
        return res;
    }
    return false;
}

template<typename T, typename>
inline T Packet::Read()
{
    T v {};
    Read(v);
    return v;
}

template<typename T>
inline T Packet::Read(size_t size)
{
    T v {};
    Read(v, size);
    return v;
}

template<typename T>
[[maybe_unused]] bool Packet::Write(const T& value)
{
    if constexpr (std::is_trivially_copyable_v<T>) {
        return Write(&value, sizeof(value));
    } else if constexpr (HasContiguousLayout<T>::value) {
        return Write(value.data(), value.size() * sizeof(typename T::value_type));
    } else {
        bool res = true;
        for (auto it = value.cbegin(); it != value.cend(); ++it) {
            res = res && Write(*it);
        }
        return res;
    }
    return false;
}

} // namespace OHOS::Rosen

#endif // RS_PROFILER_MESSAGE_HELPER_H
