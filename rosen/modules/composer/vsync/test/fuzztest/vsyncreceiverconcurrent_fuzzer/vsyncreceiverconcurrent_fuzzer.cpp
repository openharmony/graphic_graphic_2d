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

#include "vsyncreceiverconcurrent_fuzzer.h"

#include <cstdint>
#include <memory>
#include <securec.h>
#include <sstream>
#include <sys/socket.h>

#include <event_handler.h>
#include <sys/types.h>
#include <vector>
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

    void Thread1(sptr<Rosen::VSyncReceiver> &receiver, int32_t fd)
    {
        receiver->listener_->OnReadable(fd);
    }

    void Thread2(sptr<Rosen::VSyncReceiver> &receiver)
    {
        receiver->RemoveAndCloseFdLocked();
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

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        int64_t offset = GetData<int64_t>();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        sptr<Rosen::VSyncConnection> vsyncConnection = new Rosen::VSyncConnection(vsyncDistributor, "Fuzz");
        sptr<Rosen::VSyncReceiver> vsyncReceiver = new Rosen::VSyncReceiver(vsyncConnection);
        vsyncReceiver->Init();
        vsyncReceiver->looper_->RemoveFileDescriptorListener(vsyncReceiver->fd_);
        vsyncReceiver->looper_ = nullptr;
        vsyncReceiver->fd_ = -1;

        std::thread thread1 = std::thread([&vsyncReceiver] { Thread1(vsyncReceiver, vsyncReceiver->fd_); });
        std::thread thread2 = std::thread([&vsyncReceiver] { Thread2(vsyncReceiver); });
        thread1.join();
        thread2.join();

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

