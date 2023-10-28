#version 450

#define M_PI 3.1415926535897932384626433832795

// 64 bytes
layout(push_constant) uniform Transform {
	mat4 mvp;
};

struct tcMod_t {
	vec4	matrix;
	vec4	offTurb;
};

struct tcGen_t {
	vec3	vector0;
	vec3	vector1;
	int		type;
};

layout(set = 1, binding = 0) uniform UBO {
	// light/env/material parameters:
	vec4 eyePos;
	vec4 lightPos;
	vec4 lightColor;
	vec4 lightVector;
	
	// refraction parameters:
	tcMod_t u_tcMod;
	tcGen_t u_tcGen;

	// entity
	mat4	u_ModelMatrix;
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;
layout(location = 2) in vec2 in_tex_coord0;
layout(location = 5) in vec3 in_normal;

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 var_RefractPosR;
layout(location = 2) out vec4 var_RefractPosG;
layout(location = 3) out vec4 var_RefractPosB;

out gl_PerVertex {
	vec4 gl_Position;
};

const float etaR = 1.0 / 1.35;
const float etaG = 1.0 / 1.20;
const float etaB = 1.0 / 1.05;

vec2 GenTexCoords(int TCGen, vec3 position, vec3 normal, vec3 TCGenVector0, vec3 TCGenVector1)
{
	vec2 tex = in_tex_coord0.st;

	if ( TCGen == 9 ) // TCGEN_VECTOR
		tex = vec2(dot(position, TCGenVector0), dot(position, TCGenVector1));

	return tex;
}

vec2 ModTexCoords(vec2 st, vec3 position, vec4 texMatrix, vec4 offTurb)
{
	float amplitude = offTurb.z;
	float phase = offTurb.w * 2.0 * M_PI;
	vec2 st2;
	st2.x = st.x * texMatrix.x + (st.y * texMatrix.z + offTurb.x);
	st2.y = st.x * texMatrix.y + (st.y * texMatrix.w + offTurb.y);

	vec2 offsetPos = vec2(position.x + position.z, position.y);
	
	vec2 texOffset = sin(offsetPos * (2.0 * M_PI / 1024.0) + vec2(phase));
	
	return st2 + texOffset * amplitude;	
}

void main() {
	gl_Position = mvp * vec4(in_position, 1.0);

	frag_color = in_color;
	
	vec2 tex = GenTexCoords(u_tcGen.type, in_position, in_normal, u_tcGen.vector0, u_tcGen.vector1);
	//vec2 tex = in_tex_coord0.st;

	vec2 var_DiffuseTex = ModTexCoords(tex, in_position, u_tcMod.matrix, u_tcMod.offTurb);
	//vec2 var_DiffuseTex = tex;

	vec3 ws_Position	= mat3(u_ModelMatrix) * in_position;
	vec3 ws_Normal		= normalize(mat3(u_ModelMatrix) * in_normal);
	vec3 ws_ViewDir		= (vec3(0.0) + vec3(0.0) * -gl_Position.x) + vec3(0.0) * gl_Position.y;
	//vec3 ws_ViewDir	= (u_ViewForward.xyz + u_ViewLeft.xyz * gl_Position.x) + u_ViewUp.xyz * gl_Position.y;

	float distance = 10.0 * clamp(1.0 - (distance(tex, var_DiffuseTex)), 0.0, 1.0);

	mat3 inverseModel = inverse(mat3(u_ModelMatrix));

	vec3 refraction_vec = normalize(refract(ws_ViewDir, ws_Normal, etaR));
	vec3 new_pos = (distance * refraction_vec) + ws_Position;
	var_RefractPosR = vec4(inverseModel * new_pos, 1.0);
	var_RefractPosR = mvp * var_RefractPosR;
	
	refraction_vec = normalize(refract(ws_ViewDir, ws_Normal, etaG));
	new_pos = (distance * refraction_vec) + ws_Position;
	var_RefractPosG = vec4(inverseModel * new_pos, 1.0);
	var_RefractPosG = mvp * var_RefractPosG;
	
	refraction_vec = normalize(refract(ws_ViewDir, ws_Normal, etaB));
	new_pos = (distance * refraction_vec) + ws_Position;
	var_RefractPosB = vec4(inverseModel * new_pos, 1.0);
	var_RefractPosB = mvp * var_RefractPosB;
}