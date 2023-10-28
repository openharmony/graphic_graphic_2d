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

import { createProgram, loadImage } from "../utils/extension";
import { Images } from '../utils/Images';
import { Matrix4 } from "../utils/Matrix";

const vSource = `
uniform mat4 a_Matrix;
uniform mat4 u_NormalMatrix;
attribute vec4 a_Position;
attribute vec2 a_TexCoord;
attribute vec4 a_Normal;

varying vec2 v_TexCoord;
varying vec3 v_Color;
varying float v_Dist;
void main(){
    v_TexCoord = a_TexCoord;
    gl_Position =  a_Matrix * a_Position;
    vec3 normal = normalize(vec3(u_NormalMatrix * a_Normal));
    vec4 vertexPosition = a_Matrix * a_Position;
    vec3 lightPosition = vec3(3.0,8.0,16.0);
    vec3 lightDirection = normalize(lightPosition - vec3(vertexPosition));
    vec3 lightColor = vec3(1.0,1.0,1.0);
    vec3 diffuse =  lightColor * max(dot(normal,lightDirection),0.0);
    vec3 ambient = vec3(0.6,0.6,0.6);
    
    vec3 eye = vec3(25,65,35);
    vec3 h=normalize(normalize(eye-a_Position.xyz)+lightDirection);
    vec3 specular=vec3(1.0,1.0,1.0) * pow( max(0.0,dot(a_Normal.xyz,h)), 64.0 );
    v_Color = diffuse+ambient;
    v_Dist = distance(vertexPosition.xyz,eye);
}
`;
const fSource = `
precision mediump float;
uniform sampler2D u_Sampler;
varying vec2 v_TexCoord;
varying vec3 v_Color;
varying float v_Dist;
void main(){
    vec4 color = texture2D(u_Sampler,v_TexCoord);
    
    vec3 u_FogColor = vec3(0.121,0.121,0.121);
    vec2 u_FogDist = vec2(60,90);
    float fogFactor = clamp((u_FogDist.y - v_Dist) / (u_FogDist.y - u_FogDist.x),0.0,1.0);
    vec3 fogColor = mix(u_FogColor,vec3(color),fogFactor);
    
    gl_FragColor = vec4(v_Color * fogColor,color.a);
}
`;
let source = new Float32Array([
    -2.0, -2.0, -2.0, 0.0, 0.0, 0.0, 0.0, -1.0,
    -2.0, 2.0, -2.0, 0.0, 0.5, 0.0, 0.0, -1.0,
    2.0, -2.0, -2.0, 0.25, 0.0, 0.0, 0.0, -1.0,
    -2.0, 2.0, -2.0, 0.0, 0.5, 0.0, 0.0, -1.0,
    2.0, 2.0, -2.0, 0.25, 0.5, 0.0, 0.0, -1.0,
    2.0, -2.0, -2.0, 0.25, 0.0, 0.0, 0.0, -1.0,
    -2.0, -2.0, 2.0, 0.25, 0.0, 0.0, 0.0, 1.0,
    2.0, -2.0, 2.0, 0.5, 0.0, 0.0, 0.0, 1.0,
    -2.0, 2.0, 2.0, 0.25, 0.5, 0.0, 0.0, 1.0,
    -2.0, 2.0, 2.0, 0.25, 0.5, 0.0, 0.0, 1.0,
    2.0, -2.0, 2.0, 0.5, 0.0, 0.0, 0.0, 1.0,
    2.0, 2.0, 2.0, 0.5, 0.5, 0.0, 0.0, 1.0,
    -2.0, 2.0, -2.0, 0.5, 0.0, 0.0, 1.0, 0.0,
    -2.0, 2.0, 2.0, 0.5, 0.5, 0.0, 1.0, 0.0,
    2.0, 2.0, -2.0, 0.75, 0, 0.0, 1.0, 0.0,
    -2.0, 2.0, 2.0, 0.5, 0.5, 0.0, 1.0, 0.0,
    2.0, 2.0, 2.0, 0.75, 0.5, 0.0, 1.0, 0.0,
    2.0, 2.0, -2.0, 0.75, 0.0, 0.0, 1.0, 0.0,
    -2.0, -2.0, -2.0, 0, 0.5, 0.0, -1.0, 0.0,
    2.0, -2.0, -2.0, 0.25, 0.5, 0.0, -1.0, 0.0,
    -2.0, -2.0, 2.0, 0.0, 1.0, 0.0, -1.0, 0.0,
    -2.0, -2.0, 2.0, 0.0, 1.0, 0.0, -1.0, 0.0,
    2.0, -2.0, -2.0, 0.25, 0.5, 0.0, -1.0, 0.0,
    2.0, -2.0, 2.0, 0.25, 1.0, 0.0, -1.0, 0.0,
    -2.0, -2.0, -2.0, 0.25, 0.5, -1.0, 0.0, 0.0,
    -2.0, -2.0, 2.0, 0.25, 1.0, -1.0, 0.0, 0.0,
    -2.0, 2.0, -2.0, 0.5, 0.5, -1.0, 0.0, 0.0,
    -2.0, -2.0, 2.0, 0.25, 1.0, -1.0, 0.0, 0.0,
    -2.0, 2.0, 2.0, 0.5, 1.0, -1.0, 0.0, 0.0,
    -2.0, 2.0, -2.0, 0.5, 0.5, -1.0, 0.0, 0.0,
    2.0, -2.0, -2.0, 0.5, 0.5, 1.0, 0.0, 0.0,
    2.0, 2.0, -2.0, 0.75, 0.5, 1.0, 0.0, 0.0,
    2.0, -2.0, 2.0, 0.5, 1.0, 1.0, 0.0, 0.0,
    2.0, -2.0, 2.0, 0.5, 1.0, 1.0, 0.0, 0.0,
    2.0, 2.0, -2.0, 0.75, 0.5, 1.0, 0.0, 0.0,
    2.0, 2.0, 2.0, 0.75, 1.0, 1.0, 0.0, 0.0,
]);

let m1 = new Matrix4();
let m2 = new Matrix4();
let mX = new Matrix4();
let mY = new Matrix4();
let mZ = new Matrix4();
let normalMatrix = new Matrix4();
let angle = 0;
let lastTime = Date.now();

export async function cube_demo(gl) {
    let program = createProgram(gl, vSource, fSource, ["a_Position", "a_TexCoord", "a_Normal"], ["a_Matrix", "u_NormalMatrix", "u_Sampler"]);
    let buffer = gl.createBuffer();
    if (buffer === null) {
        return;
    }
    gl.bindBuffer(gl.ARRAY_BUFFER, buffer);
    gl.bufferData(gl.ARRAY_BUFFER, source, gl.STATIC_DRAW);
    gl.vertexAttribPointer(program.a_Position, 3, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 8, 0);
    gl.enableVertexAttribArray(program.a_Position);
    gl.vertexAttribPointer(program.a_TexCoord, 2, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 8, Float32Array.BYTES_PER_ELEMENT * 3);
    gl.enableVertexAttribArray(program.a_TexCoord);
    gl.vertexAttribPointer(program.a_Normal, 3, gl.FLOAT, false, Float32Array.BYTES_PER_ELEMENT * 8, Float32Array.BYTES_PER_ELEMENT * 5);
    gl.enableVertexAttribArray(program.a_Normal);
    let image = await loadImage(Images.MF_IMG);
    let texture = gl.createTexture();
    if (texture === null) {
        return;
    }
    gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, 1);
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
    gl.generateMipmap(gl.TEXTURE_2D);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
    gl.uniform1i(program.u_Sampler, 0);
    clear(gl);
    gl.enable(gl.BLEND);
    gl.enable(gl.DEPTH_TEST);
    gl.enable(gl.CULL_FACE);
    gl.enable(gl.POLYGON_OFFSET_FILL);
    gl.polygonOffset(1.0, 1.0);
    gl.viewport(0, 0, gl.width, gl.height);
    draw(program, gl);
}

function draw(program, gl) {
    angle = anim(angle);
    m1.setPerspective(30, gl.width / gl.height, 1, 100);
    m2.setLookAt(0, 6, 16, 0, 0, 0, 0, 1, 0);
    mX.setRotate(angle, 1, 0, 0);
    mY.setRotate(angle, 0, 1, 0);
    mZ.setRotate(angle, 0, 0, 1);
    let matrix4 = mY.multiply(mX);
    let mat = m1.multiply(m2).multiply(matrix4);
    gl.uniformMatrix4fv(program.a_Matrix, false, mat.getElements());
    normalMatrix.setInverseOf(matrix4);
    normalMatrix.transpose();
    gl.uniformMatrix4fv(program.u_NormalMatrix, false, normalMatrix.getElements());
    clear(gl);
    gl.drawArrays(gl.TRIANGLES, 0, 36);
    gl.flush();
    if (!gl.closed) {
        requestAnimationFrame(() => draw(program, gl));
    }
}

let anim = (angle) => {
    let now = Date.now();
    let offset = now - lastTime;
    lastTime = now;
    let newAngle = angle + (20 * offset) / 1000.0;
    return newAngle % 360;
}

function clear(gl) {
    gl.clearColor(0.0, 0.0, 0.0, 1);
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
}
