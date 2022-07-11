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

#ifndef RENDER_SERVICE_BASE_COMMON_RS_EVENT_MANAGER_H
#define RENDER_SERVICE_BASE_COMMON_RS_EVENT_MANAGER_H

#include<chrono>
#include<mutex>
#include "rs_event_detector.h"

namespace OHOS {
namespace Rosen {


struct RSEventState final {
	int eventIntervalMs = 60000; // unit:ms 60000 the mini time interval between two event 
	uint64_t prevEventTimeStampMs = 0;
};

class RSEventManager final
{
public:
	static RSEventManager& Instance();
	void UpdateParam(); // Update Param: timeout\Event Frequency...
	uint64_t GetSysTimeMs();
	void AddEvent(const std::shared_ptr<RSBaseEventDetector>& detectorPtr, int eventIntervalMs = 60000); // 60000ms: 1min
	void RemoveEvent(std::string stringId);
private:
	RSEventManager() = default;
	~RSEventManager();
	RSEventManager(const RSEventManager&) = delete;
	RSEventManager(const RSEventManager&&) = delete;
	RSEventManager& operator=(const RSEventManager&) = delete;
	RSEventManager& operator=(const RSEventManager&&) = delete;
	void UpdateDetectorParam(std::shared_ptr<RSBaseEventDetector> detectorPtr);
	void UpdateEventIntervalMs(std::shared_ptr<RSBaseEventDetector> detectorPtr);
	void EventReport(const RSSysEventMsg& eventMsg);
	void Clear();

	std::map<std::string, std::weak_ptr<RSBaseEventDetector>> eventDetectorList_;
	std::map<std::string, RSEventState> eventStateList_;
	std::mutex listMutex_;
	int updateCount_ = 0;
	int updateThreshold_ = 200; // every 200 times, update param
};

}
}
#endif // RENDER_SERVICE_BASE_COMMON_RS_EVENT_MANAGER_H