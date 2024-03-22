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

#ifndef DRAWING_CANVAS_H
#define DRAWING_CANVAS_H
#include "draw/canvas.h"
#include "utils/rect.h"
#include "utils/sampling_options.h"

namespace OHOS {
namespace Media {
class PixelMap;
}
}

class DrawingCanvasUtils {
public:
    static void DrawPixelMapRect(OHOS::Rosen::Drawing::Canvas* canvas, std::shared_ptr<OHOS::Media::PixelMap> pixelMap,
        const OHOS::Rosen::Drawing::Rect& src, const OHOS::Rosen::Drawing::Rect& dst,
        const OHOS::Rosen::Drawing::SamplingOptions& sampling);
};
#endif // DRAWING_CANVAS_H