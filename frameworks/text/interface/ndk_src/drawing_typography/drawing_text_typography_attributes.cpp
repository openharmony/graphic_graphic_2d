/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "drawing_text_typography.h"

#include <unordered_map>

#include "rosen_text/typography.h"

using namespace OHOS::Rosen;

namespace OHOS::Rosen::Text {
typedef OH_Drawing_ErrorCode (*TypographyBoolSetter)(Typography*, bool);
typedef OH_Drawing_ErrorCode (*TypographyBoolGetter)(const Typography*, bool*);

OH_Drawing_ErrorCode SetForceReuseRasterResult(Typography* typography, bool value)
{
    typography->SetForceReuseRasterResult(value);
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode GetForceReuseRasterResult(const Typography* typography, bool* value)
{
    *value = typography->GetForceReuseRasterResult();
    return OH_DRAWING_SUCCESS;
}

static std::unordered_map<OH_Drawing_TypographyAttributeId, TypographyBoolSetter>
    g_typographyBoolSetters = {
        { TYPOGRAPH_ATTR_B_FORCE_REUSE_RASTER_RESULT, SetForceReuseRasterResult },
    };

static std::unordered_map<OH_Drawing_TypographyAttributeId, TypographyBoolGetter>
    g_typographyBoolGetters = {
        { TYPOGRAPH_ATTR_B_FORCE_REUSE_RASTER_RESULT, GetForceReuseRasterResult },
    };
}

OH_Drawing_ErrorCode OH_Drawing_SetTypographyAttributeBool(OH_Drawing_Typography* typography,
    OH_Drawing_TypographyAttributeId id, bool value)
{
    if (typography == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_typographyBoolSetters.find(id);
    if (it == Text::g_typographyBoolSetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(reinterpret_cast<Typography*>(typography), value);
}

OH_Drawing_ErrorCode OH_Drawing_GetTypographyAttributeBool(const OH_Drawing_Typography* typography,
    OH_Drawing_TypographyAttributeId id, bool* value)
{
    if (typography == nullptr || value == nullptr) {
        return OH_DRAWING_ERROR_INCORRECT_PARAMETER;
    }
    auto it = Text::g_typographyBoolGetters.find(id);
    if (it == Text::g_typographyBoolGetters.end()) {
        return OH_DRAWING_ERROR_ATTRIBUTE_ID_MISMATCH;
    }
    return it->second(reinterpret_cast<const Typography*>(typography), value);
}
