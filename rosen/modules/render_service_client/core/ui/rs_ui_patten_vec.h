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
#ifndef RENDER_SERVICE_UI_PATTEN_VEC_H
#define RENDER_SERVICE_UI_PATTEN_VEC_H

namespace OHOS {
namespace Rosen {
// subtree parallel judgment conditions vec
static const std::set<std::string> g_rbPattenVec = {
    "GridItem",
    "Grid",
    "GridCol",
    "Column",
    "Row",
    "ListItemGroup",
    "ArcListItem",
    "ListItem",
    "List",
    "NodeContainer",
    "RelativeContainer",
    "Stack",
    "Swiper",
    "FlowItem",
    "WaterFlow"
};

inline bool CheckRbPatten(const std::string& tag)
{
    return g_rbPattenVec.find(tag) != g_rbPattenVec.end();
}

} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_UI_PATTEN_VEC_H