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

#include "params/rs_rcd_render_params.h"
#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
RSRcdRenderParams::RSRcdRenderParams(NodeId id) : RSRenderParams(id) {}

void RSRcdRenderParams::OnSync(const std::unique_ptr<RSRenderParams>& target)
{
    auto targetRcdRenderParam = static_cast<RSRcdRenderParams*>(target.get());
    if (targetRcdRenderParam == nullptr) {
        RS_LOGE("targetRcdRenderParam::OnSync targetRcdRenderParam is null");
        return;
    }
    targetRcdRenderParam->pathBin_ = pathBin_;
    targetRcdRenderParam->bufferSize_ = bufferSize_;
    targetRcdRenderParam->cldWidth_ = cldWidth_;
    targetRcdRenderParam->cldHeight_ = cldHeight_;
    targetRcdRenderParam->srcRect_ = srcRect_;
    targetRcdRenderParam->dstRect_ = dstRect_;
    targetRcdRenderParam->rcdBitmap_ = rcdBitmap_;
    targetRcdRenderParam->rcdEnabled_ = rcdEnabled_;
    targetRcdRenderParam->resourceChanged_ = resourceChanged_;

    RSRenderParams::OnSync(target);
}

void RSRcdRenderParams::SetPathBin(std::string pathBin)
{
    if (pathBin_ == pathBin) {
        return;
    }
    pathBin_ = pathBin;
    needSync_ = true;
}

std::string RSRcdRenderParams::GetPathBin() const
{
    return pathBin_;
}

void RSRcdRenderParams::SetBufferSize(int bufferSize)
{
    if (bufferSize_ == bufferSize) {
        return;
    }
    bufferSize_ = bufferSize;
    needSync_ = true;
}

int RSRcdRenderParams::GetBufferSize() const
{
    return bufferSize_;
}

void RSRcdRenderParams::SetCldWidth(int cldWidth)
{
    if (cldWidth_ == cldWidth) {
        return;
    }
    cldWidth_ = cldWidth;
    needSync_ = true;
}

int RSRcdRenderParams::GetCldWidth() const
{
    return cldWidth_;
}

void RSRcdRenderParams::SetCldHeight(int cldWidth)
{
    if (cldHeight_ == cldWidth) {
        return;
    }
    cldHeight_ = cldWidth;
    needSync_ = true;
}

int RSRcdRenderParams::GetCldHeight() const
{
    return cldHeight_;
}

void RSRcdRenderParams::SetSrcRect(RectI srcRect)
{
    if (srcRect_ == srcRect) {
        return;
    }
    srcRect_ = srcRect;
    needSync_ = true;
}

RectI RSRcdRenderParams::GetSrcRect() const
{
    return srcRect_;
}

void RSRcdRenderParams::SetDstRect(RectI dstRect)
{
    if (dstRect_ == dstRect) {
        return;
    }
    dstRect_ = dstRect;
    needSync_ = true;
}

RectI RSRcdRenderParams::GetDstRect() const
{
    return dstRect_;
}

void RSRcdRenderParams::SetRcdBitmap(std::shared_ptr<Drawing::Bitmap> rcdBitmap)
{
    if (rcdBitmap_ == rcdBitmap) {
        return;
    }
    rcdBitmap_ = rcdBitmap;
    needSync_ = true;
}

std::shared_ptr<Drawing::Bitmap> RSRcdRenderParams::GetRcdBitmap() const
{
    return rcdBitmap_;
}

void RSRcdRenderParams::SetRcdEnabled(bool rcdEnabled)
{
    if (rcdEnabled_ == rcdEnabled) {
        return;
    }
    rcdEnabled_ = rcdEnabled;
    needSync_ = true;
}

bool RSRcdRenderParams::GetRcdEnabled() const
{
    return rcdEnabled_;
}

void RSRcdRenderParams::SetResourceChanged(bool resourceChanged)
{
    if (resourceChanged_ == resourceChanged) {
        return;
    }
    resourceChanged_ = resourceChanged;
    needSync_ = true;
}

bool RSRcdRenderParams::GetResourceChanged() const
{
    return resourceChanged_;
}
} // namespace OHOS::Rosen
