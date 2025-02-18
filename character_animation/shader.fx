



struct light
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 dirW;  
};

struct material
{
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float  specPower;
};

uniform extern light    g_light;
uniform extern material g_material;

uniform extern float4x4 g_mvp; 
uniform extern float4x4 g_world; 

uniform extern float4x4 g_bones[75];

uniform extern texture g_tex;
sampler tex_s = sampler_state {
     Texture = <g_tex>;
 };
 

struct OutputVS{  
     float4 pos      : POSITION0; 
     float2 tex      : TEXCOORD0;  
     float3 normal   : TEXCOORD1;
	 float4 eyeCoords: TEXCOORD2;
}; 


OutputVS VertexShader_blend(
     float3 pos       : POSITION0,
     float3 normal    : NORMAL0,
     float2 tex       : TEXCOORD0,
     float4 boneindex : BLENDINDICES0,
     float4 weights   : BLENDWEIGHT0  ) {
         OutputVS outVS = (OutputVS)0;
         float4 posL = weights.x * mul(float4(pos, 1.0f), g_bones[ int(boneindex.x) ]);
         posL       += weights.y * mul(float4(pos, 1.0f), g_bones[ int(boneindex.y) ]);
         posL       += weights.z * mul(float4(pos, 1.0f), g_bones[ int(boneindex.z) ]);
         posL       += weights.w * mul(float4(pos, 1.0f), g_bones[ int(boneindex.w) ]);
         posL.w = 1.0f;
         float4 normalL = weights.x * mul(float4(normal, 0.0f), g_bones[ int(boneindex.x) ]);
         normalL       += weights.y * mul(float4(normal, 0.0f), g_bones[ int(boneindex.y) ]);
         normalL       += weights.z * mul(float4(normal, 0.0f), g_bones[ int(boneindex.z) ]);
         normalL       += weights.w * mul(float4(normal, 0.0f), g_bones[ int(boneindex.w) ]);
         normalL.w = 0.0f;
		 
		 outVS.tex = tex;
         outVS.pos = mul(posL, g_mvp);
		 
		 
		 outVS.normal =mul(normalL, g_world).xyz;
         outVS.eyeCoords = mul(g_mvp, posL);
         return outVS;
	}


float4 PixelShader_blend( float2 tex0:TEXCOORD0,float3 normal:TEXCOORD1,float4 eyeCoords:TEXCOORD2 ) : COLOR { 


     float3  s = normalize( ( g_light.dirW - eyeCoords).xyz );
     float3  v = normalize(-eyeCoords.xyz);


     float3  r = reflect( -s, normal );
     float3  ambient = g_light.ambient * g_material.ambient;
     float   sDotN = max( dot(s,normal), 0.0 );
     float3  diffuse = g_light.diffuse * g_material.diffuse * sDotN;

     float3  spec = float3(0.0f,0.0f,0.0f);

     if( sDotN > 0.0 ) 
	    {
         spec = g_light.spec * g_material.spec *
         pow( max( dot(  r   ,v), 0.0 ), g_material.spec );
        }
     return float4((  (ambient + diffuse + spec) *tex2D(tex_s, tex0).rgb) , 1.0f); 
} 

technique bone_tech { 
     pass P0 
         { 
             vertexShader = compile vs_2_0 VertexShader_blend(); 
             pixelShader  = compile ps_2_0 PixelShader_blend();
         }
}