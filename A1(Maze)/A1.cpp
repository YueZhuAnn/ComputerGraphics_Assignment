// Fall 2018

#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
#include <vector>

#include <sys/types.h>
#include <unistd.h>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <lodepng/lodepng.h>

using namespace glm;
using namespace std;
using namespace lodepng;

static const size_t DIM = 16;

const float PI = 3.14159265f;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: floor_col( 0 ), maze_col ( 0 ), avatar_col ( 0 )
{
	avatarHeight = 0.3f;
	avatarWidth = 0.2f;
	userPosX = 0;
	userPosY = 0;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{
	free(m);
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Initialize random number generator
	int rseed=getpid();
	srandom(rseed);
	// Print random number seed in case we want to rerun with
	// same random numbers
	cout << "Random number seed = " << rseed << endl;
		
	m = new Maze(DIM);

	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	initShader();
	initTexture();

	initGrid();
	initAvatar();

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, 2.*float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 30.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initShader()
{
	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	// Build the shader
	m_avatar_shader.generateProgramObject();
	m_avatar_shader.attachVertexShader(
		getAssetFilePath( "AvatarVertexShader.vs" ).c_str() );
	m_avatar_shader.attachFragmentShader(
		getAssetFilePath( "AvatarFragmentShader.fs" ).c_str() );
	m_avatar_shader.link();

	// Set up the uniforms
	P_uni2 = m_avatar_shader.getUniformLocation( "P" );
	V_uni2 = m_avatar_shader.getUniformLocation( "V" );
	M_uni2 = m_avatar_shader.getUniformLocation( "M" );

	// Build the shader
	m_maze_shader.generateProgramObject();
	m_maze_shader.attachVertexShader(
		getAssetFilePath( "MazeVertexShader.vs" ).c_str() );
	m_maze_shader.attachFragmentShader(
		getAssetFilePath( "MazeFragmentShader.fs" ).c_str() );
	m_maze_shader.link();

	// Set up the uniforms
	P_uni3 = m_maze_shader.getUniformLocation( "P" );
	V_uni3 = m_maze_shader.getUniformLocation( "V" );
	M_uni3 = m_maze_shader.getUniformLocation( "M" );
	L_uni = m_maze_shader.getUniformLocation( "L" );
	S_uni = m_maze_shader.getUniformLocation( "scale" );
	R_uni = m_maze_shader.getUniformLocation( "R" );

	texture_uni = m_maze_shader.getUniformLocation( "textureImg" );
	col_uni2 = m_maze_shader.getUniformLocation( "colour" );
}


void A1::initTexture()
{
	unsigned error = lodepng::decode(image, texWidth, texHeight, "texture.png");
	// Read the image
	if(error != 0)
	{
		cout << "error " << error << ": " << lodepng_error_text(error) << endl;
		return;
	}

  	// Enable the texture for OpenGL.
    glGenTextures(1, &textureBuffer);
    glBindTexture(GL_TEXTURE_2D, textureBuffer); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object

    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	CHECK_GL_ERRORS;
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

void A1::fillSquare(float *dataAddr, int x1, int z1, int y1, 
int x2, int z2,int y2, int type)
{
	vec3 *p1 = (vec3 *) (dataAddr);
	vec3 *p2 = (vec3 *) (dataAddr+8);
	vec3 *p3 = (vec3 *) (dataAddr+8*2);
	vec3 *p4 = (vec3 *) (dataAddr+8*3);
	vec3 *p5 = (vec3 *) (dataAddr+8*4);
	vec3 *p6 = (vec3 *) (dataAddr+8*5);

	vec2 *c1 = (vec2 *) (dataAddr+3);
	vec2 *c2 = (vec2 *) (dataAddr+8+3);
	vec2 *c3 = (vec2 *) (dataAddr+8*2+3);
	vec2 *c4 = (vec2 *) (dataAddr+8*3+3);
	vec2 *c5 = (vec2 *) (dataAddr+8*4+3);
	vec2 *c6 = (vec2 *) (dataAddr+8*5+3);

	vec3 *n1 = (vec3 *) (dataAddr+5);
	vec3 *n2 = (vec3 *) (dataAddr+8+5);
	vec3 *n3 = (vec3 *) (dataAddr+8*2+5);
	vec3 *n4 = (vec3 *) (dataAddr+8*3+5);
	vec3 *n5 = (vec3 *) (dataAddr+8*4+5);
	vec3 *n6 = (vec3 *) (dataAddr+8*5+5);

	p1[0] = p6[0] = vec3(x1, z1, y1);
	p3[0] = p4[0] = vec3(x2, z2, y2);

	c1[0] = c6[0] =  vec2(1.0f, height/5.0f);
	c2[0] = vec2(1.0f, 0.0f);
	c3[0] = c4[0] = vec2(0.0f, 0.0f);
	c5[0] = vec2(0.0f, height/5.0f);

	switch(type){
		case 1:
			p2[0] = vec3((float)x1, (float)z2,(float)y1);
			p5[0] = vec3((float)x1, (float)z1,(float)y2);
			break;
		case 2:
			p2[0] = vec3((float)x2, (float)z1,(float)y1);
			p5[0] = vec3((float)x1, (float)z1,(float)y2);
			c1[0] = c6[0] =  vec2(1.0f, 0.2f);
			c5[0] = vec2(0.0f, 0.2);
			break;
		case 3:
			p2[0] = vec3((float)x1, (float)z2,(float)y1);
			p5[0] = vec3((float)x2, (float)z1,(float)y1);
			break;
	}
	n1[0]=n2[0]=n3[0]=n4[0]=n5[0]=n6[0]=cross(p2[0]-p1[0], p3[0]-p1[0]);
}

void A1::initMaze()
{
	int blockCount = m->getTotalBlock();
	size_t sz = 2*3*8*blockCount*5;
	
	float mazeVecs[sz];
	int index = 0;
	size_t dim = m->getDim();
	for (int i=0; i<dim; i++) {
		for (int j=0; j<dim; j++) { 
			if ( m->getValue(i,j)==1 ) {
				fillSquare(mazeVecs+index*6*8*5, i,height, j, i+1,height,j+1,2);
				fillSquare(mazeVecs+index*6*8*5+6*1*8, i,height,j,i+1,0,j,3);
				fillSquare(mazeVecs+index*6*8*5+6*2*8, i,height,j,i,0,j+1,1);
				fillSquare(mazeVecs+index*6*8*5+6*3*8, i,height,j+1,i+1,0,j+1,3);
				fillSquare(mazeVecs+index*6*8*5+6*4*8, i+1,height,j+1,i+1,0,j,1);
				index++;
			}
		}

	}

	// clear the previous buffer
	glDeleteVertexArrays( 1, &m_maze_vao );
	glDeleteBuffers( 1, &m_maze_vbo );
	
	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_maze_vao );
	glBindVertexArray( m_maze_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_maze_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_maze_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(mazeVecs),
		mazeVecs, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_maze_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), nullptr );

	GLint texAttrib = m_maze_shader.getAttribLocation( "tex" );
	glEnableVertexAttribArray( texAttrib );
	glVertexAttribPointer( texAttrib, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));

	GLint normalAttrib = m_maze_shader.getAttribLocation( "normal" );
	glEnableVertexAttribArray( normalAttrib );
	glVertexAttribPointer( normalAttrib, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5*sizeof(float)));

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.

	CHECK_GL_ERRORS;
}

void A1::assignAvatarPos(int type, vec3 *addr)
{
	switch (type)
	{
		case 0:
			// for position 
			addr[0] = {(float)userPosX+0.5f, avatarHeight,  (float)userPosY+0.5f-avatarWidth};
			// for color
			addr[1] = {avatar_actual_colour[0], 
						avatar_actual_colour[1],
						avatar_actual_colour[2]};
			break;
		case 1:
			// for position 
			addr[0] = {(float)userPosX+0.5f-avatarWidth, avatarHeight, (float)userPosY+0.5f};
			// for color
			addr[1] = {avatar_actual_colour[0], 
						avatar_actual_colour[1],
						avatar_actual_colour[2]};
			break;
		case 2:
			// for position 
			addr[0] = {(float)userPosX+0.5f, avatarHeight, (float)userPosY+0.5f+avatarWidth};
			// for color
			addr[1] = {avatar_actual_colour[0], 
						avatar_actual_colour[1],
						avatar_actual_colour[2]};
			break;
		case 3:
			// for position 
			addr[0] = {(float)userPosX+0.5f+avatarWidth, avatarHeight, (float)userPosY+0.5f};
			// for color
			addr[1] = {avatar_actual_colour[0], 
						avatar_actual_colour[1],
						avatar_actual_colour[2]};
			break;
		case 4:
			// for position 
			addr[0] = {(float)userPosX+0.5f, 0,  (float)userPosY+0.5f};
			// for color
			addr[1] = {1.0f-avatar_actual_colour[0], 
						1.0f-avatar_actual_colour[1],
						1.0f-avatar_actual_colour[2]};
		case 5:
			// for position 
			addr[0] = {(float)userPosX+0.5f, 2*avatarHeight, (float)userPosY+0.5f};
			// for color
			addr[1] = {1.0f-avatar_actual_colour[0], 
						1.0f-avatar_actual_colour[1],
						1.0f-avatar_actual_colour[2]};
	}
}

void A1::initAvatar()
{
	size_t sz = 2*4*3*2*2;
	vec3 vecs[sz];

	for(int i = 0; i < 4; i++)
	{
		assignAvatarPos(4, vecs+i*2*3);
		assignAvatarPos(5, vecs+i*2*3+2*4*3);
		switch (i)
		{
			case 0:
				assignAvatarPos(0, vecs+i*3*2+2);
				assignAvatarPos(0, vecs+i*3*2+2+2*4*3);
				assignAvatarPos(1, vecs+i*3*2+4);
				assignAvatarPos(1, vecs+i*3*2+4+2*4*3);
				break;
			case 1:
				assignAvatarPos(1, vecs+i*3*2+2);
				assignAvatarPos(1, vecs+i*3*2+2+2*4*3);
				assignAvatarPos(2, vecs+i*3*2+4);
				assignAvatarPos(2, vecs+i*3*2+4+2*4*3);
				break;
			case 2:
				assignAvatarPos(2, vecs+i*3*2+2);
				assignAvatarPos(2, vecs+i*3*2+2+2*4*3);
				assignAvatarPos(3, vecs+i*3*2+4);
				assignAvatarPos(3, vecs+i*3*2+4+2*4*3);
				break;
			case 3:
				assignAvatarPos(3, vecs+i*3*2+2);
				assignAvatarPos(3, vecs+i*3*2+2+2*4*3);
				assignAvatarPos(0, vecs+i*3*2+4);
				assignAvatarPos(0, vecs+i*3*2+4+2*4*3);
				break;
		}
	}
	// For the anime
	for(int i = 0; i < 24*2; i++)
	{
		vecs[i+24*2] = vecs[i];
		if(i%2 == 0)
		{
			// only for positions
			vecs[i+24*2][1] += 0.3f;
		}
	}

	// clear the previous buffer
	glDeleteVertexArrays( 1, &m_avatar_vao );
	glDeleteBuffers( 1, &m_avatar_vao );

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_avatar_vao );
	glBindVertexArray( m_avatar_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_avatar_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_avatar_vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(vecs),
		vecs, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_avatar_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), nullptr );

	GLint colorAttrib = m_avatar_shader.getAttribLocation( "pointColor" );
	glEnableVertexAttribArray( colorAttrib );
	glVertexAttribPointer( colorAttrib, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), (void*)(sizeof(vec3)));

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.

	CHECK_GL_ERRORS;
}

void A1::reset()
{
	m->reset();
	height = 1.0f;
	initMaze();
	userPosX = 0;
	userPosY = 0;
	initAvatar();
	
	m_shape_rotation = 0.0f;
	m_previous_shape_rotation = 0.0f;
	m_shape_size = 1.0f;

	for(int i = 0; i < 3; i++)
	{
		floor_colour[i] = 1.0f;
		floor_actual_colour[i] = 1.0f;
		maze_colour[i] = 1.0f;
		maze_actual_colour[i] = 1.0f;
		avatar_colour[i] = 1.0f;
		avatar_actual_colour[i] = 1.0f;
	}
	
	floor_col = 0;
	maze_col = 0;
	avatar_col = 0;
	
	floor_prev_col = 0;
	maze_prev_col = 0;
	avatar_prev_col = 0;
}
//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Menu", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application (Q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}

		if( ImGui::Button( "Reset Application (R)" ) ) {
			reset();
		}

		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

		ImGui::PushID( 0 );
		ImGui::ColorEdit3( "##Colour", floor_colour );
		ImGui::SameLine();
		if( ImGui::RadioButton( "Floor", &floor_col, 1 ) ) {
			// Select this colour.
			maze_col = 0;
			avatar_col = 0;
			if(floor_prev_col == 0)
			{
				for(int i = 0; i < 3; i++)
				{
					floor_colour[i] = floor_actual_colour[i];
				}
			}
		}
		floor_prev_col = floor_col;
		if(floor_col)
		{
			for(int i = 0; i < 3; i++)
				{
					floor_actual_colour[i] = floor_colour[i];
				}
		}
		ImGui::PopID();

		ImGui::PushID( 1 );
		ImGui::ColorEdit3( "##Colour", maze_colour );
		ImGui::SameLine();
		if( ImGui::RadioButton( "Maze", &maze_col, 1 ) ) {
			// Select this colour.
			floor_col = 0;
			avatar_col = 0;
			if(maze_prev_col == 0)
			{
				for(int i = 0; i < 3; i++)
				{
					maze_colour[i] = maze_actual_colour[i];
				}
			}
		}
		maze_prev_col = maze_col;
		if(maze_col)
		{
			for(int i = 0; i < 3; i++)
				{
					maze_actual_colour[i] = maze_colour[i];
				}
		}
		ImGui::PopID();

		ImGui::PushID( 2 );
		ImGui::ColorEdit3( "##Colour", avatar_colour );
		ImGui::SameLine();
		if( ImGui::RadioButton( "Avatar", &avatar_col, 1 ) ) {
			// Select this colour.
			floor_col = 0;
			maze_col = 0;
			if(avatar_prev_col == 0)
			{
				for(int i = 0; i < 3; i++)
				{
					avatar_colour[i] = avatar_actual_colour[i];
				}
			}
		}
		avatar_prev_col = avatar_col;
		if(avatar_col)
		{
			for(int i = 0; i < 3; i++)
			{
				avatar_actual_colour[i] = avatar_colour[i];
			}
			initAvatar();
		}
		ImGui::PopID();

/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/

		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
	mat4 L; // for the light, no rotation
	mat4 R; // for the light, only rotation
	vec3 z_axis(0.0f,1.0f,0.0f);

	W *= glm::rotate(mat4(), m_shape_rotation, z_axis);
	W *= glm::scale(mat4(), vec3(m_shape_size));
	W = glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	L *= glm::scale(mat4(), vec3(m_shape_size));
	L = glm::translate( L, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );

	R *= glm::rotate(mat4(), m_shape_rotation, z_axis);
	
	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, floor_actual_colour[0], 
		floor_actual_colour[1], floor_actual_colour[2] );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );	

		// Highlight the active square.
	m_shader.disable();

	m_maze_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni3, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni3, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni3, 1, GL_FALSE, value_ptr( W ) );
		glUniformMatrix4fv( L_uni, 1, GL_FALSE, value_ptr( L ) );
		glUniformMatrix4fv( R_uni, 1, GL_FALSE, value_ptr( R ) );
		glUniform1f( S_uni, m_shape_size);

		glUniform3f( col_uni2, maze_actual_colour[0], 
			maze_actual_colour[1], maze_actual_colour[2] );


		if(m->getStatus())
		{
			int totalBlock = m->getTotalBlock();
			
			// Draw the maze
			glBindVertexArray( m_maze_vao );
			glDrawArrays( GL_TRIANGLES, 0, 6*5*totalBlock);
		}
	m_maze_shader.disable();

	m_avatar_shader.enable();

		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni2, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni2, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni2, 1, GL_FALSE, value_ptr( W ) );
		// Draw the avatar
		
		anime++;
		if(anime == 12)
		{
			jump = !jump;
			anime = 0;
		}
		glBindVertexArray( m_avatar_vao );
		if(jump)
		{
			glDrawArrays( GL_TRIANGLES, 0, 3*4*2);
		}
		else
		{
			glDrawArrays( GL_TRIANGLES, 3*4*2, 3*4*2);
		}

	m_avatar_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if(dragBegin)
		{
			m_shape_rotation = m_previous_shape_rotation+(xPos-xpos)/800.0f *0.5*PI;
			while(m_shape_rotation > (2*PI))
			{
				m_shape_rotation -= 2*PI;
			}
			while(m_shape_rotation < 0)
			{
				m_shape_rotation += 2*PI;
			}
		}
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		if(button == GLFW_MOUSE_BUTTON_LEFT)
		{
			if(actions == 1 && !dragBegin)
			{
				// mouse clicked
				dragBegin = !dragBegin;
				glfwGetCursorPos(m_window, &xpos, &ypos);
			}
			else if(actions == 0 && dragBegin)
			{
				// mouse released
				// update m_previous_shape_rotation.
				m_previous_shape_rotation = m_shape_rotation;
				dragBegin = !dragBegin;
			}
		}
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);

	m_shape_size -= yOffSet;
	if(m_shape_size < 0.5f)
	{
		m_shape_size = 0.5f;
	}
	if(m_shape_size > 5.0f)
	{
		m_shape_size = 5.0f;
	}
	// Zoom in or out.

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	int offset = 0;
	if(0.25f*PI <= m_shape_rotation && m_shape_rotation < 0.75f *PI)
	{
		offset = 1;
	}
	else if(0.75f *PI <= m_shape_rotation && m_shape_rotation < 1.25f*PI)
	{
		offset = 2;
	}
	else if(1.25f*PI <= m_shape_rotation && m_shape_rotation < 1.75f*PI)
	{
		offset = 3;
	}
	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_Q) {

			glfwSetWindowShouldClose(m_window, GL_TRUE);

			// TODO -- quit the application

			eventHandled = true;
		}
		else if (key == GLFW_KEY_UP)
		{
			if (GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) ||
			GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) )
			{
				// TODO -- remove up

				removeBlock(0+offset);
				initMaze();
			}
			moveAvatar(0+offset);

			// TODO -- move up

			eventHandled = true;
		}
		else if (key == GLFW_KEY_RIGHT)
		{
			if (GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) ||
			GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) )
			{
				// TODO -- remove right
				
				removeBlock(1+offset);
				initMaze();
			}
			moveAvatar(1+offset);

			// TODO -- move right

			eventHandled = true;
		}
		else if (key == GLFW_KEY_DOWN)
		{
			if (GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) ||
			GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) )
			{
				// TODO -- remove down
				
				removeBlock(2+offset);
				initMaze();
			}

			moveAvatar(2+offset);

			// TODO -- move down

			eventHandled = true;
		}
		else if (key == GLFW_KEY_LEFT)
		{
			if (GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_LEFT_SHIFT) ||
			GLFW_PRESS == glfwGetKey(m_window, GLFW_KEY_RIGHT_SHIFT) )
			{
				// TODO -- remove left
				
				removeBlock(3+offset);
				initMaze();
			}

			moveAvatar(3+offset);

			// TODO -- move left

			eventHandled = true;
		}
		else if (key == GLFW_KEY_D) {
			m->digMaze();
			// m->printMaze();
			userPosX = 0;
			userPosY = m->getStartPos();
			initMaze();
			initAvatar();

			// TODO -- dig the maze

			eventHandled = true;
		}
		else if (key == GLFW_KEY_SPACE)
		{
			if(m->getStatus())
			{
				if (height <= 4.0f){
					height += 1.0f;
					initMaze();
				}
			}

			// TODO -- increase block

			eventHandled = true;
		}
		else if (key == GLFW_KEY_BACKSPACE)
		{
			if(m->getStatus())
			{
				if(height >= 1.0f){
					height -= 1.0f;
					initMaze();
				}
			}

			// TODO -- decrease block

			eventHandled = true;
		}
		else if (key == GLFW_KEY_R)
		{
			reset();

			// TO DO -- reset the status

			eventHandled = true;
		}
	}

	return eventHandled;
}

void A1::removeBlock(int type)
{
	int removeType = type %4;
	switch(removeType)
	{
		case 0:
			if (userPosY-1 >= 0)
			{
				m->setValue(userPosX, userPosY-1,0);
			}
			break;
		case 1:
			if (userPosX+1 < DIM)
			{
				m->setValue(userPosX+1, userPosY,0);
			}
			break;
		case 2:
			if (userPosY+1 < DIM)
			{
				m->setValue(userPosX, userPosY+1,0);
			}
			break;
		case 3:
			if (userPosX-1 >= 0)
			{
				m->setValue(userPosX-1, userPosY,0);
			}
			break;
	}
	// initMaze();
}

void A1::moveAvatar(int type)
{
	int moveType = type%4;
	switch(moveType)
	{
		case 0:
			if ((userPosY-1 >= 0) && 
			(0 == m->getValue(userPosX, userPosY-1)))
			{
				userPosY -= 1;
				initAvatar();
			}
			break;
		case 1:
			if ((userPosX+1 < DIM) && 
			(0 == m->getValue(userPosX+1, userPosY)))
			{
				userPosX += 1;
				initAvatar();
			}
			break;
		case 2:
			if ((userPosY+1 < DIM) && 
			(0 == m->getValue(userPosX, userPosY+1)))
			{
				userPosY += 1;
				initAvatar();
			}
			break;
		case 3:
			if ((userPosX-1 >= 0) && 
			(0 == m->getValue(userPosX-1, userPosY)))
			{
				userPosX -= 1;
				initAvatar();
			}
			break;
	}
}
