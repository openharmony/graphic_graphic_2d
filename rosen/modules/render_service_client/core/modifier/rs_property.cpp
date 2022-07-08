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

#include "modifier/rs_property.h"

#include "animation/rs_implicit_animator.h"
#include "animation/rs_implicit_animator_map.h"
#include "command/rs_node_command.h"
#include "transaction/rs_transaction_proxy.h"
#include "pipeline/rs_node_map.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr int PID_SHIFT = 32;

PropertyId GenerateId()
{
    static pid_t pid_ = getpid();
    static std::atomic<uint32_t> currentId_ = 1;

    ++currentId_;
    if (currentId_ == UINT32_MAX) {
        // [PLANNING]:process the overflow situations
        ROSEN_LOGE("Property Id overflow");
    }

    return ((PropertyId)pid_ << PID_SHIFT) | currentId_;
}

template<typename T>
bool IsValid(const T& value)
{
    return true;
}
template<>
bool IsValid(const float& value)
{
    return !isinf(value);
}
template<>
bool IsValid(const Vector2f& value)
{
    return !value.IsInfinite();
}
template<>
bool IsValid(const Vector4f& value)
{
    return !value.IsInfinite();
}

template<typename T>
T CheckAndAdd(const T& value1, const T& value2)
{
    ROSEN_LOGE("not support +");
    return value2;
}

template<typename T, void (T::*)(const T& value) = &T::operator+>
T CheckAndAdd(const T& value1, const T& value2)
{
    ROSEN_LOGE("can support +");
    return value1 + value2;
}
} // namespace

template<typename T>
RSProperty<T>::RSProperty() : id_(GenerateId()) {}

template<typename T>
RSProperty<T>::RSProperty(const T& value) : animatingValue_(value), stagingValue_(value), id_(GenerateId()) {}

template<typename T>
RSProperty<T>& RSProperty<T>::operator+=(const T& value)
{
    ROSEN_LOGE("check and add support +");
    Set(CheckAndAdd<T>(Get(), value));
    return *this;
}

template<typename T>
void RSProperty<T>::Set(const T& value)
{
    if (value == stagingValue_ || !IsValid(value)) {
        return;
    }

    if (!hasAddToNode_) {
        stagingValue_ = value;
        return;
    }

    auto node = RSNodeMap::Instance().GetNode<RSNode>(nodeId_);
    if (node == nullptr) {
        return;
    }
    stagingValue_ = value;
    UpdateToRender(stagingValue_, false);
}

template<typename T>
void RSAnimatableProperty<T>::Set(const T& value)
{
    if (value == this->stagingValue_ || !IsValid(value)) {
        return;
    }

    if (!this->hasAddToNode_) {
        this->stagingValue_ = value;
        return;
    }

    auto node = RSNodeMap::Instance().GetNode<RSNode>(this->nodeId_);
    if (node == nullptr) {
        return;
    }
    auto implicitAnimator = RSImplicitAnimatorMap::Instance().GetAnimator(gettid());
    if (implicitAnimator && implicitAnimator->NeedImplicitAnimation()) {
        if (this->motionPathOption_ != nullptr) {
            implicitAnimator->BeginImplicitPathAnimation(this->motionPathOption_);
            implicitAnimator->CreateImplicitAnimation(node, *this, this->stagingValue_, value);
            implicitAnimator->EndImplicitPathAnimation();
        } else {
            implicitAnimator->CreateImplicitAnimation(node, *this, this->stagingValue_, value);
        }
        return;
    }
    bool hasPropertyAnimation = !this->runningPathNum_ && node->HasPropertyAnimation(this->id_);
    T sendValue = value;
    if (hasPropertyAnimation) {
        if (this->motionPathOption_ != nullptr) {
            node->FinishAnimationByProperty(this->id_);
        } else {
            sendValue = value - this->stagingValue_;
        }
    }
    this->stagingValue_ = value;
    this->UpdateToRender(sendValue, hasPropertyAnimation);
}

#define UPDATE_TO_RENDER(Command, Type, value, isDelta)                                                          \
    do {                                                                                                         \
        std::unique_ptr<RSCommand> command = std::make_unique<Command>(                                          \
                nodeId_, std::make_shared<RSAnimatableRenderProperty<Type>>(value, id_), isDelta);               \
        auto transactionProxy = RSTransactionProxy::GetInstance();                                               \
        auto node = RSNodeMap::Instance().GetNode<RSNode>(nodeId_);                                              \
        if (transactionProxy && node) {                                                                          \
            transactionProxy->AddCommand(command, node->IsRenderServiceNode());                                  \
            if (node->NeedForcedSendToRemote()) {                                                                \
                std::unique_ptr<RSCommand> commandForRemote = std::make_unique<Command>(                         \
                    nodeId_, std::make_shared<RSAnimatableRenderProperty<Type>>(value, id_), isDelta);           \
                transactionProxy->AddCommand(commandForRemote, true);                                            \
            }                                                                                                    \
        }                                                                                                        \
    } while (0)

template<>
void RSProperty<bool>::UpdateToRender(const bool& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierBool, bool, value, isDelta);
}
template<>
void RSProperty<float>::UpdateToRender(const float& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierFloat, float, value, isDelta);
}
template<>
void RSProperty<int>::UpdateToRender(const int& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierInt, int, value, isDelta);
}
template<>
void RSProperty<Color>::UpdateToRender(const Color& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierColor, Color, value, isDelta);
}
template<>
void RSProperty<Gravity>::UpdateToRender(const Gravity& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierGravity, Gravity, value, isDelta);
}
template<>
void RSProperty<Matrix3f>::UpdateToRender(const Matrix3f& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierMatrix3f, Matrix3f, value, isDelta);
}
template<>
void RSProperty<Quaternion>::UpdateToRender(const Quaternion& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierQuaternion, Quaternion, value, isDelta);
}
template<>
void RSProperty<std::shared_ptr<RSFilter>>::UpdateToRender(const std::shared_ptr<RSFilter>& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierFilter, std::shared_ptr<RSFilter>, value, isDelta);
}
template<>
void RSProperty<std::shared_ptr<RSImage>>::UpdateToRender(const std::shared_ptr<RSImage>& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierImage, std::shared_ptr<RSImage>, value, isDelta);
}
template<>
void RSProperty<std::shared_ptr<RSMask>>::UpdateToRender(const std::shared_ptr<RSMask>& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierMask, std::shared_ptr<RSMask>, value, isDelta);
}
template<>
void RSProperty<std::shared_ptr<RSPath>>::UpdateToRender(const std::shared_ptr<RSPath>& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierPath, std::shared_ptr<RSPath>, value, isDelta);
}
template<>
void RSProperty<std::shared_ptr<RSShader>>::UpdateToRender(const std::shared_ptr<RSShader>& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierShader, std::shared_ptr<RSShader>, value, isDelta);
}
template<>
void RSProperty<Vector2f>::UpdateToRender(const Vector2f& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierVector2f, Vector2f, value, isDelta);
}
template<>
void RSProperty<Vector4<uint32_t>>::UpdateToRender(const Vector4<uint32_t>& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierBorderStyle, Vector4<uint32_t>, value, isDelta);
}
template<>
void RSProperty<Vector4<Color>>::UpdateToRender(const Vector4<Color>& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierVector4Color, Vector4<Color>, value, isDelta);
}
template<>
void RSProperty<Vector4f>::UpdateToRender(const Vector4f& value, bool isDelta) const
{
    UPDATE_TO_RENDER(RSUpdateModifierVector4f, Vector4f, value, isDelta);
}
template<>
void RSProperty<std::shared_ptr<RSAnimatableBase>>::UpdateToRender(
    const std::shared_ptr<RSAnimatableBase>& value, bool isDelta) const
{}

template class RS_EXPORT RSProperty<bool>;
template class RS_EXPORT RSProperty<float>;
template class RS_EXPORT RSProperty<int>;
template class RS_EXPORT RSProperty<Color>;
template class RS_EXPORT RSProperty<Gravity>;
template class RS_EXPORT RSProperty<Matrix3f>;
template class RS_EXPORT RSProperty<Quaternion>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSFilter>>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSImage>>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSMask>>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSPath>>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSShader>>;
template class RS_EXPORT RSProperty<Vector2f>;
template class RS_EXPORT RSProperty<Vector4<uint32_t>>;
template class RS_EXPORT RSProperty<Vector4<Color>>;
template class RS_EXPORT RSProperty<Vector4f>;
template class RS_EXPORT RSProperty<std::shared_ptr<RSAnimatableBase>>;

template class RS_EXPORT RSAnimatableProperty<float>;
template class RS_EXPORT RSAnimatableProperty<Color>;
template class RS_EXPORT RSAnimatableProperty<Matrix3f>;
template class RS_EXPORT RSAnimatableProperty<Quaternion>;
template class RS_EXPORT RSAnimatableProperty<std::shared_ptr<RSFilter>>;
template class RS_EXPORT RSAnimatableProperty<Vector2f>;
template class RS_EXPORT RSAnimatableProperty<Vector4<Color>>;
template class RS_EXPORT RSAnimatableProperty<Vector4f>;
template class RS_EXPORT RSAnimatableProperty<std::shared_ptr<RSAnimatableBase>>;
} // namespace Rosen
} // namespace OHOS
