/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "drawing_text_font_descriptor.h"

#include "array_mgr.h"
#include "font_parser.h"

using namespace OHOS::Rosen;

template<typename T1, typename T2>
inline T1* ConvertToOriginalText(T2* ptr)
{
    return reinterpret_cast<T1*>(ptr);
}

void OH_Drawing_DestroySystemFontFullNames(OH_Drawing_Array* fullNameArray)
{
    ObjectArray* fullNameList = ConvertToOriginalText<ObjectArray>(fullNameArray);
    if (fullNameList == nullptr || fullNameList->type != ObjectType::STRING) {
        return;
    }
    OH_Drawing_String* nameArray = ConvertToOriginalText<OH_Drawing_String>(fullNameList->addr);
    if (nameArray == nullptr) {
        return;
    }
    for (size_t i = 0; i < fullNameList->num; ++i) {
        if (nameArray[i].strData == nullptr) {
            continue;
        }
        delete[] nameArray[i].strData;
        nameArray[i].strData = nullptr;
    }
    delete[] nameArray;
    fullNameList->addr = nullptr;
    fullNameList->num = 0;
    fullNameList->type = ObjectType::INVALID;
    delete fullNameList;
}

void OH_Drawing_DestroyFontFullDescriptors(OH_Drawing_Array* descriptorArray)
{
    ObjectArray* descriptorList = ConvertToOriginalText<ObjectArray>(descriptorArray);
    if (descriptorList == nullptr || descriptorList->type != ObjectType::FONT_FULL_DESCRIPTOR) {
        return;
    }

    TextEngine::FontParser::FontDescriptor** descArray =
        reinterpret_cast<TextEngine::FontParser::FontDescriptor**>(descriptorList->addr);
    if (descArray == nullptr) {
        return;
    }
    for (size_t i = 0; i < descriptorList->num; ++i) {
        if (descArray[i] == nullptr) {
            continue;
        }
        delete descArray[i];
        descArray[i] = nullptr;
    }
    delete[] descArray;
    descriptorList->addr = nullptr;
    descriptorList->num = 0;
    descriptorList->type = ObjectType::INVALID;
    delete descriptorList;
}