/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_CLIENT_CORE_RENDER_RS_PATH_H
#define RENDER_SERVICE_CLIENT_CORE_RENDER_RS_PATH_H

#include <memory>
#include <string>

#include "common/rs_macros.h"
#include "common/rs_matrix3.h"
#include "common/rs_rect.h"
#include "common/rs_vector2.h"

#include "draw/path.h"
namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSPath {
public:
    RSPath();
    virtual ~RSPath();
    static RSB_EXPORT std::shared_ptr<RSPath> CreateRSPath();
    static RSB_EXPORT std::shared_ptr<RSPath> CreateRSPath(const Drawing::Path& path);
    static RSB_EXPORT std::shared_ptr<RSPath> CreateRSPath(const std::string& path);
    float GetDistance() const;
    template<typename T>
    bool GetPosTan(float distance, T& pos, float& degrees) const
    {
        return true;
    }

    std::shared_ptr<RSPath> Reverse();

    void SetDrawingPath(const Drawing::Path& path);
    const Drawing::Path& GetDrawingPath() const;

private:
    RSPath(const RSPath&) = delete;
    RSPath(const RSPath&&) = delete;
    RSPath& operator=(const RSPath&) = delete;
    RSPath& operator=(const RSPath&&) = delete;

    Drawing::Path* drPath_ = nullptr;
};

template<>
RSB_EXPORT bool RSPath::GetPosTan(float distance, Vector2f& pos, float& degrees) const;
template<>
RSB_EXPORT bool RSPath::GetPosTan(float distance, Vector4f& pos, float& degrees) const;
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_RENDER_RS_PATH_H
