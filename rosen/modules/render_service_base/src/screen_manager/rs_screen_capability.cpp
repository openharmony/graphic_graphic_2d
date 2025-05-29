/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "screen_manager/rs_screen_capability.h"
#include "platform/common/rs_log.h"

namespace OHOS {
namespace Rosen {
RSScreenCapability::RSScreenCapability(std::string name, ScreenInterfaceType type, uint32_t phyWidth,
    uint32_t phyHeight, uint32_t supportLayers, uint32_t virtualDispCount,
    bool supportWriteBack, const std::vector<RSScreenProps>& props)
    : name_(name), type_(type), phyWidth_(phyWidth),
    phyHeight_(phyHeight), supportLayers_(supportLayers),
    virtualDispCount_(virtualDispCount), supportWriteBack_(supportWriteBack), props_(props)
{
}

void RSScreenCapability::SetName(const std::string& name)
{
    name_ = name;
}

void RSScreenCapability::SetType(ScreenInterfaceType type)
{
    type_ = type;
}

void RSScreenCapability::SetPhyWidth(uint32_t phyWidth)
{
    phyWidth_ = phyWidth;
}

void RSScreenCapability::SetPhyHeight(uint32_t phyHeight)
{
    phyHeight_ = phyHeight;
}

void RSScreenCapability::SetSupportLayers(uint32_t supportLayers)
{
    supportLayers_ = supportLayers;
}

void RSScreenCapability::SetVirtualDispCount(uint32_t virtualDispCount)
{
    virtualDispCount_ = virtualDispCount;
}

void RSScreenCapability::SetSupportWriteBack(bool supportWriteBack)
{
    supportWriteBack_ = supportWriteBack;
}

void RSScreenCapability::SetProps(std::vector<RSScreenProps> props)
{
    props_ = std::move(props);
}

const std::string& RSScreenCapability::GetName() const
{
    return name_;
}

ScreenInterfaceType RSScreenCapability::GetType() const
{
    return type_;
}

uint32_t RSScreenCapability::GetPhyWidth() const
{
    return phyWidth_;
}

uint32_t RSScreenCapability::GetPhyHeight() const
{
    return phyHeight_;
}

uint32_t RSScreenCapability::GetSupportLayers() const
{
    return supportLayers_;
}

uint32_t RSScreenCapability::GetVirtualDispCount() const
{
    return virtualDispCount_;
}

bool RSScreenCapability::GetSupportWriteBack() const
{
    return supportWriteBack_;
}

const std::vector<RSScreenProps>& RSScreenCapability::GetProps() const
{
    return props_;
}

bool RSScreenCapability::WriteVector(const std::vector<RSScreenProps> &props, Parcel &parcel) const
{
    for (uint32_t propIndex = 0; propIndex < props.size(); propIndex++) {
        if (!parcel.WriteParcelable(&props[propIndex])) {
            ROSEN_LOGE("RSScreenCapability::WriteVector WriteParcelable failed");
            return false;
        }
    }
    return true;
}

bool RSScreenCapability::ReadVector(std::vector<RSScreenProps> &unmarProps, uint32_t unmarPropCount, Parcel &parcel)
{
    for (uint32_t propIndex = 0; propIndex < unmarPropCount; propIndex++) {
        sptr<RSScreenProps> itemProp = parcel.ReadParcelable<RSScreenProps>();
        if (itemProp == nullptr) {
            return false;
        } else {
            unmarProps.push_back(*itemProp);
        }
    }
    return true;
}

bool RSScreenCapability::Marshalling(Parcel &parcel) const
{
    if (!parcel.WriteString(name_)) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteString failed");
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(type_))) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteUint32 1 failed");
        return false;
    }
    if (!parcel.WriteUint32(phyWidth_)) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteUint32 2 failed");
        return false;
    }
    if (!parcel.WriteUint32(phyHeight_)) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteUint32 3 failed");
        return false;
    }
    if (!parcel.WriteUint32(supportLayers_)) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteUint32 4 failed");
        return false;
    }
    if (!parcel.WriteUint32(virtualDispCount_)) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteUint32 5 failed");
        return false;
    }
    if (!parcel.WriteBool(supportWriteBack_)) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteBool failed");
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(props_.size()))) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteUint32 6 failed");
        return false;
    }
    if (!WriteVector(props_, parcel)) {
        ROSEN_LOGE("RSScreenCapability::Marshalling WriteVector failed");
        return false;
    }
    return true;
}

RSScreenCapability* RSScreenCapability::Unmarshalling(Parcel &parcel)
{
    std::string name;
    uint32_t type;
    uint32_t phyWidth;
    uint32_t phyHeight;
    uint32_t supportLayers;
    uint32_t virtualDispCount;
    bool supportWriteBack = false;
    uint32_t propCount;
    std::vector<RSScreenProps> props;
    if (!parcel.ReadString(name)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadString failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(type)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadUint32 type failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(phyWidth)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadUint32 phyWidth failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(phyHeight)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadUint32 phyHeight failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(supportLayers)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadUint32 supportLayers failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(virtualDispCount)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadUint32 virtualDispCount failed");
        return nullptr;
    }
    if (!parcel.ReadBool(supportWriteBack)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadBool supportWriteBack failed");
        return nullptr;
    }
    if (!parcel.ReadUint32(propCount)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadUint32 propCount failed");
        return nullptr;
    }
    if (!ReadVector(props, propCount, parcel)) {
        ROSEN_LOGE("RSScreenCapability::Unmarshalling ReadVector failed");
        return nullptr;
    }
    RSScreenCapability* screenCapability = new RSScreenCapability(name, static_cast<ScreenInterfaceType>(type),
        phyWidth, phyHeight, supportLayers, virtualDispCount, supportWriteBack, props);
    return screenCapability;
}
}
}