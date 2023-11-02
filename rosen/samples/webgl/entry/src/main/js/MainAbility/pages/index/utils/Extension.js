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
export async function loadImage(src) {
    return new Promise((resolve, reject) => {
        let image = new Image();
        resolve(image);
        image.src = src;
    });
}

export function createProgram(gl, vSource, fSource, attribute = [], uniform = []) {
    let initShader = (gl, type, source) => {
        let shader = gl.createShader(type);
        if (shader === null) {
            gl.deleteShader(shader);
            return null;
        }
        gl.shaderSource(shader, source);
        gl.compileShader(shader);
        if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
            console.log(gl.getShaderInfoLog(shader));
            gl.deleteShader(shader);
            return null;
        }
        return shader;
    };
    let vShader = initShader(gl, gl.VERTEX_SHADER, vSource);
    let fShader = initShader(gl, gl.FRAGMENT_SHADER, fSource);
    if (vShader === null || fShader === null) {
        return null;
    }
    let program = gl.createProgram();
    if (program === null) {
        gl.deleteProgram(program);
        return null;
    }
    gl.attachShader(program, vShader);
    gl.attachShader(program, fShader);
    gl.linkProgram(program);
    if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
        console.log(gl.getProgramInfoLog(program));
        gl.deleteProgram(program);
        return null;
    }
    gl.useProgram(program);
    gl.program = program;
    gl.vertexShader = vShader;
    gl.fragmentShader = fShader;
    if (attribute.length > 0) {
        attribute.forEach((item, index) => {
            program[item] = gl.getAttribLocation(program, item);
        });
    }
    if (uniform.length > 0) {
        uniform.forEach((item, index) => {
            program[item] = gl.getUniformLocation(program, item);
        });
    }
    return program;
}
