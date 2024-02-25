/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "c/drawing_font_collection.h"
#ifndef USE_GRAPHIC_TEXT_GINE
#include "rosen_text/ui/font_collection.h"
#else
#include "rosen_text/font_collection.h"
#ifndef USE_TEXGINE
#include "adapter/txt/font_collection.h"
#else
#include "texgine/font_collection.h"
#endif
#endif
#include "utils/object_mgr.h"

using namespace OHOS::Rosen::Drawing;

static std::shared_ptr<ObjectMgr> objectMgr = ObjectMgr::GetInstance();

template<typename T1, typename T2>
inline T1* ConvertToFontCollection(T2* ptr)
{
    return reinterpret_cast<T1*>(ptr);
}

OH_Drawing_FontCollection* OH_Drawing_CreateFontCollection(void)
{
#ifndef USE_GRAPHIC_TEXT_GINE
    OH_Drawing_FontCollection* fc = (OH_Drawing_FontCollection*)new rosen::FontCollection;
#else
#ifndef USE_TEXGINE
    OH_Drawing_FontCollection* fc = (OH_Drawing_FontCollection*)new OHOS::Rosen::AdapterTxt::FontCollection;
#else
    OH_Drawing_FontCollection* fc = (OH_Drawing_FontCollection*)new OHOS::Rosen::AdapterTextEngine::FontCollection;
#endif
#endif
    objectMgr->AddObject(fc);
    return fc;
}

void OH_Drawing_DestroyFontCollection(OH_Drawing_FontCollection* fontCollection)
{
    if (!fontCollection) {
        return;
    }

    if (!objectMgr->RemoveObject(fontCollection)) {
        return;
    }

#ifndef USE_GRAPHIC_TEXT_GINE
    delete ConvertToFontCollection<rosen::FontCollection>(fontCollection);
#else
#ifndef USE_TEXGINE
    delete ConvertToFontCollection<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection);
#else
    delete ConvertToFontCollection<OHOS::Rosen::AdapterTextEngine::FontCollection>(fontCollection);
#endif
#endif
}

void  OH_Drawing_DisableFontCollectionFallback(OH_Drawing_FontCollection* fontCollection)
{
}

void  OH_Drawing_DisableFontCollectionSystemFont(OH_Drawing_FontCollection* fontCollection)
{
}