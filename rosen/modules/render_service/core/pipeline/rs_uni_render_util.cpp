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

#include "rs_uni_render_util.h"

#include "pipeline/rs_base_render_util.h"
#include "pipeline/rs_main_thread.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
void RSUniRenderUtil::DrawBufferOnCanvas(sptr<SurfaceBuffer> buffer, const ColorGamut& dstGamut,
    RSPaintFilterCanvas& canvas, SkRect srcRect, SkRect dstRect)
{
    SkBitmap bitmap;
    std::vector<uint8_t> newBuffer;
    if (!RSBaseRenderUtil::ConvertBufferToBitmap(buffer, newBuffer, dstGamut, bitmap)) {
        RS_LOGE("RSUniRenderUtil::DrawBufferOnCanvas ConvertBufferToBitmap failed");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    canvas.save();
    canvas.drawBitmapRect(bitmap, srcRect, dstRect, &paint);
    canvas.restore();
}

#ifdef RS_ENABLE_EGLIMAGE
void RSUniRenderUtil::DrawImageOnCanvas(BufferInfo& bufferInfo, RSPaintFilterCanvas& canvas, SkRect srcRect,
    SkRect dstRect)
{
    auto renderEngine = RSMainThread::Instance()->GetRenderEngine();
    auto renderContext = renderEngine->GetRenderContext();
    auto eglImageManager =  renderEngine->GetRSEglImageManager();
    sk_sp<SkImage> image;
    if (!RSBaseRenderUtil::ConvertBufferToEglImage(bufferInfo.buffer, eglImageManager, renderContext->GetGrContext(),
        bufferInfo.acquireFence, image)) {
        RS_LOGE("RSUniRenderUtil::DrawImageOnCanvas ConvertBufferToEglImage failed");
        return;
    }

    SkPaint paint;
    paint.setAntiAlias(true);
    canvas.save();
    canvas.drawImageRect(image, srcRect, dstRect, &paint);
    canvas.restore();

    auto consumerSurface = bufferInfo.consumerSurface;
    if (consumerSurface != nullptr) {
        GSError error = consumerSurface->RegisterDeleteBufferListener([eglImageManager] (int32_t bufferId) {
            eglImageManager->UnMapEglImageFromSurfaceBuffer(bufferId);
        });
        if (error != GSERROR_OK) {
            RS_LOGE("RSUniRenderVisitor::DrawImageOnCanvas: fail to register UnMapEglImage callback.");
        }
    }
}
#endif
} // namespace Rosen
} // namespace OHOS
