/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "property/rs_properties.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
class RSSkiaFilter;
class RSPaintFilterCanvas;

class RSB_EXPORT RSPropertiesPainter {
public:
    static void Clip(SkCanvas& canvas, RectF rect);
    static void SetBgAntiAlias(bool forceBgAntiAlias);
    static bool GetBgAntiAlias();
    static void DrawBackground(const RSProperties& properties, RSPaintFilterCanvas& canvas);
    static void DrawBorder(const RSProperties& properties, SkCanvas& canvas);
    static void DrawFrame(const RSProperties& properties, RSPaintFilterCanvas& canvas, DrawCmdListPtr& drawCmdList);
    static void GetShadowDirtyRect(RectI& dirtyShadow, const RSProperties& properties, const RRect* rrect = nullptr);
    static void DrawShadow(const RSProperties& properties, RSPaintFilterCanvas& canvas, const RRect* rrect = nullptr);
    static void DrawFilter(const RSProperties& properties, RSPaintFilterCanvas& canvas,
        std::shared_ptr<RSSkiaFilter>& filter, const std::unique_ptr<SkRect>& rect = nullptr,
        SkSurface* sKSurface = nullptr);
    static void DrawForegroundColor(const RSProperties& properties, SkCanvas& canvas);
    static void DrawMask(const RSProperties& properties, SkCanvas& canvas);
    static void DrawMask(const RSProperties& properties, SkCanvas& canvas, SkRect maskBounds);
    static bool GetGravityMatrix(Gravity gravity, RectF rect, float w, float h, SkMatrix& mat);
    static SkRRect RRect2SkRRect(const RRect& rr);
    static SkRect Rect2SkRect(const RectF& r);
    static int GetAndResetBlurCnt();
    static SkColor CalcAverageColor(sk_sp<SkImage> imageSnapshot);
private:
    inline static int g_blurCnt = 0;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_PROPERTY_RS_PROPERTIES_PAINTER_H
