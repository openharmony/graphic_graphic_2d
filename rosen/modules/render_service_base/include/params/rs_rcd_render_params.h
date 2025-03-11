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

#ifndef RENDER_SERVICE_BASE_PARAMS_RS_RCD_RENDER_PARAMS_H
#define RENDER_SERVICE_BASE_PARAMS_RS_RCD_RENDER_PARAMS_H

#include "params/rs_render_params.h"

namespace OHOS::Rosen {
// remove this when rcd node is replaced by hardware composer node in OH 6.0 rcd refactoring
class RSB_EXPORT RSRcdRenderParams : public RSRenderParams {
public:
    explicit RSRcdRenderParams(NodeId id);
    ~RSRcdRenderParams() override = default;
    void OnSync(const std::unique_ptr<RSRenderParams>& target) override;

    void SetPathBin(std::string pathBin);
    std::string GetPathBin() const;
    void SetBufferSize(int bufferSize);
    int GetBufferSize() const;
    void SetCldWidth(int cldWidth);
    int GetCldWidth() const;
    void SetCldHeight(int cldWidth);
    int GetCldHeight() const;

    void SetSrcRect(RectI srcRect);
    RectI GetSrcRect() const;
    void SetDstRect(RectI dstRect);
    RectI GetDstRect() const;

    void SetRcdBitmap(std::shared_ptr<Drawing::Bitmap> rcdBitmap);
    std::shared_ptr<Drawing::Bitmap> GetRcdBitmap() const;

    void SetRcdEnabled(bool rcdEnabled_);
    bool GetRcdEnabled() const;

    void SetResourceChanged(bool resourceChanged);
    bool GetResourceChanged() const;

private:
    std::string pathBin_;
    int bufferSize_ = 0;
    int cldWidth_ = 0;
    int cldHeight_ = 0;

    RectI srcRect_;
    RectI dstRect_;

    std::shared_ptr<Drawing::Bitmap> rcdBitmap_ = nullptr;

    bool rcdEnabled_ = false;
    bool resourceChanged_ = false;
};
} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_PARAMS_RS_EFFECT_RENDER_PARAMS_H
