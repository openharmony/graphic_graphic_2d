/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "rs_render_thread_util.h"

namespace OHOS {
namespace Rosen {
void RSRenderThreadUtil::SrcRectScaleDown(Drawing::Rect& srcRect, const Drawing::Rect& localBounds)
{
    uint32_t newWidth = static_cast<uint32_t>(srcRect.GetWidth());
    uint32_t newHeight = static_cast<uint32_t>(srcRect.GetHeight());
    // Canvas is able to handle the situation when the window is out of screen, using bounds instead of dst.
    uint32_t boundsWidth = static_cast<uint32_t>(localBounds.GetWidth());
    uint32_t boundsHeight = static_cast<uint32_t>(localBounds.GetHeight());

    uint32_t newWidthBoundsHeight = newWidth * boundsHeight;
    uint32_t newHeightBoundsWidth = newHeight * boundsWidth;

    if (newWidthBoundsHeight > newHeightBoundsWidth) {
        newWidth = boundsWidth * newHeight / boundsHeight;
    } else if (newWidthBoundsHeight < newHeightBoundsWidth) {
        newHeight = boundsHeight * newWidth / boundsWidth;
    } else {
        return;
    }

    uint32_t currentWidth = static_cast<uint32_t>(srcRect.GetWidth());
    uint32_t currentHeight = static_cast<uint32_t>(srcRect.GetHeight());
    if (newWidth < currentWidth) {
        // the crop is too wide
        uint32_t dw = currentWidth - newWidth;
        auto halfdw = dw / 2;
        srcRect = Drawing::Rect(srcRect.GetLeft() + static_cast<int32_t>(halfdw), srcRect.GetTop(),
            srcRect.GetLeft() + static_cast<int32_t>(halfdw) + static_cast<int32_t>(newWidth),
            srcRect.GetTop() + srcRect.GetHeight());
    } else {
        // thr crop is too tall
        uint32_t dh = currentHeight - newHeight;
        auto halfdh = dh / 2;
        srcRect = Drawing::Rect(srcRect.GetLeft(), srcRect.GetTop() + static_cast<int32_t>(halfdh),
            srcRect.GetLeft() + srcRect.GetWidth(),
            srcRect.GetTop() + static_cast<int32_t>(halfdh) + static_cast<int32_t>(newHeight));
    }
}

void RSRenderThreadUtil::SrcRectScaleFit(
    const Drawing::Rect& srcRect, Drawing::Rect& dstRect, const Drawing::Rect& localBounds)
{
    uint32_t srcWidth = static_cast<uint32_t>(srcRect.GetWidth());
    uint32_t srcHeight = static_cast<uint32_t>(srcRect.GetHeight());
    float newWidth = 0.0f;
    float newHeight = 0.0f;
    // Canvas is able to handle the situation when the window is out of screen, using bounds instead of dst.
    uint32_t boundsWidth = static_cast<uint32_t>(localBounds.GetWidth());
    uint32_t boundsHeight = static_cast<uint32_t>(localBounds.GetHeight());
    if (boundsWidth == 0 || boundsHeight == 0 || srcWidth == 0 || srcHeight == 0) {
        return;
    }

    if (srcWidth * boundsHeight > srcHeight * boundsWidth) {
        newWidth = boundsWidth;
        newHeight = srcHeight * newWidth / srcWidth;
    } else if (srcWidth * boundsHeight < srcHeight * boundsWidth) {
        newHeight = boundsHeight;
        newWidth = newHeight * srcWidth / srcHeight;
    } else {
        newWidth = boundsWidth;
        newHeight = boundsHeight;
    }
    newHeight = newHeight * srcHeight / boundsHeight;
    newWidth = newWidth * srcWidth / boundsWidth;
    if (newWidth < srcWidth) {
        float halfdw = (srcWidth - newWidth) / 2;
        dstRect = Drawing::Rect(srcRect.GetLeft() + halfdw, srcRect.GetTop(), srcRect.GetLeft() + halfdw + newWidth,
            srcRect.GetTop() + srcRect.GetHeight());
    } else if (newHeight < srcHeight) {
        float halfdh = (srcHeight - newHeight) / 2;
        dstRect = Drawing::Rect(srcRect.GetLeft(), srcRect.GetTop() + halfdh, srcRect.GetLeft() + srcRect.GetWidth(),
            srcRect.GetTop() + halfdh + newHeight);
    }
}
} // namespace Rosen
} // namespace OHOS