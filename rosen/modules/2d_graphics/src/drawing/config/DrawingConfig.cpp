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

#ifdef DRAWING_DISABLE_API
#include "config/DrawingConfig.h"
#include <string>
#include <vector>
#include "base/startup/init/interfaces/innerkits/include/syspara/parameters.h"
#include "utils/log.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
int flagCount = static_cast<int>(DrawingConfig::DrawingDisableFlag::COUNT);

std::vector<bool> gDrawingDisableFlags(flagCount, false);

std::vector<std::string> gDrawingDisableFlagStr = {
    "disablePoint",
    "disableTextBlob",
    "disableImage",
    "disableRect",
    "disableRRect",
    "disableOval",
    "disableArc",
    "disablePath",
    "disablePicture",
    "disableImageRect",
    "disableImageLattice",
    "disableSdf",
    "disableLine",
    "disableNestedRRect",
    "disablePie",
    "disableCircle",
    "disableBackground",
    "disableShadow",
    "disableShadowStyle",
    "disableColor",
    "disableRegion",
    "disablePatch",
    "disableVertices",
    "disableAtlas",
    "disableBitmap",
    "disableImageNine",
    "disableSVGDOM",
    "disableSymbol",
    "disableClipRect",
    "disableClipIRect",
    "disableClipRRect",
    "disableClipPath",
    "disableClipRegion",
    "disableBlurImage",
    "disableColorFilter",
    "disableImageFilter",
    "disableMaskFilter",
    "disableShader",
    "disableBlender",
    "disablePathEffect"
};

void DrawingConfig::UpdateDrawingProperties()
{
    std::lock_guard<std::mutex> lock(mutex);
    if (flagCount != static_cast<int>(gDrawingDisableFlagStr.size())) {
        LOGE("The number of gDrawingDisableFlagStr and DrawingDisableFlag is inconsistent");
        return;
    }

    for (int i = 0; i < flagCount; i++) {
        std::string key = "drawing." + gDrawingDisableFlagStr[i];
        gDrawingDisableFlags[i] = OHOS::system::GetBoolParameter(key, false);
    }
}

bool DrawingConfig::IsDisabled(DrawingDisableFlag flag)
{
    std::lock_guard<std::mutex> lock(mutex);
    return gDrawingDisableFlags[static_cast<int>(flag)];
}
}
}
}
#endif
