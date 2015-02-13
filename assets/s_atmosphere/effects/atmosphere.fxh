/**
* Precomputed Atmospheric Scattering
* Copyright (c) 2008 INRIA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holders nor the names of its
*    contributors may be used to endorse or promote products derived from
*    this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
* THE POSSIBILITY OF SUCH DAMAGE.
*/

/**
* Author: Eric Bruneton
* This copy is a modified.
*/

static const float Rg = 6420.0 - 500.0;
static const float Rt = 6420.0;
static const float RL = 6421.0;

static const int TRANSMITTANCE_W = 256;
static const int TRANSMITTANCE_H = 64;

static const int SKY_W = 64;
static const int SKY_H = 16;

static const int RES_R = 64;
static const int RES_MU = 128;
static const int RES_MU_S = 32;
static const int RES_NU = 8;


static const float AVERAGE_GROUND_REFLECTANCE = 0.1;

// Rayleigh
static const float HR = 8.0 * 2.5;
//const vec3 betaR = vec3(5.8e-3, 1.35e-2, 1.75e-2); // = vec3(5.8e-3, 1.35e-2, 3.31e-2);
//static const float3 betaR = float3(5.8e-3, 1.35e-2, 1.75e-2); // = vec3(5.8e-3, 1.35e-2, 3.31e-2);
static const float3 betaR = float3(5.8e-3, 6.8e-3, 7.8e-3); // = vec3(5.8e-3, 1.35e-2, 3.31e-2);

// Mie
// DEFAULT
static const float HM = 1.2;
static const float3 betaMSca = 4e-3;
static const float3 betaMEx = betaMSca / 0.9;
static const float mieG = 0.8;

// ----------------------------------------------------------------------------
// NUMERICAL INTEGRATION PARAMETERS
// ----------------------------------------------------------------------------

static const float M_PI = 3.141592657;

static const float exposure = 0.2;
static const float ISun = 100.0;

sampler2D transmittanceSampler : register(s0);


float3 HDR(float3 L)
{
	L = L * exposure;
	L = max(L, 0);
	L.r = L.r < 1.413 ? pow(L.r * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.r);
	L.g = L.g < 1.413 ? pow(L.g * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.g);
	L.b = L.b < 1.413 ? pow(L.b * 0.38317, 1.0 / 2.2) : 1.0 - exp(-L.b);
	return L;
}

// ----------------------------------------------------------------------------
// PARAMETERIZATION FUNCTIONS
// ----------------------------------------------------------------------------

float2 getTransmittanceUV(float r, float mu) {
	float uR, uMu;
	uR = sqrt((r - Rg) / (Rt - Rg));
	uMu = atan((mu + 0.15) / (1.0 + 0.15) * tan(1.5)) / 1.5;

	return float2(uMu, uR);
}

float2 getIrradianceUV(float r, float muS)
{
	float uR = (r - Rg) / (Rt - Rg);
	float uMuS = (muS + 0.2) / (1.0 + 0.2);
	return float2(uMuS, uR);
}

float4 texture4D(sampler3D table, float r, float mu, float muS, float nu)
{
	float H = sqrt(Rt * Rt - Rg * Rg);
	float rho = sqrt(r * r - Rg * Rg);

	float rmu = r * mu;
	float delta = rmu * rmu - r * r + Rg * Rg;
	float4 cst = rmu < 0.0 && delta > 0.0 ? float4(1.0, 0.0, 0.0, 0.5 - 0.5 / float(RES_MU)) : float4(-1.0, H * H, H, 0.5 + 0.5 / float(RES_MU));
	float uR = 0.5 / float(RES_R) + rho / H * (1.0 - 1.0 / float(RES_R));
	float uMu = cst.w + (rmu * cst.x + sqrt(delta + cst.y)) / (rho + cst.z) * (0.5 - 1.0 / float(RES_MU));
	// paper formula
	//float uMuS = 0.5 / float(RES_MU_S) + max((1.0 - exp(-3.0 * muS - 0.6)) / (1.0 - exp(-3.6)), 0.0) * (1.0 - 1.0 / float(RES_MU_S));
	// better formula
	float uMuS = 0.5 / float(RES_MU_S) + (atan(max(muS, -0.1975) * tan(1.26 * 1.1)) / 1.1 + (1.0 - 0.26)) * 0.5 * (1.0 - 1.0 / float(RES_MU_S));

	float lerp = (nu + 1.0) / 2.0 * (float(RES_NU) - 1.0);
	float uNu = floor(lerp);
	lerp = lerp - uNu;
	return tex3D(table, float3((uNu + uMuS) / float(RES_NU), uMu, uR)) * (1.0 - lerp) +
		tex3D(table, float3((uNu + uMuS + 1.0) / float(RES_NU), uMu, uR)) * lerp;
}

// ----------------------------------------------------------------------------
// UTILITY FUNCTIONS
// ----------------------------------------------------------------------------

// transmittance(=transparency) of atmosphere for infinite ray (r,mu)
// (mu=cos(view zenith angle)), intersections with ground ignored
float3 transmittance(float r, float mu)
{
	float2 uv = getTransmittanceUV(r, mu);
	return tex2D(transmittanceSampler, uv).rgb;
}

// transmittance(=transparency) of atmosphere for infinite ray (r,mu)
// (mu=cos(view zenith angle)), or zero if ray intersects ground
float3 transmittanceWithShadow(float r, float mu)
{
	return mu < -sqrt(1.0 - (Rg / r) * (Rg / r)) ? 0 : transmittance(r, mu);
}

// transmittance(=transparency) of atmosphere between x and x0
// assume segment x,x0 not intersecting ground
// r=||x||, mu=cos(zenith angle of [x,x0) ray at x), v=unit direction vector of [x,x0) ray
float3 transmittance(float r, float mu, float3 v, float3 x0)
{
	float3 result;
	float r1 = length(x0);
	float mu1 = dot(x0, v) / r;
	if (mu > 0.0) {
		result = min(transmittance(r, mu) / transmittance(r1, mu1), 1.0);
	}
	else {
		result = min(transmittance(r1, -mu1) / transmittance(r, -mu), 1.0);
	}
	return result;
}

// transmittance(=transparency) of atmosphere between x and x0
// assume segment x,x0 not intersecting ground
// d = distance between x and x0, mu=cos(zenith angle of [x,x0) ray at x)
float3 transmittance(float r, float mu, float d) {
	float3 result;
	float r1 = sqrt(r * r + d * d + 2.0 * r * mu * d);
	float mu1 = (r * mu + d) / r1;
	if (mu > 0.0) {
		result = min(transmittance(r, mu) / transmittance(r1, mu1), 1.0);
	}
	else {
		result = min(transmittance(r1, -mu1) / transmittance(r, -mu), 1.0);
	}
	return result;
}

float3 irradiance(sampler2D s, float r, float muS) {
	float2 uv = getIrradianceUV(r, muS);
	return tex2D(s, uv).rgb;
}

// Rayleigh phase function
float phaseFunctionR(float mu) {
	return (3.0 / (16.0 * M_PI)) * (1.0 + mu * mu);
}

// Mie phase function
float phaseFunctionM(float mu)
{
	float f = 1.0 + (mieG*mieG) - 2.0*mieG*mu;
	f = max(f, 0);
	return 1.5 * 1.0 / (4.0 * M_PI) * (1.0 - mieG*mieG) * pow(f, -3.0 / 2.0) * (1.0 + mu * mu) / (2.0 + mieG*mieG);
}

// approximated single Mie scattering (cf. approximate Cm in paragraph "Angular precision")
float3 getMie(float4 rayMie) { // rayMie.rgb=C*, rayMie.w=Cm,r
	return rayMie.rgb * rayMie.w / max(rayMie.r, 1e-4) * (betaR.r / betaR);
}