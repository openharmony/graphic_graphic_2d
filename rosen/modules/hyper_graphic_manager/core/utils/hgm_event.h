/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef HGM_EVENT_H
#define HGM_EVENT_H

#include <mutex>
#include <unordered_set>

#include "hgm_frame_voter.h"
#include "variable_frame_rate/rs_variable_frame_rate.h"

namespace OHOS::Rosen {
class HgmEventDistributor;
class HgmEvent {
public:
    HgmEvent();
    virtual ~HgmEvent();
    virtual void HandlePackageEvent(const std::vector<std::string>& packageList) {}
    virtual void HandleSceneEvent(pid_t pid, const EventInfo& eventInfo) {}
    virtual void HandleAppStrategyConfigEvent(const std::string& pkgName,
        const std::vector<std::pair<std::string, std::string>>& newConfig) {}
private:
    wptr<HgmEventDistributor> eventDistributor_;
};

class HgmEventDistributor : public RefBase {
public:
    static sptr<HgmEventDistributor> Instance();

    void HandlePackageEvent(const std::vector<std::string>& packageList)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& event : events_) {
            event->HandlePackageEvent(packageList);
        }
    }

    void HandleSceneEvent(pid_t pid, const EventInfo& eventInfo)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& event : events_) {
            event->HandleSceneEvent(pid, eventInfo);
        }
    }

    void HandleAppStrategyConfigEvent(const std::string& pkgName,
        const std::vector<std::pair<std::string, std::string>>& newConfig)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& event : events_) {
            event->HandleAppStrategyConfigEvent(pkgName, newConfig);
        }
    }

private:
    friend class HgmEvent;
    friend class sptr<HgmEventDistributor>;
    HgmEventDistributor() = default;
    ~HgmEventDistributor() = default;

    void AddEventInstance(HgmEvent* event);
    void RemoveEventInstance(HgmEvent* event);

    mutable std::mutex mutex_;
    std::unordered_set<HgmEvent*> events_;
};

inline HgmEvent::HgmEvent()
{
    auto eventDistributorSPtr = HgmEventDistributor::Instance();
    eventDistributorSPtr->AddEventInstance(this);
    eventDistributor_ = eventDistributorSPtr;
}

inline HgmEvent::~HgmEvent()
{
    auto eventDistributorSPtr = eventDistributor_.promote();
    if (eventDistributorSPtr != nullptr) {
        eventDistributorSPtr->RemoveEventInstance(this);
    }
}

inline sptr<HgmEventDistributor> HgmEventDistributor::Instance()
{
    static std::once_flag createFlag;
    static sptr<HgmEventDistributor> instance;
    std::call_once(createFlag, [&] {
        instance = sptr<HgmEventDistributor>::MakeSptr();
    });
    return instance;
}

inline void HgmEventDistributor::AddEventInstance(HgmEvent* event)
{
    std::lock_guard<std::mutex> lock(mutex_);
    events_.insert(event);
}

inline void HgmEventDistributor::RemoveEventInstance(HgmEvent* event)
{
    std::lock_guard<std::mutex> lock(mutex_);
    events_.erase(event);
}
}
#endif // HGM_EVENT_H