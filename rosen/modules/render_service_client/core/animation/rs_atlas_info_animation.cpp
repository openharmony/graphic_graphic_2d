/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "animation/rs_atlas_info_animation.h"

#include "animation/rs_render_atlas_info_animation.h"
#include "command/rs_animation_command.h"
#include "platform/common/rs_log.h"
#include "transaction/rs_transaction_proxy.h"
#include "ui/rs_node.h"

namespace OHOS {
namespace Rosen {

RSAtlasInfoAnimation::RSAtlasInfoAnimation(
    const std::shared_ptr<RSUIContext>& rsUIContext,
    const std::shared_ptr<RSPropertyBase>& property,
    const std::shared_ptr<RSPropertyBase>& startValue, const std::shared_ptr<RSPropertyBase>& endValue)
    : RSAnimation(rsUIContext), startValue_(startValue), endValue_(endValue)
{
    if (property != nullptr) {
        propertyId_ = property->GetId();
    }
}

void RSAtlasInfoAnimation::OnStart()
{
    RSAnimation::OnStart();

    auto animation = CreateRenderAnimation();
    if (animation == nullptr) {
        ROSEN_LOGE("RSAtlasInfoAnimation::OnStart, CreateRenderAnimation failed");
        return;
    }

    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("RSAtlasInfoAnimation::OnStart, target is null");
        return;
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationCreateAtlasInfo>(target->GetId(), animation);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
}

void RSAtlasInfoAnimation::OnUpdateStagingValue(bool isFirstStart)
{
    if (!isFirstStart) {
        return;
    }
}

std::shared_ptr<RSRenderAtlasInfoAnimation> RSAtlasInfoAnimation::CreateRenderAnimation()
{
    // Use GetRenderProperty() to convert RSPropertyBase → RSRenderPropertyBase,
    // same pattern as RSCurveAnimation::CreateRenderAnimation().
    auto startRenderProp = startValue_->GetRenderProperty();
    auto endRenderProp = endValue_->GetRenderProperty();
    if (startRenderProp == nullptr || endRenderProp == nullptr) {
        ROSEN_LOGE("RSAtlasInfoAnimation::CreateRenderAnimation, GetRenderProperty failed");
        return nullptr;
    }
    auto animation = std::make_shared<RSRenderAtlasInfoAnimation>(
        GetId(), GetPropertyId(), startRenderProp, startRenderProp, endRenderProp);
    UpdateParamToRenderAnimation(animation);
    return animation;
}

void RSAtlasInfoAnimation::RebuildInRender()
{
    auto target = GetTarget().lock();
    if (target == nullptr) {
        ROSEN_LOGE("Failed to rebuild atlas info animation, target is null!");
        return;
    }
    auto animation = CreateRenderAnimation();
    if (animation == nullptr) {
        ROSEN_LOGE("RSAtlasInfoAnimation::RebuildInRender, CreateRenderAnimation failed");
        return;
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSAnimationRebuildAtlasInfo>(
            target->GetId(), animation, GetRebuildParam().fraction, GetRebuildParam().isReverseCycle);
    target->AddCommand(command, target->IsRenderServiceNode(), target->GetFollowType(), target->GetId());
    SetRebuildParam({});
}

} // namespace Rosen
} // namespace OHOS
