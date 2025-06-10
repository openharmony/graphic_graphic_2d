/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TRACE_UTILS_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TRACE_UTILS_H

#include "animation/rs_animation_common.h"
#include "animation/rs_render_property_animation.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "modifier/rs_render_property.h"


namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSAnimationTraceUtils {
public:
    static RSAnimationTraceUtils& GetInstance()
    {
        static RSAnimationTraceUtils instance;
        return instance;
    };
    void AddAnimationNameTrace(const std::string& str) const;
    void AddAnimationFinishTrace(
        const std::string info, const uint64_t nodeId, const uint64_t animationId, bool isAddLogInfo) const;
    void AddAnimationCallFinishTrace(
        const uint64_t nodeId, const uint64_t animationId, RSModifierType type, bool isAddLogInfo) const;
    void AddAnimationCreateTrace(const uint64_t nodeId, const std::string& nodeName, const uint64_t propertyId,
        const uint64_t animationId, const ImplicitAnimationParamType animationType, const RSModifierType propertyType,
        const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue,
        const int animationDelay, const int animationDur, const int repeat, const std::string& interfaceName,
        const int32_t frameNodeId, const std::string& frameNodeTag, RSUINodeType nodeType) const;

    void AddAnimationFrameTrace(const RSRenderNode* target, const uint64_t animationId, const uint64_t propertyId,
        const float fraction, const std::shared_ptr<RSRenderPropertyBase>& value, const int64_t time, const int dur,
        const int repeat) const;

    void AddSpringInitialVelocityTrace(const uint64_t propertyId, const uint64_t animationId,
        const std::shared_ptr<RSRenderPropertyBase>& initialVelocity,
        const std::shared_ptr<RSRenderPropertyBase>& value) const;

    std::string ParseRenderPropertyValue(const std::shared_ptr<RSRenderPropertyBase>& value,
        const RSPropertyType type = RSPropertyType::INVALID) const;

    void AddAnimationCancelTrace(const uint64_t nodeId, const uint64_t propertyId) const;
    void AddChangeAnimationValueTrace(
        const uint64_t propertyId, const std::shared_ptr<RSRenderPropertyBase>& endValue) const;

private:
    RSAnimationTraceUtils();
    ~RSAnimationTraceUtils() = default;
    RSAnimationTraceUtils(const RSAnimationTraceUtils&) = delete;
    RSAnimationTraceUtils& operator=(const RSAnimationTraceUtils&) = delete;

    std::string ParseRenderPropertyValueInner(const std::shared_ptr<RSRenderPropertyBase>& value,
        const RSPropertyType type = RSPropertyType::INVALID) const;

    std::string GetColorString(const Color& value) const;

    static void OnAnimationTraceEnabledChangedCallback(const char* key, const char* value, void* context);
    std::string GetModifierTypeString(RSModifierType type) const;
    std::string GetAnimationTypeString(ImplicitAnimationParamType type) const;
    std::string GetNodeTypeString(RSUINodeType type) const;

    static bool isDebugEnabled_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATION_TRACE_UTILS_H