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

#include "common/rs_obj_abs_geometry.h"
#include "pipeline/rs_render_node.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"
#include "rs_trace.h"

namespace OHOS {
namespace Rosen {
RSAnimationTraceUtils RSAnimationTraceUtils::instance_;

RSAnimationTraceUtils::RSAnimationTraceUtils()
{
    isDebugOpen_ = RSSystemProperties::GetAnimationTraceEnabled();
}

std::string RSAnimationTraceUtils::ParseRenderPropertyVauleInner(
    const std::shared_ptr<RSRenderPropertyBase>& value, const RSPropertyType type) const
{
    std::string str;
    auto propertyType = value->GetPropertyType() == RSPropertyType::INVALID ? type : value->GetPropertyType();
    switch (propertyType) {
        case RSPropertyType::FLOAT: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<float>>(value);
            if (property) {
                str = "float:" + std::to_string(property->Get());
            }
            break;
        }
        case RSPropertyType::RS_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Color>>(value);
            if (property) {
                str = "Color:" + std::to_string(property->Get().AsRgbaInt());
            }
            break;
        }
        case RSPropertyType::MATRIX3F: {
            str = "Matrix3f";
            break;
        }
        case RSPropertyType::QUATERNION: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Quaternion>>(value);
            if (property) {
                str = "Quaternion:x:" + std::to_string(property->Get().x_) + "," +
                      "y:" + std::to_string(property->Get().y_) + "," +
                      "z:" + std::to_string(property->Get().z_) + "," +
                      "w:" + std::to_string(property->Get().w_);
            }
            break;
        }
        case RSPropertyType::VECTOR2F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector2f>>(value);
            if (property) {
                str = "Vector2f:x:" + std::to_string(property->Get().x_) + "," +
                      "y:" + std::to_string(property->Get().y_);
            }
            break;
        }
        case RSPropertyType::VECTOR4F: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4f>>(value);
            if (property) {
                str = "Vector4f:x:" + std::to_string(property->Get().x_) + "," +
                      "y:" + std::to_string(property->Get().y_) + "," +
                      "z:" + std::to_string(property->Get().z_) + "," +
                      "w:" + std::to_string(property->Get().w_);
            }
            break;
        }
        case RSPropertyType::VECTOR4_COLOR: {
            auto property = std::static_pointer_cast<RSRenderAnimatableProperty<Vector4<Color>>>(value);
            if (property) {
                str = "Vector4<Color>:x:" + std::to_string(property->Get().x_.AsRgbaInt()) + "," +
                      "y:" + std::to_string(property->Get().y_.AsRgbaInt()) + "," +
                      "z:" + std::to_string(property->Get().z_.AsRgbaInt()) + "," +
                      "w:" + std::to_string(property->Get().w_.AsRgbaInt());
            }
            break;
        }
        default: {
            str = "None";
            break;
        }
    }
    return str;
}

std::string RSAnimationTraceUtils::ParseRenderPropertyVaule(
    const std::shared_ptr<RSRenderPropertyBase>& value, const RSPropertyType type) const
{
    if (value == nullptr) {
        return std::string();
    }
    // Cyclomatic complexity exceeds 20
    return ParseRenderPropertyVauleInner(value, type);
}

void RSAnimationTraceUtils::addAnimationNameTrace(const std::string str) const
{
    if (isDebugOpen_) {
        RS_TRACE_NAME(str.c_str());
    }
}

void RSAnimationTraceUtils::addAnimationFinishTrace(
    const std::string info, const uint64_t nodeId, const uint64_t animationId, bool isAddLogInfo) const
{
    if (isDebugOpen_ || OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled()) {
        RS_TRACE_NAME_FMT("%s node[%llu] animate[%llu]", info.c_str(), nodeId, animationId);
        if (isAddLogInfo) {
            ROSEN_LOGI("%{public}s node[%{public}" PRIu64 "] animate[%{public}" PRIu64 "]",
                info.c_str(), nodeId, animationId);
        }
    }
}

void RSAnimationTraceUtils::addAnimationCreateTrace(const uint64_t nodeId, const std::string& nodeName,
    const uint64_t propertyId, const uint64_t animationId, const int animationType, const int propertyType,
    const std::shared_ptr<RSRenderPropertyBase>& startValue, const std::shared_ptr<RSRenderPropertyBase>& endValue,
    const int animationDelay, const int animationDur, const int repeat) const
{
    if (isDebugOpen_) {
        auto startStr = ParseRenderPropertyVaule(startValue);
        auto endStr = ParseRenderPropertyVaule(endValue);
        RS_TRACE_NAME_FMT(
            "CreateImplicitAnimation node[%llu] name[%s] pro[%llu] animate[%llu], animateType %d, propertyType %d, "
            "startValue[%s], endValue[%s], delay %d, dur %d repeat %d",
            nodeId, nodeName.c_str(), propertyId, animationId, animationType, propertyType, startStr.c_str(),
            endStr.c_str(), animationDelay, animationDur, repeat);
    }
}

void RSAnimationTraceUtils::addAnimationFrameTrace(const uint64_t nodeId, const std::string& nodeName,
    const uint64_t animationId, const uint64_t propertyId, const float fraction,
    const std::shared_ptr<RSRenderPropertyBase>& value, const int64_t time, const int dur, const int repeat) const
{
    if (isDebugOpen_ || OHOS::Rosen::RSSystemProperties::GetDebugFmtTraceEnabled()) {
        auto propertyValue = ParseRenderPropertyVaule(value);
        RS_TRACE_NAME_FMT("frame animation node[%llu] name[%s] pro[%llu] animate[%llu], fraction %f, value[%s], "
            "time[%lld], dur[%d], repeat[%d]", nodeId, nodeName.c_str(), propertyId, animationId, fraction,
            propertyValue.c_str(), time, dur, repeat);
    }
}

void RSAnimationTraceUtils::addSpringInitialVelocityTrace(const uint64_t propertyId, const uint64_t animationId,
    const std::shared_ptr<RSRenderPropertyBase>& initialVelocity,
    const std::shared_ptr<RSRenderPropertyBase>& value) const
{
    if (isDebugOpen_) {
        auto type = (value == nullptr) ? RSPropertyType::INVALID : value->GetPropertyType();
        auto propertyValue = ParseRenderPropertyVaule(initialVelocity, type);
        RS_TRACE_NAME_FMT("spring pro[%llu] animate[%llu], initialVelocity[%s]",
            propertyId, animationId, propertyValue.c_str());
    }
}

void RSAnimationTraceUtils::addRenderNodeTrace(const RSRenderNode& node, const std::string name) const
{
    if (!isDebugOpen_) {
        return;
    }
    [[maybe_unused]] auto alpha = node.GetGlobalAlpha();
    [[maybe_unused]] auto rotation = node.GetRenderProperties().GetRotation();
    [[maybe_unused]] auto bounds = node.GetRenderProperties().GetBoundsRect().ToString();
    [[maybe_unused]] auto translateX = node.GetRenderProperties().GetTranslateX();
    [[maybe_unused]] auto translateY = node.GetRenderProperties().GetTranslateY();
    [[maybe_unused]] auto absRect =
        std::static_pointer_cast<RSObjAbsGeometry>(node.GetRenderProperties().GetBoundsGeometry())->GetAbsRect()
            .ToString();
    [[maybe_unused]] auto scaleX = node.GetRenderProperties().GetScaleX();
    [[maybe_unused]] auto scaleY = node.GetRenderProperties().GetScaleY();
    [[maybe_unused]] auto cornerRadius = node.GetRenderProperties().GetCornerRadius().x_;
    std::string nameStr;
    if (name.empty()) {
        nameStr = "ProcessCanvasRenderNode:";
    } else {
        nameStr = "ProcessSurfaceRenderNode:name " + name;
    }
    RS_TRACE_NAME_FMT("%s node[%llu] alpha %f rotation %f bounds %s, translateX %f translateY %f absRect %s, " \
                      "scaleX %f scaleY %f cornerRadius %f",
        nameStr.c_str(), node.GetId(), alpha, rotation, bounds.c_str(), translateX, translateY, absRect.c_str(),
        scaleX, scaleY, cornerRadius);
}
} // namespace Rosen
} // namespace OHOS