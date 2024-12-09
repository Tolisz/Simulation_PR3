#pragma once

#include "collisionFrame.hpp"
#include <memory>
#include <glad/glad.h>

class collisionFrameDrawer
{
public:
	collisionFrameDrawer(std::shared_ptr<collisionFrame> frame);
	~collisionFrameDrawer() = default;

	collisionFrameDrawer(const collisionFrameDrawer&) = delete;
	collisionFrameDrawer(collisionFrameDrawer&&) = delete;
	collisionFrameDrawer& operator=(const collisionFrameDrawer&) = delete;
	collisionFrameDrawer& operator=(collisionFrameDrawer&&) = delete;

	float GetEdgeLength();

private:

	void InitGL();
	void DeInitGL();

private:

	std::shared_ptr<collisionFrame> m_frame;
};
