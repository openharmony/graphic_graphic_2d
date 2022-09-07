/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef SK_IMAGE_CHAIN_H
#define SK_IMAGE_CHAIN_H

#include <memory>
#include <vector>
#include <unistd.h>
#include "ec_log.h"
#include "image_source.h"
#include "include/core/SkRect.h"
#include "include/core/SkPath.h"
#include "include/core/SkRRect.h"
#include "include/core/SkColor.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkPixmap.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkPaint.h"
#include "include/core/SkSurface.h"
#include "include/effects/SkImageFilters.h"


namespace OHOS {
namespace Rosen {
class SKImageChain {
public:
    SKImageChain(SkCanvas* canvas, sk_sp<SkImage> image);
    SKImageChain(std::shared_ptr<Media::PixelMap> srcPxielMap);
    virtual ~SKImageChain() = default;
    void Draw();
    void ForceCPU(bool forceCPU);
    void SetFilters(sk_sp<SkImageFilter> filter);
    void SetClipRect(SkRect* rect);
    void SetClipPath(SkPath* path);
    void SetClipRRect(SkRRect* rRect);
    std::shared_ptr<Media::PixelMap> GetPixelMap();

private:
    void InitWithoutCanvas();
    bool CreateCPUCanvas();
    bool CreateGPUCanvas();
    SkColorType PixelFormatConvert(const Media::PixelFormat& pixelFormat);
    SkImageInfo imageInfo_ = {};
    bool forceCPU_ = true;
    SkCanvas* canvas_ = nullptr;
    sk_sp<SkImage> image_ = nullptr;
    SkRect* rect_ = nullptr;
    SkPath* path_ = nullptr;
    SkRRect* rRect_ = nullptr;
    std::shared_ptr<SkPixmap> dstPixmap_ = nullptr;
    std::shared_ptr<Media::PixelMap> srcPixelMap_ = nullptr;
    std::shared_ptr<Media::PixelMap> dstPixelMap_ = nullptr;
    sk_sp<SkImageFilter> filters_ = nullptr;
    sk_sp<SkSurface> gpuSurface_ = nullptr;
    sk_sp<SkSurface> cpuSurface_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS
#endif // IMAGE_CHAIN_H
