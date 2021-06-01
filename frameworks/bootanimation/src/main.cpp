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

#include <securec.h>
#include <unistd.h>

#include <display_type.h>
#include <vsync_helper.h>
#include <window_manager.h>

#include "raw_parser.h"
#include "util.h"

using namespace OHOS;

namespace {
namespace {
constexpr uint32_t POSTTIME = 12 * 1000;
constexpr uint32_t PRETIME = 3 * 1000;
}

int32_t DoDraw(uint8_t *addr, uint32_t width, uint32_t height, uint32_t count)
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
    LOG("GetData time: %{public}lld, data: %{public}p, length: %{public}d", GetNowTime() - start, data, length);
    return 0;
}

void Draw(Window *window)
{
    sptr<Surface> surface = window->GetSurface();
    if (surface == nullptr) {
        LOG("surface is nullptr");
        return;
    }

    do {
        sptr<SurfaceBuffer> buffer;
        int32_t releaseFence;
        BufferRequestConfig config;
        window->GetRequestConfig(config);

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
        fflush(stdout);
        count++;
    } while (false);
}

void DoubleSync(int64_t time, void *data)
{
    static int32_t count = 0;
    if (count % 2 == 0) {
        Draw(reinterpret_cast<Window *>(data));
    }
    count++;
    RequestSync(DoubleSync, data);
}

void Main()
{
    const int32_t width = WindowManager::GetInstance()->GetMaxWidth();
    const int32_t height = WindowManager::GetInstance()->GetMaxHeight();
    WindowConfig config = {
        .width = width,
        .height = height,
        .pos_x = 0,
        .pos_y = 0,
        .format = PIXEL_FMT_RGBA_8888,
        .type = WINDOW_TYPE_NORMAL,
    };

    static std::unique_ptr<Window> window = WindowManager::GetInstance()->CreateWindow(&config);
    if (window == nullptr) {
        LOG("window is nullptr");
    } else {
        window->Move(0, 0);
        window->ReSize(width, height);
        window->SwitchTop();

        auto onWindowCreate = [](uint32_t pid) {
            constexpr uint32_t stringLength = 32;
            char filename[stringLength];
            (void)snprintf_s(filename,
                sizeof(filename), sizeof(filename) - 1, "/proc/%d/cmdline", pid);

            auto fp = fopen(filename, "r");
            if (fp == nullptr) {
                return;
            }

            char cmdline[stringLength] = {};
            fread(cmdline, sizeof(char), stringLength, fp);
            fclose(fp);

            if (strcmp(cmdline, "com.ohos.systemui") == 0 ||
                strcmp(cmdline, "com.ohos.launcher") == 0) {
                LOG("exiting");
                exit(0);
            }
        };
        window->RegistOnWindowCreateCb(onWindowCreate);

        DoubleSync(0, window.get());
    }

    PostTask([]() { LOG(""); exit(0); }, PRETIME + POSTTIME);
}
}

int main(int argc, const char *argv[])
{
    int64_t start = GetNowTime();
    std::string filename = "/system/etc/bootanimation.raw";
    if (RawParser::GetInstance()->Parse(filename)) {
        return -1;
    }
    LOG("time: %{public}lld", GetNowTime() - start);

    auto runner = AppExecFwk::EventRunner::Create(false);
    auto handler = std::make_shared<AppExecFwk::EventHandler>(runner);
    handler->PostTask(Main);
    runner->Run();
    return 0;
}
