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

#include "hdi_backend.h"
using namespace OHOS::Rosen;

namespace OHOS {
    namespace {
        constexpr size_t STR_LEN = 10;
        constexpr char STR_END_CHAR = '\0';
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
        std::memcpy(&object, data_ + pos, objectSize);
        pos += objectSize;
        return object;
    }

    /*
    * get a string from data_
    */
    std::string GetStringFromData(int strlen)
    {
        char cstr[strlen];
        cstr[strlen-1] = STR_END_CHAR;
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
        std::vector<OutputPtr> outputs;
        uint32_t screenId = GetData<uint32_t>();
        OutputPtr outputptr = HdiOutput::CreateHdiOutput(screenId);
        outputs.push_back(outputptr);
        void* data1 = static_cast<void*>(GetStringFromData(STR_LEN).data());
        void* data2 = static_cast<void*>(GetStringFromData(STR_LEN).data());

        // test
        HdiBackend* hdiBackend_ = HdiBackend::GetInstance();
        auto onScreenHotplugFunc = [](OutputPtr &, bool, void*) -> void {};
        auto onPrepareCompleteFunc = [](sptr<Surface> &, const struct PrepareCompleteParam &, void*) -> void {};
        hdiBackend_->RegScreenHotplug(onScreenHotplugFunc, data1);
        hdiBackend_->RegPrepareComplete(onPrepareCompleteFunc, data2);
        hdiBackend_->Repaint(outputs);

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

