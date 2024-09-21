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

#include "object_mgr_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <securec.h>

#include "get_object.h"
#include "utils/object_mgr.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t ARRAY_MAX_SIZE = 5000;
constexpr size_t MATH_ONE = 1;
} // namespace

namespace Drawing {
void ObjectMgrFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t str_size = GetObject<uint32_t>() % ARRAY_MAX_SIZE + MATH_ONE;

    std::shared_ptr<ObjectMgr> objectm = ObjectMgr::GetInstance();
    char* obj = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        obj[i] = GetObject<char>();
    }
    obj[str_size - MATH_ONE] = '\0';
    objectm->AddObject(obj);
    objectm->HasObject(obj);
    objectm->RemoveObject(obj);
    objectm->ObjectCount();
    if (obj != nullptr) {
        delete [] obj;
        obj = nullptr;
    }
}

void FontCollectionMgrFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t str_size = GetObject<uint32_t>() % ARRAY_MAX_SIZE + MATH_ONE;

    FontCollectionMgr& fontmgr = FontCollectionMgr::GetInstance();
    char* key = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        key[i] = GetObject<char>();
    }
    key[str_size - MATH_ONE] = '\0';
    std::shared_ptr<FontCollectionMgr::FontCollectionType> fontCollection = fontmgr.Find(key);
    fontmgr.Remove(key);
    if (key != nullptr) {
        delete [] key;
        key = nullptr;
    }
}

void TypefaceMgrFuzzTest000(const uint8_t* data, size_t size)
{
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    uint32_t str_size = GetObject<uint32_t>() % ARRAY_MAX_SIZE + MATH_ONE;

    TypefaceMgr& tpyefacemgr = TypefaceMgr::GetInstance();
    char* key = new char[str_size];
    for (size_t i = 0; i < str_size; i++) {
        key[i] = GetObject<char>();
    }
    key[str_size - MATH_ONE] = '\0';
    std::shared_ptr<Typeface> typeface = tpyefacemgr.Find(key);
    tpyefacemgr.Insert(key, typeface);
    tpyefacemgr.Remove(key);
    if (key != nullptr) {
        delete [] key;
        key = nullptr;
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::ObjectMgrFuzzTest000(data, size);
    OHOS::Rosen::Drawing::FontCollectionMgrFuzzTest000(data, size);
    OHOS::Rosen::Drawing::TypefaceMgrFuzzTest000(data, size);

    return 0;
}