// フラグメントシェーダ

uniform mat4 uLightProj;
uniform mat4 uLightView;

varying vec4 vWorldPosition;
varying vec3 vWorldNormal;
varying vec4 vColor;

/*
void main() {
	vec3 lightPosition = (uLightView * vWorldPosition).xyz; // ライト空間での位置
	gl_FragColor = vec4(vec3(-lightPosition.z / 100.0), 0.0);
}*/


// VSM
void main(){
    vec3 worldNormal = normalize(vWorldNormal);
    vec3 lightPosition = (uLightView * vWorldPosition).xyz;
    float depth = clamp(-lightPosition.z / 100.0, 0.0, 1.0);
    float dx = dFdx(depth);
    float dy = dFdy(depth);
    gl_FragColor = vec4(depth, pow(depth, 2.0) + 0.25*(dx*dx + dy*dy), 0.0, 1.0);
}
