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
#include "rs_trace.h"

#include "common/rs_special_layer_manager.h"
#include "platform/common/rs_log.h"
#include "set"

template <class TYPE_T> inline void AddType(TYPE_T& x,  TYPE_T y)
{
    x |= y;
}

template <class TYPE_T> inline void DeleteType(TYPE_T& x, TYPE_T y)
{
    x &= ~y;
}

template <class TYPE_T> inline void SetType(TYPE_T& x, TYPE_T y, bool flag)
{
    if (flag) {
        AddType(x, y);
    } else {
        DeleteType(x, y);
    }
}

template <class TYPE_T> inline bool HasType(TYPE_T x, TYPE_T y)
{
    return x & y;
}

namespace OHOS {
namespace Rosen {
std::stack<LeashPersistentId> RSSpecialLayerManager::whiteListRootIds_ = {};

void RSSpecialLayerManager::SetWhiteListRootId(LeashPersistentId id)
{
    if (id == INVALID_LEASH_PERSISTENTID) {
        return;
    }
    if (whiteListRootIds_.size() >= MAX_SPECIAL_LAYER_NUM) {
        RS_LOGE("RSSpecialLayerManager::SetWhiteListRootId whiteListRootIds_ exceeds size limit.");
        return;
    }
    whiteListRootIds_.push(id);
}

void RSSpecialLayerManager::ResetWhiteListRootId(LeashPersistentId id)
{
    if (id == INVALID_LEASH_PERSISTENTID) {
        return;
    }
    if (whiteListRootIds_.empty() || whiteListRootIds_.top() != id) {
        return;
    }
    whiteListRootIds_.pop();
}

LeashPersistentId RSSpecialLayerManager::GetCurWhiteListRootId()
{
    if (whiteListRootIds_.empty()) {
        return INVALID_LEASH_PERSISTENTID;
    }
    return whiteListRootIds_.top();
}

bool RSSpecialLayerManager::IsWhiteListRootIdsEmpty()
{
    return whiteListRootIds_.empty();
}

void RSSpecialLayerManager::ClearWhiteListRootIds()
{
    whiteListRootIds_ = std::stack<LeashPersistentId>();
}

bool RSSpecialLayerManager::Set(uint32_t type, bool enable)
{
    if (HasType(specialLayerType_, type) == enable) {
        return false;
    }
    SetType(specialLayerType_, type, enable);
    return true;
}

bool RSSpecialLayerManager::Find(uint32_t type) const
{
    return HasType(specialLayerType_, type);
}

uint32_t RSSpecialLayerManager::Get() const
{
    return specialLayerType_;
}

void RSSpecialLayerManager::AddIds(uint32_t type, NodeId id)
{
    uint32_t isType = type & IS_GENERAL_SPECIAL;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (isType != 0) {
        bool isSpecial = (isType & 1) != 0;
        if (isSpecial && specialLayerIds_[currentType].size() < MAX_SPECIAL_LAYER_NUM) {
            specialLayerIds_[currentType].insert(id);
            AddType(specialLayerType_, currentType << SPECIAL_TYPE_NUM);
        }
        isType >>= 1;
        currentType <<= 1;
    }
}

void RSSpecialLayerManager::RemoveIds(uint32_t type, NodeId id)
{
    uint32_t isType = type & IS_GENERAL_SPECIAL;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (isType != 0) {
        bool isSpecial = (isType & 1) != 0;
        if (isSpecial) {
            specialLayerIds_[currentType].erase(id);
            if (specialLayerIds_[currentType].empty()) {
                DeleteType(specialLayerType_, currentType << SPECIAL_TYPE_NUM);
                specialLayerIds_.erase(currentType);
            }
        }
        isType >>= 1;
        currentType <<= 1;
    }
}

bool RSSpecialLayerManager::SetWithScreen(ScreenId screenId, uint32_t type, bool enable)
{
    if (screenSpecialLayer_.find(screenId) != screenSpecialLayer_.end() &&
        HasType(screenSpecialLayer_.at(screenId), type) == enable) {
        return false;
    }
    SetType(screenSpecialLayer_[screenId], type, enable);
    return true;
}

bool RSSpecialLayerManager::FindWithScreen(ScreenId screenId, uint32_t type) const
{
    if (screenSpecialLayer_.find(screenId) == screenSpecialLayer_.end()) {
        return false;
    }
    return HasType(screenSpecialLayer_.at(screenId), type);
}

void RSSpecialLayerManager::AddIdsWithScreen(ScreenId screenId, uint32_t type, NodeId id)
{
    uint32_t isType = type & IS_GENERAL_SPECIAL;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (isType != 0) {
        bool isSpecial = (isType & 1) != 0;
        if (isSpecial && screenSpecialLayerIds_[screenId][currentType].size() < MAX_SPECIAL_LAYER_NUM) {
            screenSpecialLayerIds_[screenId][currentType].insert(id);
            AddType(screenSpecialLayer_[screenId], currentType << SPECIAL_TYPE_NUM);
        }
        isType >>= 1;
        currentType <<= 1;
    }
}

void RSSpecialLayerManager::RemoveIdsWithScreen(ScreenId screenId, uint32_t type, NodeId id)
{
    uint32_t isType = type & IS_GENERAL_SPECIAL;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (isType != 0) {
        bool isSpecial = (isType & 1) != 0;
        if (isSpecial) {
            screenSpecialLayerIds_[screenId][currentType].erase(id);
            if (screenSpecialLayerIds_[screenId][currentType].empty()) {
                screenSpecialLayerIds_[screenId].erase(currentType);
                DeleteType(screenSpecialLayer_[screenId], currentType << SPECIAL_TYPE_NUM);
            }
            if (screenSpecialLayerIds_[screenId].empty()) {
                screenSpecialLayerIds_.erase(screenId);
            }
        }
        isType >>= 1;
        currentType <<= 1;
    }
}

std::unordered_set<NodeId> RSSpecialLayerManager::GetIdsWithScreen(ScreenId screenId, uint32_t type) const
{
    std::unordered_set<NodeId> currentTypeIds;
    auto screenIter = screenSpecialLayerIds_.find(screenId);
    if (screenIter == screenSpecialLayerIds_.end()) {
        return currentTypeIds;
    }
    type >>= SPECIAL_TYPE_NUM;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (type != 0) {
        bool isSpecial = (type & 1) != 0;
        if (isSpecial) {
            const auto& speciallayerIds = screenIter->second;
            auto typeIter = speciallayerIds.find(currentType);
            if (typeIter != speciallayerIds.end()) {
                const auto& ids = typeIter->second;
                currentTypeIds.insert(ids.begin(), ids.end());
            }
        }
        type >>= 1;
        currentType <<= 1;
    }
    return currentTypeIds;
}

void RSSpecialLayerManager::ClearScreenSpecialLayer()
{
    screenSpecialLayer_.clear();
}

void RSSpecialLayerManager::MergeChildren(const RSSpecialLayerManager& childSlm)
{
    AddType(specialLayerType_, (childSlm.specialLayerType_ & HAS_GENERAL_SPECIAL));
    for (const auto& [type, ids] : childSlm.specialLayerIds_) {
        specialLayerIds_[type].insert(ids.begin(), ids.end());
    }
}

void RSSpecialLayerManager::Clear()
{
    specialLayerIds_.clear();
    screenSpecialLayerIds_.clear();
    hasSlInVisibleRect_.clear();
}

void RSSpecialLayerManager::SetHasSlInVisibleRect(ScreenId screenId, bool hasSlInVisibleRect)
{
    hasSlInVisibleRect_[screenId] = hasSlInVisibleRect;
}

bool RSSpecialLayerManager::GetHasSlInVisibleRect(ScreenId screenId) const
{
    auto iter = hasSlInVisibleRect_.find(screenId);
    if (iter != hasSlInVisibleRect_.end()) {
        return iter->second;
    }
    return false;
}

std::unordered_set<NodeId> RSSpecialLayerManager::GetIds(uint32_t type) const
{
    std::unordered_set<NodeId> currentTypeIds;
    type >>= SPECIAL_TYPE_NUM;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (type != 0) {
        bool isSpecial = (type & 1) != 0;
        if (isSpecial) {
            auto iter = specialLayerIds_.find(currentType);
            if (iter != specialLayerIds_.end()) {
                currentTypeIds.insert(iter->second.begin(), iter->second.end());
            }
        }
        type >>= 1;
        currentType <<= 1;
    }
    return currentTypeIds;
}
} // namespace Rosen
} // namespace OHOS