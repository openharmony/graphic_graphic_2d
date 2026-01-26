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

#ifndef RS_FOLD_SCREEN_MANAGER_H
#define RS_FOLD_SCREEN_MANAGER_H
#include <mutex>
#include <screen_manager/screen_types.h>
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
#include <sensor_agent.h>
#include <sensor_agent_type.h>
#endif

namespace OHOS {
namespace Rosen {
class RSScreenPreprocessor;
enum class FoldState : uint32_t {
    UNKNOW,
    FOLDED,
    EXPAND
};

class RSFoldScreenManager {
public:
    RSFoldScreenManager(RSScreenPreprocessor& screenPreprocessor)
        : screenPreprocessor_(screenPreprocessor) {}
    ~RSFoldScreenManager() noexcept;
    ScreenId GetActiveScreenId();
    void SetExternalScreenId(ScreenId externalScreenId);
    void Init();
private:
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    void RegisterSensorCallback();
    void UnRegisterSensorCallback();
    static void OnBootComplete(const char* key, const char* value, void *context);
    void OnBootCompleteEvent();
    void HandlePostureData(const SensorEvent* const event);
    void HandleSensorData(float angle);
    FoldState TransferAngleToScreenState(float angle);
#endif

    RSScreenPreprocessor& screenPreprocessor_;
#ifdef RS_SUBSCRIBE_SENSOR_ENABLE
    SensorUser sensorUser_;
    bool hasRegisterSensorCallback_ = false;
    mutable std::mutex registerSensorMutex_;
    const ScreenId innerScreenId_ = 0;
    ScreenId externalScreenId_ = INVALID_SCREEN_ID;
    ScreenId activeScreenId_ = 0;
    bool isPostureSensorDataHandled_ = false;
    std::condition_variable activeScreenIdAssignedCV_;
    mutable std::mutex activeScreenIdAssignedMutex_;
#endif
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_FOLD_SCREEN_MANAGER_H
