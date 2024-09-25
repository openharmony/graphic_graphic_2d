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

#include "drawing_text_font_descriptor.h"

#include "font_descriptor_mgr.h"

OH_Drawing_FontDescriptor* OH_Drawing_MatchFontDescriptors(OH_Drawing_FontDescriptor* desc, size_t* num)
{
    if (desc == nullptr || num == nullptr) {
        return nullptr;
    }
    using namespace OHOS::Rosen::TextEngine;
    std::shared_ptr<FontParser::FontDescriptor> condition = std::make_shared<FontParser::FontDescriptor>();
    condition->postScriptName = (desc->postScriptName == nullptr) ? "" : desc->postScriptName;
    condition->fullName = (desc->fullName == nullptr) ? "" : desc->fullName;
    condition->fontFamily = (desc->fontFamily == nullptr) ? "" : desc->fontFamily;
    condition->fontSubfamily = (desc->fontSubfamily == nullptr) ? "" : desc->fontSubfamily;
    condition->weight = desc->weight;
    condition->width = desc->width;
    condition->italic = desc->italic;
    condition->monoSpace = desc->monoSpace;
    condition->symbolic = desc->symbolic;
    condition->typeStyle = desc->typeStyle;

    std::set<std::shared_ptr<FontParser::FontDescriptor>> result;
    FontDescriptorMgrInstance.MatchFontDescriptors(condition, result);
    if (result.empty()) {
        *num = 0;
        return nullptr;
    }

    *num = result.size();
    OH_Drawing_FontDescriptor* descriptors = new (std::nothrow) OH_Drawing_FontDescriptor[*num];
    if (descriptors == nullptr) {
        *num = 0;
        return nullptr;
    }
    int i = 0;
    for (const auto& item : result) {
        descriptors[i].path = strdup(item->path.c_str());
        descriptors[i].postScriptName = strdup(item->postScriptName.c_str());
        descriptors[i].fullName = strdup(item->fullName.c_str());
        descriptors[i].fontFamily = strdup(item->fontFamily.c_str());
        descriptors[i].fontSubfamily = strdup(item->fontSubfamily.c_str());
        descriptors[i].weight = item->weight;
        descriptors[i].width = item->width;
        descriptors[i].italic = item->italic;
        descriptors[i].monoSpace = item->monoSpace;
        descriptors[i].symbolic = item->symbolic;
        descriptors[i].size = item->size;
        descriptors[i].typeStyle = item->typeStyle;
        ++i;
    }
    return descriptors;
}

void OH_Drawing_DestroyFontDescriptors(OH_Drawing_FontDescriptor* descriptors, size_t num)
{
    if (descriptors == nullptr || num == 0) {
        return;
    }
    for (size_t i = 0; i < num; ++i) {
        free(descriptors[i].path);
        free(descriptors[i].postScriptName);
        free(descriptors[i].fullName);
        free(descriptors[i].fontFamily);
        free(descriptors[i].fontSubfamily);
    }
    delete[] descriptors;
}