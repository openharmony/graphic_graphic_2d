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

#include "texgine_data.h"

namespace OHOS {
namespace Rosen {
namespace TextEngine {
#ifndef USE_ROSEN_DRAWING
sk_sp<SkData> TexgineData::GetData() const
#else
std::shared_ptr<RSData> TexgineData::GetData() const
#endif
{
    return data_;
}
#ifndef USE_ROSEN_DRAWING
void TexgineData::SetData(const sk_sp<SkData> &data)
#else
void TexgineData::SetData(const std::shared_ptr<RSData> &data)
#endif
{
    data_ = data;
}

std::shared_ptr<TexgineData> TexgineData::MakeFromFileName(const std::string &path)
{
    auto data = std::make_shared<TexgineData>();
#ifndef USE_ROSEN_DRAWING
    data->SetData(SkData::MakeFromFileName(path.c_str()));
#else
    data->SetData(RSData::MakeFromFileName(path.c_str()));
#endif
    return data;
}
} // namespace TextEngine
} // namespace Rosen
} // namespace OHOS
