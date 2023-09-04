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

#ifndef DRAWING_SURFACE_H
#define DRAWING_SURFACE_H

#include "impl_interface/surface_impl.h"

#include "draw/canvas.h"
#include "image/bitmap.h"
#include "image/image.h"
#include "utils/drawing_macros.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {

#ifdef ACE_ENABLE_GPU
struct FrameBuffer {
    int width;
    int height;
    int FBOID;
    int Format;
    std::shared_ptr<GPUContext> gpuContext;
    std::shared_ptr<ColorSpace> colorSpace;
};
#endif

class DRAWING_API Surface {
public:
    Surface();
    ~Surface() {}

    /*
     * @brief         Bind raster Surface.
     * @param bitmap  Raster pixel array.
     */
    bool Bind(const Bitmap& bitmap);

#ifdef ACE_ENABLE_GPU
    /*
     * @brief         Bind GPU texture Surface.
     * @param image   In GPU memory as a GPU texture.
     */
    bool Bind(const Image& image);

    /*
     * @brief       Bind
     * @param info  FrameBuffer object info.
     */
    bool Bind(const FrameBuffer& frameBuffer);
#endif

    /*
     * @brief   Gets Canvas that draws into Surface.
     */
    std::shared_ptr<Canvas> GetCanvas();

    /*
     * @brief   Gets Image capturing Surface contents.
     */
    std::shared_ptr<Image> GetImageSnapshot() const;

    /*
     * @brief         Gets Image capturing Surface contents.
     * @param bounds  Bounds.
     *                If bounds extends beyond the Surface, it will be trimmed to just the intersection of it
     *                and the Surface.
     *                If bounds does not intersect the surface, then this returns nullptr.
     *                If bounds == the surface, then this is the same as calling the no-parameter variant.
     */
    std::shared_ptr<Image> GetImageSnapshot(const RectI& bounds) const;

private:
    std::shared_ptr<SurfaceImpl> impl_;
    std::shared_ptr<Canvas> cachedCanvas_;
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
