/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef METADATA_CONVERTOR_H
#define METADATA_CONVERTOR_H

#include <vector>

#include <securec.h>

namespace OHOS {
namespace MetadataManager {
// 0 for success, -1 for fail
template <typename T>
static int32_t ConvertMetadataToVec(const T& metadata, std::vector<uint8_t>& data)
{
    data.resize(sizeof(T));
    if (memcpy_s(data.data(), data.size(), &metadata, sizeof(T)) != EOK) {
        return -1;
    }
    return 0;
}

template <typename T>
static int32_t ConvertVecToMetadata(const std::vector<uint8_t>& data, T& metadata)
{
    if (data.size() != sizeof(T)) {
        return -1;
    }

    if (memcpy_s(&metadata, sizeof(T), data.data(), data.size()) != EOK) {
        return -1;
    }
    return 0;
}
} // namespace MetadataManager
} // namespace OHOS

#endif // METADATA_CONVERTOR_H