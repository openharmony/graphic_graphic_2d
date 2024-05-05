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

void Archive::Serialize(char& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(float& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(double& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(int8_t& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(int16_t& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(int32_t& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(uint8_t& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(uint16_t& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(uint32_t& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(uint64_t& value)
{
    Serialize(&value, sizeof(value));
}

void Archive::Serialize(std::string& value)
{
    size_t size = value.size();
    Serialize(size);

    if (IsReading()) {
        value.resize(size);
    }

    Serialize(value.data(), size);
}

void Archive::Serialize(void* data, size_t size)
{
    if (!data || (size == 0)) {
        return;
    }

    if (IsReading()) {
        Read(data, size);
    } else {
        Write(data, size);
    }
}

} // namespace OHOS::Rosen