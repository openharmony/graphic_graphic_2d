/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

import { cube_demo } from "./sample/cube_demo";
import { scene_demo } from './sample/scene_demo';
import { shadow_demo } from './sample/shadow_demo';
import { panorama_demo } from './sample/panorama_demo';
import { texture_2d_demo } from './sample/texture_2d_demo';
import { cube2_demo } from './sample2/cube2_demo';
import { panorama2_demo } from './sample2/panorama2_demo';
import { shadow2_demo } from './sample2/shadow2_demo';

export class Demo {
    //webgl1_demo
    static WEBGL1_CUBE_DEMO = "webgl1_cube_demo"
    static WEBGL1_PANORAMA_DEMO = "webgl1_panorama_demo"
    static WEBGL1_SCENE_DEMO = "webgl1_scene_demo"
    static WEBGL1_SHADOW_DEMO = "webgl1_shadow_demo"
    static WEBGL1_TEXTURE2D_DEMO = "webgl1_texture_2d_demo"
    //webgl2_demo
    static WEBGL2_CUBE_DEMO = "webgl2_cube_demo"
    static WEBGL2_PANORAMA_DEMO = "webgl2_panorama_demo"
    static WEBGL2_SHADOW_DEMO = "webgl2_shadow_demo"
}

export async function main(gl, type) {
    if(!type){
        type = Demo.WEBGL1_CUBE_DEMO
    }
    switch (type) {
        case Demo.WEBGL1_CUBE_DEMO:
            await cube_demo(gl);
            break;
        case Demo.WEBGL1_PANORAMA_DEMO:
            await panorama_demo(gl);
            break;
        case Demo.WEBGL1_SCENE_DEMO:
            await scene_demo(gl);
            break;
        case Demo.WEBGL1_SHADOW_DEMO:
            await shadow_demo(gl);
            break;
        case Demo.WEBGL1_TEXTURE2D_DEMO:
            await texture_2d_demo();
            break;
        case Demo.WEBGL2_CUBE_DEMO:
            await cube2_demo(gl);
            break;
        case Demo.WEBGL2_PANORAMA_DEMO:
            await panorama2_demo(gl);
            break;
        case Demo.WEBGL2_SHADOW_DEMO:
            await shadow2_demo(gl);
            break;
        default:
            break;
    }
}
