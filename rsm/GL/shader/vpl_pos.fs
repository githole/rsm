// フラグメントシェーダ
uniform vec4 uAmbient;
uniform vec4 uLightIntensity;

uniform mat4 uCamProj;
uniform mat4 uCamView;
uniform mat4 uLightProj;
uniform mat4 uLightView;

varying vec4 vWorldPosition;
varying vec3 vWorldNormal;
varying vec4 vColor;

void main() {
	gl_FragColor = vec4(vWorldPosition.xyz, 1.0);
}