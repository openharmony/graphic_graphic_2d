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
#ifndef RENDER_SERVICE_BASE_RENDER_RS_EFFECT_LUMINANCE_MANAGER_H
#define RENDER_SERVICE_BASE_RENDER_RS_EFFECT_LUMINANCE_MANAGER_H

#include "common/rs_common_def.h"
#include "draw/color.h"
#include "effect/rs_render_filter_base.h"
#include "effect/rs_render_shader_base.h"

namespace OHOS {
namespace Rosen {

constexpr float EFFECT_MAX_LUMINANCE = 2.0f;

class RSB_EXPORT RSEffectLuminanceManager {
public:
    static RSEffectLuminanceManager& GetInstance();

    virtual ~RSEffectLuminanceManager() = default;

    void SetDisplayHeadroom(NodeId id, float brightness);

    float GetDisplayHeadroom(NodeId id) const;

    static bool GetEnableHdrEffect(std::shared_ptr<RSNGRenderFilterBase> renderFilter);

    static bool GetEnableHdrEffect(std::shared_ptr<RSNGRenderShaderBase> renderShader);

    void SetCurrentScreenId(ScreenId id);

    ScreenId GetCurrentScreenId() const;

    void SetCurrentScreenshotType(RSPaintFilterCanvas::ScreenshotType type);

    RSPaintFilterCanvas::ScreenshotType GetCurrentScreenshotType() const;

    void SetHdrPipelineStatus(bool isHdrOn);

    bool GetHdrPipelineStatus() const;

private:
    RSEffectLuminanceManager() = default;

    std::map<uint64_t, float> displayHeadroomMap_;
    std::atomic<ScreenId> currentScreenId_ = Rosen::INVALID_SCREEN_ID;
    std::atomic<RSPaintFilterCanvas::ScreenshotType> currentScreenshotType_ =
        RSPaintFilterCanvas::ScreenshotType::NON_SHOT;
    std::atomic<bool> isHdrPipelineOn_ = false;
};
} // Rosen
} // OHOS
#endif // RENDER_SERVICE_BASE_RENDER_RS_EFFECT_LUMINANCE_MANAGER_H
