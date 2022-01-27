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

#ifndef RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
#define RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H

#include "pipeline/rs_draw_cmd_list.h"
#include "property/rs_properties.h"

namespace OHOS {
namespace Rosen {
class RSSkiaFilter;
class RSPaintFilterCanvas;
class RSPropertiesPainter {
public:
    static void Clip(SkCanvas& canvas, RectF rect);
    static void DrawBackground(const RSProperties& properties, SkCanvas& canvas);
    static void DrawBorder(const RSProperties& properties, SkCanvas& canvas);
    static void DrawFrame(
        const RSProperties& properties, RSPaintFilterCanvas& canvas, std::shared_ptr<DrawCmdList>& drawCmdList);
    static void DrawShadow(const RSProperties& properties, SkCanvas& canvas);
    static void SaveLayerForFilter(
        const RSProperties& properties, SkCanvas& canvas, std::shared_ptr<RSSkiaFilter>& filter);
    static void RestoreForFilter(SkCanvas& canvas);
    static void DrawForegroundColor(const RSProperties& properties, SkCanvas& canvas);
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
