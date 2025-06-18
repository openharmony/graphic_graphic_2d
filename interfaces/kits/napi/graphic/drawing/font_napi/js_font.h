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

#ifndef OHOS_ROSEN_JS_FONT_H
#define OHOS_ROSEN_JS_FONT_H

#include <memory>
#include <native_engine/native_engine.h>
#include <native_engine/native_value.h>

#include "text/font.h"
#include "text/typeface.h"

namespace OHOS::Rosen {
namespace Drawing {
class JsFont final {
public:
    explicit JsFont(std::shared_ptr<Font> font) : m_font(font) {};
    ~JsFont();

    static napi_value Init(napi_env env, napi_value exportObj);
    static napi_value CreateFont(napi_env env, napi_callback_info info);
    static napi_value Constructor(napi_env env, napi_callback_info info);
    static void Destructor(napi_env env, void *nativeObject, void *finalize);

    static napi_value EnableSubpixel(napi_env env, napi_callback_info info);
    static napi_value EnableEmbolden(napi_env env, napi_callback_info info);
    static napi_value EnableLinearMetrics(napi_env env, napi_callback_info info);
    static napi_value SetBaselineSnap(napi_env env, napi_callback_info info);
    static napi_value SetEmbeddedBitmaps(napi_env env, napi_callback_info info);
    static napi_value SetForceAutoHinting(napi_env env, napi_callback_info info);
    static napi_value SetSize(napi_env env, napi_callback_info info);
    static napi_value SetTypeface(napi_env env, napi_callback_info info);
    static napi_value GetTypeface(napi_env env, napi_callback_info info);
    static napi_value GetSize(napi_env env, napi_callback_info info);
    static napi_value GetMetrics(napi_env env, napi_callback_info info);
    static napi_value GetWidths(napi_env env, napi_callback_info info);
    static napi_value IsBaselineSnap(napi_env env, napi_callback_info info);
    static napi_value IsEmbeddedBitmaps(napi_env env, napi_callback_info info);
    static napi_value IsForceAutoHinting(napi_env env, napi_callback_info info);
    static napi_value MeasureSingleCharacter(napi_env env, napi_callback_info info);
    static napi_value MeasureSingleCharacterWithFeatures(napi_env env, napi_callback_info info);
    static napi_value MeasureText(napi_env env, napi_callback_info info);
    static napi_value SetScaleX(napi_env env, napi_callback_info info);
    static napi_value SetSkewX(napi_env env, napi_callback_info info);
    static napi_value SetEdging(napi_env env, napi_callback_info info);
    static napi_value SetHinting(napi_env env, napi_callback_info info);
    static napi_value CountText(napi_env env, napi_callback_info info);
    static napi_value IsSubpixel(napi_env env, napi_callback_info info);
    static napi_value IsLinearMetrics(napi_env env, napi_callback_info info);
    static napi_value GetSkewX(napi_env env, napi_callback_info info);
    static napi_value IsEmbolden(napi_env env, napi_callback_info info);
    static napi_value GetScaleX(napi_env env, napi_callback_info info);
    static napi_value GetHinting(napi_env env, napi_callback_info info);
    static napi_value GetEdging(napi_env env, napi_callback_info info);
    static napi_value TextToGlyphs(napi_env env, napi_callback_info info);
    static napi_value CreatePathForGlyph(napi_env env, napi_callback_info info);
    static napi_value GetBounds(napi_env env, napi_callback_info info);
    static napi_value CreatePathForText(napi_env env, napi_callback_info info);
    static napi_value SetThemeFontFollowed(napi_env env, napi_callback_info info);
    static napi_value IsThemeFontFollowed(napi_env env, napi_callback_info info);

    std::shared_ptr<Font> GetFont();
    void SetFont(std::shared_ptr<Font> font);

private:
    napi_value OnEnableSubpixel(napi_env env, napi_callback_info info);
    napi_value OnEnableEmbolden(napi_env env, napi_callback_info info);
    napi_value OnEnableLinearMetrics(napi_env env, napi_callback_info info);
    napi_value OnSetBaselineSnap(napi_env env, napi_callback_info info);
    napi_value OnSetEmbeddedBitmaps(napi_env env, napi_callback_info info);
    napi_value OnSetForceAutoHinting(napi_env env, napi_callback_info info);
    napi_value OnSetSize(napi_env env, napi_callback_info info);
    napi_value OnSetTypeface(napi_env env, napi_callback_info info);
    napi_value OnGetTypeface(napi_env env, napi_callback_info info);
    napi_value OnGetSize(napi_env env, napi_callback_info info);
    napi_value OnGetMetrics(napi_env env, napi_callback_info info);
    napi_value OnGetWidths(napi_env env, napi_callback_info info);
    napi_value OnIsBaselineSnap(napi_env env, napi_callback_info info);
    napi_value OnIsEmbeddedBitmaps(napi_env env, napi_callback_info info);
    napi_value OnIsForceAutoHinting(napi_env env, napi_callback_info info);
    napi_value OnMeasureSingleCharacter(napi_env env, napi_callback_info info);
    napi_value OnMeasureSingleCharacterWithFeatures(napi_env env, napi_callback_info info);
    napi_value OnMeasureText(napi_env env, napi_callback_info info);
    napi_value OnSetScaleX(napi_env env, napi_callback_info info);
    napi_value OnSetSkewX(napi_env env, napi_callback_info info);
    napi_value OnSetEdging(napi_env env, napi_callback_info info);
    napi_value OnSetHinting(napi_env env, napi_callback_info info);
    napi_value OnCountText(napi_env env, napi_callback_info info);
    napi_value OnIsSubpixel(napi_env env, napi_callback_info info);
    napi_value OnIsLinearMetrics(napi_env env, napi_callback_info info);
    napi_value OnGetSkewX(napi_env env, napi_callback_info info);
    napi_value OnIsEmbolden(napi_env env, napi_callback_info info);
    napi_value OnGetScaleX(napi_env env, napi_callback_info info);
    napi_value OnGetHinting(napi_env env, napi_callback_info info);
    napi_value OnGetEdging(napi_env env, napi_callback_info info);
    napi_value OnTextToGlyphs(napi_env env, napi_callback_info info);
    napi_value OnCreatePathForGlyph(napi_env env, napi_callback_info info);
    napi_value OnGetBounds(napi_env env, napi_callback_info info);
    napi_value OnCreatePathForText(napi_env env, napi_callback_info info);
    napi_value OnSetThemeFontFollowed(napi_env env, napi_callback_info info);
    napi_value OnIsThemeFontFollowed(napi_env env, napi_callback_info info);

    static bool CreateConstructor(napi_env env);
    static thread_local napi_ref constructor_;
    std::shared_ptr<Font> m_font = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_JS_FONT_H