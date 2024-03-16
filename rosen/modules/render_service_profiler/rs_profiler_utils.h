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

#ifndef RS_PROFILER_UTILS_H
#define RS_PROFILER_UTILS_H

#include <chrono>
#include <string>
#include <vector>

#ifdef REPLAY_TOOL_CLIENT
#include "rs_adapt.h"
#endif

namespace OHOS::Rosen {
class ArgList final {
public:
    explicit ArgList() = default;
    explicit ArgList(std::vector<std::string> args) : args_(std::move(args)) {}

    size_t Count() const
    {
        return args_.size();
    }

    bool Empty() const
    {
        return args_.empty();
    }

    void Clear()
    {
        args_.clear();
    }

    template<typename T>
    T Arg(uint32_t index = 0u) const
    {
        const std::string& string = String(index);
        const char* start = string.data();
        char* end = const_cast<char*>(start) + string.size();
        return static_cast<T>(std::strtod(start, &end));
    }

    template<>
    const std::string& Arg(uint32_t index) const
    {
        return String(index);
    }

    const std::string& String(uint32_t index = 0u) const
    {
        static const std::string EMPTY;
        return index < Count() ? args_[index] : EMPTY;
    }

    int32_t Int32(uint32_t index = 0u) const
    {
        return Arg<int32_t>(index);
    }

    uint32_t Uint32(uint32_t index = 0u) const
    {
        return Arg<uint32_t>(index);
    }

    int64_t Int64(uint32_t index = 0u) const
    {
        return Arg<int64_t>(index);
    }

    uint64_t Uint64(uint32_t index = 0u) const
    {
        return Arg<uint64_t>(index);
    }

    float Fp32(uint32_t index = 0u) const
    {
        return Arg<float>(index);
    }

    double Fp64(uint32_t index = 0u) const
    {
        return Arg<double>(index);
    }

    pid_t Pid(uint32_t index = 0u) const
    {
        return Uint32(index);
    }

    uint64_t Node(uint32_t index = 0u) const
    {
        return Uint64(index);
    }

protected:
    std::vector<std::string> args_;
};
} // namespace OHOS::Rosen

namespace OHOS::Rosen::Utils {

uint64_t RawNowNano();
uint64_t NowNano();
double Now();
std::vector<std::string> Split(const std::string& input);

constexpr float MILLI = 1e-3f; // NOLINT
constexpr float MICRO = 1e-6f; // NOLINT
constexpr float NANO = 1e-9f;  // NOLINT

FILE* FileOpen(const std::string& path, const std::string& openOptions);
void FileClose(FILE* file);
bool IsFileValid(FILE* file);
size_t FileSize(FILE* file);
size_t FileTell(FILE* file);
void FileSeek(FILE* file, int64_t offset, int origin);
void FileRead(FILE* file, void* data, size_t size);
void FileWrite(FILE* file, const void* data, size_t size);

template<typename T>
void FileRead(FILE* file, T* data, size_t size)
{
    FileRead(file, reinterpret_cast<void*>(data), size);
}

template<typename T>
void FileWrite(FILE* file, const T* data, size_t size)
{
    FileWrite(file, reinterpret_cast<const void*>(data), size);
}

// deprecated
void FileRead(void* data, size_t size, size_t count, FILE* file);
void FileWrite(const void* data, size_t size, size_t count, FILE* file);

template<typename T>
void FileRead(T* data, size_t size, size_t count, FILE* file)
{
    FileRead(reinterpret_cast<void*>(data), size, count, file);
}

template<typename T>
void FileWrite(const T* data, size_t size, size_t count, FILE* file)
{
    FileWrite(reinterpret_cast<const void*>(data), size, count, file);
}
// end of deprecation

constexpr pid_t ExtractPid(uint64_t id)
{
    constexpr uint32_t bits = 32u;
    return static_cast<pid_t>(id >> bits);
}

constexpr pid_t GetMockPid(pid_t pid)
{
    constexpr uint32_t bits = 30u;
    return (1 << bits) | pid;
}

constexpr uint64_t ExtractNodeId(uint64_t id)
{
    constexpr uint32_t mask = 0xFFFFFFFF;
    return (id & mask);
}

constexpr uint64_t ComposeNodeId(uint64_t pid, uint64_t nodeId)
{
    constexpr uint32_t bits = 32u;
    return (pid << bits) | nodeId;
}

constexpr uint64_t ComposeMockNodeId(uint64_t id, uint64_t nodeId)
{
    return ComposeNodeId(GetMockPid(id), nodeId);
}

constexpr uint64_t GetRootNodeId(uint64_t id)
{
    return ComposeNodeId(id, 1);
}

constexpr uint64_t PatchNodeId(uint64_t id)
{
    constexpr uint32_t bits = 62u;
    return id | (static_cast<uint64_t>(1) << bits);
}

constexpr uint64_t IsNodeIdPatched(uint64_t id)
{
    constexpr uint32_t bits = 62u;
    return id & (static_cast<uint64_t>(1) << bits);
}

} // namespace OHOS::Rosen::Utils

#endif // RS_PROFILER_UTILS_H