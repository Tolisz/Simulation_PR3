#version 450 core 

// =========================================
in FS_IN
{
    flat int pointIndex;
    flat int pointAttribute;
} i;

layout(location = 0) out vec4 oColor;
// =========================================

uniform int chosenPoint;

const vec4 cHighlightedPoint = vec4(0.0f, 1.0f, 0.0f, 1.0f);
const vec4 cChosenPoint = vec4(1.0f);

void main()
{
	// Highlighted 
	if (((i.pointAttribute >> 0) & 1) == 1)
	{
		oColor = cHighlightedPoint;
	}
	// Can be potentialy chosen
	else if (((i.pointAttribute >> 1) & 1) == 1)
	{
		oColor = cChosenPoint;
	}
	// Is chosen
	else if (((i.pointAttribute >> 2) & 1) == 1)
	{
		oColor = cChosenPoint;
	}
	// default color
	else 
	{
		oColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	}


}