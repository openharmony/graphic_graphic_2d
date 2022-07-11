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

#include "platform/common/rs_event_manager.h"

namespace OHOS {
namespace Rosen {
std::shared_ptr<RSBaseEventDetector> RSBaseEventDetector::CreateRSTimeOutDetector(int timeOutThresholdMs, std::string detectorStringId)
{
	std::shared_ptr<RSTimeOutDetector> eventPtr(new RSTimeOutDetector(timeOutThresholdMs, detectorStringId));
	return  eventPtr;
}

RSTimeOutDetector::RSTimeOutDetector(int timeOutThresholdMs, std::string detectorStringId) :RSBaseEventDetector(detectorStringId)
{
    RS_LOGD("RSTimeOutDetector ::RSTimeOutDetector timeOutThresholdMs is %d ", timeOutThresholdMs);
	timeOutThredsholdMs_ = timeOutThresholdMs;
	paramList_["timeOutThredsholdMs"] = std::to_string(timeOutThredsholdMs_);
}


void RSTimeOutDetector::SetParam(const std::string& key, const std::string& value)
{
	if (paramList_.count(key) == 0) {
        RS_LOGD("RSTimeOutDetector :: SetParam Invaild Key ");
		return;
	}
	int valueInt = atoi(value.c_str());
	if (valueInt <= 0 || valueInt > 1000000) { // 1000000Ms->1000s
		RS_LOGD("RSTimeOutDetector :: SetParam Invaild Value ");
		return;
	}
	timeOutThredsholdMs_ = valueInt;
}


void RSTimeOutDetector::SetLoopStartTag()
{
	startTimeStampMs_ = RSEventManager::Instance().GetSysTimeMs();
}

void RSTimeOutDetector::SetLoopFinishTag()
{
	uint64_t finishTimeStampMs = RSEventManager::Instance().GetSysTimeMs();
	if (finishTimeStampMs > startTimeStampMs_ && finishTimeStampMs - startTimeStampMs_ > timeOutThredsholdMs_) {
		EventReport(finishTimeStampMs - startTimeStampMs_);
	}
}

void RSTimeOutDetector::EventReport(uint64_t costTimeMs)
{
	std::string msg = "RS TimeOut: one loop cost " + std::to_string(costTimeMs) + "ms";
	RSSysEventMsg eventMsg = {
		stringId_,
		msg,
		OHOS::HiviewDFX::HiSysEvent::EventType::STATISTIC
	};
	
	if (eventCallback_ != nullptr) {
		eventCallback_(eventMsg);
	}
}

}
}