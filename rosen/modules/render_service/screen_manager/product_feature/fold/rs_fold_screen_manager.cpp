/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "rs_fold_screen_manager.h"

#include <parameter.h>
#include <parameters.h>
#include <screen_manager/rs_screen_preprocessor.h>

#include "param/sys_param.h"
#include "platform/common/rs_log.h"
#include "platform/common/rs_system_properties.h"

#undef LOG_TAG
#define LOG_TAG "RSFoldScreenManager"
namespace OHOS {
namespace Rosen {
namespace {
constexpr float ANGLE_MIN_VAL = 0.0F;
constexpr float ANGLE_MAX_VAL = 180.0F;
constexpr float OPEN_HALF_FOLDED_MIN_THRESHOLD = 25.0F;
constexpr uint32_t WAIT_FOR_ACTIVE_SCREEN_ID_TIMEOUT = 1000;
constexpr int32_t SENSOR_SUCCESS = 0;
constexpr int32_t POSTURE_INTERVAL = 4000000;
constexpr uint16_t SENSOR_EVENT_FIRST_DATA = 0;
const std::string BOOTEVENT_BOOT_COMPLETED = "bootevent.boot.completed";
std::function<void(SensorEvent*)> sensorCallback = nullptr;

FoldState TransferAngleToScreenState(float angle)
{
    return std::islessequal(angle, OPEN_HALF_FOLDED_MIN_THRESHOLD) ? FoldState::FOLDED : FoldState::EXPAND;
}
} // namespace

RSFoldScreenManager::~RSFoldScreenManager() noexcept {}

void RSFoldScreenManager::SetExternalScreenId(ScreenId externalScreenId)
{
    RS_LOGI("%{public}s The externalScreenId_ is set %{public}" PRIu64 ".", __func__, externalScreenId);
    externalScreenId_ = externalScreenId;
}

void RSFoldScreenManager::Init()
{
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    RS_LOGI("%{public}s FoldScreen need to RegisterSensorCallback.", __func__);
    RegisterSensorCallback();
    RSSystemProperties::WatchSystemProperty(BOOTEVENT_BOOT_COMPLETED.c_str(), OnBootComplete, this);
    bool bootCompleted = RSSystemProperties::GetBootCompleted();
    if (UNLIKELY(bootCompleted)) {
        RS_LOGW("%{public}s boot completed.", __func__);
        UnRegisterSensorCallback();
    }
#endif // RS_SUBSCRIBE_SENSOR_ENABLE
}

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
void RSFoldScreenManager::HandleSensorData(float angle)
{
    if (std::isless(angle, ANGLE_MIN_VAL) || std::isgreater(angle, ANGLE_MAX_VAL)) {
        RS_LOGW("%{public}s Invalid angle value, angle is %{public}f.", __func__, angle);
        return;
    }
    std::unique_lock<std::mutex> lock(activeScreenIdAssignedMutex_);
    if (TransferAngleToScreenState(angle) == FoldState::FOLDED) {
        if (activeScreenId_ != externalScreenId_ || !isPostureSensorDataHandled_) {
            RS_LOGI("%{public}s: foldState is FoldState::FOLDED, angle is %{public}.2f.", __func__, angle);
        }
        if (activeScreenId_ != externalScreenId_) {
            activeScreenId_ = externalScreenId_;
            screenPreprocessor_.NotifyActiveScreenIdChanged(activeScreenId_);
        }
    } else {
        if (activeScreenId_ != innerScreenId_ || !isPostureSensorDataHandled_) {
            RS_LOGI("%{public}s: foldState is not FoldState::FOLDED, angle is %{public}.2f.", __func__, angle);
        }
        if (activeScreenId_ != innerScreenId_) {
            activeScreenId_ = innerScreenId_;
            screenPreprocessor_.NotifyActiveScreenIdChanged(activeScreenId_);
        }
    }
    isPostureSensorDataHandled_ = true;
    activeScreenIdAssignedCV_.notify_one();
}
#endif // RS_SUBSCRIBE_SENSOR_ENABLE

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
ScreenId RSFoldScreenManager::GetActiveScreenId()
{
    std::unique_lock<std::mutex> lock(activeScreenIdAssignedMutex_);
    if (isPostureSensorDataHandled_) {
        RS_LOGW("%{public}s: posture sensor data has been handled, activeScreenId: %{public}" PRIu64, __func__,
                activeScreenId_);
        return activeScreenId_;
    }
    activeScreenIdAssignedCV_.wait_until(lock, std::chrono::system_clock::now() +
        std::chrono::milliseconds(WAIT_FOR_ACTIVE_SCREEN_ID_TIMEOUT), [this]() {
            return isPostureSensorDataHandled_;
        });
    if (!isPostureSensorDataHandled_) {
        RS_LOGW("%{public}s: timeout", __func__);
    }
    HILOG_COMM_WARN("GetActiveScreenId activeScreenId: %{public}" PRIu64, activeScreenId_);
    return activeScreenId_;
}
#else // RS_SUBSCRIBE_SENSOR_ENABLE
ScreenId RSFoldScreenManager::GetActiveScreenId()
{
    return INVALID_SCREEN_ID;
}
#endif // RS_SUBSCRIBE_SENSOR_ENABLE

#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
void RSFoldScreenManager::RegisterSensorCallback()
{
    std::unique_lock<std::mutex> lock(registerSensorMutex_);
    if (hasRegisterSensorCallback_) {
        RS_LOGE("%{public}s hasRegisterSensorCallback_ is true", __func__);
        return;
    }
    hasRegisterSensorCallback_ = true;
    sensorCallback = std::bind(&RSFoldScreenManager::HandlePostureData, this, std::placeholders::_1);
    sensorUser_.callback = [](SensorEvent* event) { sensorCallback(event); };
    int32_t subscribeRet;
    int32_t setBatchRet;
    int32_t activateRet;
    int tryCnt = 0;
    constexpr int tryLimit = 5; // 5 times failure limit
    do {
        subscribeRet = SubscribeSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_);
        RS_LOGI("%{public}s: subscribeRet: %{public}d", __func__, subscribeRet);
        setBatchRet = SetBatch(SENSOR_TYPE_ID_POSTURE, &sensorUser_, POSTURE_INTERVAL, POSTURE_INTERVAL);
        RS_LOGI("%{public}s: setBatchRet: %{public}d", __func__, setBatchRet);
        activateRet = ActivateSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_);
        RS_LOGI("%{public}s: activateRet: %{public}d", __func__, activateRet);
        if (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS || activateRet != SENSOR_SUCCESS) {
            RS_LOGE("%{public}s failed subscribeRet:%{public}d, setBatchRet:%{public}d, activateRet:%{public}d",
                    __func__, subscribeRet, setBatchRet, activateRet);
            usleep(1000); // wait 1000 us for next try
            tryCnt++;
        }
    } while (tryCnt <= tryLimit && (subscribeRet != SENSOR_SUCCESS || setBatchRet != SENSOR_SUCCESS ||
        activateRet != SENSOR_SUCCESS));
    if (tryCnt <= tryLimit) {
        RS_LOGI("%{public}s success.", __func__);
    }
}

void RSFoldScreenManager::UnRegisterSensorCallback()
{
    std::unique_lock<std::mutex> lock(registerSensorMutex_);
    if (!hasRegisterSensorCallback_) {
        RS_LOGE("%{public}s hasRegisterSensorCallback_ is false", __func__);
        return;
    }
    hasRegisterSensorCallback_ = false;
    int32_t deactivateRet = DeactivateSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_);
    int32_t unsubscribeRet = UnsubscribeSensor(SENSOR_TYPE_ID_POSTURE, &sensorUser_);
    if (deactivateRet == SENSOR_SUCCESS && unsubscribeRet == SENSOR_SUCCESS) {
        RS_LOGI("%{public}s success.", __func__);
    } else {
        RS_LOGE("%{public}s failed, deactivateRet:%{public}d, unsubscribeRet:%{public}d",
                __func__, deactivateRet, unsubscribeRet);
    }
}

void RSFoldScreenManager::OnBootComplete(const char* key, const char* value, void* context)
{
    if (strcmp(key, BOOTEVENT_BOOT_COMPLETED.c_str()) == 0 && strcmp(value, "true") == 0) {
        if (!context) {
            RS_LOGI("%{public}s: data is nullptr", __func__);
            return;
        } else {
            RS_LOGI("%{public}s: data is not nullptr", __func__);
        }
        auto foldScreenManager = static_cast<RSFoldScreenManager*>(context);
        foldScreenManager->OnBootCompleteEvent();
    } else {
        RS_LOGE("%{public}s key:%{public}s, value:%{public}s", __func__, key, value);
    }
}

void RSFoldScreenManager::OnBootCompleteEvent()
{
    RS_LOGI("%{public}s: UnRegisterSensorCallback", __func__);
    UnRegisterSensorCallback();
}

void RSFoldScreenManager::HandlePostureData(const SensorEvent* const event)
{
    if (event == nullptr) {
        RS_LOGW("%{public}s SensorEvent is nullptr.", __func__);
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].data == nullptr) {
        RS_LOGW("%{public}s SensorEvent[0].data is nullptr.", __func__);
        return;
    }
    if (event[SENSOR_EVENT_FIRST_DATA].dataLen < sizeof(PostureData)) {
        RS_LOGW("%{public}s SensorEvent dataLen less than posture data size.", __func__);
        return;
    }
    PostureData* postureData = reinterpret_cast<PostureData*>(event[SENSOR_EVENT_FIRST_DATA].data);
    float angle = postureData->angle;
    RS_LOGD("%{public}s angle value in PostureData is: %{public}f.", __func__, angle);
    HandleSensorData(angle);
}
#endif // RS_SUBSCRIBE_SENSOR_ENABLE
} // namespace Rosen
} // namespace OHOS