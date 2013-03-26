// フラグメントシェーダ

uniform float time;
uniform vec2 resolution;
uniform sampler2D texture;

float rand(vec2 n) {
  return 0.5 + 0.5 * 
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main() {

	gl_FragColor = texture2D(texture, gl_FragCoord.xy / resolution.xy);
//	gl_FragColor =  vec4(rand(gl_FragCoord.xy + vec2(time, time * 2))) * 0.1 + texture2D(texture, gl_FragCoord.xy / vec2(1280, 720));
}