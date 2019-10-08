#version 330

layout(location = 0) out vec4 gFragColor;
//layout (location = 0) out vec3 gFragPos;
//layout (location = 1) out vec3 gFragNormal;

in vec3 FragPos;
in vec3 FragNormal;
in vec2 FragUV;

#define PI        3.14159265358979323
#define TWO_PI    6.28318530717958648
#define EPS 0.001

uniform sampler2D materialsTex;
uniform sampler2D lightsTex;
uniform sampler2DArray textureMapsArrayTex;
uniform sampler2D hdrTex;

uniform vec3 randomVector;
uniform int textureIndex;
uniform vec3 albedo;
uniform vec3 cameraPos;
uniform float hdrMultiplier;
uniform int numOfLights;
uniform bool useEnvMap;

struct Light { vec3 position; vec3 emission; vec3 u; vec3 v; vec3 radiusAreaType; };
struct LightSampleRec { vec3 surfacePos; vec3 normal; vec3 emission; float pdf; };

vec2 seed;
vec3 materialAlbedo;

//-----------------------------------------------------------------------
float rand()
//-----------------------------------------------------------------------
{
	seed -= vec2(randomVector.x * randomVector.y);
	return fract(sin(dot(seed, vec2(12.9898, 78.233))) * 43758.5453);
}

//-----------------------------------------------------------------------
vec3 UniformSampleSphere(float u1, float u2)
//-----------------------------------------------------------------------
{
	float z = 1.0 - 2.0 * u1;
	float r = sqrt(max(0.f, 1.0 - z * z));
	float phi = 2.0 * PI * u2;
	float x = r * cos(phi);
	float y = r * sin(phi);

	return vec3(x, y, z);
}

//-----------------------------------------------------------------------
void sampleSphereLight(in Light light, inout LightSampleRec lightSampleRec)
//-----------------------------------------------------------------------
{
	float r1 = rand();
	float r2 = rand();

	lightSampleRec.surfacePos = light.position + UniformSampleSphere(r1, r2) * light.radiusAreaType.x;
	lightSampleRec.normal = normalize(lightSampleRec.surfacePos - light.position);
	lightSampleRec.emission = light.emission * float(numOfLights);
}

//-----------------------------------------------------------------------
void sampleQuadLight(in Light light, inout LightSampleRec lightSampleRec)
//-----------------------------------------------------------------------
{
	float r1 = rand();
	float r2 = rand();

	lightSampleRec.surfacePos = light.position + light.u * r1 + light.v * r2;
	lightSampleRec.normal = normalize(cross(light.u, light.v));
	lightSampleRec.emission = light.emission * float(numOfLights);
}

//-----------------------------------------------------------------------
void sampleLight(in Light light, inout LightSampleRec lightSampleRec)
//-----------------------------------------------------------------------
{
	if (int(light.radiusAreaType.z) == 0) // Quad Light
		sampleQuadLight(light, lightSampleRec);
	else
		sampleSphereLight(light, lightSampleRec);
}

//-----------------------------------------------------------------------
vec3 DirectLight()
//-----------------------------------------------------------------------
{
	vec3 L = vec3(0.0);

	vec3 surfacePos = FragPos + FragNormal * EPS;

	/* Since we are not randomly sampling for progressive mode, sample all analytic Lights */
	for (int i = 0; i < numOfLights; i++)
	{
		LightSampleRec lightSampleRec;
		Light light;

		//Pick a light to sample
		int index = i;

		// Fetch light Data
		vec3 p = texelFetch(lightsTex, ivec2(index * 5 + 0, 0), 0).xyz;
		vec3 e = texelFetch(lightsTex, ivec2(index * 5 + 1, 0), 0).xyz;
		vec3 u = texelFetch(lightsTex, ivec2(index * 5 + 2, 0), 0).xyz;
		vec3 v = texelFetch(lightsTex, ivec2(index * 5 + 3, 0), 0).xyz;
		vec3 rad = texelFetch(lightsTex, ivec2(index * 5 + 4, 0), 0).xyz;

		light = Light(p, e, u, v, rad);
		sampleLight(light, lightSampleRec);

		vec3 lightDir = p - surfacePos;
		float lightDist = length(lightDir);
		float lightDistSq = lightDist * lightDist;
		lightDir /= sqrt(lightDistSq);

		if (dot(lightDir, FragNormal) <= 0.0 || dot(lightDir, lightSampleRec.normal) >= 0.0)
			continue;

		float lightPdf = lightDistSq / (light.radiusAreaType.y * abs(dot(lightSampleRec.normal, lightDir)));

		L += (materialAlbedo / PI) * abs(dot(FragNormal, lightDir)) * e / lightPdf;

	}
	return L;
}

void main()
{
	seed = gl_FragCoord.xy;
	vec3 radiance = vec3(0.);
	vec3 I = normalize(FragPos - cameraPos);
	vec3 R = reflect(I, normalize(FragNormal));

	vec2 uv = vec2((PI + atan(R.z, R.x)) * (1.0 / TWO_PI), acos(R.y) * (1.0 / PI));

	if (textureIndex != -1)
		materialAlbedo = pow(texture(textureMapsArrayTex, vec3(FragUV, textureIndex)), vec4(2.2)).xyz;
	else
		materialAlbedo = albedo;

	// If we have albedo textures then choose this over HDR reflection
	if (textureIndex != -1 && numOfLights == 0)
	{
		radiance = materialAlbedo;
	}
	else if (numOfLights > 0)
	{
		//if (dot(-I, normalize(FragNormal)) > 0.0)
			radiance = DirectLight();
	}
	else if (useEnvMap)
	{
		radiance = texture(hdrTex, uv).xyz * hdrMultiplier * materialAlbedo;
	}

	gFragColor = vec4(radiance, 1.0);
}