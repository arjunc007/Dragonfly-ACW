#define NUM_LIGHTS 4

Texture2D shaderTexture : register (t0);
SamplerState sampleType : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	matrix View;
	matrix Projection;
}

cbuffer LightBuffer : register(b0)
{
	float4 ambientColor;
	float4 diffuseColor;
	float4 specularColor;
	float specularPower;
	float3 attenuation;
	float range;
	float cone;
	float2 lightpadding;
}

cbuffer LightPositionBuffer : register(b1)
{
	float3 lightPosition[NUM_LIGHTS];
	float3 lightFocus[NUM_LIGHTS];
}

cbuffer CameraBuffer : register(b2)
{
	float3 cameraPosition;
	float padding;
};

cbuffer ModelBuffer : register(b3)
{
	 matrix World;
}

cbuffer ParticleBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD0;
	float3 viewDirection : TEXCOORD1;
	float3 lightPos1 : TEXCOORD2;
	float3 lightPos2 : TEXCOORD3;
	float3 lightPos3 : TEXCOORD4;
	float3 lightPos4 : TEXCOORD5;
	float3 lightDir1 : TECOORD6;
	float3 lightDir2 : TECOORD7;
	float3 lightDir3 : TECOORD8;
	float3 lightDir4 : TECOORD9;
};

struct ParticleVS_INPUT
{
	float4 position : POSITION;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

struct ParticlePS_INPUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 color : COLOR;
};


//Light and Texture
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;
	float4 worldPosition;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.pos.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.pos = mul(input.pos, World);
	output.pos = mul(output.pos, View);
	output.pos = mul(output.pos, Projection);

	// Store the input texture for the pixel shader to use.
	output.tex = input.tex;

	//Diffuse lighting
	output.norm = normalize(mul(input.norm, (float3x3)World));
	
	//Calculate position of vertex in world
	worldPosition = mul(input.pos, World);

	//Calculate lightPos in world
	output.lightPos1 = lightPosition[0].xyz - worldPosition.xyz;
	output.lightPos2 = lightPosition[1].xyz - worldPosition.xyz;
	output.lightPos3 = lightPosition[2].xyz - worldPosition.xyz;
	output.lightPos4 = lightPosition[3].xyz - worldPosition.xyz;

	output.lightDir1 = lightFocus[0] - lightPosition[0];
	output.lightDir2 = lightFocus[1] - lightPosition[1];
	output.lightDir3 = lightFocus[2] - lightPosition[2];
	output.lightDir4 = lightFocus[3] - lightPosition[3];

	output.viewDirection = normalize(cameraPosition.xyz - worldPosition.xyz);

	return output;
}

float4 PS(PS_INPUT input) : SV_TARGET
{
	float4 color1, color2, color3, color4;

	color1 = color2 = color3 = color4 = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 diffuseLight = shaderTexture.Sample(sampleType, input.tex);
	float4 finalAmbient = diffuseLight*ambientColor;

	float distToLight1 = length(input.lightPos1);
	float distToLight2 = length(input.lightPos2);
	float distToLight3 = length(input.lightPos3);
	float distToLight4 = length(input.lightPos4);

	float3 vectFromLight1 = normalize(input.lightPos1);
	float3 vectFromLight2 = normalize(input.lightPos2);
	float3 vectFromLight3 = normalize(input.lightPos3);
	float3 vectFromLight4 = normalize(input.lightPos4);

	float intensity = dot(vectFromLight1, input.norm);

	float3 reflection;
	float4 specular;

	//If light is striking the front side of the pixel
	if (intensity > 0.0f)
	{
		//Add light to the finalColor of the pixel
		color1 += intensity;

		//addd specular lighting
		reflection = normalize(reflect(vectFromLight1 * intensity, input.norm));

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

		//Calculate Light's Falloff factor
		color1 /= attenuation[0] + (attenuation[1] * distToLight1) + (attenuation[2] * (distToLight1*distToLight1));

		color1 *= pow(max(dot(-vectFromLight1, input.lightDir1), 0.0f), cone);
	}

	//make sure the values are between 1 and 0, and add the ambient
	color1 = saturate(color1 + finalAmbient + specular);

	intensity = dot(vectFromLight2, input.norm);

	//If light is striking the front side of the pixel
	if (intensity > 0.0f)
	{
		//Add light to the finalColor of the pixel
		color2 += intensity;

		//addd specular lighting
		reflection = normalize(reflect(vectFromLight2 * intensity, input.norm));

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

		//Calculate Light's Falloff factor
		color2 /= attenuation[0] + (attenuation[1] * distToLight2) + (attenuation[2] * (distToLight2*distToLight2));

		color2 *= pow(max(dot(-vectFromLight2, input.lightDir2), 0.0f), cone);
	}

	//make sure the values are between 1 and 0, and add the ambient
	color2 = saturate(color2 + finalAmbient + specular);

	intensity = dot(vectFromLight3, input.norm);

	//If light is striking the front side of the pixel
	if (intensity > 0.0f)
	{
		//Add light to the finalColor of the pixel
		color3 += intensity;

		//addd specular lighting
		reflection = normalize(reflect(vectFromLight3 * intensity, input.norm));

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

		//Calculate Light's Falloff factor
		color3 /= attenuation[0] + (attenuation[1] * distToLight3) + (attenuation[2] * (distToLight3*distToLight3));

		//Calculate falloff from center to edge of pointlight cone
		color3 *= pow(max(dot(-vectFromLight3, input.lightDir3), 0.0f), cone);
	}

	//make sure the values are between 1 and 0, and add the ambient
	color3 = saturate(color3 + finalAmbient + specular);


	intensity = dot(vectFromLight4, input.norm);

	//If light is striking the front side of the pixel
	if (intensity > 0.0f)
	{
		//Add light to the finalColor of the pixel
		color4 += intensity;

		//addd specular lighting
		reflection = normalize(reflect(vectFromLight4 * intensity, input.norm));

		// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
		specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);

		//Calculate Light's Falloff factor
		color4 /= attenuation[0] + (attenuation[1] * distToLight4) + (attenuation[2] * (distToLight4*distToLight4));

		color4 *= pow(max(dot(-vectFromLight4, input.lightDir4), 0.0f), cone);
	}

	//make sure the values are between 1 and 0, and add the ambient
	color4 = saturate(color4 + finalAmbient + specular);

	//If pixel is too far, return pixel color with ambient light
	if (distToLight1 > range)
		color1 = finalAmbient;
	if (distToLight2 > range)
		color2 = finalAmbient;
	if (distToLight3 > range)
		color3 = finalAmbient;
	if (distToLight4 > range)
		color4 = finalAmbient;

	return saturate(color1 + color2 + color3 + color4)  * diffuseLight * diffuseColor;

	//float4 textureColor;
	//float lightIntensity;
	//float4 color1, color2, color3, color4;
	//float3 lightDir;
	//float3 reflection;
	//float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//// Set the default output color to the ambient light value for all pixels.
	//color1 = ambientColor;
	//color2 = color3 = color4 = float4(0.0f, 0.0f, 0.0f, 0.0f);

	//lightDir = normalize(input.lightPos1);
	//lightIntensity = saturate(dot(input.norm, lightDir));

	//if (lightIntensity > 0.0f)
	//{
	//	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	//	color1 += (diffuseColor * lightIntensity);

	//	// Saturate ambient and diffuse color.
	//	color1 = saturate(color1);

	//	reflection = normalize(reflect(lightDir * lightIntensity, input.norm));

	//	// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
	//	specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	//}

	////Add specular component
	//color1 = saturate(color1 + specular);

	//lightDir = normalize(input.lightPos2);
	//lightIntensity = saturate(dot(input.norm, lightDir));

	//if (lightIntensity > 0.0f)
	//{
	//	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	//	color2 += (diffuseColor * lightIntensity);

	//	// Saturate ambient and diffuse color.
	//	color2 = saturate(color2);

	//	reflection = normalize(reflect(lightDir * lightIntensity, input.norm));

	//	// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
	//	specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	//}

	////Add specular component
	//color2 = saturate(color2 + specular);

	//lightDir = normalize(input.lightPos3);
	//lightIntensity = saturate(dot(input.norm, lightDir));

	//if (lightIntensity > 0.0f)
	//{
	//	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	//	color3 += (diffuseColor * lightIntensity);

	//	// Saturate ambient and diffuse color.
	//	color3 = saturate(color3);

	//	reflection = normalize(reflect(lightDir * lightIntensity, input.norm));

	//	// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
	//	specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	//}

	////Add specular component
	//color3 = saturate(color3 + specular);

	//lightDir = normalize(input.lightPos4);
	//lightIntensity = saturate(dot(input.norm, lightDir));

	//if (lightIntensity > 0.0f)
	//{
	//	// Determine the final diffuse color based on the diffuse color and the amount of light intensity.
	//	color4 += (diffuseColor * lightIntensity);

	//	// Saturate ambient and diffuse color.
	//	color4 = saturate(color4);

	//	reflection = normalize(reflect(lightDir * lightIntensity, input.norm));

	//	// Determine the amount of specular light based on the reflection vector, viewing direction, and specular power.
	//	specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
	//}

	////Add specular component
	//color4 = saturate(color4 + specular);

	////Sample pixel color from texture using sampler at this texutre coordinate location
	//textureColor = shaderTexture.Sample(sampleType, input.tex);
	////color *= textureColor;

	//return (color1 + color2 + color3 + color4) * textureColor;
}


////////////////////////////////////////////////////////
//Particle System
///////////////////////////////////////////////////////

ParticlePS_INPUT ParticleVertexShader(ParticleVS_INPUT input)
{
	ParticlePS_INPUT output;


	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;

	// Store the particle color for the pixel shader. 
	output.color = input.color;

	return output;
}

float4 ParticlePixelShader(ParticlePS_INPUT input) : SV_TARGET
{
	float4 textureColor;
	float4 finalColor;

	// Sample the pixel color from the texture using the sampler at this texture coordinate location.
	textureColor = shaderTexture.Sample(sampleType, input.tex);

	// Combine the texture color and the particle color to get the final color result.
	finalColor = textureColor * input.color;

	return finalColor;
}