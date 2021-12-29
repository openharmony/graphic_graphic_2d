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

namespace OHOS {
namespace Rosen {
RSScreenCapability::RSScreenCapability(std::string name, ScreenInterfaceType type, uint32_t phyWidth,
    uint32_t phyHeight, uint32_t supportLayers, uint32_t virtualDispCount,
    bool supportWriteback, const std::vector<RSScreenProps>& props)
    : name_(name), type_(type), phyWidth_(phyWidth),
    phyHeight_(phyHeight), supportLayers_(supportLayers),
    virtualDispCount_(virtualDispCount), supportWriteback_(supportWriteback), props_(props)
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

void RSScreenCapability::SetSupportWriteback(bool supportWriteback)
{
    supportWriteback_ = supportWriteback;
}

void RSScreenCapability::SetProps(const std::vector<RSScreenProps>& props)
{
    props_ = props;
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

bool RSScreenCapability::GetSupportWriteback() const
{
    return supportWriteback_;
}

const std::vector<RSScreenProps>& RSScreenCapability::GetProps() const
{
    return props_;
}

bool RSScreenCapability::WriteVector(const std::vector<RSScreenProps> &props, Parcel &parcel) const
{
    for (uint32_t propIndex = 0; propIndex < props.size(); propIndex++) {
        if (!parcel.WriteParcelable(&props[propIndex])) {
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
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(type_))) {
        return false;
    }
    if (!parcel.WriteUint32(phyWidth_)) {
        return false;
    }
    if (!parcel.WriteUint32(phyHeight_)) {
        return false;
    }
    if (!parcel.WriteUint32(supportLayers_)) {
        return false;
    }
    if (!parcel.WriteUint32(virtualDispCount_)) {
        return false;
    }
    if (!parcel.WriteBool(supportWriteback_)) {
        return false;
    }
    if (!parcel.WriteUint32(static_cast<uint32_t>(props_.size()))) {
        return false;
    }
    if (!WriteVector(props_, parcel)) {
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
    bool supportWriteback = false;
    uint32_t propCount;
    std::vector<RSScreenProps> props;
    if (!parcel.ReadString(name)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(type)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(phyWidth)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(phyHeight)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(supportLayers)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(virtualDispCount)) {
        return nullptr;
    }
    if (!parcel.ReadBool(supportWriteback)) {
        return nullptr;
    }
    if (!parcel.ReadUint32(propCount)) {
        return nullptr;
    }
    if (!ReadVector(props, propCount, parcel)) {
        return nullptr;
    }
    RSScreenCapability* screenCapability = new RSScreenCapability(name, static_cast<ScreenInterfaceType>(type),
        phyWidth, phyHeight, supportLayers, virtualDispCount, supportWriteback, props);
    return screenCapability;
}
}
}