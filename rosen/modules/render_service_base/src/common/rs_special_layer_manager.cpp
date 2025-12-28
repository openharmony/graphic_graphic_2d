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

template<typename K, typename V>
void EraseEmptyValues(std::unordered_map<K, V>& map) {
    auto it = map.begin();
    while (it != map.end()) {
        if (it->second.empty()) {
            it = map.erase(it);
        } else {
            ++it;
        }
    }
}

namespace OHOS {
namespace Rosen {
std::stack<LeashPersistentId> RSSpecialLayerManager::whiteListRootIds_ = {};
std::unordered_map<SpecialLayerType, std::unordered_map<NodeId, std::unordered_set<ScreenId>>>
    ScreenSpecialLayerInfo::screenSpecialLayerInfoByNode_ = {};
std::unordered_set<NodeId> ScreenSpecialLayerInfo::globalBlackList_ = {};

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

bool RSSpecialLayerManager::Set(uint32_t type, bool is)
{
    if (HasType(specialLayerType_, type) == is) {
        return false;
    }
    SetType(specialLayerType_, type, is);
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
        auto IsSpecial = isType & 1;
        if (IsSpecial) {
            specialLayerIds_[currentType].erase(id);
            if (specialLayerIds_[currentType].empty()) {
                DeleteType(specialLayerType_, currentType << SPECIAL_TYPE_NUM);
            }
        }
        isType >>= 1;
        currentType <<= 1;
    }
}

bool RSSpecialLayerManager::SetWithScreen(uint64_t screenId, uint32_t type, bool is)
{
    if (screenSpecialLayer_.find(screenId) != screenSpecialLayer_.end() &&
        HasType(screenSpecialLayer_.at(screenId), type) == is) {
        return false;
    }
    SetType(screenSpecialLayer_[screenId], type, is);
    return true;
}

bool RSSpecialLayerManager::FindWithScreen(uint64_t screenId, uint32_t type) const
{
    if (screenSpecialLayer_.find(screenId) == screenSpecialLayer_.end()) {
        return false;
    }
    return HasType(screenSpecialLayer_.at(screenId), type);
}

uint32_t RSSpecialLayerManager::GetWithScreen(uint64_t screenId) const
{
    if (screenSpecialLayer_.find(screenId) == screenSpecialLayer_.end()) {
        return SpecialLayerType::NONE;
    }
    return screenSpecialLayer_.at(screenId);
}

void RSSpecialLayerManager::AddIdsWithScreen(uint64_t screenId, uint32_t type, NodeId id)
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

void RSSpecialLayerManager::RemoveIdsWithScreen(uint64_t screenId, uint32_t type, NodeId id)
{
    uint32_t isType = type & IS_GENERAL_SPECIAL;
    uint32_t currentType = SpecialLayerType::SECURITY;
    while (isType != 0) {
        auto IsSpecial = isType & 1;
        if (IsSpecial) {
            if (screenSpecialLayerIds_.find(screenId) == screenSpecialLayerIds_.end()) {
                break;
            }
            auto& specialLayerIds = screenSpecialLayerIds_.at(screenId);
            if (specialLayerIds.find(currentType) == specialLayerIds.end()) {
                break;
            }
            auto& typeIds = specialLayerIds.at(currentType);
            typeIds.erase(id);
            // check weather 'screenSpecialLayerIds_[screenId][currentType]' is empty
            if (typeIds.empty()) {
                specialLayerIds.erase(currentType);
                DeleteType(screenSpecialLayer_[screenId], currentType << SPECIAL_TYPE_NUM);
            }
            // check weather 'screenSpecialLayerIds_[screenId]' is empty
            if (specialLayerIds.empty()) {
                screenSpecialLayerIds_.erase(screenId);
            }
        }
        isType >>= 1;
        currentType <<= 1;
    }
}

const std::unordered_set<uint64_t> RSSpecialLayerManager::FindScreenHasType(uint32_t type) const
{
    std::unordered_set<uint64_t> screenIds;
    for (const auto& [screenId, specialLayerType] : screenSpecialLayer_) {
        if (HasType(specialLayerType, type)) {
            screenIds.insert(screenId);
        }
    }
    return screenIds;
}

void RSSpecialLayerManager::ClearScreenSpecialLayer()
{
    screenSpecialLayer_.clear();
    screenSpecialLayerIds_.clear();
}

void ScreenSpecialLayerInfo::Update(
    SpecialLayerType type, ScreenId screenId, const std::unordered_set<NodeId>& nodeIds)
{
    auto& typeInfo = screenSpecialLayerInfoByNode_[type];
    // clear
    for (const auto& [nodeId, screenIds] : typeInfo) {
        typeInfo[nodeId].erase(screenId);
    }
    // reset
    for (const auto& nodeId : nodeIds) {
        typeInfo[nodeId].insert(screenId);
    }
    ClearEmptyInfo();
}

void ScreenSpecialLayerInfo::ClearByScreenId(ScreenId screenId)
{
    for (auto& [type, typeInfo] : screenSpecialLayerInfoByNode_) {
        for (auto& [nodeId, nodeInfo] : typeInfo) {
            nodeInfo.erase(screenId);
        }
    }
    ClearEmptyInfo();
}

void ScreenSpecialLayerInfo::ClearEmptyInfo()
{
    for (auto& [_, typeInfo] : screenSpecialLayerInfoByNode_) {
        EraseEmptyValues(typeInfo);
    }
    EraseEmptyValues(screenSpecialLayerInfoByNode_);
}

std::unordered_set<ScreenId> ScreenSpecialLayerInfo::QueryEnableScreen(SpecialLayerType type, std::pair<NodeId, LeashPersistentId> id)
{
    auto typeIter = screenSpecialLayerInfoByNode_.find(type);
    if (typeIter == screenSpecialLayerInfoByNode_.end()) {
        return {};
    }

    std::unordered_set<ScreenId> screenIds = {};
    const auto& typeInfo = typeIter->second;
    // find with nodeId
    auto nodeIdIter =  typeInfo.find(id.first);
    if (nodeIdIter != typeInfo.end()) {
        const auto& screenIdsForNodeId = nodeIdIter->second;
        screenIds.insert(screenIdsForNodeId.begin(), screenIdsForNodeId.end());
    }
    // find with persistId
    auto persistIdIter =  typeInfo.find(id.second);
    if (persistIdIter != typeInfo.end()) {
        const auto& screenIdsForPersistId = persistIdIter->second;
        screenIds.insert(screenIdsForPersistId.begin(), screenIdsForPersistId.end());
    }
    return screenIds;
}

bool ScreenSpecialLayerInfo::ExistEnableScreen(SpecialLayerType type)
{
    return screenSpecialLayerInfoByNode_.find(type) != screenSpecialLayerInfoByNode_.end();
}

void ScreenSpecialLayerInfo::SetGlobalBlackList(const std::unordered_set<NodeId>& globalBlackList)
{
    globalBlackList_ = globalBlackList;
}

const std::unordered_set<NodeId>& ScreenSpecialLayerInfo::GetGlobalBlackList()
{
    return globalBlackList_;
}
} // namespace Rosen
} // namespace OHOS