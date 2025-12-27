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

#ifndef OHOS_ROSEN_ANI_TEXT_BLOB_H
#define OHOS_ROSEN_ANI_TEXT_BLOB_H

#include "ani_drawing_utils.h"
#include "text/text_blob.h"
#include "text/text_blob_builder.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniTextBlob final {
public:
    explicit AniTextBlob(std::shared_ptr<TextBlob> textBlob) : textBlob_(textBlob) {}
    
    ~AniTextBlob() = default;

    static ani_status AniInit(ani_env *env);
    static ani_object Bounds(ani_env* env, ani_object obj, ani_object aniRectObj);
    static ani_long UniqueID(ani_env* env, ani_object obj);
    static ani_object MakeFromPosText(ani_env* env, ani_object obj, ani_string aniText, ani_int len,
        ani_object aniPoints, ani_object aniFontObj);
    static ani_object MakeFromString(ani_env* env, ani_object obj, ani_string aniText, ani_object aniFontObj,
        ani_enum_item aniEncoding);
    static ani_object MakeFromRunBuffer(ani_env* env, ani_object obj, ani_object posArray, ani_object aniFontObj,
        ani_object aniRectObj);

    std::shared_ptr<TextBlob> GetTextBlob();
private:
    static std::shared_ptr<Font> GetValidFont(const std::shared_ptr<Font>& font);
    static bool MakePoints(ani_env* env, Point* points, uint32_t size, ani_array array);
    static ani_object GetTextBlobObj(ani_env* env, const char* buffer, size_t bufferLen, const Point points[],
        const std::shared_ptr<Font>& font);
    static ani_object CreateTextBlobObj(ani_env* env, const std::shared_ptr<TextBlob> textBlob);
    static bool MakeRunBuffer(ani_env* env, TextBlobBuilder::RunBuffer& runBuffer, uint32_t size, ani_object posArray);
    std::shared_ptr<TextBlob> textBlob_ = nullptr;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_TEXT_BLOB_H