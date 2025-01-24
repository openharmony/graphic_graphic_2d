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

#include <map>

#include "command/rs_command.h"
#include "command/rs_command_factory.h"

namespace OHOS {
namespace Rosen {
class RSRenderPropertyBase;

//Each command HAVE TO have UNIQUE ID in ALL HISTORY
//If a command is not used and you want to delete it,
//just COMMENT it - and never use this value anymore
enum RSNodeShowingCommandType : uint16_t {
    GET_RENDER_PROPERTY = 0,
    GET_RENDER_PROPERTIES = 1,
    GET_VALUE_FRACTION = 2,
};

class RSB_EXPORT RSNodeGetShowingPropertyAndCancelAnimation : public RSSyncTask {
    constexpr static uint16_t commandType = RS_NODE_SYNCHRONOUS_READ_PROPERTY;
    constexpr static uint16_t commandSubType = GET_RENDER_PROPERTY;

public:
    explicit RSNodeGetShowingPropertyAndCancelAnimation(
        NodeId targetId, std::shared_ptr<RSRenderPropertyBase> property, uint64_t timeoutNS = 1e8)
        : RSSyncTask(timeoutNS), targetId_(targetId), property_(property)
    {}
    ~RSNodeGetShowingPropertyAndCancelAnimation() override = default;

    bool Marshalling(Parcel& parcel) const override;
    static RSCommand* Unmarshalling(Parcel& parcel);

    bool CheckHeader(Parcel& parcel) const override;
    bool ReadFromParcel(Parcel& parcel) override;

    void Process(RSContext& context) override;
    bool IsCallingPidValid(pid_t callingPid, const RSRenderNodeMap& nodeMap) const override;
    std::shared_ptr<RSRenderPropertyBase> GetProperty() const
    {
        return property_;
    }
    bool IsTimeout() const
    {
        return isTimeout_;
    }
    uint16_t GetType() const override
    {
        return commandType;
    }

private:
    NodeId targetId_ = 0;
    std::shared_ptr<RSRenderPropertyBase> property_;
    bool isTimeout_ = true;
    using Registrar = RSCommandRegister<commandType, commandSubType, Unmarshalling>;
    static Registrar instance_;
};

class RSB_EXPORT RSNodeGetShowingPropertiesAndCancelAnimation : public RSSyncTask {
    constexpr static uint16_t commandType = RS_NODE_SYNCHRONOUS_READ_PROPERTY;
    constexpr static uint16_t commandSubType = GET_RENDER_PROPERTIES;

public:
    using PropertiesMap = std::map<std::pair<NodeId, PropertyId>,
        std::pair<std::shared_ptr<RSRenderPropertyBase>, std::vector<AnimationId>>>;
    explicit RSNodeGetShowingPropertiesAndCancelAnimation(uint64_t timeoutNS, PropertiesMap&& map)
        : RSSyncTask(timeoutNS), propertiesMap_(std::move(map))
    {}
    ~RSNodeGetShowingPropertiesAndCancelAnimation() override = default;

    bool Marshalling(Parcel& parcel) const override;
    static RSCommand* Unmarshalling(Parcel& parcel);

    bool CheckHeader(Parcel& parcel) const override;
    bool ReadFromParcel(Parcel& parcel) override;

    void Process(RSContext& context) override;
    bool IsCallingPidValid(pid_t callingPid, const RSRenderNodeMap& nodeMap) const override;

    const PropertiesMap& GetProperties() const
    {
        return this->propertiesMap_;
    }

    uint16_t GetType() const override
    {
        return commandType;
    }

private:
    RSNodeGetShowingPropertiesAndCancelAnimation(uint64_t timeoutNS): RSSyncTask(timeoutNS) {}
    PropertiesMap propertiesMap_;
    using Registrar = RSCommandRegister<commandType, commandSubType, Unmarshalling>;
    static Registrar instance_;
};

class RSB_EXPORT RSNodeGetAnimationsValueFraction: public RSSyncTask {
    constexpr static uint16_t commandType = RS_NODE_SYNCHRONOUS_GET_VALUE_FRACTION;
    constexpr static uint16_t commandSubType = GET_VALUE_FRACTION;

public:
    explicit RSNodeGetAnimationsValueFraction(uint64_t timeoutNS, NodeId nodeId, AnimationId animationId)
        : RSSyncTask(timeoutNS), nodeId_(nodeId), animationId_(animationId)
    {}
    ~RSNodeGetAnimationsValueFraction() override = default;

    bool Marshalling(Parcel& parcel) const override;
    static RSCommand* Unmarshalling(Parcel& parcel);

    bool CheckHeader(Parcel& parcel) const override;
    bool ReadFromParcel(Parcel& parcel) override;

    void Process(RSContext& context) override;
    bool IsCallingPidValid(pid_t callingPid, const RSRenderNodeMap& nodeMap) const override;

    float GetFraction() const
    {
        return fraction_;
    }

    uint16_t GetType() const override
    {
        return commandType;
    }

private:
    RSNodeGetAnimationsValueFraction(uint64_t timeoutNS): RSSyncTask(timeoutNS) {}
    NodeId nodeId_;
    AnimationId animationId_;
    float fraction_ { 0.0f };
    static inline RSCommandRegister<commandType, commandSubType, Unmarshalling> registry;
};
} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_SHOWING_COMMAND_H