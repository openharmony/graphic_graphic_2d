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

#ifndef OHOS_ROSEN_ANI_CANVAS_H
#define OHOS_ROSEN_ANI_CANVAS_H

#ifdef ROSEN_OHOS
#include "pixel_map.h"
#endif
#include <cstdint>
#include "ani_drawing_utils.h"
#include "draw/canvas.h"
#include "sampling_options_ani/ani_sampling_options.h"

namespace OHOS::Rosen {
namespace Drawing {
struct DrawPixelMapMeshArgs {
    ~DrawPixelMapMeshArgs()
    {
        if (vertices != nullptr) {
            delete []vertices;
        }
    }
    std::shared_ptr<Media::PixelMap>& pixelMap;
    float* vertices = nullptr;
    uint32_t verticesSize = 0;
    int32_t vertOffset = 0;
    int32_t column = 0;
    int32_t row = 0;
};
class AniCanvas final {
public:
    AniCanvas() = default;
    explicit AniCanvas(Canvas* canvas, bool owned = false) : m_canvas(canvas), owned_(owned) {};
    ~AniCanvas();

    static ani_status AniInit(ani_env *env);
    static void Constructor(ani_env* env, ani_object obj, ani_object pixelmapObj);
    static void DrawRect(ani_env* env, ani_object obj,
        ani_double left, ani_double top, ani_double right, ani_double bottom);
    static void DrawRectWithRect(ani_env* env, ani_object obj, ani_object aniRect);
    static void DrawRoundRect(ani_env* env, ani_object obj, ani_object roundRectObj);
    static void DrawNestedRoundRect(ani_env* env, ani_object obj, ani_object outerObj, ani_object innerObj);
    static void DrawBackground(ani_env* env, ani_object obj, ani_object brushObj);
    static void DrawShadow(ani_env* env, ani_object obj, ani_object pathObj,
        ani_object planeParams, ani_object devLightPos, ani_double lightRadius,
        ani_object ambientColor, ani_object spotColor, ani_enum_item flagEnum);
    static void DrawShadowWithOption(ani_env* env, ani_object obj, ani_object pathObj,
        ani_object planeParams, ani_object devLightPos, ani_double lightRadius,
        ani_object ambientColorOps, ani_object spotColorOps, ani_enum_item flagEnum);
    static void DrawCircle(ani_env* env, ani_object obj, ani_double x, ani_double y, ani_double radius);
    static void DrawImage(ani_env* env, ani_object obj, ani_object pixelmapObj,
        ani_double left, ani_double top, ani_object samplingOptionsObj);
    static void DrawImageLattice(ani_env* env, ani_object obj, ani_object pixelmapObj,
        ani_object latticeObj, ani_object dstRectObj, ani_enum_item filterModeEnum);
    static void DrawImageNine(ani_env* env, ani_object obj, ani_object pixelmapObj,
        ani_object centerRectObj, ani_object rectObj, ani_enum_item filterModeEnum);
    static void DrawImageRect(ani_env* env, ani_object obj,
        ani_object pixelmapObj, ani_object rectObj, ani_object samplingOptionsObj);

    static void DrawImageRectWithSrc(ani_env* env, ani_object obj, ani_object pixelmapObj,
        ani_object srcRectObj, ani_object dstRectObj, ani_object samplingOptionsObj, ani_object constraintObj);
    static void DrawColorWithObject(ani_env* env, ani_object obj, ani_object colorObj, ani_object aniBlendMode);
    static void DrawColorWithArgb(ani_env* env, ani_object obj, ani_int alpha, ani_int red,
        ani_int green, ani_int blue, ani_object aniBlendMode);
    static void DrawColor(ani_env* env, ani_object obj, ani_int color, ani_object aniBlendMode);
    static void DrawPath(ani_env* env, ani_object obj, ani_object pathObj);
    static void DrawLine(ani_env* env, ani_object obj, ani_double x0,
        ani_double y0, ani_double x1, ani_double y1);
    static void DrawSingleCharacter(ani_env* env, ani_object obj, ani_string text,
        ani_object fontObj, ani_double x, ani_double y);
    static void DrawTextBlob(ani_env* env, ani_object obj, ani_object textBlobObj, ani_double x, ani_double y);
    static void DrawOval(ani_env* env, ani_object obj, ani_object rectObj);
    static void DrawArc(ani_env* env, ani_object obj, ani_object rectObj,
        ani_double startAngle, ani_double sweepAngle);
    static void DrawArcWithCenter(ani_env* env, ani_object obj, ani_object rectObj,
        ani_double startAngle, ani_double sweepAngle, ani_boolean useCenter);
    static void DrawPoint(ani_env* env, ani_object obj, ani_double x, ani_double y);
    static void DrawPoints(ani_env* env, ani_object obj, ani_array pointsObj, ani_object aniPointMode);
    static void DrawRegion(ani_env* env, ani_object obj, ani_object regionObj);
    static void DrawPixelMapMesh(ani_env* env, ani_object obj,
        ani_object pixelmapObj, ani_int aniMeshWidth, ani_int aniMeshHeight,
        ani_array verticesObj, ani_int aniVertOffset, ani_array colorsObj, ani_int aniColorOffset);
    static void DrawVertices(ani_env* env, ani_object obj, ani_enum_item aniVertexMode,
        ani_int aniVertexCount, ani_array positionsObj, ani_object texsObj,
        ani_object colorsObj, ani_int aniIndexCount, ani_object indicesObj, ani_enum_item aniBlendMode);
    static void AttachBrush(ani_env* env, ani_object obj, ani_object brushObj);
    static void AttachPen(ani_env* env, ani_object obj, ani_object penObj);
    static void DetachBrush(ani_env* env, ani_object obj);
    static void DetachPen(ani_env* env, ani_object obj);
    static ani_int Save(ani_env* env, ani_object obj);
    static ani_long SaveLayer(ani_env* env, ani_object obj, ani_object rectObj, ani_object brushObj);
    static void Clear(ani_env* env, ani_object obj, ani_object objColor);
    static void ClearWithOption(ani_env* env, ani_object obj, ani_object objColor);
    static void Restore(ani_env* env, ani_object obj);
    static void RestoreToCount(ani_env* env, ani_object obj, ani_int count);
    static ani_int GetSaveCount(ani_env* env, ani_object obj);
    static ani_int GetWidth(ani_env* env, ani_object obj);
    static ani_int GetHeight(ani_env* env, ani_object obj);
    static ani_object GetLocalClipBounds(ani_env* env, ani_object obj);
    static ani_object GetTotalMatrix(ani_env* env, ani_object obj);
    static void Scale(ani_env* env, ani_object obj, ani_double sx, ani_double sy);
    static void Skew(ani_env* env, ani_object obj, ani_double sx, ani_double sy);
    static void Rotate(ani_env* env, ani_object obj, ani_double degrees, ani_double sx, ani_double sy);
    static void Translate(ani_env* env, ani_object obj, ani_double dx, ani_double dy);
    static void ClipRect(ani_env* env, ani_object obj, ani_object rectObj, ani_object clipOpObj, ani_object aaObj);
    static void ClipPath(ani_env* env, ani_object obj, ani_object pathObj, ani_object clipOpObj, ani_object aaObj);
    static void ConcatMatrix(ani_env* env, ani_object obj, ani_object matrixObj);
    static void ClipRegion(ani_env* env, ani_object obj, ani_object regionObj, ani_object clipOpObj);
    static void ClipRoundRect(ani_env* env, ani_object obj, ani_object roundRectObj,
        ani_object clipOpObj, ani_object aaObj);
    static ani_boolean IsClipEmpty(ani_env* env, ani_object obj);
    static void SetMatrix(ani_env* env, ani_object obj, ani_object matrixObj);
    static void ResetMatrix(ani_env* env, ani_object obj);
    static ani_boolean QuickRejectPath(ani_env* env, ani_object obj, ani_object pathObj);
    static ani_boolean QuickRejectRect(ani_env* env, ani_object obj, ani_object rectObj);
    static void DrawSingleCharacterWithFeatures(ani_env* env, ani_object obj, ani_string text, ani_object font,
        ani_double x, ani_double y, ani_array features);

    Canvas* GetCanvas();
    DRAWING_API void ResetCanvas();
    DRAWING_API void ClipCanvas(float width, float height);
    DRAWING_API void SaveCanvas();
    DRAWING_API void RestoreCanvas();
    DRAWING_API static ani_object CreateAniCanvas(ani_env* env, Canvas* canvas);

private:
    void NotifyDirty();
#ifdef ROSEN_OHOS
    void DrawImageRectInner(std::shared_ptr<Media::PixelMap> pixelmap,
        Drawing::Rect& rect, AniSamplingOptions* samplingOptions);
    void DrawImageRectWithSrcInner(std::shared_ptr<Media::PixelMap> pixelmap, Drawing::Rect& srcRect,
        Drawing::Rect& dstRect, AniSamplingOptions* samplingOptions, int32_t srcRectConstraint);
    static bool GetVertices(ani_env* env, ani_array verticesObj, float* vertices, uint32_t verticesSize);
    static bool GetVerticesUint16(ani_env* env, ani_array verticesObj, uint16_t* vertices, uint32_t verticesSize);
    static bool GetColorsUint32(ani_env* env, ani_array colorsObj, uint32_t* colors, uint32_t colorsSize);
    static bool GetVertexModeAndBlendMode(ani_env* env, ani_enum_item aniVertexMode, ani_enum_item aniBlendMode,
        VertexMode& vertexMode, BlendMode& blendMode);
    static bool GetIndices(ani_env* env, ani_int indexCount, ani_object indicesObj,
        std::unique_ptr<uint16_t[]>& indices);
    static bool GetColors(ani_env* env, ani_int vertexCount, ani_object colorsObj,
        std::unique_ptr<uint32_t[]>& colors);
    static bool GetPositions(ani_env* env, ani_int vertexCount, ani_array positionsObj,
        std::vector<Drawing::Point>& pointPositions);
    static bool GetTexs(ani_env* env, ani_int vertexCount, ani_object texsObj,
        std::vector<Drawing::Point>& pointTexs);
    static bool CheckDrawVerticesParams(ani_env* env, ani_int& vertexCount, ani_int& indexCount);
    static void GetColorsAndDraw(ani_env* env, ani_array colorsObj, int32_t colorOffset,
        DrawPixelMapMeshArgs& args, AniCanvas* aniCanvas);
    std::shared_ptr<Media::PixelMap>* GetPixelMapPtrAddr();
#endif
    static ani_object CanvasTransferStatic(ani_env*  env, [[maybe_unused]]ani_object obj, ani_object input);
    static ani_long GetCanvasAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    static ani_long GetPixelMapAddr(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);
    static ani_boolean GetCanvasOwned(ani_env* env, [[maybe_unused]]ani_object obj, ani_object input);

    Canvas* m_canvas = nullptr;
    bool owned_ = false;
#ifdef ROSEN_OHOS
    std::shared_ptr<Media::PixelMap> mPixelMap_ = nullptr;
#endif
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_CANVAS_H