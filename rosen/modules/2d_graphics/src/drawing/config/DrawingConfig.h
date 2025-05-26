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

#ifndef DRAWING_CONFIG_H
#define DRAWING_CONFIG_H

#include <mutex>

namespace OHOS {
namespace Rosen {
namespace Drawing {

class DrawingConfig {
public:
#ifdef DRAWING_DISABLE_API
    static void UpdateDrawingProperties(); //更新Drawing的各类配置项, 非debug时仅初始化调用, debug时每帧调用。

    enum class DrawingDisableFlag : int {
        DISABLE_POINT = 0,
        DISABLE_TEXTBLOB,
        DISABLE_IMAGE,
        DISABLE_RECT,
        DISABLE_RRECT,
        DISABLE_OVAL,
        DISABLE_ARC,
        DISABLE_PATH,
        DISABLE_PICTURE,
        DISABLE_IMAGE_RECT,
        DISABLE_IMAGE_LATTICE,
        DISABLE_SDF,
        DISABLE_LINE,
        DISABLE_NESTED_RRECT,
        DISABLE_PIE,
        DISABLE_CIRCLE,
        DISABLE_BACKGROUND,
        DISABLE_SHADOW,
        DISABLE_SHADOW_STYLE,
        DISABLE_COLOR,
        DISABLE_REGION,
        DISABLE_PATCH,
        DISABLE_VERTICES,
        DISABLE_ATLAS,
        DISABLE_BITMAP,
        DISABLE_IMAGE_NINE,
        DISABLE_SVGDOM,
        DISABLE_SYMBOL,
        DISABLE_CLIP_RECT,
        DISABLE_CLIP_IRECT,
        DISABLE_CLIP_RRECT,
        DISABLE_CLIP_PATH,
        DISABLE_CLIP_REGION,
        DISABLE_BLUR_IMAGE,
        DISABLE_COLOR_FILTER,
        DISABLE_IMAGE_FILTER,
        DISABLE_MASK_FILTER,
        DISABLE_SHADER,
        DISABLE_BLENDER,
        DISABLE_PATH_EFFECT,
        COUNT // The total number of flags.
    };

    static bool IsDisabled(DrawingDisableFlag flag);
private:
    std::mutex mutex;
#endif
};
}
}
}
#endif // DRAWING_CONFIG_H
