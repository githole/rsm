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

float texture2DCompare(sampler2D depths, vec2 uv, float compare){
	float depth = texture2D(depths, uv).r;
	return step(compare, depth);
}

float texture2DShadowLerp(sampler2D depths, vec2 size, vec2 uv, float compare){
	vec2 texelSize = vec2(1.0)/size;
	vec2 f = fract(uv*size+0.5);
	vec2 centroidUV = floor(uv*size+0.5)/size;

	float lb = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 0.0), compare);
	float lt = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 1.0), compare);
	float rb = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 0.0), compare);
	float rt = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 1.0), compare);
	float a = mix(lb, lt, f.y);
	float b = mix(rb, rt, f.y);
	float c = mix(a, b, f.x);
	return c;
}


float linstep(float low, float high, float v){
	return clamp((v-low)/(high-low), 0.0, 1.0);
}

float VSM(sampler2D depths, vec2 uv, float compare){
	vec2 moments = texture2D(depths, uv).xy;
	float p = smoothstep(compare-0.02, compare, moments.x);
	float variance = max(moments.y - moments.x*moments.x, -0.001);
	float d = compare - moments.x;
	float p_max = linstep(0.2, 1.0, variance / (variance + d*d));
	return clamp(max(p, p_max), 0.0, 1.0);
}
float chebyshevUpperBound(vec2 uv, float distance)
{
	// We retrive the two moments previously stored (depth and depth*depth)
	vec2 moments = texture2D(shadowMap, uv).rg;
	
	// Surface is fully lit. as the current fragment is before the light occluder
	if (distance <= moments.x)
		return 1.0 ;

	// The fragment is either in shadow or penumbra. We now use chebyshev's upperBound to check
	// How likely this pixel is to be lit (p_max)
	float variance = moments.y - (moments.x*moments.x);
	variance = max(variance,0.00002);

	float d = distance - moments.x;
	float p_max = variance / (variance + d*d);

	return p_max;
}
	

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

	// VSM
/*
	float shadow = VSM(shadowMap, lightUV, depth1);
	vec4 vsm = texture2D(shadowMap, lightUV);
	float depth_sq = depth.x * depth.x;
	float variance = depth.y - depth_sq;
*/
//	float shadow = chebyshevUpperBound(lightUV, depth1);


//	float shadow = texture2DShadowLerp(shadowMap, vec2(1024, 1024), lightUV, depth1 - bias);
	


	vec3 col = shadow * uLightIntensity * lambert * (vColor / 3.14159) + 0.8 * uAmbient * (vColor / 3.14159);

	// GlobalIllumination
	vec4 icol = texture2D(giTexture, gl_FragCoord.xy / resolution.xy);
//	col = icol; 
  	col = col + icol;


	col = pow(col * 2.2, 1.0 / 1.0);

	gl_FragColor = vec4(col, 1.0);
}
