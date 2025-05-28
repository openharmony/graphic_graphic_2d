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

#ifndef OHOS_ROSEN_ANI_PATH_H
#define OHOS_ROSEN_ANI_PATH_H

#include "ani_drawing_utils.h"
#include "draw/path.h"

namespace OHOS::Rosen {
namespace Drawing {
class AniPath final {
public:
    AniPath() = default;
    explicit AniPath(const Path& path) : path_(path) {}
    ~AniPath() = default;

    static ani_status AniInit(ani_env *env);

    static void Constructor(ani_env* env, ani_object obj);
    static void ConstructorWithPath(ani_env* env, ani_object obj, ani_object aniPathObj);
    static void ArcTo(ani_env* env, ani_object obj, ani_double x1, ani_double y1, ani_double x2,
        ani_double y2, ani_double startDeg, ani_double sweepDeg);
    static void Reset(ani_env* env, ani_object obj);

    Path& GetPath();

private:
    Path path_;
};
} // namespace Drawing
} // namespace OHOS::Rosen
#endif // OHOS_ROSEN_ANI_PATH_H