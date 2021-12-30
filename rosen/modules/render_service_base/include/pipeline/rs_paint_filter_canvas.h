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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H

#ifdef ROSEN_OHOS
#include <include/utils/SkPaintFilterCanvas.h>
#include <stack>

#include "common/rs_macros.h"

class SkDrawable;
namespace OHOS {
namespace Rosen {

class RSPaintFilterCanvas : public SkPaintFilterCanvas {
public:
    RSPaintFilterCanvas(SkCanvas* canvas, float alpha = 1.0f);
    ~RSPaintFilterCanvas() override {};

    void MultiplyAlpha(float alpha);
    void SaveAlpha();
    void RestoreAlpha();
    float GetAlpha() { return alpha_; }

protected:
    bool onFilter(SkPaint& paint) const override;
    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint) override;

private:
    std::stack<float> alphaStack_;
    float alpha_ = 1.0f;
};

} // namespace Rosen
} // namespace OHOS
#endif // ROSEN_OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_PAINT_FILTER_CANVAS_H
