
openGL = {
	versionMajor = 4,
	versionMinor = 4
}

window = {
	name = "Different Types of Shading",
	width = 1280,
	height = 960
}


vertexShaders = {
	debug = "shaders/debugVS.glsl",
	material =  "shaders/materialVS.glsl",
	simple =  "shaders/simpleVS.glsl",
	skinning = "shaders/skinningVS.glsl",
	phong_skinning = "shaders/phong_skinningVS.glsl",
	phong = "shaders/phongVS.glsl",
	skybox = "shaders/skyboxVS.glsl"
}

fragmentShaders = {
	debug = {
		shaderDir = "shaders/debugVS.glsl",
	},
	texture_ds = {
		shaderDir = "shaders/texture_ds_FS.glsl",
		samplers = {
			diffuse = {"texture_diffuse1"},
			specular = {"texture_specular1"}
		}
	},
	texture_d = {
		shaderDir = "shaders/texture_d_FS.glsl",
		samplers = {
			diffuse = {"texture_diffuse1"}
		}
	},
	material = {
		shaderDir = "shaders/materialFS.glsl"
	},
	phong_material = {
		shaderDir = "shaders/phong_materialFS.glsl"
	},
	skybox = {
		shaderDir = "shaders/skyboxFS.glsl",
		samplers = {
			diffuse = {"texture_diffuse1"}
		}
	},	

}

