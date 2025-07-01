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

#include "animation/rs_animation_trace_utils.h"

#include <sstream>

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr const char* ANIMATION_TRACE_ENABLE_NAME = "persist.rosen.animationtrace.enabled";
}
bool RSAnimationTraceUtils::isDebugEnabled_ = false;

RSAnimationTraceUtils::RSAnimationTraceUtils()
{
    isDebugEnabled_ = RSSystemProperties::GetAnimationTraceEnabled();
    RSSystemProperties::WatchSystemProperty(
        ANIMATION_TRACE_ENABLE_NAME, OnAnimationTraceEnabledChangedCallback, nullptr);
}

void RSAnimationTraceUtils::OnAnimationTraceEnabledChangedCallback(const char* key, const char* value, void* context)
{
    if (strcmp(key, ANIMATION_TRACE_ENABLE_NAME) != 0) {
        return;
    }
    isDebugEnabled_ = (std::string_view(value) == "1");
}

std::string RSAnimationTraceUtils::GetColorString(const Color& value) const
{
    std::string colorString;
    value.Dump(colorString);
    return colorString;
}

std::string RSAnimationTraceUtils::ParseRenderPropertyValueInner(
    const std::shared_ptr<RSRenderPropertyBase>& value) const
{
    std::string str;
    auto propertyType = value->GetPropertyType();
    switch (propertyType) {
        case RSPropertyType::FLOAT: {
            str = "float:" + std::to_string(std::static_pointer_cast<RSRenderAnimatableProperty<float>>(value)->Get());
            break;
        }
        case RSPropertyType::RS_COLOR: {
            str = GetColorString(std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(value)->Get());
            break;
        }
        case RSPropertyType::QUATERNION: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Quaternion>>(value)->Get();
            str = "Quaternion:x:" + std::to_string(property.x_) + "," + "y:" + std::to_string(property.y_) + "," +
                  "z:" + std::to_string(property.z_) + "," + "w:" + std::to_string(property.w_);
            break;
        }
        case RSPropertyType::VECTOR2F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(value)->Get();
            str = "Vector2f:x:" + std::to_string(property.x_) + "," + "y:" + std::to_string(property.y_);
            break;
        }
        case RSPropertyType::VECTOR4F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(value)->Get();
            str = "Vector4f:x:" + std::to_string(property.x_) + "," + "y:" + std::to_string(property.y_) + "," +
                  "z:" + std::to_string(property.z_) + "," + "w:" + std::to_string(property.w_);
            break;
        }
        case RSPropertyType::VECTOR4_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4<Color>>>(value)->Get();
            str = "Vector4<Color>:x:" + GetColorString(property.x_) + "," + "y:" + GetColorString(property.y_) + "," +
                  "z:" + GetColorString(property.z_) + "," + "w:" + GetColorString(property.w_);
            break;
        }
        case RSPropertyType::RRECT: {
            str = "RRECT " + std::static_pointer_cast<RSRenderAnimatableProperty<RRect>>(value)->Get().ToString();
            break;
        }
        default: {
            str = "None";
            break;
        }
    }
    return str;
}

std::string RSAnimationTraceUtils::ParseRenderPropertyValue(const std::shared_ptr<RSRenderPropertyBase>& value) const
{
    if (value == nullptr) {
        return {};
    }
    // Cyclomatic complexity exceeds 20
    return ParseRenderPropertyValueInner(value);
}

void RSAnimationTraceUtils::AddAnimationNameTrace(const std::string& str) const
{
    if (isDebugEnabled_) {
        RS_TRACE_NAME(str.c_str());
    }
}

void RSAnimationTraceUtils::AddAnimationCancelTrace(const uint64_t nodeId, const uint64_t propertyId) const
{
    if (isDebugEnabled_) {
        RS_TRACE_NAME_FMT("CancelAnimationByProperty node[%llu] pro[%llu]", nodeId, propertyId);
    }
}

void RSAnimationTraceUtils::AddChangeAnimationValueTrace(
    const uint64_t propertyId, const std::shared_ptr<RSRenderPropertyBase>& endValue) const
{
    if (isDebugEnabled_) {
        auto endStr = ParseRenderPropertyValue(endValue);
        RS_TRACE_NAME_FMT("animation value be changed pro[%llu] endValue[%s]", propertyId, endStr.c_str());
    }
}

void RSAnimationTraceUtils::AddAnimationFinishTrace(
    const std::string info, const uint64_t nodeId, const uint64_t animationId, bool isAddLogInfo) const
{
    if (isDebugEnabled_ || OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled()) {
        RS_TRACE_NAME_FMT("%s node[%llu] animate[%llu]", info.c_str(), nodeId, animationId);
        if (isAddLogInfo) {
            ROSEN_LOGI("%{public}s node[%{public}" PRIu64 "] animate[%{public}" PRIu64 "]",
                info.c_str(), nodeId, animationId);
        }
    }
}

std::string RSAnimationTraceUtils::GetModifierTypeString(RSModifierType type) const
{
    auto modifierTypeString = std::make_shared<RSModifierTypeString>();
    return modifierTypeString->GetModifierTypeString(type);
}

std::string RSAnimationTraceUtils::GetAnimationTypeString(ImplicitAnimationParamType type) const
{
    switch (type) {
        case ImplicitAnimationParamType::INVALID:
            return "Invalid";
        case ImplicitAnimationParamType::CURVE:
            return "Curve";
        case ImplicitAnimationParamType::KEYFRAME:
            return "Keyframe";
        case ImplicitAnimationParamType::PATH:
            return "Path";
        case ImplicitAnimationParamType::SPRING:
            return "Spring";
        case ImplicitAnimationParamType::INTERPOLATING_SPRING:
            return "InterpolatingSpring";
        case ImplicitAnimationParamType::TRANSITION:
            return "Transition";
        case ImplicitAnimationParamType::CANCEL:
            return "Cancel";
        default:
            return "Invalid";
    }
}

std::string RSAnimationTraceUtils::GetNodeTypeString(RSUINodeType type) const
{
    switch (type) {
        case RSUINodeType::UNKNOW:
            return "UNKNOW";
        case RSUINodeType::DISPLAY_NODE:
            return "DisplayNode";
        case RSUINodeType::RS_NODE:
            return "RsNode";
        case RSUINodeType::SURFACE_NODE:
            return "SurfaceNode";
        case RSUINodeType::PROXY_NODE:
            return "ProxyNode";
        case RSUINodeType::CANVAS_NODE:
            return "CanvasNode";
        case RSUINodeType::ROOT_NODE:
            return "RootNode";
        case RSUINodeType::EFFECT_NODE:
            return "EffectNode";
        case RSUINodeType::CANVAS_DRAWING_NODE:
            return "CanvasDrawingNode";
        default:
            return "UNKNOW";
    }
}

void RSAnimationTraceUtils::AddAnimationCallFinishTrace(
    const uint64_t nodeId, const uint64_t animationId, RSModifierType type, bool isAddLogInfo) const
{
    if (!isDebugEnabled_ && !OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled()) {
        return;
    }
    RS_TRACE_NAME_FMT("Animation Call FinishCallback node[%llu] animate[%llu] propertyType[%s]", nodeId, animationId,
        GetModifierTypeString(type).c_str());
    if (isAddLogInfo) {
        ROSEN_LOGI("Animation Call FinishCallback node[%{public}" PRIu64 "] animate[%{public}" PRIu64 "]"
                   "propertyType[%{public}s]",
            nodeId, animationId, GetModifierTypeString(type).c_str());
    }
}

void RSAnimationTraceUtils::AddAnimationCallFinishTrace(
    const uint64_t nodeId, const uint64_t animationId, ModifierNG::RSPropertyType propertyType, bool isAddLogInfo) const
{
    if (!isDebugEnabled_ && !OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled()) {
        return;
    }
    auto propertyTypeStr = ModifierNG::RSModifierTypeString::GetPropertyTypeString(propertyType);
    RS_TRACE_NAME_FMT("Animation Call FinishCallback node[%llu] animate[%llu] propertyType[%s]", nodeId, animationId,
        propertyTypeStr.c_str());
    if (isAddLogInfo) {
        ROSEN_LOGI("Animation Call FinishCallback node[%{public}" PRIu64 "] animate[%{public}" PRIu64 "]"
                   "propertyType[%{public}s]",
            nodeId, animationId, propertyTypeStr.c_str());
    }
}

void RSAnimationTraceUtils::AddAnimationCreateTrace(const uint64_t nodeId, const std::string& nodeName,
    const uint64_t propertyId, const uint64_t animationId, const ImplicitAnimationParamType animationType,
    const RSModifierType propertyType, const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue, const int animationDelay, const int animationDur,
    const int repeat, const std::string& interfaceName, const int32_t frameNodeId, const std::string& frameNodeTag,
    RSUINodeType nodeType) const
{
    if (!isDebugEnabled_) {
        return;
    }

    std::ostringstream oss;
    oss << "CreateImplicitAnimation node[" << nodeId << "]";

    if (!nodeName.empty()) {
        oss << " name[" << nodeName << "]";
    }

    oss << " animate[" << animationId << "] animateType[" << GetAnimationTypeString(animationType) << "] dur["
        << animationDur << "]";

    if (animationDelay != 0) {
        oss << " delay[" << animationDelay << "]";
    }

    if (repeat != 1) {
        oss << " repeat[" << repeat << "]";
    }

    if (!interfaceName.empty()) {
        oss << " interfaceName[" << interfaceName << "]";
    }

    oss << " frameNodeId[" << frameNodeId << "] frameNodeTag[" << frameNodeTag << "] nodeType["
        << GetNodeTypeString(nodeType) << "]";

    RS_TRACE_NAME_FMT("%s", oss.str().c_str());

    std::ostringstream propertyOss;
    propertyOss << "CreateImplicitAnimation pro[" << propertyId << "] propertyType["
                << GetModifierTypeString(propertyType) << "] startValue[" << ParseRenderPropertyValue(startValue)
                << "] endValue[" << ParseRenderPropertyValue(endValue) << "]";

    RS_TRACE_NAME_FMT("%s", propertyOss.str().c_str());
}

void RSAnimationTraceUtils::AddAnimationCreateTrace(const uint64_t nodeId, const std::string& nodeName,
    const uint64_t propertyId, const uint64_t animationId, const ImplicitAnimationParamType animationType,
    const ModifierNG::RSPropertyType propertyType, const std::shared_ptr<RSRenderPropertyBase>& startValue,
    const std::shared_ptr<RSRenderPropertyBase>& endValue, const int animationDelay, const int animationDur,
    const int repeat, const std::string& interfaceName, const int32_t frameNodeId, const std::string& frameNodeTag,
    RSUINodeType nodeType) const
{
    if (!isDebugEnabled_) {
        return;
    }

    std::ostringstream oss;
    oss << "CreateImplicitAnimation node[" << nodeId << "]";

    if (!nodeName.empty()) {
        oss << " name[" << nodeName << "]";
    }

    oss << " animate[" << animationId << "] animateType[" << GetAnimationTypeString(animationType) << "] dur["
        << animationDur << "]";

    if (animationDelay != 0) {
        oss << " delay[" << animationDelay << "]";
    }

    if (repeat != 1) {
        oss << " repeat[" << repeat << "]";
    }

    if (!interfaceName.empty()) {
        oss << " interfaceName[" << interfaceName << "]";
    }

    oss << " frameNodeId[" << frameNodeId << "] frameNodeTag[" << frameNodeTag << "] nodeType["
        << GetNodeTypeString(nodeType) << "]";

    RS_TRACE_NAME_FMT("%s", oss.str().c_str());

    std::ostringstream propertyOss;
    propertyOss << "CreateImplicitAnimation pro[" << propertyId << "] propertyType["
                << ModifierNG::RSModifierTypeString::GetPropertyTypeString(propertyType) << "] startValue["
                << ParseRenderPropertyValue(startValue) << "] endValue[" << ParseRenderPropertyValue(endValue) << "]";

    RS_TRACE_NAME_FMT("%s", propertyOss.str().c_str());
}

void RSAnimationTraceUtils::AddAnimationFrameTrace(const RSRenderNode* target, const uint64_t animationId,
    const uint64_t propertyId, const float fraction, const std::shared_ptr<RSRenderPropertyBase>& value,
    const int64_t time, const int dur, const int repeat) const
{
    if (!isDebugEnabled_ && !OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled()) {
        return;
    }

    auto propertyValue = ParseRenderPropertyValue(value);
    uint64_t nodeId = 0;
    std::string nodeName = "";
    bool isOnTheTree = false;
    if (target != nullptr) {
        nodeId = target->GetId();
        nodeName = target->GetNodeName();
        isOnTheTree = target->IsOnTheTree();
    }

    std::ostringstream oss;
    oss << "frame animation node[" << nodeId << "]";

    if (!nodeName.empty()) {
        oss << " name[" << nodeName << "]";
    }

    if (!isOnTheTree) {
        oss << " onTree[" << isOnTheTree << "]";
    }

    oss << " pro[" << propertyId << "]";
    oss << " animate[" << animationId << "]";
    oss << " fraction[" << fraction << "]";
    oss << " value[" << propertyValue << "]";
    oss << " time[" << time << "]";
    oss << " dur[" << dur << "]";

    if (repeat != 1) {
        oss << " repeat[" << repeat << "]";
    }

    RS_TRACE_NAME_FMT("%s", oss.str().c_str());
}

void RSAnimationTraceUtils::AddSpringInitialVelocityTrace(const uint64_t propertyId, const uint64_t animationId,
    const std::shared_ptr<RSRenderPropertyBase>& initialVelocity,
    const std::shared_ptr<RSRenderPropertyBase>& value) const
{
    if (isDebugEnabled_) {
        auto propertyValue = ParseRenderPropertyValue(initialVelocity);
        RS_TRACE_NAME_FMT("spring pro[%llu] animate[%llu], initialVelocity[%s]",
            propertyId, animationId, propertyValue.c_str());
    }
}

} // namespace Rosen
} // namespace OHOS