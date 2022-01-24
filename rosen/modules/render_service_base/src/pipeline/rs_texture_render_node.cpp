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

#include "pipeline/rs_texture_render_node.h"

#include <algorithm>

#include "flutter/flow/ohos_layers/texture_register.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkRect.h"
#include "pipeline/rs_paint_filter_canvas.h"
#include "pipeline/rs_render_thread.h"
#include "platform/common/rs_log.h"
#include "property/rs_properties_painter.h"
//#include "rs_trace.h"

namespace {
std::shared_ptr<flutter::OHOS::TextureRegistry> g_textureRegistry;
}
namespace OHOS {
namespace Rosen {
RSTextureRenderNode::RSTextureRenderNode(NodeId id, std::weak_ptr<RSContext> context) : RSCanvasRenderNode(id, context)
{}

RSTextureRenderNode::~RSTextureRenderNode() {}

void RSTextureRenderNode::SetTextureRegistry(std::shared_ptr<flutter::OHOS::TextureRegistry> registry)
{
    g_textureRegistry = registry;
    // RSRenderThread::Instance().RequestNextVSync();
}

void RSTextureRenderNode::UpdateTexture(int64_t textureId, bool freeze, RectF drawRect)
{
    textureId_ = textureId;
    freeze_ = freeze;
    drawRect_ = drawRect;
}

void RSTextureRenderNode::ProcessRenderContents(RSPaintFilterCanvas& canvas)
{
#ifdef ROSEN_OHOS
    if (g_textureRegistry == nullptr) {
        return;
    }
    auto layerTextureHandle = g_textureRegistry->GetTexture(textureId_).handle_;
    if (layerTextureHandle == 0) {
        return;
    }
    if (!textureLayerPtr || !textureLayerPtr->backingLayer()) {
        return;
    }
    sk_sp<SkImage> layerImage = textureLayerPtr->backingLayer()->getImage();

    if (layerImage == nullptr) {
        return;
    }
    const int layerWidth = textureLayerPtr->backingLayer()->getWidth();
    const int layerHeight = textureLayerPtr->backingLayer()->getHeight();
    SkMatrix layerTransform = textureLayerPtr->backingLayer()->getTransform();
    SkMatrix textureMatrixInv = textureLayerPtr->backingLayer()->getTexTransform();
    SkMatrix flipV;
    flipV.setAll(1, 0, 0, 0, -1, 1, 0, 0, 1);
    textureMatrixInv.preConcat(flipV);
    textureMatrixInv.preScale(1.0f / layerWidth, 1.0f / layerHeight);
    textureMatrixInv.postScale(layerImage->width(), layerImage->height());
    SkMatrix textureMatrix;
    if (!textureMatrixInv.invert(&textureMatrix)) {
        textureMatrix = textureMatrixInv;
    }

    canvas.save();
    canvas.translate(drawRect_.left_, drawRect_.top_);
    canvas.concat(SkMatrix::Concat(layerTransform, textureMatrix));
    SkPaint paint;
    canvas.drawImage(layerImage.get(), 0, 0, &paint);
    canvas.restore();
#endif
}

bool RSTextureRenderNode::IsDirty() const
{
    return ((textureId_ >= 0) && (g_textureRegistry != nullptr) && GetRenderProperties().GetVisible()) ||
           RSCanvasRenderNode::IsDirty();
}
} // namespace Rosen
} // namespace OHOS
