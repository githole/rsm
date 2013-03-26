// フラグメントシェーダ
uniform vec2 resolution;

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

uniform sampler2D shadowMap;
uniform sampler2D giTexture;

void main() {
	vec3 worldNormal = normalize(vWorldNormal);
	vec3 lightPosition = (uLightView * vWorldPosition).xyz; // ライト空間での位置
	vec3 lightPositionNoramalized = normalize(lightPosition);

	vec3 lightSurfaceNormal = (uLightNormalMatrix * vec4(worldNormal, 1.0)).xyz;

	vec3 lambert = max(0.0, dot(lightSurfaceNormal, vec3(0, 0, 1)));

	// 影
	float bias = 0.0004;
	float depth1 = clamp(-lightPosition.z / 100.0, 0.0, 1.0); // ライトからの距離を求める
	vec4 lightDevice = uLightProj * uLightView * vWorldPosition; // ライトへの射影した結果
	vec2 lightDeviceNormalized = lightDevice.xy / lightDevice.w;
	vec2 lightUV = (lightDeviceNormalized.xy / 2.0) + vec2(0.5, 0.5);

	// 普通のShadowMap
	float depth2 = texture2D(shadowMap, lightUV).x 	+ bias;
	float shadow = 0.0;
	if (depth1 < depth2)
		shadow = 1.0;

	vec3 col = shadow * uLightIntensity * lambert * (vColor / 3.14159) + 0.8 * uAmbient * (vColor / 3.14159);

	// GlobalIllumination
	vec4 icol = texture2D(giTexture, gl_FragCoord.xy / resolution.xy);
//	col = icol; 
  	col = col + icol;

	col = pow(col * 2.2, 1.0 / 1.0);
	gl_FragColor = vec4(col, 1.0);
}
