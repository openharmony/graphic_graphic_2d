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

#include "hdibackend_fuzzer.h"

#include <securec.h>

#include "hdi_backend.h"
#include "surface.h"
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
        if (data == nullptr) {
            return false;
        }

        // initialize
        data_ = data;
        size_ = size;
        pos = 0;

        // get data
        uint32_t screenId = GetData<uint32_t>();
        void* data1 = static_cast<void*>(GetStringFromData(STR_LEN).data());
        void* data2 = static_cast<void*>(GetStringFromData(STR_LEN).data());

        // test
        std::vector<OutputPtr> outputs;
        OutputPtr outputptr = HdiOutput::CreateHdiOutput(screenId);
        outputptr->Init();
        std::vector<LayerInfoPtr> layerInfos;
        LayerInfoPtr layerInfoptr = HdiLayerInfo::CreateHdiLayerInfo();
        sptr<Surface> cSurface = Surface::CreateSurfaceAsConsumer();
        layerInfoptr->SetSurface(cSurface);
        layerInfos.push_back(layerInfoptr);
        outputptr->SetLayerInfo(layerInfos);
        outputs.push_back(outputptr);

        HdiBackend* hdiBackend_ = HdiBackend::GetInstance();
        auto onScreenHotplugFunc = [](OutputPtr &, bool, void*) -> void {};
        auto onPrepareCompleteFunc = [](sptr<Surface> &, const struct PrepareCompleteParam &, void*) -> void {};
        hdiBackend_->RegScreenHotplug(onScreenHotplugFunc, data1);
        hdiBackend_->RegPrepareComplete(onPrepareCompleteFunc, data2);
        hdiBackend_->Repaint(outputs);
        for (auto output : outputs) {
            hdiBackend_->GetLayersReleaseFence(output);
        }

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

