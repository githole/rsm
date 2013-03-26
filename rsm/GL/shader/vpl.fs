// フラグメントシェーダ
uniform vec4 uAmbient;
uniform vec4 uLightIntensity;

uniform mat4 uCamProj;
uniform mat4 uCamView;
uniform mat4 uLightProj;
uniform mat4 uLightView;

uniform mat4 uLightNormalMatrix;

varying vec4 vWorldPosition;
varying vec3 vWorldNormal;
varying vec4 vColor;

void main() {
	vec3 worldNormal = normalize(vWorldNormal);
	vec3 lightPosition = (uLightView * vWorldPosition).xyz; // ライト空間での位置
	vec3 lightPositionNoramalized = normalize(lightPosition);

	vec3 lightSurfaceNormal = (uLightNormalMatrix * vec4(worldNormal, 1.0)).xyz;

	vec3 lambert = max(0.0, dot(lightSurfaceNormal, vec3(0, 0, 1)));

	vec3 col = uLightIntensity * lambert * (vColor / 3.14159);

	gl_FragColor = vec4(col, 1.0);
}