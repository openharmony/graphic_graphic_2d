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

#ifndef UI_FILTER_TEST_DATA_H
#define UI_FILTER_TEST_DATA_H

#include <unordered_map>
#include <vector>
#include "ui_filter_test_factory.h"

namespace OHOS {
namespace Rosen {

enum class TestDataGroupType {
    INVALID_DATA_MIN,
    VALID_DATA1,
    VALID_DATA2,
    VALID_DATA3,
    VALID_DATA4,
    INVALID_DATA_MAX,
    COUNT
};

class UIFilterTestDataManager {
public:
    static void Initialize();
    static Filter* GetFilter(FilterPara::ParaType type, TestDataGroupType groupIdx);
    static Filter* GetCombinedFilter(std::vector<FilterPara::ParaType>& types, TestDataGroupType groupIdx);

private:
    static void CreateFilterGroups();
    static std::shared_ptr<Media::PixelMap> CreatePixelMap(int width, int height);
    static std::vector<FilterTestData>& GetTestDatas(TestDataGroupType groupIdx);
    static inline std::unordered_map<TestDataGroupType,
        std::unordered_map<FilterPara::ParaType, std::unique_ptr<Filter>>> filterGroups_;
};

} // namespace Rosen
} // namespace OHOS

#endif // UI_FILTER_TEST_DATA_H
