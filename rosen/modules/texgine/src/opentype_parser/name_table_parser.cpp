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

#include "name_table_parser.h"

#include <iomanip>
#include <string>

#include "texgine/utils/exlog.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
const struct NameTable* NameTableParser::Parse(const char* data, int32_t size)
{
    return reinterpret_cast<const struct NameTable*>(data);
}

void NameTableParser::Dump() const
{
    if (data_ == nullptr) {
        return;
    }
    const auto& nameTable = *reinterpret_cast<const struct NameTable*>(data_);
    LOGSO_FUNC_LINE(INFO) << "nameTable size:" << size_ << ", version:" << nameTable.version.Get();
    auto count = nameTable.count.Get();
    auto strOffset = data_ + nameTable.storageOffset.Get();
    for (auto i = 0; i < count; ++i) {
        auto len = nameTable.nameRecord[i].length.Get();
        if (len == 0) {
            continue;
        }
        auto offset = nameTable.nameRecord[i].stringOffset.Get();
        std::string recordinfo(strOffset + offset, len);
        auto nameId = nameTable.nameRecord[i].nameId.Get();
        LOGSO_FUNC_LINE(INFO) << "nameId:" << nameId << ", recordinfo:" << std::uppercase << std::hex << std::setw(len)
            << recordinfo;
    }
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
