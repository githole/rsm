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

uniform sampler2D shadowMap;

uniform sampler2D vplTexture;
uniform sampler2D vplPosTexture;
uniform sampler2D vplNormalTexture;

float rand(vec2 n) {
  return 0.5 + 0.5 * 
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

float RND_1d(vec2 x)
{
	uint n = floatBitsToUint(x.y * 214013.0 + x.x * 2531011.0);
	n = n * (n * n * 15731u + 789221u);
	n = (n >> 9u) | 0x3F800000u;

	return 2.0 - uintBitsToFloat(n);
}

void main() {
	vec3 worldNormal = normalize(vWorldNormal);
	vec3 lightPosition = (uLightView * vWorldPosition).xyz; // ライト空間での位置
	vec3 lightPositionNoramalized = normalize(lightPosition);

	mat4 lightNormalMatrix = transpose(inverse(uLightView)); // CPU上で求めておきましょう
	vec3 lightSurfaceNormal = (lightNormalMatrix * vec4(worldNormal, 1.0)).xyz;

	vec3 lambert = max(0.0, dot(lightSurfaceNormal, vec3(0, 0, 1)));

	// 影
	float depth1 = clamp(-lightPosition.z / 100.0, 0.0, 1.0); // ライトからの距離を求める
	vec4 lightDevice = uLightProj * uLightView * vWorldPosition; // ライトへの射影した結果
	vec2 lightDeviceNormalized = lightDevice.xy / lightDevice.w;
	vec2 lightUV = (lightDeviceNormalized.xy / 2.0) + vec2(0.5, 0.5);

	// やばい
	
	vec4 indc;
	float rm = 0.1;
	vec2 seed = vec2(vWorldPosition.x + vWorldPosition.y, vWorldPosition.y + vWorldPosition.z);
//	vec2 seed;
	float gzi1, gzi2;
	for (int i = 0; i < 64; i ++) {
		gzi1 = RND_1d(seed + vec2(gzi1 + i, gzi2 + i));
		gzi2 = RND_1d(seed + vec2(gzi1 + i, gzi2 + i));
	
		vec2 sample = lightUV + vec2(rm * gzi1 * sin(2.0 * 3.14159 * gzi2), rm * gzi1 * cos(2.0 * 3.14159 * gzi2));
		vec4 vpl = texture2D(vplTexture, sample);
		vec4 vplPos = texture2D(vplPosTexture, sample);
		vec4 vplNormal = texture2D(vplNormalTexture, sample);

		vec4 L = normalize(vplPos - vWorldPosition);
		vec4 c = (vColor / 3.14159) * max(0.0, dot(worldNormal, L)) * step(0.0, -dot(worldNormal, vplNormal)) * vpl;
//		vec4 c = (vColor / 3.14159) * max(0.0, dot(worldNormal, L)) * (dot(worldNormal, vplNormal) <= 0.0 ? 1.0 : 0.0) * vpl;
//		indc = dot(worldNormal, vplNormal) < 0.0 ? vec4(1.0): vec4(0.0);
//		break;

//		indc = indc + vpl / 64;
		indc = indc + (gzi1 * gzi1) * c * (4.0 * 3.14159) / 64.0;
	}
/*
	vec4 indc;
	float rm = 0.1;
	vec2 seed = vec2(vWorldPosition.x + vWorldPosition.y, vWorldPosition.y + vWorldPosition.z);
//	vec2 seed;
	float gzi1, gzi2;
	for (int i = 0; i < 32; i ++) {
		for (int j = 0; j < 32; j ++) {	
			vec2 sample = vec2(i, j) / 32.0;
			vec4 vpl = texture2D(vplTexture, sample);
			vec4 vplPos = texture2D(vplPosTexture, sample);
			vec4 vplNormal = texture2D(vplNormalTexture, sample);

			vec4 L = normalize(vplPos - vWorldPosition);
			vec4 c = (vColor / 3.14159) * max(0.0, dot(worldNormal, L)) * step(0.0, -dot(worldNormal, vplNormal)) * vpl;
	//		vec4 c = (vColor / 3.14159) * max(0.0, dot(worldNormal, L)) * (dot(worldNormal, vplNormal) <= 0.0 ? 1.0 : 0.0) * vpl;

			indc = indc + c * (2.0 * 3.14159) / (32 * 32);
		}
	}*/


	gl_FragColor = indc;
}