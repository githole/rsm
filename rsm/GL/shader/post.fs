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
}