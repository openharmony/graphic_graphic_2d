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
    static void DrawImageRect(ani_env* env, ani_object obj,
        ani_object pixelmapObj, ani_object rectObj, ani_object samplingOptionsObj);

    static void DrawPixelMapMesh(ani_env* env, ani_object obj,
        ani_object pixelmapObj, ani_int aniMeshWidth, ani_int aniMeshHeight,
        ani_object verticesObj, ani_int aniVertOffset, ani_object colorsObj, ani_int aniColorOffset);
    static void DrawVertices(ani_env* env, ani_object obj, ani_enum_item aniVertexMode,
        ani_int aniVertexCount, ani_object positionsObj, ani_object texsObj,
        ani_object colorsObj, ani_int aniIndexCount, ani_object indicesObj, ani_enum_item aniBlendMode);
    static void AttachBrush(ani_env* env, ani_object obj, ani_object brushObj);
    static void AttachPen(ani_env* env, ani_object obj, ani_object penObj);
    static void DetachBrush(ani_env* env, ani_object obj);
    static void DetachPen(ani_env* env, ani_object obj);
    static ani_int Save(ani_env* env, ani_object obj);
    static ani_long SaveLayer(ani_env* env, ani_object obj, ani_object rectObj, ani_object brushObj);
    static void Restore(ani_env* env, ani_object obj);
    static ani_int GetSaveCount(ani_env* env, ani_object obj);
    static void Rotate(ani_env* env, ani_object obj, ani_double degrees, ani_double sx, ani_double sy);

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
    static bool GetVertices(ani_env* env, ani_object verticesObj, float* vertices, uint32_t verticesSize);
    static bool GetVerticesUint16(ani_env* env, ani_object verticesObj, uint16_t* vertices, uint32_t verticesSize);
    static bool GetColorsUint32(ani_env* env, ani_object verticesObj, uint32_t* vertices, uint32_t verticesSize);
    static bool GetVertexModeAndBlendMode(ani_env* env, ani_enum_item aniVertexMode, ani_enum_item aniBlendMode,
        VertexMode& vertexMode, BlendMode& blendMode);
    static bool GetIndices(ani_env* env, ani_int indexCount, ani_object indicesObj,
        std::unique_ptr<uint16_t[]>& indices);
    static bool GetColors(ani_env* env, ani_int vertexCount, ani_object colorsObj,
        std::unique_ptr<uint32_t[]>& colors);
    static bool GetPositions(ani_env* env, ani_int vertexCount, ani_object positionsObj,
        std::vector<Drawing::Point>& pointPositions);
    static bool GetTexs(ani_env* env, ani_int vertexCount, ani_object texsObj,
        std::vector<Drawing::Point>& pointTexs);
    static bool CheckDrawVerticesParams(ani_env* env, ani_int& vertexCount, ani_int& indexCount);
    static void GetColorsAndDraw(ani_env* env, ani_object colorsObj, int32_t colorOffset,
        DrawPixelMapMeshArgs& args, AniCanvas* aniCanvas);
#endif
    Canvas* m_canvas = nullptr;
    bool owned_ = false;
#ifdef ROSEN_OHOS
    std::shared_ptr<Media::PixelMap> mPixelMap_ = nullptr;
#endif
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_CANVAS_H