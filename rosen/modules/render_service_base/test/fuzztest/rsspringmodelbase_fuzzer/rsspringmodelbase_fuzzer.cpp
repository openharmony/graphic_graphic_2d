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

#include "rsspringmodelbase_fuzzer.h"

#include <securec.h>

#include "animation/rs_spring_model.h"
#include "modifier/rs_render_property.h"

namespace OHOS {
    using namespace Rosen;
    namespace {
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

    void RSSpringModelFuzzerTest()
    {
        // get data
        float initialOffset = GetData<float>();
        float initialVelocity = GetData<float>();
        float response = GetData<float>();
        float dampingRatio = GetData<float>();
        float minimumAmplitude = GetData<float>();
        double time = GetData<double>();
        auto initialOffsetValue = std::make_shared<RSRenderAnimatableProperty<float>>(initialOffset);
        auto initialVelocityValue = std::make_shared<RSRenderAnimatableProperty<float>>(initialVelocity);

        // test
        std::make_shared<RSSpringModel<float>>();
        auto model1 = std::make_shared<RSSpringModel<float>>(
            response, dampingRatio, initialOffset, initialVelocity, minimumAmplitude);
        model1->CalculateDisplacement(time);
        model1->EstimateDuration();

        auto model2 = std::make_shared<RSSpringModel<std::shared_ptr<RSRenderPropertyBase>>>(
            response, dampingRatio, initialOffsetValue, initialVelocityValue, minimumAmplitude);
        model2->CalculateDisplacement(time);
        model2->EstimateDuration();
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
        RSSpringModelFuzzerTest();
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

