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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_SPECIAL_LAYER_MANAGER_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_SPECIAL_LAYER_MANAGER_H

#include <map>
#include <set>
#include <stack>

#include "common/rs_common_def.h"
#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {

constexpr uint32_t SPECIAL_TYPE_NUM = 10;
constexpr uint32_t IS_GENERAL_SPECIAL = 0x3FF;
constexpr uint32_t HAS_GENERAL_SPECIAL = 0xFFC00;
constexpr uint32_t VIRTUALSCREEN_IS_SPECIAL = 0X7;  //contain security、skip、protected layer
constexpr uint32_t VIRTUALSCREEN_HAS_SPECIAL = 0X1C00;  //contain security、skip、protected layer
constexpr uint32_t SNAPSHOT_IS_SPECIAL = 0XF;   //contain security、skip、protected、snapshotskip layer
constexpr uint32_t SNAPSHOT_HAS_SPECIAL = 0X3C00;   //contain security、skip、protected、snapshotskip layer

enum SpecialLayerType : uint32_t {
    NONE = 0,
    SECURITY = 0x00000001,
    SKIP = 0x00000002,
    PROTECTED = 0x00000004,
    SNAPSHOT_SKIP = 0x00000008,

    IS_BLACK_LIST = 0x00000080,
    IS_WHITE_LIST = 0x00000100,

    HAS_SECURITY = SECURITY << SPECIAL_TYPE_NUM,
    HAS_SKIP = SKIP << SPECIAL_TYPE_NUM,
    HAS_PROTECTED = PROTECTED << SPECIAL_TYPE_NUM,
    HAS_SNAPSHOT_SKIP = SNAPSHOT_SKIP << SPECIAL_TYPE_NUM,

    HAS_BLACK_LIST = IS_BLACK_LIST << SPECIAL_TYPE_NUM,
    HAS_WHITE_LIST = IS_WHITE_LIST << SPECIAL_TYPE_NUM,
};

class RSB_EXPORT RSSpecialLayerManager {
public:
    static void SetWhiteListRootId(LeashPersistentId id);
    static LeashPersistentId GetCurWhiteListRootId();
    static void ResetWhiteListRootId(LeashPersistentId id);

    RSSpecialLayerManager() = default;
    ~RSSpecialLayerManager() = default;

    bool Set(uint32_t type, bool is);
    bool Find(uint32_t type) const;
    uint32_t Get() const;
    void AddIds(uint32_t type, NodeId id);
    void RemoveIds(uint32_t type, NodeId id);

    bool SetWithScreen(uint64_t screenId, uint32_t type, bool is);
    bool FindWithScreen(uint64_t screenId, uint32_t type) const;
    void ClearScreenSpecialLayer();

private:
    static std::stack<LeashPersistentId> whiteListRootIds_;

    uint32_t specialLayerType_ = SpecialLayerType::NONE;
    // key:SpecialLayerType value:std::set<NodeId>
    std::map<uint32_t, std::set<NodeId>> specialLayerIds_;
    // key:ScreenId value:SpecialLayerType
    std::map<uint64_t, uint32_t> screenSpecialLayer_;
};

class AutoSpecialLayerStateRecover {
public:
    /** Preserve specialLayer state and recover. Used for whiteListRootId currently.
        @param LeashPersistentId whiteListRootId
        @return utility to special layer state recovery on destructor
    */
    AutoSpecialLayerStateRecover(LeashPersistentId id)
        : currentRootId_(id)
    {
        RSSpecialLayerManager::SetWhiteListRootId(id);
    }

    AutoSpecialLayerStateRecover(AutoSpecialLayerStateRecover&&) = delete;
    AutoSpecialLayerStateRecover(const AutoSpecialLayerStateRecover&) = delete;
    AutoSpecialLayerStateRecover& operator=(AutoSpecialLayerStateRecover&&) = delete;
    AutoSpecialLayerStateRecover& operator=(const AutoSpecialLayerStateRecover&) = delete;

    ~AutoSpecialLayerStateRecover()
    {
        RSSpecialLayerManager::ResetWhiteListRootId(currentRootId_);
    }

private:
    LeashPersistentId currentRootId_ = INVALID_LEASH_PERSISTENTID;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_SPECIAL_LAYER_MANAGER_H