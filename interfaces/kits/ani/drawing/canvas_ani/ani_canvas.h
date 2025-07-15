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
#include "ani_drawing_utils.h"
#include "draw/canvas.h"
#include "sampling_options_ani/ani_sampling_options.h"

namespace OHOS::Rosen {
namespace Drawing {
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