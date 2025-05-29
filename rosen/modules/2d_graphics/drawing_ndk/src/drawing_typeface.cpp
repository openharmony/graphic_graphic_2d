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
#include <vector>

#include "include/core/SkTypes.h"
#ifdef USE_M133_SKIA
#include "include/core/SkFourByteTag.h"
#endif

#include "drawing_canvas_utils.h"
#include "drawing_font_utils.h"

#include "text/typeface.h"
#include "utils/log.h"
#include "utils/object_mgr.h"

static constexpr uint32_t AXIS_TAG_ZERO = 0;
static constexpr uint32_t AXIS_TAG_ONE = 1;
static constexpr uint32_t AXIS_TAG_TWO = 2;
static constexpr uint32_t AXIS_TAG_THREE = 3;
static constexpr uint32_t VARIATION_AXIS_LENGTH = 4;

using namespace OHOS;
using namespace Rosen;
using namespace Drawing;

static const std::string G_SYSTEM_FONT_DIR = "/system/fonts/";

struct FontArgumentsHelper {
    struct Coordinate {
        uint32_t axis;
        float value;
    };

    int fontCollectionIndex_ = 0;
    std::vector<Coordinate> coordinates_;
};

static MemoryStream* CastToMemoryStream(OH_Drawing_MemoryStream* cMemoryStream)
{
    return reinterpret_cast<MemoryStream*>(cMemoryStream);
}

static const FontArgumentsHelper* CastToFontArgumentsHelper(const OH_Drawing_FontArguments* cFontArguments)
{
    return reinterpret_cast<const FontArgumentsHelper*>(cFontArguments);
}

static FontArgumentsHelper* CastToFontArgumentsHelper(OH_Drawing_FontArguments* cFontArguments)
{
    return reinterpret_cast<FontArgumentsHelper*>(cFontArguments);
}

static const Typeface* CastToTypeface(const OH_Drawing_Typeface* cTypeface)
{
    return reinterpret_cast<const Typeface*>(cTypeface);
}

static void ConvertToDrawingFontArguments(const FontArgumentsHelper& fontArgumentsHelper, FontArguments& fontArguments)
{
    fontArguments.SetCollectionIndex(fontArgumentsHelper.fontCollectionIndex_);
    fontArguments.SetVariationDesignPosition({reinterpret_cast<const FontArguments::VariationPosition::Coordinate*>(
        fontArgumentsHelper.coordinates_.data()), fontArgumentsHelper.coordinates_.size()});
}

static OH_Drawing_Typeface* RegisterAndConvertTypeface(std::shared_ptr<Typeface> typeface)
{
    if (typeface == nullptr) {
        LOGE("RegisterAndConvertTypeface: typeface nullptr.");
        return nullptr;
    }
    // system font is not sent to RenderService to optimize performance.
    if (typeface->IsCustomTypeface() && Typeface::GetTypefaceRegisterCallBack() != nullptr &&
        !Typeface::GetTypefaceRegisterCallBack()(typeface)) {
        LOGE("RegisterAndConvertTypeface: register typeface failed.");
        return nullptr;
    }
    OH_Drawing_Typeface* drawingTypeface = reinterpret_cast<OH_Drawing_Typeface*>(typeface.get());
    TypefaceMgr::GetInstance().Insert(drawingTypeface, typeface);
    return drawingTypeface;
}

OH_Drawing_Typeface* OH_Drawing_TypefaceCreateDefault()
{
    std::shared_ptr<Typeface> typeface = DrawingFontUtils::GetZhCnTypeface();
    if (typeface == nullptr) {
        LOGE("OH_Drawing_TypefaceCreateDefault: create failed.");
        return nullptr;
    }
    OH_Drawing_Typeface* drawingTypeface = reinterpret_cast<OH_Drawing_Typeface*>(typeface.get());
    TypefaceMgr::GetInstance().Insert(drawingTypeface, typeface);
    return drawingTypeface;
}

OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromFile(const char* path, int index)
{
    if (path == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromFile(path, index);
    if (typeface == nullptr) {
        return nullptr;
    }
    std::string pathStr(path);
    // system font is not sent to RenderService to optimize performance.
    if (pathStr.substr(0, G_SYSTEM_FONT_DIR.length()) != G_SYSTEM_FONT_DIR &&
        Drawing::Typeface::GetTypefaceRegisterCallBack() != nullptr) {
        bool ret = Drawing::Typeface::GetTypefaceRegisterCallBack()(typeface);
        if (!ret) {
            LOGE("OH_Drawing_TypefaceCreateFromFile: register typeface failed.");
            return nullptr;
        }
    }
    OH_Drawing_Typeface* drawingTypeface = reinterpret_cast<OH_Drawing_Typeface*>(typeface.get());
    TypefaceMgr::GetInstance().Insert(drawingTypeface, typeface);
    return drawingTypeface;
}

OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromFileWithArguments(const char* path,
    const OH_Drawing_FontArguments* cFontArguments)
{
    const FontArgumentsHelper* fontArgumentsHelper = CastToFontArgumentsHelper(cFontArguments);
    if (path == nullptr || fontArgumentsHelper == nullptr) {
        return nullptr;
    }
    FontArguments fontArguments;
    ConvertToDrawingFontArguments(*fontArgumentsHelper, fontArguments);
    return RegisterAndConvertTypeface(Typeface::MakeFromFile(path, fontArguments));
}

OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromCurrent(const OH_Drawing_Typeface* cCurrentTypeface,
    const OH_Drawing_FontArguments* cFontArguments)
{
    const Typeface* currentTypeface = CastToTypeface(cCurrentTypeface);
    const FontArgumentsHelper* fontArgumentsHelper = CastToFontArgumentsHelper(cFontArguments);
    if (currentTypeface == nullptr || fontArgumentsHelper == nullptr) {
        return nullptr;
    }
    FontArguments fontArguments;
    ConvertToDrawingFontArguments(*fontArgumentsHelper, fontArguments);
    std::shared_ptr<Typeface> typeface = currentTypeface->MakeClone(fontArguments);
    if (typeface == nullptr || currentTypeface->GetUniqueID() == typeface->GetUniqueID()) {
        return nullptr;
    }
    return RegisterAndConvertTypeface(typeface);
}

OH_Drawing_Typeface* OH_Drawing_TypefaceCreateFromStream(OH_Drawing_MemoryStream* cMemoryStream, int32_t index)
{
    if (cMemoryStream == nullptr) {
        g_drawingErrorCode = OH_DRAWING_ERROR_INVALID_PARAMETER;
        return nullptr;
    }
    std::unique_ptr<MemoryStream> memoryStream(CastToMemoryStream(cMemoryStream));
    std::shared_ptr<Typeface> typeface = Typeface::MakeFromStream(std::move(memoryStream), index);
    if (typeface == nullptr) {
        return nullptr;
    }
    if (Drawing::Typeface::GetTypefaceRegisterCallBack() != nullptr) {
        bool ret = Drawing::Typeface::GetTypefaceRegisterCallBack()(typeface);
        if (!ret) {
            LOGE("OH_Drawing_TypefaceCreateFromStream: register typeface failed.");
            return nullptr;
        }
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
    auto typeface = TypefaceMgr::GetInstance().Find(cTypeface);
    if (Drawing::Typeface::GetTypefaceUnRegisterCallBack() != nullptr && typeface) {
        Drawing::Typeface::GetTypefaceUnRegisterCallBack()(typeface);
    }
    TypefaceMgr::GetInstance().Remove(cTypeface);
}

OH_Drawing_FontArguments* OH_Drawing_FontArgumentsCreate(void)
{
    return (OH_Drawing_FontArguments*)new FontArgumentsHelper();
}

OH_Drawing_ErrorCode OH_Drawing_FontArgumentsAddVariation(OH_Drawing_FontArguments* cFontArguments,
    const char* axis, float value)
{
    if (axis == nullptr || strlen(axis) != VARIATION_AXIS_LENGTH) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    FontArgumentsHelper* fontArgumentsHelper = CastToFontArgumentsHelper(cFontArguments);
    if (fontArgumentsHelper == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    fontArgumentsHelper->coordinates_.push_back(
        {SkSetFourByteTag(axis[AXIS_TAG_ZERO], axis[AXIS_TAG_ONE], axis[AXIS_TAG_TWO], axis[AXIS_TAG_THREE]), value});
    return OH_DRAWING_SUCCESS;
}

OH_Drawing_ErrorCode OH_Drawing_FontArgumentsDestroy(OH_Drawing_FontArguments* cFontArguments)
{
    if (cFontArguments == nullptr) {
        return OH_DRAWING_ERROR_INVALID_PARAMETER;
    }
    delete CastToFontArgumentsHelper(cFontArguments);
    return OH_DRAWING_SUCCESS;
}