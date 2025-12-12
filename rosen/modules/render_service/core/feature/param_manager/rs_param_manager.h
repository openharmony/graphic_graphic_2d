/*
 * Copyright (c) 2025-2025 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_PIPELINE_RS_PARAM_MANAGER_H
#define RENDER_SERVICE_PIPELINE_RS_PARAM_MANAGER_H

#include <algorithm>
#include <mutex>
#include <string>
#include <vector>

#include "common_event_subscriber.h"
#include "want.h"

namespace OHOS {
namespace Rosen {

class RSParamManager {
public:
    RSParamManager() = default;
    ~RSParamManager();
    static RSParamManager& GetInstance();

    RSParamManager(const RSParamManager&) = delete;
    RSParamManager& operator=(const RSParamManager&) = delete;

    static inline void Trim(std::string& inputStr)
    {
        if (inputStr.empty()) {
            return;
        }
        inputStr.erase(inputStr.begin(), std::find_if(inputStr.begin(), inputStr.end(),
            [](unsigned char ch) { return !std::isspace(static_cast<unsigned char>(ch)); }));
        inputStr.erase(std::find_if(inputStr.rbegin(), inputStr.rend(),
            [](unsigned char ch) { return !std::isspace(static_cast<unsigned char>(ch)); }).base(), inputStr.end());
    }

    static inline bool IsNumber(const std::string &str)
    {
        for (char c : str) {
            if (!std::isdigit(c)) {
                return false;
            }
        }
        return true;
    }

    bool IsCloudDisableDDGR();
    void SubscribeEvent();
    void UnSubscribeEvent();
    void OnReceiveEvent(const AAFwk::Want &want);
    bool IsCloudDisableSubtree();

private:
    class ParamCommonEventSubscriber : public EventFwk::CommonEventSubscriber {
    public:
        ParamCommonEventSubscriber(const EventFwk::CommonEventSubscribeInfo& subscriberInfo,
            RSParamManager& registry)
            : CommonEventSubscriber(subscriberInfo), registry_(registry)
        {}
        ~ParamCommonEventSubscriber() = default;

        void OnReceiveEvent(const EventFwk::CommonEventData &data) override
        {
            registry_.OnReceiveEvent(data.GetWant());
        }
    private:
        RSParamManager& registry_;
    };

    std::string GetHigherVersionPath();
    std::vector<std::string> SplitString(const std::string &str, char pattern);
    std::vector<std::string> GetVersionNums(const std::string& filePathStr);
    bool CompareVersion(const std::vector<std::string>& localVersion, const std::vector<std::string>& pathVersion);
    void HandleParamUpdate(const AAFwk::Want &want);
    using EventHandle = std::function<void(const OHOS::AAFwk::Want &)>;
    typedef void (RSParamManager::*HandleEventFunc)(const AAFwk::Want &);
    std::map<std::string, EventHandle> eventHandles_;
    std::map<std::string, HandleEventFunc> handleEventFunc_;
    std::shared_ptr<ParamCommonEventSubscriber> subscriber_ = nullptr;
    std::mutex subscriberMutex_;
};

} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_PIPELINE_RS_PARAM_MANAGER_H