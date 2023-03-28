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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H

#include <include/utils/SkPaintFilterCanvas.h>
#include <stack>
#include <vector>

#include "include/core/SkSurface.h"

#include "common/rs_color.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

// This class is used to filter the paint before drawing. currently, it is used to filter the alpha and foreground
// color.
class RSB_EXPORT RSPaintFilterCanvas : public SkPaintFilterCanvas {
public:
    RSPaintFilterCanvas(SkCanvas* canvas, float alpha = 1.0f);
    RSPaintFilterCanvas(SkSurface* skSurface, float alpha = 1.0f);
    ~RSPaintFilterCanvas() override {};

    void CopyConfiguration(const RSPaintFilterCanvas& other);

    // alpha related
    void MultiplyAlpha(float alpha);
    float GetAlpha() const;
    int SaveAlpha();
    void RestoreAlpha();
    int GetAlphaSaveCount() const;
    void RestoreAlphaToCount(int count);

    // env related
    void SetEnvForegroundColor(Color color);
    Color GetEnvForegroundColor() const;
    int SaveEnv();
    void RestoreEnv();
    int GetEnvSaveCount() const;
    void RestoreEnvToCount(int count);

    // save/restore utils
    using SaveStatus = struct {
        int canvasSaveCount;
        int alphaSaveCount;
        int envSaveCount;
    };
    SaveStatus Save();
    SaveStatus GetSaveStatus() const;
    void RestoreStatus(const SaveStatus& status);

    SkSurface* GetSurface() const;

    // high contrast
    void SetHighContrast(bool enabled);
    bool isHighContrastEnabled() const;

    enum CacheType : uint8_t {
        UNDEFINED, // do not change current cache status
        ENABLED,   // explicitly enable cache
        DISABLED   // explicitly disable cache
    };
    // cache
    void SetCacheType(CacheType type);
    CacheType GetCacheType() const;

    // visible rect
    void SetVisibleRect(SkRect visibleRect);
    SkRect GetVisibleRect() const;

protected:
    bool onFilter(SkPaint& paint) const override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint) override;

private:
    SkSurface* skSurface_ = nullptr;
    std::stack<float> alphaStack_;
    using Env = struct {
        Color envForegroundColor;
    };
    std::stack<Env> envStack_;

    std::atomic_bool isHighContrastEnabled_ { false };
    CacheType cacheType_ { UNDEFINED };
    SkRect visibleRect_ = SkRect::MakeEmpty();
};

// This class extends RSPaintFilterCanvas to also create a color filter for the paint.
class RSB_EXPORT RSColorFilterCanvas : public RSPaintFilterCanvas {
public:
    explicit RSColorFilterCanvas(RSPaintFilterCanvas* canvas);
    ~RSColorFilterCanvas() override {};

protected:
    bool onFilter(SkPaint& paint) const override;
};

// Helper class similar to SkAutoCanvasRestore, but also restores alpha and/or env
class RSB_EXPORT RSAutoCanvasRestore {
public:
    enum SaveType : uint8_t {
        kNone   = 0x0,
        kCanvas = 0x1,
        kAlpha  = 0x2,
        kEnv    = 0x4,
        kALL    = kCanvas | kAlpha | kEnv,
    };

    /** Preserves canvas save count. Optionally call SkCanvas::save() and/or RSPaintFilterCanvas::SaveAlpha() and/or
       RSPaintFilterCanvas::SaveEnv().
        @param canvas     RSPaintFilterCanvas to guard
        @param saveCanvas call SkCanvas::save()
        @param saveAlpha  call RSPaintFilterCanvas::SaveAlpha()
        @return           utility to restore RSPaintFilterCanvas state on destructor
    */
    RSAutoCanvasRestore(RSPaintFilterCanvas* canvas, SaveType type = SaveType::kALL);

    /** Allow RSAutoCanvasRestore to be used with std::unique_ptr and std::shared_ptr */
    RSAutoCanvasRestore(const std::unique_ptr<RSPaintFilterCanvas>& canvas, SaveType type = SaveType::kALL)
        : RSAutoCanvasRestore(canvas.get(), type)
    {}
    RSAutoCanvasRestore(const std::shared_ptr<RSPaintFilterCanvas>& canvas, SaveType type = SaveType::kALL)
        : RSAutoCanvasRestore(canvas.get(), type)
    {}

    /** Restores RSPaintFilterCanvas to saved state. Destructor is called when container goes out of
        scope.
    */
    ~RSAutoCanvasRestore()
    {
        if (canvas_) {
            canvas_->RestoreStatus(saveCount_);
        }
    }

    /** Restores RSPaintFilterCanvas to saved state immediately. Subsequent calls and
        ~RSAutoCanvasRestore() have no effect.
    */
    void restore()
    {
        if (canvas_) {
            canvas_->RestoreStatus(saveCount_);
            canvas_ = nullptr;
        }
    }

private:
    RSPaintFilterCanvas* canvas_;
    RSPaintFilterCanvas::SaveStatus saveCount_;

    RSAutoCanvasRestore(RSAutoCanvasRestore&&) = delete;
    RSAutoCanvasRestore(const RSAutoCanvasRestore&) = delete;
    RSAutoCanvasRestore& operator=(RSAutoCanvasRestore&&) = delete;
    RSAutoCanvasRestore& operator=(const RSAutoCanvasRestore&) = delete;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
