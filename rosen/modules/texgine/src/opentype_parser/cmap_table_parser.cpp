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

#include "cmap_table_parser.h"

#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
const struct CmapTables* CmapTableParser::Parse(const char* data, int32_t size)
{
    return reinterpret_cast<const struct CmapTables*>(data);
}

void CmapTableParser::Dump() const
{
    auto cmapTable = reinterpret_cast<const struct CmapTables*>(data_);
    LOGSO_FUNC_LINE(INFO) << "cmapTable size: " << size_;
    for (auto i = 0; i < cmapTable->numTables.Get(); ++i) {
        const auto& record = cmapTable->encodingRecords[i];
        LOGSO_FUNC_LINE(INFO) << "platformID: " << record.platformID.Get()
            << ", encodingID: " << record.encodingID.Get();
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
