/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "skia_hm_symbol.h"

#include "include/core/SkPath.h"

#include "skia_adapter/skia_path.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
void SkiaHMSymbol::PathOutlineDecompose(const Path& path, std::vector<Path>& paths)
{
    // paths only call push_back, Improve performance!
    std::vector<SkPath> skPaths;
    const SkPath& skPath = path.GetImpl<SkiaPath>()->GetPath();
    HMSymbol::PathOutlineDecompose(skPath, skPaths);
    for (const SkPath& skPathTmp : skPaths) {
        Path pathTmp;
        pathTmp.GetImpl<SkiaPath>()->SetPath(skPathTmp);
        paths.push_back(pathTmp);
    }
}

void SkiaHMSymbol::MultilayerPath(const std::vector<std::vector<size_t>>& multMap,
    const std::vector<Path>& paths, std::vector<Path>& multPaths)
{
    // multPaths only call push_back, Improve performance!
    std::vector<SkPath> skMultPaths;
    std::vector<SkPath> skPaths;
    for (const Path& pathTmp : paths) {
        skPaths.push_back(pathTmp.GetImpl<SkiaPath>()->GetPath());
    }
    HMSymbol::MultilayerPath(multMap, skPaths, skMultPaths);
    for (const SkPath& skPathTmp : skMultPaths) {
        Path pathTmp;
        pathTmp.GetImpl<SkiaPath>()->SetPath(skPathTmp);
        multPaths.push_back(pathTmp);
    }
}
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS