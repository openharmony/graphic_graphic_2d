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

#ifndef RENDER_SERVICE_BASE_APS_PLUGIN_H
#define RENDER_SERVICE_BASE_APS_PLUGIN_H

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <mutex>
#include <refbase.h>

namespace OHOS::Rosen {

class IApsPlugin : public RefBase {
private:
    using LoadApsFunc = void (*)(sptr<OHOS::Rosen::IApsPlugin>& instance);
    static void* loadFileHandle_;
    static sptr<IApsPlugin> instance_;
    static std::mutex mutex_;

public:
    static sptr<IApsPlugin>& Instance()
    {
        if (instance_ == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (instance_ != nullptr) {
                return instance_;
            }
#ifdef _WIN32
            instance_ = new IApsPlugin();
            return instance_;
#else
            loadFileHandle_ = dlopen("libaps_client.z.so", RTLD_NOW);
            if (loadFileHandle_ == nullptr) {
                instance_ = new IApsPlugin();
                return instance_;
            }
            LoadApsFunc loadApsFunc = (LoadApsFunc)(dlsym(loadFileHandle_, "LoadApsPlugin"));
            if (loadApsFunc == nullptr) {
                dlclose(loadFileHandle_);
                instance_ = new IApsPlugin();
                return instance_;
            }
            loadApsFunc(instance_);
            if (instance_ == nullptr) {
                instance_ = new IApsPlugin();
            }
#endif
        }
        return instance_;
    }

    IApsPlugin() = default;
    virtual ~IApsPlugin() = default;

    virtual void InitGameFpsCtrol() {}
    virtual void PowerCtrllofSwapbuffer() {}
};
void* IApsPlugin::loadFileHandle_ = nullptr;
sptr<IApsPlugin> IApsPlugin::instance_ = nullptr;
std::mutex IApsPlugin::mutex_;
} // namespace OHOS::Rosen
#endif
