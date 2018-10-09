// Fall 2018

#pragma once

#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <vector>
#include "maze.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

private:
	void initShader();
	void initGrid();
	void initFloor();
	void initMaze();
	void initAvatar();
	void initTexture();
	void reset();
	void fillSquare(float *dataAddr, int x1, int z1, int y1, 
	int x2, int z2,int y2, int type);
	void assignAvatarPos(int type, glm::vec3 *addr);
	void moveAvatar(int type);
	void removeBlock(int type);
	// create a maze
	Maze *m;

	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	ShaderProgram m_light_shader;
	ShaderProgram m_avatar_shader;
	ShaderProgram m_maze_shader;

	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for grid colour.

	GLint P_uni2; // Uniform location for Projection matrix.
	GLint V_uni2; // Uniform location for View matrix.
	GLint M_uni2; // Uniform location for Model matrix.

	GLint P_uni3; // Uniform location for Projection matrix.
	GLint V_uni3; // Uniform location for View matrix.
	GLint M_uni3; // Uniform location for Model matrix.
	GLint L_uni; // Uniform location for Light matrix.
	GLint S_uni; // Uniform location for Light matrix.
	GLint R_uni; // Uniform location for Light matrix.
	GLint col_uni2;   // Uniform location for cube colour.
	GLint texture_uni; // Uniform location for texture image.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object

	// Fields related to maze geometry.
	GLuint m_maze_vao; // Vertex Array Object
	GLuint m_maze_vbo; // Vertex Buffer Object
	float height = 1.0f;

	// Fields related to avatar.
	int userPosX;
	int userPosY;
	GLuint m_avatar_vao; // Vertex Array Object
	GLuint m_avatar_vbo; // Vertex Buffer Object
	float avatarHeight;
	float avatarWidth;
	unsigned anime = 0;
	bool jump = false;

	// Fields related to texture image
	//std::vector <unsigned char, unsigned char> image;
	std::vector<unsigned char> image;
	unsigned texWidth, texHeight;
	unsigned textureBuffer;
	
	// Fields related to rotation
	bool dragBegin = false;
	double xpos, ypos;

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	// Shape rotation angle in radians.
	float m_shape_rotation = 0.0f;
	float m_previous_shape_rotation = 0.0f;

	// Positive float range for size of shape.
	// Clamped to be within [0.5, 5.0].
	// Shape starts at size 1.0
	float m_shape_size = 1.0f;

	float floor_colour[3] = {1.0f, 1.0f, 1.0f};
	float maze_colour[3] = {1.0f, 1.0f, 1.0f};
	float avatar_colour[3] = {1.0f, 1.0f, 1.0f};
	float floor_actual_colour[3] = {1.0f, 1.0f, 1.0f};
	float maze_actual_colour[3] = {1.0f, 1.0f, 1.0f};
	float avatar_actual_colour[3] = {1.0f, 1.0f, 1.0f};
	
	// Status for the radio button
	int floor_col;
	int maze_col;
	int avatar_col;
	// Stored previous status for the radio button
	int floor_prev_col = 0;
	int maze_prev_col = 0;
	int avatar_prev_col = 0;
};
