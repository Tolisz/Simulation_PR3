#include "jelly_Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

jelly_Renderer::jelly_Renderer(
		std::unique_ptr<bezierCubeDrawer> cubeDrawer,
		std::unique_ptr<collisionFrameDrawer> colFrameDrawer):
	m_bCube{std::move(cubeDrawer)},
	m_cFrameDrawer{std::move(colFrameDrawer)}
{
	SetUpFramebuffer();
	PrepareScene();

	m_drawParams = std::make_shared<drawParameters>();
}

jelly_Renderer::~jelly_Renderer()
{
	glDeleteFramebuffers(1, &m_Framebuffer);
	glDeleteTextures(1, &m_colorTexture);
	glDeleteRenderbuffers(1, &m_Renderbuffer);
}

void jelly_Renderer::Render()
{
	glViewport(0, 0, m_sceneSize.x, m_sceneSize.y);
	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);

	RenderScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void jelly_Renderer::UpdateRenderArea(int width, int height)
{
	m_sceneSize.x = width;
	m_sceneSize.y = height;

	glBindRenderbuffer(GL_RENDERBUFFER, m_Renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_sceneSize.x, m_sceneSize.y);

	glBindTexture(GL_TEXTURE_2D, m_colorTexture);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, m_sceneSize.x, m_sceneSize.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

GLuint jelly_Renderer::GetRenderTexture()
{
	return m_colorTexture;
}

glm::uvec2 jelly_Renderer::GetRenderAreaSize()
{
	return m_sceneSize;
}

const camera& jelly_Renderer::Camera()
{
	return m_camera;
}

void jelly_Renderer::CameraRotate(float deltaX, float deltaY)
{
	m_camera.Rotate(deltaX, deltaY);
}

void jelly_Renderer::CameraMove(float deltaX, float deltaY)
{
	m_camera.Move(deltaX, deltaY, static_cast<float>(m_sceneSize.x), static_cast<float>(m_sceneSize.y));
}

void jelly_Renderer::CameraZoom(float factor)
{
	m_camera.Zoom(factor);
}

glm::mat4 jelly_Renderer::GetProjectionMatrix()
{
	float aspect = static_cast<float>(m_sceneSize.x)/m_sceneSize.y;
	return m_camera.GetProjectionMatrix(aspect);
}

glm::mat4 jelly_Renderer::GetViewMatrix()
{
	return m_camera.GetViewMatrix();
}

glm::vec3 jelly_Renderer::GetCameraPos()
{
	return m_camera.GetPosition();
}

void jelly_Renderer::SetPointAttribute(int pointIndex, int attributeIndex, bool value)
{
	m_bCube->SetPointAttribute(pointIndex, attributeIndex, value);
}

bool jelly_Renderer::IsControlFrameChoosen()
{
	return m_bCube->IsControlFrameChoosen();
}

void jelly_Renderer::SetControlFrameChoosen(bool isChoosen)
{
	m_bCube->SetControlFrameChoosen(isChoosen);
}

std::shared_ptr<drawParameters> jelly_Renderer::GetDrawParameters()
{
	return m_drawParams;
}

void jelly_Renderer::SetNewModel(model&& newModel)
{
	m_loadedModel = newModel;
}

bool jelly_Renderer::IsModelLoaded()
{
	return m_loadedModel.IsValid();
}

std::string jelly_Renderer::GetModelName()
{
	return m_loadedModel.GetName();
}	

void jelly_Renderer::RenderScene()
{
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// SCENE BEGIN
	// ===========
	float aspect = static_cast<float>(m_sceneSize.x)/m_sceneSize.y;
    glm::mat4 viewProj[2] = {m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix(aspect)};
	
    /* Update UBOs */
	m_b_matrices.BindUBO();
	m_b_matrices.SetBufferData(0, viewProj, 2 * sizeof(glm::mat4));

	m_b_lights.BindUBO();
	m_b_lights.SetBufferData(0, &m_ambientColor, sizeof(glm::vec4));
    m_b_lights.SetBufferData(sizeof(glm::vec4), m_lights.data(), 3 * sizeof(glm::vec4) * m_lights.size());

	/* Bezier Cube */
	m_bCube->UpdateBuffers();

	m_s_bCubePoints.Use();
	if (m_drawParams->bPoints)
	{
		m_s_bCubePoints.set4fv("cDefaultColor", m_drawParams->cPoints);
		m_bCube->DrawCubePoints();
	}

	/* Cube's springs */
	m_s_cubeSprings.Use();
	if (m_drawParams->bShortSrpings)
	{
		m_s_cubeSprings.set4fv("springColor", m_drawParams->cShortSprings);
		m_bCube->DrawShortSprings();
	}
	if (m_drawParams->bLongSprings)
	{
		m_s_cubeSprings.set4fv("springColor", m_drawParams->cLongSprings);
		m_bCube->DrawLongSprings();
	}

	/* Collition Cube (Frame) */
	m_s_simpleCube.Use();
	if (m_drawParams->bCollisionFrame)
	{
		glm::mat4 model = glm::scale(glm::mat4(1.0f), glm::vec3(m_cFrameDrawer->GetEdgeLength() / 2));

		glCullFace(GL_FRONT);
		m_s_simpleCube.set1b("bOverrideColor", false);
		m_s_simpleCube.setM4fv("model", GL_FALSE, model);
		m_o_collitionFrame.Draw();	
		glCullFace(GL_BACK);
	}

	/* Lights */
	glm::mat4 LightBoxModelMat = glm::scale(glm::mat4(1.0f), glm::vec3(0.05f));
	for (int i = 0; i < m_o_lights.size(); ++i)
	{
		m_s_simpleCube.setM4fv("model", GL_FALSE, glm::translate(glm::mat4(1.0f), glm::vec3(m_lights[i].m_position)) * LightBoxModelMat);
		m_s_simpleCube.set1b("bOverrideColor", true);
		m_s_simpleCube.set4fv("overrideColor", m_lights[i].m_diffuseColor);
		
		m_o_collitionFrame.Draw();
	}

	if (m_drawParams->bControlFrame)
	{
		m_s_cubeSprings.Use();
		glDisable(GL_DEPTH_TEST);
		if (IsControlFrameChoosen())
		{
			m_s_cubeSprings.set4fv("springColor", glm::vec4(1.0f));
		}
		else 
		{
			m_s_cubeSprings.set4fv("springColor", m_drawParams->cControlFrame);
		}
		m_bCube->DrawControlFrame();
		glEnable(GL_DEPTH_TEST);
	}

	/* Jelly */
	if (m_drawParams->bJelly)
	{
		m_s_bezierPatches.Use();

		m_s_bezierPatches.set1i("numberOfLights", m_lights.size());

		const material& mat = m_materials["jelly"]; 
    	m_s_bezierPatches.set3fv("material.ka", mat.ka);
    	m_s_bezierPatches.set3fv("material.kd", mat.kd);
    	m_s_bezierPatches.set3fv("material.ks", mat.ks);
    	m_s_bezierPatches.set1f("material.shininess", mat.shininess);

		m_s_bezierPatches.set3fv("cameraPos", m_camera.GetPosition());
		
		m_s_bezierPatches.set4fv("patchColor", m_drawParams->cJelly);
		m_s_bezierPatches.set1f("tessellationLevel", m_drawParams->mJellyTessellationLevel);
		
		m_bCube->DrawBezierPatches();
	}

	/* Model */
	if (m_drawParams->bModel && m_loadedModel.IsValid())
	{
		m_s_deformedModel.Use();
		m_loadedModel.Draw(m_s_deformedModel);
	}

	// =========
	// SCENE END
}

void jelly_Renderer::SetUpFramebuffer()
{
	glGenFramebuffers(1, &m_Framebuffer);
	glGenTextures(1, &m_colorTexture);
	glGenRenderbuffers(1, &m_Renderbuffer);
	
	UpdateRenderArea(1, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_Renderbuffer);

	// DEBUG ONLY !!!!!!
	assert(("Framebuffer is not complete", glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE));
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void jelly_Renderer::PrepareScene()
{
	PrepareShaders();
	PrepareSceneObjects();
}

void jelly_Renderer::PrepareShaders()
{
	// PrepareShaders
	m_s_simpleCube.AttachShaderFromFile("shaders/cube.vert", GL_VERTEX_SHADER);
	m_s_simpleCube.AttachShaderFromFile("shaders/cube.frag", GL_FRAGMENT_SHADER);
	m_s_simpleCube.Link();

	m_s_bCubePoints.AttachShaderFromFile("shaders/bezierPoints.vert", GL_VERTEX_SHADER);
	m_s_bCubePoints.AttachShaderFromFile("shaders/bezierPoints.frag", GL_FRAGMENT_SHADER);
	m_s_bCubePoints.Link();

	m_s_cubeSprings.AttachShaderFromFile("shaders/springs.vert", GL_VERTEX_SHADER);
	m_s_cubeSprings.AttachShaderFromFile("shaders/springs.frag", GL_FRAGMENT_SHADER);
	m_s_cubeSprings.Link();

	m_s_bezierPatches.AttachShaderFromFile("shaders/bezierPatch.vert", GL_VERTEX_SHADER);
	m_s_bezierPatches.AttachShaderFromFile("shaders/bezierPatch.tesc", GL_TESS_CONTROL_SHADER);
	m_s_bezierPatches.AttachShaderFromFile("shaders/bezierPatch.tese", GL_TESS_EVALUATION_SHADER);
	m_s_bezierPatches.AttachShaderFromFile("shaders/bezierPatch.frag", GL_FRAGMENT_SHADER);
	m_s_bezierPatches.Link();

	m_s_deformedModel.AttachShaderFromFile("shaders/deformedModel.vert", GL_VERTEX_SHADER);
	m_s_deformedModel.AttachShaderFromFile("shaders/deformedModel.frag", GL_FRAGMENT_SHADER);
	m_s_deformedModel.Link();

	// Prepare UBO
	m_b_matrices.CreateUBO(2 * sizeof(glm::mat4));
    m_b_matrices.BindBufferBaseToBindingPoint(0);

    m_b_lights.CreateUBO((1 + 3 * m_maxLightsNum) * sizeof(glm::vec4));
    m_b_lights.BindBufferBaseToBindingPoint(1);
}

void jelly_Renderer::PrepareSceneObjects()
{
	material m;
	m_materials.insert(std::make_pair("jelly", m));

	m_lights[0].m_position = glm::vec4(-3.0f,  3.0f,  3.0f, 0.0f);
	m_lights[1].m_position = glm::vec4( 3.0f, -3.0f, -3.0f, 0.0f);
}