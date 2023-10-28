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

import { PlaneShape } from "../utils/PlaneShape";
import { CubeShape } from "../utils/CubeShape";
import { Point } from "../utils/Point";
import { Scene } from "../utils/Scene";
import { loadImage } from "../utils/extension";
import { Shape } from '../utils/Shape';
import { Images } from '../utils/Images';

let angle = 0;
let last = Date.now();
let anim = (angle) => {
    let now = Date.now();
    let elapsed = now - last;
    last = now;
    let newAngle = angle + (10 * elapsed) / 1000.0;
    return newAngle % 360;
};

export async function scene_demo(gl) {
    let image = await loadImage(Images.FLOOR);
    let box = await loadImage(Images.BOX);
    let scene = new Scene(gl);
    scene.camera().perspective(30, gl.width / gl.height, 1, 100).eye(0, 1.5, 8).center(0, 0, 0);
    let plane = new PlaneShape(20.0, 20.0, new Point(0, -0.2501, 0), 1, 1)
        .setImage(image, 10, 10)
        .setAlpha(1.0);
    scene.addShape(plane);
    scene.addShape(new CubeShape(0.5, 0.5, 0.5, new Point(-1.0, 0, 0), 1, 1, 1)
        .setImage(box, 1, 1, 0)
        .setAlpha(1.0));
    let cube2 = new CubeShape(0.5, 0.5, 0.5, new Point(0.0, 1.0, 0), 1, 1, 1)
        .setImage(box, 3, 3, 1)
        .setAlpha(0.0);
    scene.addShape(cube2);
    scene.addShape(new CubeShape(0.5, 0.5, 0.5, new Point(0.0, 0, -1.0), 1, 1, 1)
        .setImage(box, 1, 1, 2)
        .setAlpha(1.0));
    scene.addShape(new CubeShape(0.5, 0.5, 0.5, new Point(0.0, 0, 1.0), 1, 1, 1)
        .setImage(box, 1, 1, 3)
        .setAlpha(1.0));
    scene.addShape(new CubeShape(0.5, 0.5, 0.5, new Point(1, 0, 0), 1, 1, 1)
        .setImage(box, 1, 1, 4)
        .setAlpha(1.0));
    scene.setRender(() => {
        if (scene.camera().pointLightY > 5.0) {
            scene.camera().pointMoveDirect = 0;
        }
        if (scene.camera().pointLightY <= 3.0) {
            scene.camera().pointMoveDirect = 1;
        }
        if (scene.camera().pointMoveDirect === 0) {
            scene.camera().pointLightY -= 0.02;
            cube2.alpha -= 0.02;
            if (cube2.alpha <= 0)
                cube2.alpha = 0;
        }
        else {
            cube2.alpha += 0.02;
            if (cube2.alpha >= 1)
                cube2.alpha = 1;
            scene.camera().pointLightY += 0.02;
        }
        angle = anim(angle);
        Shape.modelMatrix.setRotate(angle, 0, -1, 0);
    });
}

