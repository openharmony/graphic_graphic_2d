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
#include "platform/common/rs_system_properties.h"

namespace OHOS {
namespace Rosen {

RSEventManager& RSEventManager::Instance()
{
	static RSEventManager eventManager;
	return eventManager;
}

RSEventManager::~RSEventManager()
{
	Clear();
}

void RSEventManager::Clear()
{
	eventDetectorList_.clear();
	eventStateList_.clear();
	std::map<std::string, std::weak_ptr<RSBaseEventDetector>> tempDetectorList;
	std::map<std::string, RSEventState> tempStateList;
	eventDetectorList_.swap(tempDetectorList);
	eventStateList_.swap(tempStateList);
	RS_LOGD("RSEventManager::Clear finish");
}

void RSEventManager::UpdateDetectorParam(std::shared_ptr<RSBaseEventDetector> detectorPtr)
{
	if (detectorPtr == nullptr) {
		RS_LOGD("RSEventManager::UpdateDetectorParam detectorPtr nullptr");
		return;
	}
	auto paramList = detectorPtr->GetParamList();
	for (auto& item : paramList) {
		std::string paraName = "rosen.RsDFXEvent." + detectorPtr->GetStringId() +
			"." + item.first;
        RS_LOGD("RSEventManager::UpdateDetectorParam paraName: %s", paraName.c_str());
		detectorPtr->SetParam(item.first, RSSystemProperties::GetRSEventProperty(paraName));
	}
}

void RSEventManager::UpdateEventIntervalMs(std::shared_ptr<RSBaseEventDetector> detectorPtr)
{
	if (detectorPtr == nullptr) {
		RS_LOGD("RSEventManager::UpdateEventIntervalMs detectorPtr nullptr");
		return;
	}
	if (eventStateList_.count(detectorPtr->GetStringId()) == 0) {
        RS_LOGD("RSEventManager::UpdateEventIntervalMs detector:%s is not in list",
        detectorPtr->GetStringId().c_str());
		return;
	}
	std::string paraName = "rosen.RsDFXEvent." + detectorPtr->GetStringId() +
		".eventIntervalMs";
    RS_LOGD("RSEventManager::UpdateEventIntervalMs paraName: %s", paraName.c_str());
	int valueInt = atoi(RSSystemProperties::GetRSEventProperty(paraName).c_str());
	if (valueInt <= 0 || valueInt > 1000000) { //1000000 ms ->1000s
        RS_LOGD("RSEventManager::UpdateEventIntervalMs detector:%s Invaild Value:%d",
        detectorPtr->GetStringId().c_str(), valueInt);
		return;
	}
	eventStateList_[detectorPtr->GetStringId()].eventIntervalMs = valueInt;
    RS_LOGD("RSEventManager::UpdateEventIntervalMs detector:%s eventIntervalMs:%d suceess",
    detectorPtr->GetStringId().c_str(), valueInt);
}

void RSEventManager::UpdateParam()
{
	RS_LOGD("RSEventManager::UpdateParam updateCount_:%d updateThreshold_:%d ", updateCount_, updateThreshold_);
	updateCount_++;
	if (updateCount_ % updateThreshold_ != 0) { 
		return;
	}
    updateCount_ = 0;
	RS_LOGD("RSEventManager::UpdateParam  update");
	std::unique_lock<std::mutex> listLock(listMutex_);
	for (auto& item : eventDetectorList_) {
		auto detectorPtr = item.second.lock();
		if (detectorPtr == nullptr) {
			RS_LOGD("RSEventManager::UpdateParam %s failed: nullptr", detectorPtr->GetStringId().c_str());
			return;
		}
		UpdateDetectorParam(detectorPtr);
		UpdateEventIntervalMs(detectorPtr);
	}
}

void RSEventManager::AddEvent(const std::shared_ptr<RSBaseEventDetector>& detectorPtr, int eventIntervalMs)
{
	if (detectorPtr == nullptr) {
		RS_LOGD("RSEventManager::AddEvent detectorPtr nullptr");
		return;
	}
	std::weak_ptr<RSBaseEventDetector> detectorWeakPtr(detectorPtr);
	{
		std::unique_lock<std::mutex> listLock(listMutex_);
		if (eventDetectorList_.count(detectorPtr->GetStringId()) != 0) {
            RS_LOGD("RSEventManager::AddEvent %s failed ", detectorPtr->GetStringId().c_str());
			return;
		}
		detectorPtr->AddEventReportCallback([this](const RSSysEventMsg& eventMsg) {
					this->EventReport(eventMsg);
					});
		eventDetectorList_[detectorPtr->GetStringId()] = detectorWeakPtr;
		RSEventState state = {
			eventIntervalMs,
			0
		};
		eventStateList_[detectorPtr->GetStringId()] = state;
        RS_LOGD("RSEventManager::AddEvent %s success ", detectorPtr->GetStringId().c_str());
	}
}

void RSEventManager::RemoveEvent(std::string stringId)
{
	std::unique_lock<std::mutex> listLock(listMutex_);
	if (eventDetectorList_.count(stringId) != 0) {
        RS_LOGD("RSEventManager::RemoveEvent %s failed ", stringId.c_str());
		return;
	}
	eventDetectorList_.erase(stringId);
	eventStateList_.erase(stringId);
    RS_LOGD("RSEventManager::RemoveEvent %s success ", stringId.c_str());
}

uint64_t RSEventManager::GetSysTimeMs()
{
	auto now = std::chrono::steady_clock::now().time_since_epoch();
	return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}

void RSEventManager::EventReport(const RSSysEventMsg& eventMsg)
{
	std::unique_lock<std::mutex> listLock(listMutex_);
	if (eventStateList_.count(eventMsg.stringId) == 0) {
        RS_LOGD("RSEventManager::EventReport %s failed ", eventMsg.stringId.c_str());
		return;
	}
	RSEventState& state = eventStateList_[eventMsg.stringId];
	uint64_t currentTimeMs = GetSysTimeMs();
	if (currentTimeMs > state.prevEventTimeStampMs&&
		currentTimeMs - state.prevEventTimeStampMs > static_cast<uint64_t>(state.eventIntervalMs)) {
            std::string domain = "GRAPHIC";

    OHOS::HiviewDFX::HiSysEvent::Write(domain, eventMsg.stringId,
        eventMsg.eventType,
        "MSG", eventMsg.msg);
		state.prevEventTimeStampMs = currentTimeMs;
	}
    RS_LOGD("RSEventManager::EventReport %s success ", eventMsg.stringId.c_str());
}


}
}