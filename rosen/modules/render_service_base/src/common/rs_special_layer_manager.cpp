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
    whiteListRootIds_.push(id);
}

LeashPersistentId RSSpecialLayerManager::GetCurWhiteListRootId()
{
    if (whiteListRootIds_.empty()) {
        return INVALID_LEASH_PERSISTENTID;
    }
    return whiteListRootIds_.top();
}

void RSSpecialLayerManager::ResetWhiteListRootId(LeashPersistentId id)
{
    if (!whiteListRootIds_.empty() && whiteListRootIds_.top() == id) {
        whiteListRootIds_.pop();
    }
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
        auto IsSpecial = isType & 1;
        if (IsSpecial && specialLayerIds_[currentType].size() < MAX_IDS_SIZE) {
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

void RSSpecialLayerManager::ClearScreenSpecialLayer()
{
    screenSpecialLayer_.clear();
}

void RSSpecialLayerManager::ClearSpecialLayerIds()
{
    specialLayerIds_.clear();
}
} // namespace Rosen
} // namespace OHOS