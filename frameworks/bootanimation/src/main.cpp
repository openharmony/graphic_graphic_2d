/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <memory>
#include <securec.h>
#include <sstream>
#include <unistd.h>

#include <display_type.h>
#include <vsync_helper.h>
#include <window_manager.h>
#include <window_manager_service_client.h>

#include "raw_parser.h"
#include "util.h"

using namespace OHOS;

class Main : public IWindowChangeListenerClazz {
public:
    void Init(int32_t width, int32_t height);
    int32_t DoDraw(uint8_t *addr, uint32_t width, uint32_t height, uint32_t count);
    void Draw(Window *window);
    void Sync(int64_t time, void *data);
    void OnWindowCreate(int32_t pid, int32_t wid) override;
    void OnWindowDestroy(int32_t pid, int32_t wid) override;
};

int32_t Main::DoDraw(uint8_t *addr, uint32_t width, uint32_t height, uint32_t count)
{
    constexpr uint32_t stride = 4;
    int32_t addrSize = width * height * stride;
    static auto frame = std::make_unique<uint8_t[]>(addrSize);
    static uint32_t last = -1;

    int64_t start = GetNowTime();

    uint8_t *data = nullptr;
    uint32_t length;
    uint32_t offset;
    if (RawParser::GetInstance()->GetData(count, data, offset, length)) {
        return -1;
    }

    if (last != count && length > 0) {
        memcpy_s(frame.get() + offset, addrSize - offset, data, length);
    }

    memcpy_s(addr, addrSize, frame.get(), addrSize);
    last = count;
    LOG("GetData time: %{public}" PRIu64 ", data: %{public}p, length: %{public}d",
        GetNowTime() - start, data, length);
    return 0;
}

void Main::Draw(Window *window)
{
    sptr<Surface> surface = window->GetSurface();
    if (surface == nullptr) {
        LOG("surface is nullptr");
        return;
    }

    do {
        sptr<SurfaceBuffer> buffer;
        int32_t releaseFence;
        BufferRequestConfig config = {
            .width = surface->GetDefaultWidth(),
            .height = surface->GetDefaultHeight(),
            .strideAlignment = 0x8,
            .format = PIXEL_FMT_RGBA_8888,
            .usage = surface->GetDefaultUsage(),
        };

        SurfaceError ret = surface->RequestBuffer(buffer, releaseFence, config);
        if (ret == SURFACE_ERROR_NO_BUFFER) {
            break;
        }
        if (ret) {
            LOG("RequestBuffer failed: %{public}s", SurfaceErrorStr(ret).c_str());
            break;
        }
        if (buffer == nullptr) {
            break;
        }

        static uint32_t count = 0;
        auto addr = static_cast<uint8_t *>(buffer->GetVirAddr());
        while (true) {
            int32_t drawRet = DoDraw(addr, buffer->GetWidth(), buffer->GetHeight(), count);
            if (drawRet && count == 0) {
                exit(1);
            }
            if (drawRet) {
                count--;
                continue;
            }
            break;
        }

        BufferFlushConfig flushConfig = {
            .damage = {
                .w = buffer->GetWidth(),
                .h = buffer->GetHeight(),
            },
        };
        ret = surface->FlushBuffer(buffer, -1, flushConfig);

        LOG("Sync %{public}d %{public}s", count, SurfaceErrorStr(ret).c_str());
        count++;
    } while (false);
}

void Main::Sync(int64_t time, void *data)
{
    Draw(reinterpret_cast<Window *>(data));
    struct FrameCallback cb = {
        .frequency_ = 30,
        .timestamp_ = 0,
        .userdata_ = data,
        .callback_ = std::bind(&Main::Sync, this, SYNC_FUNC_ARG),
    };

    VsyncError ret = VsyncHelper::Current()->RequestFrameCallback(cb);
    if (ret) {
        LOG("RequestFrameCallback inner %{public}d\n", ret);
    }
}

void Main::Init(int32_t width, int32_t height)
{
    const auto &wmi = WindowManager::GetInstance();
    auto option = WindowOption::Get();
    option->SetWindowType(WINDOW_TYPE_NORMAL);
    option->SetWidth(width);
    option->SetHeight(height);
    option->SetX(0);
    option->SetY(0);

    sptr<Window> window = nullptr;
    auto wret = wmi->CreateWindow(window, option);
    if (wret != WM_OK || window == nullptr) {
        LOG("WindowManager::CreateWindow() return %{public}s", WMErrorStr(wret).c_str());
        exit(1);
    }
    window->SwitchTop();

    const auto &wmsc = WindowManagerServiceClient::GetInstance();
    wret = wmsc->Init();
    if (wret != WM_OK) {
        LOG("WindowManagerServiceClient::Init() return %{public}s", WMErrorStr(wret).c_str());
        exit(1);
    }

    const auto &wms = wmsc->GetService();
    wms->OnWindowListChange(this);

    Sync(0, nullptr);

    constexpr int32_t exitTime = 15 * 1000;
    PostTask(std::bind(exit, 0), exitTime);
}

void Main::OnWindowCreate(int32_t pid, int32_t wid)
{
    std::stringstream ss;
    ss << "/proc/" << pid << "/cmdline";
    std::ifstream ifs(ss.str(), std::ios::in);
    if (ifs.is_open()) {
        constexpr const char *systemui = "com.ohos.systemui";
        constexpr const char *launcher = "com.ohos.launcher";
        char cmdline[0x100] = {};
        if (ifs.getline(cmdline, sizeof(cmdline))) {
            bool have = false;
            have = have || strstr(cmdline, systemui) == cmdline;
            have = have || strstr(cmdline, launcher) == cmdline;
            if (have == true) {
                LOG("exiting");
                exit(0);
            }
        }
    }
}

void Main::OnWindowDestroy(int32_t pid, int32_t wid)
{
}

int main(int argc, const char *argv[])
{
    const auto &wmi = WindowManager::GetInstance();
    auto wret = wmi->Init();
    if (wret != WM_OK) {
        LOG("WindowManager::Init() return %{public}s", WMErrorStr(wret).c_str());
        return 1;
    }

    std::vector<struct WMDisplayInfo> displays;
    wret = wmi->GetDisplays(displays);
    if (wret != WM_OK) {
        LOG("WindowManager::GetDisplays() return %{public}s", WMErrorStr(wret).c_str());
        return 1;
    }

    if (displays.size() == 0) {
        LOG("no display, cannot continue");
        return 1;
    }

    const int32_t width = displays[0].width;
    const int32_t height = displays[0].height;

    int64_t start = GetNowTime();
    std::stringstream ss;
    ss << "/system/etc/bootanimation-" << width << "x" << height << ".raw";
    std::string filename = ss.str();
    if (RawParser::GetInstance()->Parse(filename)) {
        return -1;
    }
    LOG("time: %{public}" PRIu64 "", GetNowTime() - start);

    Main m;

    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask(std::bind(&Main::Init, &m, width, height));
    runner->Run();
    return 0;
}
