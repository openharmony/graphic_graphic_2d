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

#include "rscommondef_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <securec.h>
#include <unistd.h>

#include "common/rs_common_def.cpp"
#include "common/rs_common_def.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
} // namespace

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
bool DoInline(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    float valuex = GetData<float>();
    float valuey = GetData<float>();
    float epsilon = GetData<float>();
    ROSEN_EQ(valuex, valuey);
    ROSEN_EQ(valuex, valuey, epsilon);

    std::shared_ptr<float> sharedObject1 = std::make_shared<float>(valuex);
    std::shared_ptr<float> sharedObject2 = std::make_shared<float>(valuey);
    std::weak_ptr<float> weakPtr1 = sharedObject1;
    std::weak_ptr<float> weakPtr2 = sharedObject2;
    ROSEN_EQ(weakPtr1, weakPtr2);

    ROSEN_LNE(valuex, valuey);
    ROSEN_GNE(valuex, valuey);
    ROSEN_GE(valuex, valuey);
    ROSEN_LE(valuex, valuey);

    uint64_t id = GetData<uint64_t>();
    ExtractPid(id);
    return true;
}

bool DoMemObject(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t size1 = GetData<size_t>();
    MemObject obj3(size1);
    MemObject* obj1 = new MemObject(size1);
    delete obj1;
    MemObject* obj2 = new (std::nothrow) MemObject(size1);
    if (obj2) {
        delete obj2;
    }
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoInline(data, size);    // inline
    OHOS::Rosen::DoMemObject(data, size); // MemObject
    return 0;
}
