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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_H

#include "common/rs_macros.h"
#include "common/rs_rect.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColorFilter.h"
#include "include/core/SkImage.h"

namespace OHOS {
namespace Rosen {
enum class ImageRepeat {
    NO_REPEAT = 0,
    REPEAT_X,
    REPEAT_Y,
    REPEAT,
};

enum class ImageFit {
    FILL,
    CONTAIN,
    COVER,
    FIT_WIDTH,
    FIT_HEIGHT,
    NONE,
    SCALE_DOWN,
};

class RSImage {
public:
    RSImage() = default;
    ~RSImage() = default;

    void CanvasDrawImage(SkCanvas& canvas, const SkRect& rect, const SkPaint& paint, bool isBackground = false);
    void SetImage(const sk_sp<SkImage> image);
    void SetDstRect(const RectF& dstRect);
    void SetImageFit(int fitNum);
    void SetImageRepeat(int repeatNum);
    void SetRadius(float radius);

private:
    void ApplyImageFit();
    void ApplyCanvasClip(SkCanvas& canvas);
    void DrawImageRepeatRect(const SkPaint& paint, SkCanvas& canvas);

    sk_sp<SkImage> image_;
    ImageFit imageFit_ = ImageFit::COVER;
    ImageRepeat imageRepeat_ = ImageRepeat::NO_REPEAT;
    float cornerRadius_ = 0.0;
    RectF srcRect_;
    RectF dstRect_;
    RectF frameRect_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_IMAGE_H
