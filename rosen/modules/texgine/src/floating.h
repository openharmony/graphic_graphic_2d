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

#ifndef ROSEN_MODULES_TEXGINE_SRC_FLOATING_H
#define ROSEN_MODULES_TEXGINE_SRC_FLOATING_H

#include <cmath>

double FloatingAbs(double floating)
{
    return fabs(floating);
}

#define FLOATING_EPSILON 1e-6
#define FLOATING_EQ(LEFT, RIGHT) (FloatingAbs((LEFT) - (RIGHT)) < FLOATING_EPSILON)
#define FLOATING_NE(LEFT, RIGHT) (!FLOATING_EQ(LEFT, RIGHT))
#define FLOATING_GE(LEFT, RIGHT) (FLOATING_GT(LEFT, RIGHT) || FLOATING_EQ(LEFT, RIGHT))
#define FLOATING_GT(LEFT, RIGHT) (((LEFT) - (RIGHT)) > FLOATING_EPSILON)
#define FLOATING_LE(LEFT, RIGHT) (FLOATING_LT(LEFT, RIGHT) || FLOATING_EQ(LEFT, RIGHT))
#define FLOATING_LT(LEFT, RIGHT) FLOATING_GT(RIGHT, LEFT)

#endif // ROSEN_MODULES_TEXGINE_SRC_FLOATING_H
