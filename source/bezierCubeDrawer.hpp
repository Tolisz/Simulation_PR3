#pragma once

#include <glad/glad.h>
#include <memory>
#include "bezierCube.hpp"

class bezierCubeDrawer
{
public:

	bezierCubeDrawer(std::shared_ptr<bezierCube> cube);
	~bezierCubeDrawer();

	bezierCubeDrawer(const bezierCubeDrawer&) = delete;
	bezierCubeDrawer(bezierCubeDrawer&&) = delete;
	bezierCubeDrawer& operator=(const bezierCubeDrawer&) = delete;
	bezierCubeDrawer& operator=(bezierCubeDrawer&&) = delete;

	void UpdateBuffers();
	void DrawCubePoints();
	void DrawShortSprings();
	void DrawLongSprings();
	void DrawControlFrame();
	void DrawBezierPatches();

	int GetChosenPoint();
	void SetPointAttribute(int pointIndex, int attributeIndex, bool value);

	bool IsControlFrameChoosen();
	void SetControlFrameChoosen(bool isChoosen);
private:

	void InitGL();
	void InitGL_MainCube();
	void InitGL_ControlCube();
	void DeInitGL();

	void GetCubeSpringIndices(
		std::vector<std::pair<unsigned int, unsigned int>>& shortSprings, 
		std::vector<std::pair<unsigned int, unsigned int>>& longSprings);

	std::vector<std::pair<unsigned int, unsigned int>> GetFrameSpringIndices();
	std::vector<unsigned int> GetBezierPatchesIndices();
private:

	std::shared_ptr<bezierCube> m_cube;
	
	// 000X - is hilighted
	// 00X0 - is potentialy choosen
	// 0X00 - is choosen
	std::vector<GLint> m_pointsAttributes;

	// Indices buffers
	int m_shortSpringsNum;
	int m_longSpringsNum;

	bool b_ControlFrameChoosen = false;

	// OpenGL

	/* Main Cube */
	GLuint m_VAO_points;
	GLuint m_VAO_shortSprings;
	GLuint m_VAO_longSprings;
	GLuint m_VAO_patches;
	
	GLuint m_cubePointsBuffer;
	GLuint m_cubePointsAtribBuffer;

	GLuint m_EBO_shortSprings;
	GLuint m_EBO_longSprings;
	GLuint m_EBO_patches;

	/* Control Frame */
	GLuint m_VAO_framePoints;
	GLuint m_framePointsBuffer;
	GLuint m_EBO_frameSprings;
};