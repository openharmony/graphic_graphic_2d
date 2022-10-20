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

#include "hdioutput_fuzzer.h"

#include <securec.h>

#include "hdi_output.h"
#include "surface_buffer.h"
#include "surface_buffer_impl.h"
using namespace OHOS::Rosen;

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        const uint8_t* data_ = nullptr;
        size_t size_ = 0;
        size_t pos;
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

    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        if (data == nullptr || size < 0) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        uint32_t screenId = GetData<uint32_t>();
        uint32_t num = GetData<uint32_t>();
        IRect outputDamage = GetData<IRect>();
        int64_t timeStamp = GetData<int64_t>();
        bool enableStatus = GetData<bool>();
        uint32_t seqNum = GetData<uint32_t>();
        uint32_t layerCompositionCapacity = GetData<uint32_t>();
        std::string result = GetStringFromData(STR_LEN);
        std::string arg = GetStringFromData(STR_LEN);

        // test
        std::shared_ptr<HdiOutput> hdiOutput = HdiOutput::CreateHdiOutput(screenId);
        std::vector<LayerInfoPtr> layerInfos;
        LayerInfoPtr layerInfoptr = HdiLayerInfo::CreateHdiLayerInfo();
        layerInfos.push_back(layerInfoptr);
        hdiOutput->SetLayerInfo(layerInfos);
        hdiOutput->SetOutputDamage(num, outputDamage);
        hdiOutput->RecordCompositionTime(timeStamp);
        hdiOutput->SetDirectClientCompEnableStatus(enableStatus);
        sptr<SurfaceBuffer> buffer = new SurfaceBufferImpl(seqNum);
        sptr<SyncFence> syncFence = SyncFence::INVALID_FENCE;
        hdiOutput->ReleaseFramebuffer(buffer, syncFence);
        hdiOutput->SetLayerCompCapacity(layerCompositionCapacity);
        hdiOutput->Dump(result);
        hdiOutput->DumpFps(result, arg);
        
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

