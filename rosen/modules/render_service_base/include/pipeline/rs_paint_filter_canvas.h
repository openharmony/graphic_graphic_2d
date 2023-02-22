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
    void RestoreAlphaToCount(int count);

    std::pair<int, int> SaveCanvasAndAlpha();
    void RestoreCanvasAndAlpha(std::pair<int, int>& count);

    SkSurface* GetSurface() const;

    void SetHighContrast(bool enabled)
    {
        isHighContrastEnabled_  = enabled;
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

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
