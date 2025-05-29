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

#ifndef ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_POST_TABLE_PARSER_H
#define ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_POST_TABLE_PARSER_H

#include <cstdint>
#include <string>

#include "opentype_basic_type.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
struct PostTable {
    OpenTypeBasicType::Uint32 version;
    OpenTypeBasicType::Fixed italicAngle;
    OpenTypeBasicType::Int16 underlinePosition;
    OpenTypeBasicType::Int16 underlineThickness;
    OpenTypeBasicType::Uint32 isFixedPitch;
    OpenTypeBasicType::Uint32 minMemType42;
    OpenTypeBasicType::Uint32 maxMemType42;
    OpenTypeBasicType::Uint32 minMemType1;
    OpenTypeBasicType::Uint32 maxMemType1;

    static constexpr const char* tag = "post";
};
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULES_TEXGINE_SRC_OPENTYPE_PARSER_POST_TABLE_PARSER_H
