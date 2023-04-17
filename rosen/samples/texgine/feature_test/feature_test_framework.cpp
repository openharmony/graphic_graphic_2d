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

#include "feature_test_framework.h"

TestFeature::TestFeature(const std::string &testName)
    : testName_(testName)
{
    FeatureTestCollection::GetInstance().RegisterTest(this);
}

FeatureTestCollection &FeatureTestCollection::GetInstance()
{
    static FeatureTestCollection ftf;
    return ftf;
}

void FeatureTestCollection::RegisterTest(const TestFeature *test)
{
    if (test == nullptr) {
        return;
    }
    tests_.push_back(const_cast<TestFeature *>(test));
}
