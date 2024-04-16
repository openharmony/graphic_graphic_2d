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

enum RSNodeShowingCommandType : uint16_t {
    GET_RENDER_PROPERTY,
    GET_RENDER_PROPERTIES,
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
    std::shared_ptr<RSRenderPropertyBase> GetProperty() const
    {
        return property_;
    }
    bool IsTimeout() const
    {
        return isTimeout_;
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
    using PropertiesMap = std::map<std::pair<NodeId, PropertyId>, std::shared_ptr<RSRenderPropertyBase>>;
    explicit RSNodeGetShowingPropertiesAndCancelAnimation(uint64_t timeoutNS, PropertiesMap&& map)
        : RSSyncTask(timeoutNS), propertiesMap_(std::move(map))
    {}
    ~RSNodeGetShowingPropertiesAndCancelAnimation() override = default;

    bool Marshalling(Parcel& parcel) const override;
    static RSCommand* Unmarshalling(Parcel& parcel);

    bool CheckHeader(Parcel& parcel) const override;
    bool ReadFromParcel(Parcel& parcel) override;

    void Process(RSContext& context) override;

    const PropertiesMap& GetProperties() const
    {
        return this->propertiesMap_;
    }

private:
    RSNodeGetShowingPropertiesAndCancelAnimation(uint64_t timeoutNS): RSSyncTask(timeoutNS) {}
    PropertiesMap propertiesMap_;
    using Registrar = RSCommandRegister<commandType, commandSubType, Unmarshalling>;
    static Registrar instance_;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_COMMAND_RS_NODE_SHOWING_COMMAND_H