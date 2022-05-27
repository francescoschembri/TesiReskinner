#pragma once

#include <glad/glad.h>
#include <iostream>

class FrameBuffer
{
public:
	FrameBuffer(int width, int height);
	void DeleteBuffer();
	void Bind();
	void Unbind();
	void Resize(int width, int height);
	unsigned int GetTexture();

private:
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;
	//screen size
	int width, height;
};