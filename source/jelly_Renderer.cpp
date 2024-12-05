#include "jelly_Renderer.hpp"

#include <glm/gtc/matrix_transform.hpp>

jelly_Renderer::jelly_Renderer(std::unique_ptr<bezierCubeDrawer> cubeDrawer):
	m_bCube{std::move(cubeDrawer)}
{
	SetUpFramebuffer();
	PrepareScene();
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

void jelly_Renderer::RenderScene()
{
	glClearColor(0.5f, 0.5f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	
	// SCENE BEGIN
	// ===========
	float aspect = static_cast<float>(m_sceneSize.x)/m_sceneSize.y;
    glm::mat4 viewProj[2] = {m_camera.GetViewMatrix(), m_camera.GetProjectionMatrix(aspect)};
	
	m_b_matrices.BindUBO();
	m_b_matrices.SetBufferData(0, viewProj, 2 * sizeof(glm::mat4));

	// test cube
	// ---------	
	// m_s_test.Use();
	// m_s_test.setM4fv("model", GL_FALSE, glm::identity<glm::mat4>());

	// m_o_cube.Draw();

	// Bezier Cube

	m_bCube->UpdateBuffers();
	int chosenPoint = m_bCube->GetChosenPoint();

	m_s_bCubePoints.Use();
	m_s_bCubePoints.set1i("chosenPoint", chosenPoint);
	m_bCube->DrawPoints();

	m_s_cubeSprings.Use();

	m_bCube->DrawShortSprings();
	//m_bCube->DrawLongSprings();

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
	m_s_test.AttachShaderFromFile("shaders/cube.vert", GL_VERTEX_SHADER);
	m_s_test.AttachShaderFromFile("shaders/cube.frag", GL_FRAGMENT_SHADER);
	m_s_test.Link();

	m_s_bCubePoints.AttachShaderFromFile("shaders/bezierPoints.vert", GL_VERTEX_SHADER);
	m_s_bCubePoints.AttachShaderFromFile("shaders/bezierPoints.frag", GL_FRAGMENT_SHADER);
	m_s_bCubePoints.Link();

	m_s_cubeSprings.AttachShaderFromFile("shaders/springs.vert", GL_VERTEX_SHADER);
	m_s_cubeSprings.AttachShaderFromFile("shaders/springs.frag", GL_FRAGMENT_SHADER);
	m_s_cubeSprings.Link();

	// Prepare UBO
	m_b_matrices.CreateUBO(2 * sizeof(glm::mat4));
    m_b_matrices.BindBufferBaseToBindingPoint(0);
}

void jelly_Renderer::PrepareSceneObjects()
{

}