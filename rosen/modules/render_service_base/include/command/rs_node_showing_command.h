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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_SHOWING_COMMAND_H
#define ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_SHOWING_COMMAND_H

#include "command/rs_command.h"
#include "command/rs_command_factory.h"
#include "common/rs_common_def.h"
#include "pipeline/rs_context.h"
#include "pipeline/rs_render_node.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS {
namespace Rosen {

enum RSNodeShowingCommandType : uint16_t {
    GET_RENDER_PROPERTY,
};

template<uint16_t commandType, uint16_t commandSubType>
class RSB_EXPORT RSGetRenderPropertyAndCancelAnimationTask : public RSSyncTask {
public:
    explicit RSGetRenderPropertyAndCancelAnimationTask(
        NodeId targetId, std::shared_ptr<RSRenderPropertyBase> property, uint64_t timeoutNS = 1e8)
        : RSSyncTask(timeoutNS), targetId_(targetId), property_(property)
    {}

    virtual ~RSGetRenderPropertyAndCancelAnimationTask() = default;

#ifdef ROSEN_OHOS
    bool Marshalling(Parcel& parcel) const override
    {
        auto success = RSMarshallingHelper::Marshalling(parcel, commandType);
        success &= RSMarshallingHelper::Marshalling(parcel, commandSubType);
        success &= RSMarshallingHelper::Marshalling(parcel, targetId_);
        success &= RSMarshallingHelper::Marshalling(parcel, timeoutNS_);
        success &= RSMarshallingHelper::Marshalling(parcel, result_);
        if (property_ != nullptr) {
            success &= RSRenderPropertyBase::Marshalling(parcel, property_);
        }

        return success;
    }

    static RSCommand* Unmarshalling(Parcel& parcel)
    {
        NodeId targetId;
        std::shared_ptr<RSRenderPropertyBase> property;
        uint64_t timeoutNS;
        if (!(RSMarshallingHelper::Unmarshalling(parcel, targetId) &&
                RSMarshallingHelper::Unmarshalling(parcel, timeoutNS))) {
            return nullptr;
        }
        auto command = new RSGetRenderPropertyAndCancelAnimationTask(targetId, property, timeoutNS);
        if (!command->ReadFromParcel(parcel)) {
            delete command;
            return nullptr;
        }
        return command;
    }

    bool CheckHeader(Parcel& parcel) const override
    {
        uint16_t type;
        uint16_t subType;
        uint64_t timeoutNS;
        NodeId targetId;
        return RSMarshallingHelper::Unmarshalling(parcel, type) && type == commandType &&
               RSMarshallingHelper::Unmarshalling(parcel, subType) && subType == commandSubType &&
               RSMarshallingHelper::Unmarshalling(parcel, targetId) && targetId == targetId_ &&
               RSMarshallingHelper::Unmarshalling(parcel, timeoutNS) && timeoutNS == timeoutNS_;
    }

    bool ReadFromParcel(Parcel& parcel) override
    {
        auto success = RSMarshallingHelper::Unmarshalling(parcel, result_);
        success &= RSRenderPropertyBase::Unmarshalling(parcel, property_);

        return success;
    }

    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
#else
    bool Marshalling(Parcel& parcel) const override
    {
        return false;
    }

    bool CheckHeader(Parcel& parcel) const override
    {
        return false;
    }

    bool ReadFromParcel(Parcel& parcel) override
    {
        return false;
    }
#endif // ROSEN_OHOS
    void Process(RSContext& context) override
    {
        auto& nodeMap = context.GetNodeMap();
        auto node = nodeMap.GetRenderNode<RSRenderNode>(targetId_);
        if (!node || !property_) {
            result_ = false;
            return;
        }
        auto modifier = node->GetModifier(property_->GetId());
        if (!modifier) {
            result_ = false;
            return;
        }
        property_ = modifier->GetProperty();
        result_ = (property_ == nullptr) ? false : true;
        if (result_) {
            auto& animationManager = node->GetAnimationManager();
            animationManager.CancelAnimationByPropertyId(property_->GetId());
        }
    }

    std::shared_ptr<RSRenderPropertyBase> GetProperty() const
    {
        return property_;
    }

private:
    NodeId targetId_ = 0;
    std::shared_ptr<RSRenderPropertyBase> property_;
};

#ifndef ARG
#define ARG(...) __VA_ARGS__
#endif

#ifdef ROSEN_INSTANTIATE_COMMAND_TEMPLATE
#define ADD_SHOWING_COMMAND(ALIAS, TYPE)                           \
    using ALIAS = RSGetRenderPropertyAndCancelAnimationTask<TYPE>; \
    template class RSGetRenderPropertyAndCancelAnimationTask<TYPE>
#else

#define ADD_SHOWING_COMMAND(ALIAS, TYPE) using ALIAS = RSGetRenderPropertyAndCancelAnimationTask<TYPE>
#endif

ADD_SHOWING_COMMAND(
    RSNodeGetShowingPropertyAndCancelAnimation, ARG(RS_NODE_SYNCHRONOUS_READ_PROPERTY, GET_RENDER_PROPERTY));
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_SHOWING_COMMAND_H