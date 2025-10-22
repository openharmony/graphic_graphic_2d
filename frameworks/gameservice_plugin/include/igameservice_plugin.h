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

#ifndef RENDER_SERVICE_BASE_GAMESERVICE_PLUGIN_H
#define RENDER_SERVICE_BASE_GAMESERVICE_PLUGIN_H

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif
#include <mutex>
#include <refbase.h>

namespace OHOS::Rosen {
class IGameServicePlugin : public RefBase {
private:
    using LoadGameServiceFunc = void (*)(sptr<OHOS::Rosen::IGameServicePlugin>& instance);
    static void* loadFileHandle_;
    static sptr<IGameServicePlugin> instance_;
    static std::mutex mutex_;

public:
    static sptr<IGameServicePlugin>& Instance()
    {
        if (instance_ == nullptr) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (instance_ != nullptr) {
                return instance_;
            }
#ifdef _WIN32
            instance_ = new IGameServicePlugin();
            return instance_;
#else
            loadFileHandle_ = dlopen("libgameservice_graphic_plugin.z.so", RTLD_NOW);
            if (loadFileHandle_ == nullptr) {
                instance_ = new IGameServicePlugin();
                return instance_;
            }
            LoadGameServiceFunc loadGameServiceFunc =
                reinterpret_cast<LoadGameServiceFunc>(dlsym(loadFileHandle_, "LoadGameServicePlugin"));
            if (loadGameServiceFunc == nullptr) {
                dlclose(loadFileHandle_);
                instance_ = new IGameServicePlugin();
                return instance_;
            }
            loadGameServiceFunc(instance_);
            if (instance_ == nullptr) {
                instance_ = new IGameServicePlugin();
            }
#endif
        }
        return instance_;
    }

    IGameServicePlugin() = default;
    virtual ~IGameServicePlugin()
    {
#ifndef _WIN32
        if (loadFileHandle_ != nullptr) {
            dlclose(loadFileHandle_);
            loadFileHandle_ = nullptr;
        }
#endif
    }

    virtual void InitEglSliceReport() {}
    virtual void AddEglGraphicCount() {}

    virtual void InitVulkanReport() {}
    virtual void ReportVulkanRender() {}
};
void* IGameServicePlugin::loadFileHandle_ = nullptr;
sptr<IGameServicePlugin> IGameServicePlugin::instance_ = nullptr;
std::mutex IGameServicePlugin::mutex_;
}  // namespace OHOS::Rosen

#endif // RENDER_SERVICE_BASE_GAMESERVICE_PLUGIN_H