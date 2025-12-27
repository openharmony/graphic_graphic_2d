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

#ifndef ROSEN_RENDER_SERVICE_BASE_COMMON_RS_COMMON_TOOL_H
#define ROSEN_RENDER_SERVICE_BASE_COMMON_RS_COMMON_TOOL_H

#include <memory>
#include <string>

namespace OHOS {
namespace Media {
class PixelMap;
}
namespace Rosen {
namespace CommonTools {
std::string GetLocalTime();
void SavePixelmapToFile(const std::shared_ptr<Media::PixelMap>& pixelMap, const std::string& dst);
} // namespace CommonTools

namespace TemplateUtils {
template<typename Map>
static auto findMapValueRef(Map& map, const int8_t key)
    -> std::enable_if_t<
        std::is_constructible_v<typename Map::mapped_type, std::nullptr_t>,
        typename Map::mapped_type&
    >
{
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    }
    thread_local static typename Map::mapped_type null_instance{nullptr};
    return null_instance;
}

template<typename Map>
static auto findMapValueRef(const Map& map, const int8_t key)
    -> std::enable_if_t<
        std::is_constructible_v<typename Map::mapped_type, std::nullptr_t>,
        const typename Map::mapped_type&
    >
{
    auto it = map.find(key);
    if (it != map.end()) {
        return it->second;
    }
    thread_local static typename Map::mapped_type null_instance{nullptr};
    return null_instance;
}

template <typename Container, typename T, typename Key>
void assignOrEraseOnAccess(Container& container, Key key, T value)
{
    using KeyType = typename Container::key_type;
    const KeyType key_cast = static_cast<KeyType>(key);
    if (value != nullptr) {
        container[key_cast] = value;
    } else {
        container.erase(key);
    }
}
} // namespace TemplateUtils
} // namespace Rosen
} // namespace OHOS

#endif // RS_FILE_UTILs