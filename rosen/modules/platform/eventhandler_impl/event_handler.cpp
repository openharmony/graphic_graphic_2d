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

#include "event_handler.h"

#include <unistd.h>
#include "event_handler_utils.h"
#ifdef HAS_HICHECKER_NATIVE_PART
#include "hichecker.h"
#endif // HAS_HICHECKER_NATIVE_PART
#include "thread_local_data.h"

#ifdef _WIN32
#include <windows.h>
#define gettid GetCurrentThreadId
#define localtime_r(tm, time) localtime_s(time, tm)
#endif

#ifdef __APPLE__
#define gettid getpid
#endif

#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/syscall.h>
#define gettid []() -> int32_t { return static_cast<int32_t>(syscall(SYS_gettid)); }
#endif

DEFINE_HILOG_LABEL("EventHandler");

using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace AppExecFwk {
static constexpr int DATETIME_STRING_LENGTH = 80;

static thread_local std::weak_ptr<EventHandler> currentEventHandler;

std::shared_ptr<EventHandler> EventHandler::Current()
{
    return currentEventHandler.lock();
}

EventHandler::EventHandler(const std::shared_ptr<EventRunner> &runner) : eventRunner_(runner)
{}

EventHandler::~EventHandler()
{
    if (eventRunner_ && eventRunner_->GetEventQueue()) {
        /*
         * This handler is finishing, need to remove all events belong to it.
         * But events only have weak pointer of this handler,
         * now weak pointer is invalid, so these events become orphans.
         */
        eventRunner_->GetEventQueue()->RemoveOrphan();
    }
}

bool EventHandler::SendEvent(InnerEvent::Pointer &event, int64_t delayTime, Priority priority)
{
    if (!event) {
        HILOGE("SendEvent: Could not send an invalid event");
        return false;
    }

    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("SendEvent: MUST Set event runner before sending events");
        return false;
    }

    InnerEvent::TimePoint now = InnerEvent::Clock::now();
    event->SetSendTime(now);

    if (delayTime > 0) {
        event->SetHandleTime(now + std::chrono::milliseconds(delayTime));
    } else {
        event->SetHandleTime(now);
    }

    event->SetOwner(shared_from_this());

    eventRunner_->GetEventQueue()->Insert(event, priority);
    return true;
}

bool EventHandler::SendTimingEvent(InnerEvent::Pointer &event, int64_t taskTime, Priority priority)
{
    InnerEvent::TimePoint nowSys = InnerEvent::Clock::now();
    auto epoch = nowSys.time_since_epoch();
    long nowSysTime = std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count();
    int64_t delayTime = taskTime - nowSysTime;
    if (delayTime < 0) {
        HILOGE("SendTimingEvent: SendTime is before now systime, change to 0 delaytime Event");
        return SendEvent(event, 0, priority);
    }

    return SendEvent(event, delayTime, priority);
}

bool EventHandler::SendSyncEvent(InnerEvent::Pointer &event, Priority priority)
{
    if ((!event) || (priority == Priority::IDLE)) {
        HILOGE("SendSyncEvent: Could not send an invalid event or idle event");
        return false;
    }

    if ((!eventRunner_) || (!eventRunner_->IsRunning())) {
        HILOGE("SendSyncEvent: MUST Set a running event runner before sending sync events");
        return false;
    }

    // If send a sync event in same event runner, distribute here.
    if (eventRunner_ == EventRunner::Current()) {
        DistributeEvent(event);
        return true;
    }

    // Create waiter, used to block.
    auto waiter = event->CreateWaiter();
    // Send this event as normal one.
    if (!SendEvent(event, 0, priority)) {
        return false;
    }
    // Wait until event is processed(recycled).
    if (!waiter) {
        return false;
    }
    waiter->Wait();

    return true;
}

void EventHandler::RemoveAllEvents()
{
    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("RemoveAllEvents: MUST Set event runner before removing all events");
        return;
    }

    eventRunner_->GetEventQueue()->Remove(shared_from_this());
}

void EventHandler::RemoveEvent(uint32_t innerEventId)
{
    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("RemoveEvent: MUST Set event runner before removing events by id");
        return;
    }

    eventRunner_->GetEventQueue()->Remove(shared_from_this(), innerEventId);
}

void EventHandler::RemoveEvent(uint32_t innerEventId, int64_t param)
{
    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("RemoveEvent: MUST Set event runner before removing events by id and param");
        return;
    }

    eventRunner_->GetEventQueue()->Remove(shared_from_this(), innerEventId, param);
}

void EventHandler::RemoveTask(const std::string &name)
{
    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("RemoveTask: MUST Set event runner before removing events by task name");
        return;
    }

    eventRunner_->GetEventQueue()->Remove(shared_from_this(), name);
}

ErrCode EventHandler::AddFileDescriptorListener(
    int32_t fileDescriptor, uint32_t events, const std::shared_ptr<FileDescriptorListener> &listener)
{
    if ((fileDescriptor < 0) || ((events & FILE_DESCRIPTOR_EVENTS_MASK) == 0) || (!listener)) {
        HILOGE("AddFileDescriptorListener(%{public}d, %{public}u, %{public}s): Invalid parameter",
            fileDescriptor,
            events,
            listener ? "valid" : "null");
        return EVENT_HANDLER_ERR_INVALID_PARAM;
    }

    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("AddFileDescriptorListener: MUST Set event runner before adding fd listener");
        return EVENT_HANDLER_ERR_NO_EVENT_RUNNER;
    }

    listener->SetOwner(shared_from_this());
    return eventRunner_->GetEventQueue()->AddFileDescriptorListener(fileDescriptor, events, listener);
}

void EventHandler::RemoveAllFileDescriptorListeners()
{
    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("RemoveAllFileDescriptorListeners: MUST Set event runner before removing all fd listener");
        return;
    }

    eventRunner_->GetEventQueue()->RemoveFileDescriptorListener(shared_from_this());
}

void EventHandler::RemoveFileDescriptorListener(int32_t fileDescriptor)
{
    if (fileDescriptor < 0) {
        HILOGE("RemoveFileDescriptorListener(%{public}d): Invalid parameter", fileDescriptor);
        return;
    }

    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("RemoveFileDescriptorListener: MUST Set event runner before removing fd listener by fd");
        return;
    }

    eventRunner_->GetEventQueue()->RemoveFileDescriptorListener(fileDescriptor);
}

void EventHandler::SetEventRunner(const std::shared_ptr<EventRunner> &runner)
{
    if (eventRunner_ == runner) {
        return;
    }

    if (eventRunner_) {
        HILOGW("SetEventRunner: It is not recommended to change the event runner dynamically");

        // Remove all events and listeners from old event runner.
        RemoveAllEvents();
        RemoveAllFileDescriptorListeners();
    }

    // Switch event runner.
    eventRunner_ = runner;
    return;
}

void EventHandler::DeliveryTimeAction(const InnerEvent::Pointer &event, InnerEvent::TimePoint nowStart)
{
#ifdef HAS_HICHECKER_NATIVE_PART
    if (!HiChecker::NeedCheckSlowEvent()) {
        return;
    }
    if (!eventRunner_ || !event) {
        return;
    }
    int64_t deliveryTimeout = eventRunner_->GetDeliveryTimeout();
    if (deliveryTimeout > 0) {
        std::string threadName = eventRunner_->GetRunnerThreadName();
        std::string eventName = GetEventName(event);
        int64_t threadId = gettid();
        std::string threadIdCharacter = std::to_string(threadId);
        std::chrono::duration<double> deliveryTime = nowStart - event->GetSendTime();
        std::string deliveryTimeCharacter = std::to_string((deliveryTime).count());
        std::string deliveryTimeoutCharacter = std::to_string(deliveryTimeout);
        std::string handOutTag = "threadId: " + threadIdCharacter + "," + "threadName: " + threadName + "," +
            "eventName: " + eventName + "," + "deliveryTime: " + deliveryTimeCharacter + "," +
            "deliveryTimeout: " + deliveryTimeoutCharacter;
        if ((nowStart - std::chrono::milliseconds(deliveryTimeout)) > event->GetHandleTime()) {
            HiChecker::NotifySlowEvent(handOutTag);
            if (deliveryTimeoutCallback_) {
                deliveryTimeoutCallback_();
            }
        }
    }
#endif // HAS_HICHECKER_NATIVE_PART
}

void EventHandler::DistributeTimeAction(const InnerEvent::Pointer &event, InnerEvent::TimePoint nowStart)
{
#ifdef HAS_HICHECKER_NATIVE_PART
    if (!HiChecker::NeedCheckSlowEvent()) {
        return;
    }
    if (!eventRunner_ || !event) {
        return;
    }
    int64_t distributeTimeout = eventRunner_->GetDistributeTimeout();
    if (distributeTimeout > 0) {
        std::string threadName = eventRunner_->GetRunnerThreadName();
        std::string eventName = GetEventName(event);
        int64_t threadId = gettid();
        std::string threadIdCharacter = std::to_string(threadId);
        InnerEvent::TimePoint nowEnd = InnerEvent::Clock::now();
        std::chrono::duration<double> distributeTime = nowEnd - nowStart;
        std::string distributeTimeCharacter = std::to_string((distributeTime).count());
        std::string distributeTimeoutCharacter = std::to_string(distributeTimeout);
        std::string executeTag = "threadId: " + threadIdCharacter + "," + "threadName: " + threadName + "," +
            "eventName: " + eventName + "," + "distributeTime: " + distributeTimeCharacter + "," +
            "distributeTimeout: " + distributeTimeoutCharacter;
        if ((nowEnd - std::chrono::milliseconds(distributeTimeout)) > nowStart) {
            HiChecker::NotifySlowEvent(executeTag);
            if (distributeTimeoutCallback_) {
                distributeTimeoutCallback_();
            }
        }
    }
#endif // HAS_HICHECKER_NATIVE_PART
}

void EventHandler::DistributeEvent(const InnerEvent::Pointer &event)
{
    if (!event) {
        HILOGE("DistributeEvent: Could not distribute an invalid event");
        return;
    }

    currentEventHandler = shared_from_this();

    InnerEvent::TimePoint nowStart = InnerEvent::Clock::now();
    DeliveryTimeAction(event, nowStart);

    if (event->HasTask()) {
        // Call task callback directly if contains a task.
        (event->GetTaskCallback())();
    } else {
        // Otherwise let developers to handle it.
        ProcessEvent(event);
    }

    DistributeTimeAction(event, nowStart);
}

void EventHandler::Dump(Dumper &dumper)
{
    auto now = std::chrono::system_clock::now();
    auto tp = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto tt = std::chrono::system_clock::to_time_t(now);
    auto us = tp.time_since_epoch().count() % 1000;
    struct tm curTime = {0};
    localtime_r(&tt, &curTime);
    char sysTime[DATETIME_STRING_LENGTH];
    std::strftime(sysTime, sizeof(char) * DATETIME_STRING_LENGTH, "%Y%m%d %I:%M:%S.", &curTime);
    auto message = dumper.GetTag() + " EventHandler dump begain curTime:" +
                   std::string(sysTime) + std::to_string(us) + LINE_SEPARATOR;
    dumper.Dump(message);

    if (eventRunner_ == nullptr) {
        dumper.Dump(dumper.GetTag() + " event runner uninitialized!" + LINE_SEPARATOR);
    } else {
        eventRunner_->Dump(dumper);
    }
}

bool EventHandler::HasInnerEvent(uint32_t innerEventId)
{
    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("event runner uninitialized!");
        return false;
    }
    return eventRunner_->GetEventQueue()->HasInnerEvent(shared_from_this(), innerEventId);
}

bool EventHandler::HasInnerEvent(int64_t param)
{
    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        HILOGE("event runner uninitialized!");
        return false;
    }
    return eventRunner_->GetEventQueue()->HasInnerEvent(shared_from_this(), param);
}

std::string EventHandler::GetEventName(const InnerEvent::Pointer &event)
{
    std::string eventName;
    if (!event) {
        return eventName;
    }

    if (event->HasTask()) {
        eventName = event->GetTaskName();
    } else {
        eventName = std::to_string(event->GetInnerEventId());
    }
    return eventName;
}

bool EventHandler::IsIdle()
{
    if (!eventRunner_ || !eventRunner_->GetEventQueue()) {
        return false;
    }
    return eventRunner_->GetEventQueue()->IsIdle();
}

void EventHandler::ProcessEvent(const InnerEvent::Pointer &)
{}
}  // namespace AppExecFwk
}  // namespace OHOS
