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

#include "drawing_font_collection.h"

#include "rosen_text/font_collection.h"
#include "skia_txt/font_collection.h"

#include "utils/object_mgr.h"

using namespace OHOS::Rosen::Drawing;

static std::shared_ptr<ObjectMgr> g_objectMgr = ObjectMgr::GetInstance();

template<typename T1, typename T2>
inline T1* ConvertToFontCollection(T2* ptr)
{
    return reinterpret_cast<T1*>(ptr);
}

OH_Drawing_FontCollection* OH_Drawing_CreateFontCollection(void)
{
    OH_Drawing_FontCollection* fc =
        (OH_Drawing_FontCollection*)new (std::nothrow) OHOS::Rosen::AdapterTxt::FontCollection;
    if (fc == nullptr) {
        return nullptr;
    }
    g_objectMgr->AddObject(fc);
    return fc;
}

OH_Drawing_FontCollection* OH_Drawing_CreateSharedFontCollection(void)
{
    auto fc = std::make_shared<OHOS::Rosen::AdapterTxt::FontCollection>();
    OH_Drawing_FontCollection* pointer = reinterpret_cast<OH_Drawing_FontCollection*>(fc.get());
    FontCollectionMgr::GetInstance().Insert(pointer, fc);
    return pointer;
}

void OH_Drawing_DestroyFontCollection(OH_Drawing_FontCollection* fontCollection)
{
    if (!fontCollection) {
        return;
    }

    if (FontCollectionMgr::GetInstance().Remove(fontCollection)) {
        return;
    }
    if (!g_objectMgr->RemoveObject(fontCollection)) {
        return;
    }

    delete ConvertToFontCollection<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection);
}

void OH_Drawing_DisableFontCollectionFallback(OH_Drawing_FontCollection* fontCollection)
{
    if (!fontCollection) {
        return;
    }
    ConvertToFontCollection<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection)->DisableFallback();
}

void OH_Drawing_DisableFontCollectionSystemFont(OH_Drawing_FontCollection* fontCollection)
{
    if (fontCollection == nullptr) {
        return;
    }
    ConvertToFontCollection<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection)->DisableSystemFont();
}

void OH_Drawing_ClearFontCaches(OH_Drawing_FontCollection* fontCollection)
{
    if (!fontCollection) {
        return;
    }

    if (FontCollectionMgr::GetInstance().Find(fontCollection)) {
        ConvertToFontCollection<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection)->ClearCaches();
        return;
    }

    if (g_objectMgr->HasObject(fontCollection)) {
        ConvertToFontCollection<OHOS::Rosen::AdapterTxt::FontCollection>(fontCollection)->ClearCaches();
        return;
    }
    return;
}

OH_Drawing_FontCollection* OH_Drawing_GetFontCollectionGlobalInstance(void)
{
    std::shared_ptr<OHOS::Rosen::FontCollection> fc = OHOS::Rosen::FontCollection::Create();
    OH_Drawing_FontCollection* pointer = reinterpret_cast<OH_Drawing_FontCollection*>(fc.get());
    if (!FontCollectionMgr::GetInstance().Find(pointer)) {
        FontCollectionMgr::GetInstance().Insert(pointer, fc);
    }
    return pointer;
}