/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "vsyncreceiver_fuzzer.h"

#include <securec.h>
#include <sstream>

#include "graphic_common_c.h"
#include "graphic_common.h"
#include "vsync_receiver.h"
#include "vsync_distributor.h"
#include "vsync_controller.h"


namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;

        void OnVSync(int64_t now, void* data) {}
    }

    /*
    * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
    * tips: only support basic type
    */
    template<class T>
    T GetData()
    {
        T object {};
        size_t objectSize = sizeof(object);
        if (data_ == nullptr || objectSize > size_ - pos) {
            return object;
        }
        errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
        if (ret != EOK) {
            return {};
        }
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
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

    void GraphicCommonTest()
    {
        GSError err = GetData<GSError>();
        GSError err1 = GetData<GSError>();
        GSError err2 = GetData<GSError>();
        std::stringstream ss;

        LowErrorStr(err);
        GSErrorStr(GSERROR_OK);
        GSErrorStr(GSERROR_INVALID_ARGUMENTS);
        GSErrorStr(GSERROR_NO_PERMISSION);
        GSErrorStr(GSERROR_CONNOT_CONNECT_SAMGR);
        GSErrorStr(GSERROR_CONNOT_CONNECT_SERVER);
        GSErrorStr(GSERROR_CONNOT_CONNECT_WESTON);
        GSErrorStr(GSERROR_NO_BUFFER);
        GSErrorStr(GSERROR_NO_ENTRY);
        GSErrorStr(GSERROR_OUT_OF_RANGE);
        GSErrorStr(GSERROR_NO_SCREEN);
        GSErrorStr(GSERROR_INVALID_OPERATING);
        GSErrorStr(GSERROR_NO_CONSUMER);
        GSErrorStr(GSERROR_NOT_INIT);
        GSErrorStr(GSERROR_TYPE_ERROR);
        GSErrorStr(GSERROR_API_FAILED);
        GSErrorStr(GSERROR_INTERNAL);
        GSErrorStr(GSERROR_NO_MEM);
        GSErrorStr(GSERROR_PROXY_NOT_INCLUDE);
        GSErrorStr(GSERROR_SERVER_ERROR);
        GSErrorStr(GSERROR_ANIMATION_RUNNING);
        GSErrorStr(GSERROR_NOT_IMPLEMENT);
        GSErrorStr(GSERROR_NOT_SUPPORT);
        GSErrorStr(GSERROR_BINDER);
        GSErrorStr(err);
        SurfaceErrorStr(err);
        if (err1 == err2) {
            ss << err1;
        } else if (err1 != err2) {
            ss << err2;
        }
    }

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        int64_t offset = GetData<int64_t>();
        int32_t rate = GetData<int32_t>();
        void* data1 = static_cast<void*>(GetStringFromData(STR_LEN).data());
        std::string name = GetStringFromData(STR_LEN);
        bool rnvFlag = GetData<bool>();

        // test
        sptr<Rosen::VSyncGenerator> vsyncGenerator = Rosen::CreateVSyncGenerator();
        sptr<Rosen::VSyncController> vsyncController = new Rosen::VSyncController(vsyncGenerator, offset);
        sptr<Rosen::VSyncDistributor> vsyncDistributor = new Rosen::VSyncDistributor(vsyncController, "Fuzz");
        sptr<Rosen::VSyncConnection> vsyncConnection = new Rosen::VSyncConnection(vsyncDistributor, "Fuzz");
        sptr<Rosen::VSyncReceiver> vsyncReceiver = new Rosen::VSyncReceiver(vsyncConnection);
        Rosen::VSyncReceiver::FrameCallback fcb = {
            .userData_ = data1,
            .callback_ = OnVSync,
        };
        vsyncReceiver->SetVSyncRate(fcb, rate);
        vsyncReceiver->RequestNextVSync(fcb);
        vsyncReceiver->RequestNextVSyncWithMultiCallback(fcb);
        std::shared_ptr<Rosen::VSyncCallBackListener> vsyncCallBackListener(
            std::make_shared<Rosen::VSyncCallBackListener>());
        vsyncCallBackListener->SetCallback(fcb);
        vsyncCallBackListener->SetName(name);
        vsyncCallBackListener->SetRNVFlag(rnvFlag);
        vsyncCallBackListener->GetRNVFlag();
        vsyncCallBackListener->GetPeriod();
        vsyncCallBackListener->GetTimeStamp();
        vsyncCallBackListener->GetPeriodShared();
        vsyncCallBackListener->GetTimeStampShared();
        vsyncCallBackListener->AddCallback(fcb);

        GraphicCommonTest();
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

