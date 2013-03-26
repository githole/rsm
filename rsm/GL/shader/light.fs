// フラグメントシェーダ

uniform mat4 uLightProj;
uniform mat4 uLightView;

varying vec4 vWorldPosition;
varying vec3 vWorldNormal;
varying vec4 vColor;

void main() {
	vec3 lightPosition = (uLightView * vWorldPosition).xyz; // ライト空間での位置
	gl_FragColor = vec4(vec3(-lightPosition.z / 100.0), 0.0);
}

