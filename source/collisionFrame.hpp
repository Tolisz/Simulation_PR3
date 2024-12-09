#pragma once 

#include <array>
#include <glm/vec3.hpp>

class collisionFrame
{
public:

	collisionFrame(float a);
	~collisionFrame() = default;

	collisionFrame(const collisionFrame&) = delete;
	collisionFrame(collisionFrame&&) = delete;
	collisionFrame& operator=(const collisionFrame&) = delete;
	collisionFrame& operator=(collisionFrame&&) = delete;

	float GetEdgeLength();
	glm::vec3 Collide(glm::vec3 P, float mu);

private: 

	void Reset(float a);

private:

	float m_a; // edge size;
	float m_a2; // half of edge size;


};