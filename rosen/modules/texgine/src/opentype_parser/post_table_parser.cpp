/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "post_table_parser.h"

#include "utils/text_log.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
const struct PostTable* PostTableParser::Parse(const char* data, int32_t size)
{
    return reinterpret_cast<const struct PostTable*>(data);
}

void PostTableParser::Dump() const
{
    const auto& table = *reinterpret_cast<const struct PostTable*>(data_);
    TEXT_LOGI("PostTable size: %{public}d, version: %{public}d, italicAngle: %{public}f, isFixedPitch: %{public}d",
        size_, table.version.Get(), table.italicAngle.Get(), table.isFixedPitch.Get());
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
