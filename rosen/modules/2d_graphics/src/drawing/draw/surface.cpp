/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "draw/surface.h"

#include "impl_factory.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
Surface::Surface() : impl_(ImplFactory::CreateSurfaceImpl()) {}

bool Surface::Bind(const Bitmap& bitmap)
{
    return impl_->Bind(bitmap);
}

#ifdef ACE_ENABLE_GPU
bool Surface::Bind(const Image& image)
{
    return impl_->Bind(image);
}

bool Surface::Bind(const FrameBuffer& frameBuffer)
{
    return impl_->Bind(frameBuffer);
}
#endif

std::shared_ptr<Canvas> Surface::GetCanvas() const
{
    return impl_->GetCanvas();
}

std::shared_ptr<Image> Surface::GetImageSnapshot() const
{
    return impl_->GetImageSnapshot();
}

std::shared_ptr<Image> Surface::GetImageSnapshot(const RectI& bounds) const
{
    return impl_->GetImageSnapshot(bounds);
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
