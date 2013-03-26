// 頂点シェーダ
varying vec4 vWorldPosition;
varying vec3 vWorldNormal;
varying vec4 vColor;

uniform mat4 uModel;
uniform mat4 uCamProj;
uniform mat4 uCamView;

uniform mat4 uLightProj;
uniform mat4 uLightView;


void main() {
	vColor = gl_Color;
	vWorldNormal = gl_Normal;
	vWorldPosition = uModel * gl_Vertex;

	gl_Position = uLightProj * uLightView * vWorldPosition;
}