#version 400
 

uniform vec4 specular;
uniform vec4 diffuse;
uniform vec4 ambient;
uniform float roughness;
uniform float refraction;
uniform vec3 l_dir;    // camera space
 
in Data {
    vec3 normal;
    vec4 eye;
} DataIn;
 
out vec4 colorOut;

void main() {
    vec4 finalSpecular = vec4(0.0);
    vec4 finalColor = vec4(0.0);
    vec3 n = normalize(DataIn.normal);
    vec3 e = normalize(vec3(DataIn.eye));

//compute diffuse
    float diffuseIntensity = max(dot(n,l_dir), 0.0);
    // if the vertex is lit compute the specular color
    if (diffuseIntensity > 0.0) {

        vec3 halfVec = normalize(l_dir + e);
    // compute aliases and intermediary values
        float NdotL = clamp(dot(n,l_dir), 0.0, 1.0);
        float NdotH = clamp(dot(n,halfVec), 0.0, 1.0);
        float NdotV = clamp(dot(n,e), 0.0, 1.0);
        float VdotH = clamp(dot(e,halfVec), 0.0, 1.0);
        float r_sq = roughness * roughness;
    //evaluate geometric term
        float geo_numerator = 2.0 * NdotH;
        float geo_denominator = VdotH;

        float geo_b = (geo_numerator * NdotV) / geo_denominator;
        float geo_c = (geo_numerator * NdotL) / geo_denominator;
        float geo = min(1.0, min(geo_b, geo_c));

    //evaluate roughness term using beckmann
        float finalRoughness;
        float roughness_a = 1.0 / ( 4.0 * r_sq * pow( NdotH, 4 ) );
        float roughness_b = NdotH * NdotH - 1.0;
        float roughness_c = r_sq * NdotH * NdotH;

        finalRoughness = roughness_a * exp( roughness_b / roughness_c ); 
    //evaluate fresnel value
        float fresnel = pow( 1.0 - VdotH, 5.0);
        fresnel *= ( 1.0 - refraction );
        fresnel += refraction;


    //compute the specular term 

        float Rs_numerator   = ( fresnel * geo * finalRoughness );
        float Rs_denominator  = NdotV * NdotL;
        vec4 Rs             = vec4(vec3(Rs_numerator/ Rs_denominator), 1.0);
        finalSpecular = specular * Rs;
        finalColor = max(max(0.0, NdotL) * (finalSpecular + diffuse * diffuseIntensity), ambient);
}
    colorOut = finalColor;
}