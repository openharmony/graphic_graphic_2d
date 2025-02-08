/*
 * SPDX-License-Identifier: Apache-2.0
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
#ifndef DDGR_TEST_DTK_DTK_TEST_EXT_H
#define DDGR_TEST_DTK_DTK_TEST_EXT_H

#include "dtk_test_register.h"

namespace OHOS {
namespace Rosen {
template<typename T>
class EmptyMixin {};

#define DEF_DTK_CLASS(classUniqueId, test_case_name, test_level, test_case_index, mixin)                    \
class Dtk_##classUniqueId : public OHOS::Rosen::TestBase, public mixin<Dtk_##classUniqueId> {     \
public:                                                                                                 \
    Dtk_##classUniqueId() : TestBase() {}                                                            \
    virtual ~Dtk_##classUniqueId() = default;                                                        \
protected:                                                                                              \
    friend class mixin<Dtk_##classUniqueId>;                                                         \
    void OnRecording() override;                                                                        \
};                                                                                                      \
bool DTK_UNIQUE_ID_##classUniqueId =                                                                 \
    OHOS::Rosen::DtkTestRegister::Instance()->regist(#test_case_name,                                \
    []()->std::shared_ptr<OHOS::Rosen::TestBase> {return std::make_shared<Dtk_##classUniqueId>();},  \
    test_level, test_case_index);                                                                       \
void Dtk_##classUniqueId::OnRecording()

#define DEF_DTK_CLASS_UNIQUE_ID(file_line, test_case_name, test_level, test_case_index, mixin) \
DEF_DTK_CLASS(test_case_name##_##file_line, test_case_name, test_level, test_case_index, mixin)

#define DEF_DTK_CLASS_UNIQUE_ID_2(file_line, test_case_name, test_level, test_case_index, mixin) \
DEF_DTK_CLASS_UNIQUE_ID(file_line, test_case_name, test_level, test_case_index, mixin)

#define DEF_DTK(test_case_name, test_level, test_case_index) \
DEF_DTK_CLASS_UNIQUE_ID_2(__LINE__, test_case_name, test_level, test_case_index, EmptyMixin)

#define DEF_DTK_MIXIN(test_case_name, test_level, test_case_index, mixin) \
DEF_DTK_CLASS_UNIQUE_ID_2(__LINE__, test_case_name, test_level, test_case_index, mixin)
} // namespace Rosen
} // namespace OHOS
#endif // DDGR_TEST_DTK_DTK_TEST_EXT_H