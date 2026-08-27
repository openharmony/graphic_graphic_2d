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
 
#ifndef RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_DISPLAY_EFFECT_SWING_CONTROL_H
#define RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_DISPLAY_EFFECT_SWING_CONTROL_H
 
#include <cstdint>
#include <mutex>
#include "common/rs_common_def.h"
#include "common/rs_macros.h"
#include "screen_manager/screen_types.h"
 
namespace OHOS {
namespace Rosen {
struct SwingData {
    float eye_x;
    float eye_y;
    float eye_z;
};
 
class RSDisplayEffectSwingControlInterface {
public:
    virtual ~RSDisplayEffectSwingControlInterface() = default;
    virtual bool Init() = 0;
    virtual void SetSwingEnabled(ScreenId screenId, bool enable) = 0;
    virtual bool IsSwingRegistered(ScreenId screenId) = 0;
    virtual SwingData GetSwingData(ScreenId screenId) = 0;
};
 
class RSB_EXPORT RSDisplayEffectSwingControl {
public:
    RSDisplayEffectSwingControl(const RSDisplayEffectSwingControl&) = delete;
    RSDisplayEffectSwingControl& operator=(const RSDisplayEffectSwingControl&) = delete;
    RSDisplayEffectSwingControl(RSDisplayEffectSwingControl&&) = delete;
    RSDisplayEffectSwingControl& operator=(RSDisplayEffectSwingControl&&) = delete;
 
    RSB_EXPORT static RSDisplayEffectSwingControl& Get();
    RSB_EXPORT void Init();
    RSB_EXPORT void Deinit();
 
    RSB_EXPORT void SetSwingEnabled(ScreenId screenId, bool enable);
    RSB_EXPORT bool IsSwingRegistered(ScreenId screenId);
    RSB_EXPORT SwingData GetSwingData(ScreenId screenId);
 
private:
    RSDisplayEffectSwingControl() = default;
    ~RSDisplayEffectSwingControl();
    bool LoadLibrary();
    void CloseLibrary();
 
    std::mutex mLibMutex{};
    bool isInited_{false};
    void *extLibHandle_{nullptr};
    RSDisplayEffectSwingControlInterface* swingControlInterface_{nullptr};
    using CreateFunc = RSDisplayEffectSwingControlInterface*(*)();
    using DestroyFunc = void(*)();
    CreateFunc create_{nullptr};
    DestroyFunc destroy_{nullptr};
};
} // namespace Rosen
} // namespace OHOS
 
#endif // RENDER_SERVICE_BASE_DISPLAY_ENGINE_RS_DISPLAY_EFFECT_SWING_CONTROL_H