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

#include "rs_profiler_archive.h"

namespace OHOS::Rosen {
Archive& Archive::Serialize(char& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(float& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(double& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(int8_t& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(int16_t& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(int32_t& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(uint8_t& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(uint16_t& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(uint32_t& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(uint64_t& value)
{
    return Serialize(&value, sizeof(value));
}

Archive& Archive::Serialize(std::string& value)
{
    size_t size = value.size();
    Serialize(size);

    if (IsReading() && Good()) {
        constexpr auto maxSize = 64 * 1024 * 1024;
        MarkCorruptedIf(size >= maxSize);
        value.resize(Good() ? size : 0);
    }
    return Serialize(value.data(), value.size());
}

Archive& Archive::Serialize(void* data, size_t size)
{
    if (Good() && data && size) {
        MarkCorruptedIf(IsReading() ? !Read(data, size) : !Write(data, size));
    }
    return *this;
}
} // namespace OHOS::Rosen