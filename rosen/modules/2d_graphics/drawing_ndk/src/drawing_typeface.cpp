/*
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include "drawing_typeface.h"
#include <mutex>
#include <unordered_map>

#include "text/typeface.h"
#include "utils/object_mgr.h"

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static MemoryStream* CastToMemoryStream(OH_Drawing_MemoryStream* cCanvas)
{
    return reinterpret_cast<MemoryStream*>(cCanvas);
}

OH_Drawing_Typeface* OH_Drawing_TypefaceCreateDefault()
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeDefault();
    OH_Drawing_Typeface* drawingTypeface = reinterpret_cast<OH_Drawing_Typeface*>(typeface.get());
    TypefaceMgr::GetInstance().Insert(drawingTypeface, typeface);
    return drawingTypeface;
}

OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromFile(const char* path, int index)
{
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(path, index);
    if (typeface == nullptr) {
        return nullptr;
    }
    OH_Drawing_Typeface* drawingTypeface = reinterpret_cast<OH_Drawing_Typeface*>(typeface.get());
    TypefaceMgr::GetInstance().Insert(drawingTypeface, typeface);
    return drawingTypeface;
}

OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromStream(OH_Drawing_MemoryStream* cMemoryStream, int32_t index)
{
    if (cMemoryStream == nullptr) {
        return nullptr;
    }
    std::unique_ptr<MemoryStream> memoryStream(CastToMemoryStream(cMemoryStream));
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromStream(std::move(memoryStream), index);
    if (typeface == nullptr) {
        return nullptr;
    }
    OH_Drawing_Typeface* drawingTypeface = reinterpret_cast<OH_Drawing_Typeface*>(typeface.get());
    TypefaceMgr::GetInstance().Insert(drawingTypeface, typeface);
    return drawingTypeface;
}

void OH_Drawing_TypefaceDestroy(OH_Drawing_Typeface* cTypeface)
{
    if (cTypeface == nullptr) {
        return;
    }
    TypefaceMgr::GetInstance().Remove(cTypeface);
}
