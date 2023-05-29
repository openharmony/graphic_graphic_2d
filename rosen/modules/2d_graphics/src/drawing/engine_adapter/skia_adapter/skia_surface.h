/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.. All rights reserved.
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

#ifndef SKIA_SURFACE_H
#define SKIA_SURFACE_H

#include "include/core/SkSurface.h"

#include "impl_interface/surface_impl.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
class SkiaSurface : public SurfaceImpl {
public:
    static inline constexpr AdapterType TYPE = AdapterType::SKIA_ADAPTER;
    SkiaSurface();
    ~SkiaSurface() override = default;

    bool Bind(const Bitmap& bitmap) override;
#ifdef ACE_ENABLE_GPU
    bool Bind(const Image& image) override;
    bool Bind(const FrameBuffer& frameBuffer) override;
#endif

    std::shared_ptr<Canvas> GetCanvas() const override;
    std::shared_ptr<Image> GetImageSnapshot() const override;
    std::shared_ptr<Image> GetImageSnapshot(const RectI& bounds) const override;

private:
    sk_sp<SkSurface> skSurface_ = nullptr;
    sk_sp<SkImage> skImage_ = nullptr;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // SKIA_SURFACE_H
