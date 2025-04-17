/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include <ani.h>

#include "ani_font_collection.h"
#include "ani_paragraph.h"

namespace {
using namespace OHOS::Rosen;

#define STRUCT_LIST(...) using AniTypes = std::tuple<__VA_ARGS__>

// add new struct in this macro
STRUCT_LIST(AniFontCollection, AniParagraph);

template<typename T>
static ani_status InitOneStruct(ani_vm* vm, uint32_t* result)
{
    return T::AniInit(vm, result);
}

template<typename Tuple, size_t... Is>
static ani_status InitAllStruct(ani_vm* vm, uint32_t* result, std::index_sequence<Is...>)
{
    ani_status ret;
    [[maybe_unused]] bool _ = (((ret = InitOneStruct<std::tuple_element_t<Is, Tuple>>(vm, result)) == ANI_OK) && ...);
    return ret;
}

static ani_status Init(ani_vm* vm, uint32_t* result)
{
    return InitAllStruct<AniTypes>(vm, result, std::make_index_sequence<std::tuple_size_v<AniTypes>>());
}
} // namespace

extern "C" {
ANI_EXPORT ani_status ANI_Constructor(ani_vm* vm, uint32_t* result)
{
    ani_status status = Init(vm, result);
    if (status == ANI_OK) {
        *result = ANI_VERSION_1;
    }
    return status;
}
}