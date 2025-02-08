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
#ifndef DDGR_TEST_DTK_DTK_TEST_UTILS_H
#define DDGR_TEST_DTK_DTK_TEST_UTILS_H

#include "draw/surface.h"
#include "platform/ohos/backend/rs_vulkan_context.h"

namespace OHOS {
namespace Rosen {
using TestPlaybackCanvas = Drawing::Canvas;
inline std::shared_ptr<Drawing::Surface> CreateDrawingSurface(int width, int height)
{
    auto drawingContext = RsVulkanContext::GetSingleton().CreateDrawingContext(true);
    std::shared_ptr<Drawing::GPUContext> gpuContext(drawingContext);
    Drawing::ImageInfo info = Drawing::ImageInfo { width, height, Drawing::ColorType::COLORTYPE_RGBA_8888,
        Drawing::AlphaType::ALPHATYPE_PREMUL };
    return Drawing::Surface::MakeRenderTarget(gpuContext.get(), false, info);
}
class TestUtils {
public:
    static void OnDrawPoint(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawPoints(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawLine(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawRect(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawRegion(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawOval(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawRRect(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawDRRect(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawCircle(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawArc(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawPath(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawImageRect(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawImageNine(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawImageLattice(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawTextBlob(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawVertices(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawAtlas(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);
    static void OnDrawPatch(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen, Drawing::Brush& brush,
        Drawing::Image& image);

    static std::vector<std::function<void(TestPlaybackCanvas* playbackCanvas_, Drawing::Pen& pen,
        Drawing::Brush& brush, Drawing::Image& image)>> gDrawFunctions;
};

} // namespace Rosen
} // namespace OHOS
#endif // DDGR_TEST_DTK_DTK_TEST_UTILS_H