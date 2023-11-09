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

#ifndef INTERFACES_INNERKITS_COMMON_METADATA_CONVERTOR_H
#define INTERFACES_INNERKITS_COMMON_METADATA_CONVERTOR_H

#include <vector>

#include <securec.h>

#include "graphic_common.h"

namespace OHOS {
namespace MetadataManager {
// GSERROR_OK for success, GSERROR_API_FAILED for fail
template <typename T>
static GSError ConvertMetadataToVec(const T& metadata, std::vector<uint8_t>& data)
{
    data.resize(sizeof(T));
    if (memcpy_s(data.data(), data.size(), &metadata, sizeof(T)) != EOK) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}

template <typename T>
static int32_t ConvertVecToMetadata(const std::vector<uint8_t>& data, T& metadata)
{
    if (data.size() != sizeof(T)) {
        return GSERROR_NOT_SUPPORT;
    }

    if (memcpy_s(&metadata, sizeof(T), data.data(), data.size()) != EOK) {
        return GSERROR_API_FAILED;
    }
    return GSERROR_OK;
}
} // namespace MetadataManager
} // namespace OHOS

#endif // INTERFACES_INNERKITS_COMMON_METADATA_CONVERTOR_H