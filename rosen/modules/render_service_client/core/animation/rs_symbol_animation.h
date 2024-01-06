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
#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_SYMBOL_ANIMATION_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_SYMBOL_ANIMATION_H

#include <string>
#include <map>
#include <vector>
#include "ui/rs_canvas_node.h"
#include "ui/rs_node.h"
#include "common/rs_vector2.h"
#include "common/rs_vector4.h"
#include "modifier/rs_property.h"
#include "modifier/rs_property_modifier.h"
#include "animation/rs_animation_timing_curve.h"
#include "symbol_animation_config.h"
#include "platform/common/rs_log.h"

// #include "rosen_text/export/rosen_text/symbol_animation_config.h"

// #include "SkHMSymbol.h"

namespace OHOS {
namespace Rosen {

const Vector2f CENTER_NODE_COORDINATE = {0.5f,0.5f}; //scale from the center of the node
const int SCALE_ANIMATION_INTERVAL = 1000; // scale animation consists of two animation, the interval between two animation is 1000
const unsigned int UNIT_GROUP = 0;  // the number of group is 0 when AnimationSubType is UNIT
const unsigned int UNIT_PERIOD = 0;  // the number of time period is 0 when AnimationSubType is UNIT
const unsigned int UNIT_NODE = 0;  // the number of node is 0 when AnimationSubType is UNIT


class RSC_EXPORT RSSymbolAnimation {
public:
    RSSymbolAnimation();
    virtual ~RSSymbolAnimation();

    bool SetScaleUnitAnimation(const std::shared_ptr<RSNode>& rsNode);
    bool SetSymbolAnimation(const std::shared_ptr<TextEngine::SymbolAnimationConfig>& symbolAnimationConfig);

    void SetNode(std::shared_ptr<RSNode>& rsNode){
        rsNode_ = rsNode;

        if(rsNode_){
            RS_LOGD("HmSymbol RSSymbolAnimation::SetNode get ID rsnodeid = %{public}lu", rsNode_->GetId());

        }else{
            RS_LOGE("HmSymbol RSSymbolAnimation::SetNode :failed");
        }

    }

private:
    std::shared_ptr<RSNode> rsNode_ = nullptr;

    std::shared_ptr<RSAnimation> ScaleSymbolAnimation(const std::shared_ptr<RSNode>& rsNode,
        const Vector2f& scaleValueBegin=Vector2f{0.f,0.f}, const Vector2f& scaleValue=Vector2f{0.f,0.f},
        const Vector2f& scaleValueEnd=Vector2f{0.f,0.f}, const int delay = 0);

    bool isEqual(const Vector2f val1, const Vector2f val2);
    RSAnimationTimingCurve SetScaleSpringTimingCurve();

    template<typename T>
    bool CreateOrSetModifierValue(std::shared_ptr<RSAnimatableProperty<T>>& property, const T& value);

    RSAnimationTimingProtocol scaleProtocol_;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleStartProperty_;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleProperty_;
    std::shared_ptr<RSAnimatableProperty<Vector2f>> scaleEndProperty_;



    std::shared_ptr<RSAnimatableProperty<Vector2f>> pivotProperty_;
    std::shared_ptr<RSAnimatableProperty<float>> alphaProperty_;
    std::shared_ptr<RSAnimatableProperty<float>> alphaProperty1_;
    std::shared_ptr<RSAnimatableProperty<float>> alphaProperty2_;
    std::shared_ptr<RSAnimatableProperty<float>> alphaProperty3_;


};
} // namespace Rosen
} // namespace OHOS

#endif
