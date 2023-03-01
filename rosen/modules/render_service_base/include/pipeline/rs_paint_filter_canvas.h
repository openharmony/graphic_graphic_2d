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

#include "common/rs_macros.h"

class SkDrawable;
namespace OHOS {
namespace Rosen {

class RSB_EXPORT RSPaintFilterCanvas : public SkPaintFilterCanvas {
public:
    RSPaintFilterCanvas(SkCanvas* canvas, float alpha = 1.0f);
    RSPaintFilterCanvas(SkSurface* skSurface, float alpha = 1.0f);
    ~RSPaintFilterCanvas() override {};

    void MultiplyAlpha(float alpha);
    float GetAlpha() const;

    int SaveAlpha();
    void RestoreAlpha();
    int GetAlphaSaveCount() const;
    void RestoreAlphaToCount(int count);

    std::pair<int, int> SaveCanvasAndAlpha();
    std::pair<int, int> GetSaveCount() const;
    void RestoreCanvasAndAlpha(std::pair<int, int>& count);

    SkSurface* GetSurface() const;

    void SetHighContrast(bool enabled)
    {
        isHighContrastEnabled_ = enabled;
    }
    bool isHighContrastEnabled() const
    {
        return isHighContrastEnabled_;
    }

    void SetCacheEnabled(bool enabled)
    {
        isCacheEnabled_ = enabled;
    }
    bool isCacheEnabled() const
    {
        return isCacheEnabled_;
    }

    void SetVisibleRect(SkRect visibleRect)
    {
        visibleRect_ = visibleRect;
    }

    SkRect GetVisibleRect() const
    {
        return visibleRect_;
    }

protected:
    bool onFilter(SkPaint& paint) const override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint) override;

private:
    SkSurface* skSurface_ = nullptr;
    std::stack<float> alphaStack_;
    std::atomic_bool isHighContrastEnabled_ { false };
    bool isCacheEnabled_ { false };
    SkRect visibleRect_ = SkRect::MakeEmpty();
};

// Helper class similar to SkAutoCanvasRestore, but also restores alpha
class RSAutoCanvasRestore {
public:
    /** Preserves SkCanvas::save() and RSPaintFilterCanvas::SaveAlpha() count. Optionally saves SkCanvas clip /
       SkCanvas matrix and RSPaintFilterCanvas alpha.

        @param canvas     RSPaintFilterCanvas to guard
        @param saveCanvas call SkCanvas::save()
        @param saveAlpha  call RSPaintFilterCanvas::SaveAlpha()
        @return           utility to restore RSPaintFilterCanvas state on destructor
    */
    RSAutoCanvasRestore(RSPaintFilterCanvas* canvas, bool saveCanvas = true, bool saveAlpha = true)
        : canvas_(canvas), saveCount_({ 0, 0 })
    {
        if (canvas_) {
            saveCount_ = canvas->GetSaveCount();
            if (saveCanvas) {
                canvas->save();
            }
            if (saveAlpha) {
                canvas->SaveAlpha();
            }
        }
    }

    /** Allow RSAutoCanvasRestore to be used with std::unique_ptr and std::shared_ptr */
    RSAutoCanvasRestore(
        const std::unique_ptr<RSPaintFilterCanvas>& canvas, bool saveCanvas = true, bool saveAlpha = true)
        : RSAutoCanvasRestore(canvas.get(), saveCanvas, saveAlpha)
    {}
    RSAutoCanvasRestore(
        const std::shared_ptr<RSPaintFilterCanvas>& canvas, bool saveCanvas = true, bool saveAlpha = true)
        : RSAutoCanvasRestore(canvas.get(), saveCanvas, saveAlpha)
    {}

    /** Restores RSPaintFilterCanvas to saved state. Destructor is called when container goes out of
        scope.
    */
    ~RSAutoCanvasRestore()
    {
        if (canvas_) {
            canvas_->RestoreCanvasAndAlpha(saveCount_);
        }
    }

    /** Restores RSPaintFilterCanvas to saved state immediately. Subsequent calls and
        ~RSAutoCanvasRestore() have no effect.
    */
    void restore()
    {
        if (canvas_) {
            canvas_->RestoreCanvasAndAlpha(saveCount_);
            canvas_ = nullptr;
        }
    }

private:
    RSPaintFilterCanvas* canvas_;
    std::pair<int, int> saveCount_;

    RSAutoCanvasRestore(RSAutoCanvasRestore&&) = delete;
    RSAutoCanvasRestore(const RSAutoCanvasRestore&) = delete;
    RSAutoCanvasRestore& operator=(RSAutoCanvasRestore&&) = delete;
    RSAutoCanvasRestore& operator=(const RSAutoCanvasRestore&) = delete;
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
