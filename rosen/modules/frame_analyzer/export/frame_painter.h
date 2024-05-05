/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_PAINTER_H
#define ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_PAINTER_H

#include "frame_collector.h"

#include <vector>

#include "draw/canvas.h"
#include "draw/color.h"


namespace OHOS {
namespace Rosen {
class FramePainter {
public:
    FramePainter(FrameCollector &collector);

    void Draw(Drawing::Canvas &canvas);

private:
    struct TimeBar {
        bool isHeavy = false;
        uint32_t color = 0;
        double posX = 0;
        double posY = 0;
        double width = 0;
        double height = 0;
    };

    void DrawFPSLine(Drawing::Canvas &canvas, uint32_t fps, double thickness, uint32_t color);
    std::vector<struct TimeBar> GenerateTimeBars(uint32_t width, uint32_t height, uint32_t fps);
    double SumTimesInMs(const struct FrameInfo &info);

    FrameCollector &collector_;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_MODULE_FRAME_ANALYZER_EXPORT_FRAME_PAINTER_H
