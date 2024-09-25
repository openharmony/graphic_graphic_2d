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

#ifndef RS_PROFILER_COMMAND_H
#define RS_PROFILER_COMMAND_H

#include "rs_profiler_utils.h"

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

    const std::string& String(size_t index = 0u) const
    {
        static const std::string EMPTY;
        return index < Count() ? args_[index] : EMPTY;
    }

    int8_t Int8(size_t index = 0u) const
    {
        return Utils::ToInt8(String(index));
    }

    int16_t Int16(size_t index = 0u) const
    {
        return Utils::ToInt16(String(index));
    }

    int32_t Int32(size_t index = 0u) const
    {
        return Utils::ToInt32(String(index));
    }

    int64_t Int64(size_t index = 0u) const
    {
        return Utils::ToInt64(String(index));
    }

    uint8_t Uint8(size_t index = 0u) const
    {
        return Utils::ToUint8(String(index));
    }

    uint16_t Uint16(size_t index = 0u) const
    {
        return Utils::ToUint16(String(index));
    }

    uint32_t Uint32(size_t index = 0u) const
    {
        return Utils::ToUint32(String(index));
    }

    uint64_t Uint64(size_t index = 0u) const
    {
        return Utils::ToUint64(String(index));
    }

    float Fp32(size_t index = 0u) const
    {
        return Utils::ToFp32(String(index));
    }

    double Fp64(size_t index = 0u) const
    {
        return Utils::ToFp64(String(index));
    }

    pid_t Pid(size_t index = 0u) const
    {
        return static_cast<pid_t>(Uint32(index));
    }

    uint64_t Node(size_t index = 0u) const
    {
        return Uint64(index);
    }

protected:
    std::vector<std::string> args_;
};

} // namespace OHOS::Rosen

#endif // RS_PROFILER_COMMAND_H