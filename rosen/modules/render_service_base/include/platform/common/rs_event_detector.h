/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_DETECTOR_H
#define RENDER_SERVICE_BASE_COMMON_RS_DETECTOR_H

#include <atomic>
#include <map>
#include <memory>
#include "hisysevent.h"
#include "rs_log.h"

namespace OHOS {
namespace Rosen {
struct RSSysEventMsg final {
    std::string stringId;
    std::string msg;
    OHOS::HiviewDFX::HiSysEvent::EventType eventType;
    int32_t pid;
    int32_t uid;
    std::string bundleName;
    std::string abilityName;
};

class RSEventTimer final {
public:
    ~RSEventTimer() = default;
    static uint64_t GetSysTimeMs();
private:
    RSEventTimer() = default;
};

class RSBaseEventDetector {
public:
    using EventReportCallback = std::function<void(const RSSysEventMsg&)>;
    static std::shared_ptr<RSBaseEventDetector> CreateRSTimeOutDetector(int timeOutThresholdMs,
    std::string detectorStringId);
    virtual ~RSBaseEventDetector()
    {
        ClearParamList();
        RS_LOGD("RSBaseEventDetector::~RSBaseEventDetector finish");
    }

    std::string GetStringId()
    {
        return stringId_;
    }

    const std::map<std::string, std::string>& GetParamList()
    {
        return paramList_;
    }
	
    void AddEventReportCallback(const EventReportCallback& callback)
    {
        eventCallback_ = callback;
    }

    virtual void SetParam(const std::string& key, const std::string& value) = 0;
    virtual void SetLoopStartTag() = 0;
    virtual void SetLoopFinishTag(
        int32_t focusAppPid, int32_t focusAppUid,
        std::string& focusAppBundleName, std::string& focusAppAbilityName) = 0;

protected:
    RSBaseEventDetector() = default;
    RSBaseEventDetector(std::string stringId)
    {
        stringId_ = stringId;
    }
    
    void ClearParamList()
    {
        paramList_.clear();
        std::map<std::string, std::string> tempParamList;
        paramList_.swap(tempParamList);
        RS_LOGD("RSBaseEventDetector::ClearParamList finish");
    }

    std::map<std::string, std::string> paramList_; // key: paramName
    std::string stringId_;
    EventReportCallback eventCallback_;
    int32_t focusAppPid_ = -1;
    int32_t focusAppUid_ = -1;
    std::string focusAppBundleName_ = "";
    std::string focusAppAbilityName_ = "";
};


class RSTimeOutDetector : public RSBaseEventDetector {
public:
    RSTimeOutDetector(int timeOutThresholdMs, std::string detectorStringId);
    ~RSTimeOutDetector() = default;
    void SetParam(const std::string& key, const std::string& value) override;
    void SetLoopStartTag() override;
    void SetLoopFinishTag(
        int32_t focusAppPid, int32_t focusAppUid,
        std::string& focusAppBundleName, std::string& focusAppAbilityName) override;
private:
    void EventReport(uint64_t costTimeMs);
    int timeOutThresholdMs_ = INT_MAX; // default: No Detector
    std::atomic_uint64_t startTimeStampMs_ = 0;
};
}
}

#endif // RENDER_SERVICE_BASE_COMMON_RS_DETECTOR_H