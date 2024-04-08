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
#include "test_case_factory.h"
#include "test_case/draw_path.h"
#include "test_case/draw_rect.h"

using namespace OHOS::Rosen;
namespace {
std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> FunctionalMap = {
    {"drawrect", []() -> std::shared_ptr<TestBase> {return std::make_shared<DrawRectTest>();}},
    {"drawpath", []() -> std::shared_ptr<TestBase> {return std::make_shared<DrawPathTest>();}},
};
std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> PerformanceMap = {
    {"drawrect", []() -> std::shared_ptr<TestBase> {return std::make_shared<DrawRectTest>();}},
    {"drawpath", []() -> std::shared_ptr<TestBase> {return std::make_shared<DrawPathTest>();}},
};
} // namespace

namespace OHOS {
namespace Rosen {
std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> TestCaseFactory::GetFunctionCase()
{
    return FunctionalMap;
}

std::unordered_map<std::string, std::function<std::shared_ptr<TestBase>()>> TestCaseFactory::GetPerformanceCase()
{
    return PerformanceMap;
}
} // namespace Rosen
} // namespace OHOS