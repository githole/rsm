// フラグメントシェーダ

uniform sampler2D texture;
uniform vec2 resolution;

const float blurSize = 3/512.0; 
 
void main(void)
{
	vec4 sum = vec4(0.0);
	vec2 vTexCoord = gl_FragCoord.xy / resolution.xy;

	// blur in y (vertical)
	// take nine samples, with the distance blurSize between them
	sum += texture2D(texture, vec2(vTexCoord.x - 4.0*blurSize, vTexCoord.y)) * 0.05;
	sum += texture2D(texture, vec2(vTexCoord.x - 3.0*blurSize, vTexCoord.y)) * 0.09;
	sum += texture2D(texture, vec2(vTexCoord.x - 2.0*blurSize, vTexCoord.y)) * 0.12;
	sum += texture2D(texture, vec2(vTexCoord.x - blurSize, vTexCoord.y)) * 0.15;
	sum += texture2D(texture, vec2(vTexCoord.x, vTexCoord.y)) * 0.16;
	sum += texture2D(texture, vec2(vTexCoord.x + blurSize, vTexCoord.y)) * 0.15;
	sum += texture2D(texture, vec2(vTexCoord.x + 2.0*blurSize, vTexCoord.y)) * 0.12;
	sum += texture2D(texture, vec2(vTexCoord.x + 3.0*blurSize, vTexCoord.y)) * 0.09;
	sum += texture2D(texture, vec2(vTexCoord.x + 4.0*blurSize, vTexCoord.y)) * 0.05;

	gl_FragColor = sum;
}