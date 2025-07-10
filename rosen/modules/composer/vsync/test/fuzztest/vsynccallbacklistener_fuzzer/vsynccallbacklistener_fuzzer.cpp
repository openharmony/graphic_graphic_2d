/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "vsynccallbacklistener_fuzzer.h"

#include <cstdint>
#include <memory>
#include <securec.h>
#include <sstream>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <event_handler.h>
#include <sys/types.h>
#include "graphic_common_c.h"
#include "graphic_common.h"
#include "vsync_receiver.h"
#include "vsync_distributor.h"
#include "vsync_controller.h"


namespace OHOS {
    namespace {
        const uint8_t* g_data = nullptr;
        size_t g_size = 0;
        size_t g_pos;

        constexpr size_t STR_LEN = 10;
        void OnVSync(int64_t now, void* data) {}
    }

    /*
    * describe: get data from outside untrusted data(g_data) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (g_data == nullptr || objectSize > g_size - g_pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        g_pos += objectSize;
        return object;
    }

    /*
    * get a string from g_data
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen - 1] = '\0';
        for (int i = 0; i < strlen - 1; i++) {
            cstr[i] = GetData<char>();
        }
        std::string str(cstr);
        return str;
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        g_data = data;
        g_size = size;
        g_pos = 0;

        // get data
        bool needCreateNewThread = GetData<bool>();
        int64_t now = GetData<int64_t>();
        int32_t fileDescriptor = GetData<int32_t>();
        bool fdClosed = GetData<bool>();

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, 0);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        sptr<Rosen::VSyncConnection> vsyncConnection = new Rosen::VSyncConnection(vsyncDistributor, "Fuzz");
        std::shared_ptr<AppExecFwk::EventRunner> eventRunner = AppExecFwk::EventRunner::Create(needCreateNewThread);
        std::shared_ptr<AppExecFwk::EventHandler> eventHandler =
            std::make_shared<AppExecFwk::EventHandler>(eventRunner);
        sptr<Rosen::VSyncReceiver> vsyncReceiver = new Rosen::VSyncReceiver(
            vsyncConnection, nullptr, eventHandler, "FuzzTest");
        vsyncReceiver->listener_->OnReadable(vsyncReceiver->fd_);
        if (fileDescriptor >= 0 && fileDescriptor <= 2) { // 0, 1, 2 not allowed
            fileDescriptor = vsyncReceiver->fd_;
        }
        vsyncReceiver->listener_->OnShutdown(fileDescriptor);
        vsyncReceiver->listener_->CalculateExpectedEndLocked(now);
        vsyncReceiver->listener_->SetFdClosedFlagLocked(fdClosed);
        vsyncReceiver->listener_->RegisterFdShutDownCallback(nullptr);

        vsyncReceiver->Init();
        void* data1 = static_cast<void*>(GetStringFromData(STR_LEN).data());
        Rosen::VSyncReceiver::FrameCallback fcb = {
            .userData_ = data1,
            .callback_ = OnVSync,
        };
        vsyncReceiver->RequestNextVSync(fcb);
        vsyncReceiver->RequestNextVSyncWithMultiCallback(fcb);
        int32_t rate = GetData<int32_t>();
        vsyncReceiver->SetVSyncRate(fcb, rate);
        int64_t period = GetData<int64_t>();
        int64_t timestamp = GetData<int64_t>();
        bool isThreadShared = GetData<bool>();
        vsyncReceiver->GetVSyncPeriodAndLastTimeStamp(period, timestamp, isThreadShared);
        vsyncReceiver->IsRequestedNextVSync();
        bool uiDVSyncSwitch = GetData<bool>();
        bool nativeDVSyncSwitch = GetData<bool>();
        int32_t bufferCount = GetData<int32_t>();
        bool delayEnable = GetData<bool>();
        bool nativeDelayEnable = GetData<bool>();
        vsyncReceiver->SetNativeDVSyncSwitch(nativeDVSyncSwitch);
        vsyncReceiver->SetUiDvsyncSwitch(uiDVSyncSwitch);
        vsyncReceiver->SetUiDvsyncConfig(bufferCount, delayEnable, nativeDelayEnable);

        vsyncReceiver->fd_ = open("fuzztest", O_RDWR | O_CREAT);
        int64_t dataTmp[3];
        write(vsyncReceiver->fd_, dataTmp, sizeof(int64_t) * 3);
        vsyncReceiver->listener_->OnReadable(vsyncReceiver->fd_);
        return true;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

