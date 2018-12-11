// Fall 2018

#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include <math.h> 

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;
const vec3 xAxis = {1,0,0};
const vec3 yAxis = {0,1,0};
//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{
	m_operationStatus.resize(6, false);
	m_operationStatus[OperationType::ZBuffer] = true;
	m_trackballNode = new GeometryNode("sphere", "trackball", false);
	GeometryNode * geometryNode = static_cast<GeometryNode *>(m_trackballNode);
	geometryNode->translate(vec3(0.0, 0.0, -4.0));
	m_zTranslate = translate( m_zTranslate, vec3(0, 0,  -5.0));
	vec3 kd = {1.0, 1.0, 1.0};
	vec3 ks = {0.1, 0.1, 0.1};
	float shininess = 10.0f;
	geometryNode->material.kd = kd;
	geometryNode->material.ks = ks;
	geometryNode->material.shininess = shininess;
}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{
	delete m_trackballNode;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.85, 0.85, 0.85, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("head.obj"),
			getAssetFilePath("body.obj"),
			getAssetFilePath("body2.obj"),
			getAssetFilePath("arm.obj"),
			getAssetFilePath("foot.obj"),
			getAssetFilePath("head.obj"),
			getAssetFilePath("head2.obj"),
			getAssetFilePath("nose.obj"),
			getAssetFilePath("eye.obj"),
			getAssetFilePath("ear.obj"),
			getAssetFilePath("ear2.obj"),
			getAssetFilePath("tail1.obj"),
			getAssetFilePath("tail2.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could Not Open " << filename << std::endl;
	}
	initColorMap(&*m_rootNode);
	initAnime(&*m_rootNode);
	originStatus.clear();
	saveJointStatus(originStatus, &*m_rootNode);
	undoStack.push(originStatus);
}

//----------------------------------------------------------------------------------------
void A3::initColorMap(SceneNode *node)
{
	if (!node) return;

	if(node->m_nodeType == NodeType::GeometryNode)
	{
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);
		if(geometryNode->ableSelect)
		{
			const Material material = geometryNode->material;
			m_colorMap[node->m_nodeId] = node;
		}
	} 

	for (SceneNode * childNode : node->children) {
		initColorMap(childNode);
	}
}

void A3::initAnime(SceneNode *node)
{
	if (!node) return;

	if(node->m_nodeType == NodeType::JointNode)
	{
		JointNode * jointNode = static_cast<JointNode *>(node);
		string sChildName = jointNode->children.front()->m_name;
		string sMap = sChildName.substr(0,1);
		if(sMap == "R" || sMap == "L" ||
		sMap == "T" || sMap == "H")
		{
			m_animeNodes.push_back(make_pair(0, jointNode));
		}
	} 
	for (SceneNode * childNode : node->children) {
		initAnime(childNode);
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
	// World-space position
	m_light.position = vec3(-2.0f, 5.0f, 0.5f);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms(bool model) {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

		location = m_shader.getUniformLocation("picking");
		glUniform1i( location, m_picking? 1:0 );

		//-- Set WorldView matrix;
		location = m_shader.getUniformLocation("WorldView");
		mat4 worldView;
		if(model) worldView = translate( worldView, vec3(m_xTranslation, m_yTranslation,  m_zTranslation));
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(worldView));
		CHECK_GL_ERRORS;

		if(!m_picking)
		//-- Set LightSource uniform for the scene:
		{
			location = m_shader.getUniformLocation("light.position");
			glUniform3fv(location, 1, value_ptr(m_light.position));
			location = m_shader.getUniformLocation("light.rgbIntensity");
			glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
			CHECK_GL_ERRORS;
		}

		if(!m_picking)
		//-- Set background light ambient intensity
		{
			location = m_shader.getUniformLocation("ambientIntensity");
			vec3 ambientIntensity(0.05f);
			glUniform3fv(location, 1, value_ptr(ambientIntensity));
			CHECK_GL_ERRORS;
		}
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// play animation
	if(m_anime)
	{
		if(m_startTime <= m_animeLength*m_loopTime*m_animeFrameRate)
		{
			if(m_startTime % m_animeFrameRate == 0)
			{
				playAnime();
			}
			m_startTime++;
		}
		else
		{
			endAnime();
		}
	}
	uploadCommonSceneUniforms();
}


void A3::showSelectedNodes(SceneNode *node)
{
	if (!node) return;

	if(node->m_nodeType == NodeType::GeometryNode)
	{
		const GeometryNode * geometryNode = static_cast<const GeometryNode *>(node);
		if (geometryNode->isSelected)
		{
			ImGui::Text( "Selected: %s", geometryNode->m_name.c_str() );
		}
	} 

	for (SceneNode * childNode : node->children) {
		showSelectedNodes(childNode);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}
	
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}
	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("MenuBar", &showDebugWindow, ImVec2(100,100), opacity, ImGuiWindowFlags_MenuBar);
	if (ImGui::BeginMenu("Application"))
    {
		if (ImGui::MenuItem("Anime (E)"))
		{
			anime();
		}
        if (ImGui::MenuItem("Reset Position (I)"))
		{
			resetPosition();
		}
		if (ImGui::MenuItem("Reset Orientation (O)"))
		{
			resetOrientation();
		}
		if (ImGui::MenuItem("Reset Joints (S)"))
		{
			resetJoints();
		}
		if (ImGui::MenuItem("Reset All (A)"))
		{
			resetAll();
		}
		if (ImGui::MenuItem("Quit (Q)"))
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
        ImGui::EndMenu();
    }
	if (ImGui::BeginMenu("Edit"))
    {
        if (ImGui::MenuItem("Undo (U)"))
		{
			undo();
		}
		if (ImGui::MenuItem("Redo (R)"))
		{
			redo();
		}
        ImGui::EndMenu();
    }
	
	bool Circle 			= m_operationStatus[OperationType::Circle];
	bool ZBuffer			= m_operationStatus[OperationType::ZBuffer];
	bool BackfaceCulling	= m_operationStatus[OperationType::BackfaceCulling];
	bool FrontfaceCulling 	= m_operationStatus[OperationType::FrontfaceCulling];
	bool Position 			= m_operationStatus[OperationType::Position];

	if (ImGui::BeginMenu("Option"))
	{
        if (ImGui::Checkbox( "Circle (C)", &Circle))
		{
			updateOperationStatus(OperationType::Circle);
		}
		if (ImGui::Checkbox( "Z-buffer (Z)", &ZBuffer))
		{
			updateOperationStatus(OperationType::ZBuffer);
		}
		if (ImGui::Checkbox( "Backface culling (B)", &BackfaceCulling))
		{
			updateOperationStatus(OperationType::BackfaceCulling);
		}
		if (ImGui::Checkbox( "Frontface culling (F)", &FrontfaceCulling))
		{
			updateOperationStatus(OperationType::FrontfaceCulling);
		}
		if (ImGui::RadioButton( "Position/Orientation (P)",  &m_operationStatus[OperationType::Position],1))
		{
			m_operationStatus[OperationType::Joints] = false;
		}
		if (ImGui::RadioButton( "Joints (J)", &m_operationStatus[OperationType::Joints],1))
		{
			m_operationStatus[OperationType::Position] = false;
		}
        ImGui::EndMenu();
    }
	showSelectedNodes(&*m_rootNode);
	ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );
	ImGui::End();
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * node.trans;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;

		//-- Set NormMatrix:
		location = shader.getUniformLocation("NormalMatrix");
		mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
		glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
		CHECK_GL_ERRORS;


		//-- Set Material values:
		location = shader.getUniformLocation("material.kf");
		vec3 kf = node.material.kf;
		glUniform3fv(location, 1, value_ptr(kf));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.kd");
		vec3 kd = node.material.kd;
		glUniform3fv(location, 1, value_ptr(kd));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.ks");
		vec3 ks = node.material.ks;
		glUniform3fv(location, 1, value_ptr(ks));
		CHECK_GL_ERRORS;
		location = shader.getUniformLocation("material.shininess");
		glUniform1f(location, node.material.shininess);
		CHECK_GL_ERRORS;

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {
	if(m_operationStatus[OperationType::ZBuffer]){
		glEnable( GL_DEPTH_TEST );
	}
	if(m_operationStatus[OperationType::FrontfaceCulling] &&
		m_operationStatus[OperationType::BackfaceCulling])
	{
		glEnable( GL_CULL_FACE );
		glCullFace(GL_FRONT_AND_BACK);
	} else if(m_operationStatus[OperationType::FrontfaceCulling])
	{
		glEnable( GL_CULL_FACE );
		glCullFace(GL_FRONT);
	} else if(m_operationStatus[OperationType::BackfaceCulling])
	{	
		glEnable( GL_CULL_FACE );
		glCullFace(GL_BACK);
	}

	// Reset m_transformStack
	while (!m_transformStack.empty())
	{
		m_transformStack.pop();
	}
	renderSceneGraph(*m_rootNode, m_transformStack);
	m_rotationUpdateX = m_rotationUpdateY = 0;
	if(m_trackball)
	{
		glBindVertexArray(m_vao_meshData);
		// Draw the geometry node
		GeometryNode * geometryNode = static_cast<GeometryNode *>(m_trackballNode);

		updateShaderUniforms(m_shader, *geometryNode, m_view);
		uploadCommonSceneUniforms(false);
		BatchInfo BatchInfo = m_batchInfoMap[geometryNode->meshId];

		m_shader.enable();
			glDrawArrays(GL_TRIANGLES, BatchInfo.startIndex, BatchInfo.numIndices);
		m_shader.disable();
		glBindVertexArray(0);
	}
	if(m_operationStatus[OperationType::Circle]){
		renderArcCircle();
	}
	if(m_operationStatus[OperationType::ZBuffer]){
		glDisable( GL_DEPTH_TEST );
	}
	if(m_operationStatus[OperationType::FrontfaceCulling] ||
		m_operationStatus[OperationType::BackfaceCulling])
	{
		glDisable( GL_CULL_FACE );
	}
}


//----------------------------------------------------------------------------------------
void A3::renderSceneRecursive(SceneNode *node, SceneNode *parent, stack<mat4> &transformStack)
{
	if (!node) return;
	NodeType type = node->m_nodeType;
	NodeType parentType = parent->m_nodeType;

	// set the transform mat	
	const mat4 prevTransformMatrix = node->get_transform();

	switch(parentType)
	{
		case NodeType::GeometryNode:
			transformStack.push(transformStack.top()*prevTransformMatrix);
			break;
		case NodeType::JointNode:
		{
			JointNode * jointNode = static_cast<JointNode *>(parent);
			JointNode::JointRange jointX = jointNode->m_joint_x;
			JointNode::JointRange jointY = jointNode->m_joint_y;
			if(m_operationStatus[OperationType::Joints]){
				// Rotatin around the joint
				// update x and y range
				double newInitX = jointX.init + m_rotationUpdateX;
				double newInitY = jointY.init + m_rotationUpdateY;
				if(node->isSelected && m_bodyJoint)
				{
					if(jointX.min <= newInitX && newInitX <= jointX.max)
					{
						jointNode->m_joint_x.init = newInitX;
					}
					if(jointY.min <= newInitY && newInitY <= jointY.max)
					{
						jointNode->m_joint_y.init = newInitY;
					}
				}
				else if(node->isSelected && node->m_name == "Head" && m_headJoint)
				{
					if(jointY.min <= newInitY && newInitY <= jointY.max)
					{
						jointNode->m_joint_y.init = newInitY;
					}
				}
				jointX = jointNode->m_joint_x;
				jointY = jointNode->m_joint_y;
			}
			mat4 rotX_matrix = rotate(mat4(), (float)jointX.init*(float)PI, xAxis);
			mat4 rotY_matrix = rotate(mat4(), (float)jointY.init*(float)PI, yAxis);
			transformStack.push(transformStack.top()*rotX_matrix*rotY_matrix*prevTransformMatrix);
			break;
		}
		default:
			// Initialize Stack
			transformStack.push(prevTransformMatrix);
	}
	node->set_transform(m_zTranslate*m_worldRotation*transformStack.top()*node->get_shape());

	if(type == NodeType::GeometryNode)
	{
		glBindVertexArray(m_vao_meshData);

		// Draw the geometry node
		GeometryNode * geometryNode = static_cast<GeometryNode *>(node);
		if(geometryNode->isSelected)
		{
			geometryNode->material.kd *= 1.5;
		}
		updateShaderUniforms(m_shader, *geometryNode, m_view);
		// Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
		BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
		//-- Now render the mesh:
		m_shader.enable();
			glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
		m_shader.disable();	

		if(geometryNode->isSelected)
		{
			geometryNode->material.kd /= 1.5;
		}
		glBindVertexArray(0);	
	}

	// Draw the children
	for (SceneNode * childNode : node->children) {
		renderSceneRecursive(childNode, node, transformStack);
	}
	node->set_transform(prevTransformMatrix);
	transformStack.pop();
}

void A3::renderSceneGraph(SceneNode & root, stack<mat4> &transformStack) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

	// This is emphatically *not* how you should be drawing the scene graph in
	// your final implementation.  This is a non-hierarchical demonstration
	// in which we assume that there is a list of GeometryNodes living directly
	// underneath the root node, and that we can draw them in a loop.  It's
	// just enough to demonstrate how to get geometry and materials out of
	// a GeometryNode and onto the screen.

	// You'll want to turn this into recursive code that walks over the tree.
	// You can do that by putting a method in SceneNode, overridden in its
	// subclasses, that renders the subtree rooted at every node.  Or you
	// could put a set of mutually recursive functions in this class, which
	// walk down the tree from nodes of different types.

	for (SceneNode * node : root.children) {
		renderSceneRecursive(node, &root, transformStack);	
	}

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

	if(m_dragBegin)
	{
		int width, height;
		glfwGetWindowSize(m_window, &width, &height);
		double x = xPos, y = yPos;
		x > width ? x = width:x=x;
		x < 0 ? x = 0: x=x;
		y > height ? y = height: y = y;
		y < 0 ? y = 0: y = y;
		float updateX = (x-m_xpos)/width;
		float updateY = (y-m_ypos)/height;
		if(m_operationStatus[OperationType::Position])
		{
			if(!m_trackball){
				positionUpdateHandler(updateX, updateY);
			}
			else
			{
				positionTrackBallHandler(m_xpos-width/2, x-width/2, 
										m_ypos-height/2, y-height/2);
			}
		}
		if(m_bodyJoint || m_headJoint)
		{
			m_rotationUpdateY = -updateY*2;
			m_rotationUpdateX = updateX*2;
		}
		m_xpos = x;
		m_ypos = y;
	}

	return eventHandled;
}

// Anime

void A3::anime()
{
	if(!m_anime)
	{
		beginAnime();
	}
	else
	{
		endAnime();
	}
}

void A3::beginAnime()
{
	if(m_anime) return;
	// First save the previous status
	JointStatus status;
	saveJointStatus(status, &*m_rootNode);
	m_animePrevStatus = status;

	m_anime = true;
	m_startTime = 0;
	for(unsigned i = 0; i < 4; i++)
	{
		JointNode *node = m_animeNodes[i*3+1].second;
		double init = node->m_joint_x.init;
		double min = node->m_joint_x.min;
		double max = node->m_joint_x.max;
		double percentage = (init-min)/(max-min);
		unsigned length = (unsigned)(m_animeLength/2*percentage);
		for(unsigned p = 0; p < 3; p++)
		{
			m_animeNodes[i*3+2-p].first = length;
		}
	}
	for(unsigned i = 12; i < m_animeNodes.size(); i++)
	{
		JointNode *node = m_animeNodes[i].second;
		double init = node->m_joint_x.init;
		double min = node->m_joint_x.min;
		double max = node->m_joint_x.max;
		double percentage = (init-min)/(max-min);
		unsigned length = (unsigned)(m_animeLength/2*percentage);
		m_animeNodes[i].first = length;
	}

}

void A3::playAnime()
{
	if (!m_anime) return;
	for(auto it = m_animeNodes.begin(); it != m_animeNodes.end(); ++it)
	{
		unsigned length = it->first;
		if (length == m_animeLength)
		{
			it->first = 0;
			length = it->first;
		}
		JointNode *node = it->second;
		double updateValue = node->m_joint_x.init;
		if(length < m_animeLength/2)
		{
			updateValue += m_updateAnimeValue;
		}
		else
		{
			updateValue -= m_updateAnimeValue;
		}
		if(node->m_joint_x.min <= updateValue &&
		updateValue <= node->m_joint_x.max)
		{
			node->m_joint_x.init = updateValue;
		}
		it->first++;
	}
}

void A3::endAnime()
{
	if(!m_anime) return;
	m_anime = false;
	resetJoints();
}
//----------------------------------------------------------------------------------------
/*
 * Application handler
 */
void A3::resetPosition()
{
	m_xTranslation = 0;
	m_yTranslation = 0;
	m_zTranslation = 0;
}

//----------------------------------------------------------------------------------------
void A3::resetOrientation()
{
	m_worldRotation = mat4();
}

//----------------------------------------------------------------------------------------
void A3::resetJoints()
{
	m_applyIndex = 0;
	applyJointStatus(originStatus, &*m_rootNode);
	redoStack = stack<JointStatus>();
	undoStack = stack<JointStatus>();
	undoStack.push(originStatus);
}

//----------------------------------------------------------------------------------------
void A3::resetAll()
{
	resetPosition();
	resetOrientation();
	resetJoints();
	m_operationStatus[OperationType::Circle] = false;
	m_operationStatus[OperationType::ZBuffer] = true;
	m_operationStatus[OperationType::BackfaceCulling] = false;
	m_operationStatus[OperationType::FrontfaceCulling] = false;
	m_operationStatus[OperationType::Position] = false;
}

//----------------------------------------------------------------------------------------
// Edit Handler
void A3::redo()
{
	if(redoStack.empty()) return;
	JointStatus status = redoStack.top();
	redoStack.pop();
	undoStack.push(status);
	m_applyIndex = 0;
	applyJointStatus(status, &*m_rootNode);
}

//----------------------------------------------------------------------------------------
void A3::undo()
{
	if(undoStack.size() == 1) return;
	JointStatus status = undoStack.top();
	undoStack.pop();
	redoStack.push(status);
	m_applyIndex = 0;
	applyJointStatus(undoStack.top(), &*m_rootNode);
}

//----------------------------------------------------------------------------------------
void A3::saveJointStatus(JointStatus &status, SceneNode *node)
{
	if (!node) return;

	if(node->m_nodeType == NodeType::JointNode)
	{
		const JointNode * jointNode = static_cast<const JointNode *>(node);
		status.push_back(make_pair(jointNode->m_joint_x, jointNode->m_joint_y));
	} 

	for (SceneNode * childNode : node->children) {
		saveJointStatus(status, childNode);
	}
}

//----------------------------------------------------------------------------------------
void A3::applyJointStatus(JointStatus &status, SceneNode *node)
{
	if (!node) return;

	if(node->m_nodeType == NodeType::JointNode)
	{
		JointNode * jointNode = static_cast<JointNode *>(node);
		auto jointRange = status[m_applyIndex];
		jointNode->m_joint_x = jointRange.first;
		jointNode->m_joint_y = jointRange.second;
		m_applyIndex++;
	} 

	for (SceneNode * childNode : node->children) {
		applyJointStatus(status, childNode);
	}
}

//----------------------------------------------------------------------------------------
/*
 * Operation handler.  Handles operation update.
 */
void A3::updateOperationStatus(OperationType type)
{
	m_operationStatus[type] = !m_operationStatus[type];
}

bool operator==(const JointNode::JointRange& lhs, const JointNode::JointRange& rhs)
{
	return lhs.min == rhs.min && lhs.init == rhs.init && lhs.max == rhs.max;
}

//----------------------------------------------------------------------------------------
/*
 * Operation handler.  Handles joints operations.
 */
void A3::jointsHandle(int button, int actions)
{
	if (actions == GLFW_PRESS)
	{
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			jointsSelectHandle();
			return;
		}
		else
		{
			if (button == GLFW_MOUSE_BUTTON_MIDDLE)
			{
				m_bodyJoint = true;
			}
			if (button == GLFW_MOUSE_BUTTON_RIGHT)
			{
				m_headJoint = true;
			}
		}
	}
	else if (actions == GLFW_RELEASE)
	{
		if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			m_bodyJoint = false;
		}
		if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			m_headJoint = false;
		}
		// First save the status
		JointStatus status;
		saveJointStatus(status, &*m_rootNode);
		if (undoStack.empty())
		{
			undoStack.push(status);
			return;
		}
		if (status != undoStack.top())
		{
			undoStack.push(status);
		}
	}
}

//----------------------------------------------------------------------------------------
void A3::jointsSelectHandle()
{
	double xpos, ypos;
	glfwGetCursorPos( m_window, &xpos, &ypos );
	
	m_picking = true;
	uploadCommonSceneUniforms();
	glClearColor(1.0, 1.0, 1.0, 1.0 );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor(0.85, 0.85, 0.85, 1.0);
		
	draw();

	m_picking = false;
	CHECK_GL_ERRORS;
		
	xpos *= double(m_framebufferWidth) / double(m_windowWidth);
	ypos = m_windowHeight - ypos;
	ypos *= double(m_framebufferHeight) / double(m_windowHeight);
		
	GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
	glReadBuffer( GL_BACK );

	// Actually read the pixel at the mouse location.
	glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
	CHECK_GL_ERRORS;

	// Reassemble the object ID.
	unsigned int color = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
	auto findNode = m_colorMap.find(color);
	if (findNode != m_colorMap.end())
	{
		(findNode->second)->isSelected = !(findNode->second)->isSelected;
	}
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------

/*
 * Operation handler.  Handles position operations.
 */
void A3::positionHandle(int button, int actions)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if(actions == GLFW_PRESS)
		{
			m_xTanslate = true;
			m_yTanslate = true;
		}
		if(actions == GLFW_RELEASE)
		{
			m_xTanslate = false;
			m_yTanslate = false;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		if(actions == GLFW_PRESS)
		{
			m_zTanslate = true;
			m_zTanslate = true;
		}
		if(actions == GLFW_RELEASE)
		{
			m_zTanslate = false;
			m_zTanslate = false;
		}
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if(actions == GLFW_PRESS)
		{
			m_trackball = true;
		}
		if(actions == GLFW_RELEASE)
		{
			m_trackball = false;
		}
	}
}

//----------------------------------------------------------------------------------------
void A3::positionUpdateHandler(float updateX, float updateY)
{
	if (m_xTanslate && m_yTanslate)
	{
		m_xTranslation += updateX*10;
		m_yTranslation -= updateY*10;
	}
	if (m_zTanslate)
	{
		m_zTranslation += updateY*10;
	}
}

//----------------------------------------------------------------------------------------
void A3::positionTrackBallHandler(float m_xpos, float x, float m_ypos, float y)
{
	float halfWidth = m_framebufferWidth/8;
	float halfHeight = m_framebufferHeight/8;
	float radius = halfWidth < halfHeight ? halfWidth:halfHeight;
	float radius_sqr = radius*radius;
	vec2 s_2d = vec2(m_xpos, m_ypos);
	vec2 t_2d = vec2(x, y);
	if(length(s_2d)*length(s_2d) > radius_sqr)
	{
		s_2d *= radius/length(s_2d);
	}
	if(length(t_2d)*length(t_2d) > radius_sqr)
	{
		t_2d *= radius/length(t_2d);
	}
	vec3 s_3d = normalize(vec3(s_2d, sqrt(radius_sqr-length(s_2d)*length(s_2d))));
	vec3 t_3d = normalize(vec3(t_2d, sqrt(radius_sqr-length(t_2d)*length(t_2d))));
	if(isnan(s_3d[0]) || isnan(s_3d[1]) || isnan(s_3d[2])) return;
	if(isnan(t_3d[0]) || isnan(t_3d[1]) || isnan(t_3d[2])) return;
	vec3 p = normalize(s_3d);
	vec3 d = normalize(t_3d-s_3d);
	m_rotateAxis = cross(s_3d,t_3d);
	float bound = -1.0e-05;
	// Check whether valid
	if(m_rotateAxis == vec3(0,0,0)) return;
	if(isnan(m_rotateAxis[0]) || isnan(m_rotateAxis[1]) || isnan(m_rotateAxis[2])) return;
	if(bound < m_rotateAxis[0] && m_rotateAxis[0] < -bound) return;
	if(bound < m_rotateAxis[1] && m_rotateAxis[1] < -bound) return;
	if(bound < m_rotateAxis[2] && m_rotateAxis[2] < -bound) return;
	m_rotateAngle = acos(dot(s_3d, t_3d));
	m_worldRotation = rotate(mat4(), m_rotateAngle, m_rotateAxis)*m_worldRotation;
}
//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);
	if(m_operationStatus[OperationType::Joints])
	{
		jointsHandle(button, actions);
	}
	if(m_operationStatus[OperationType::Position])
	{
		positionHandle(button, actions);
	}
	if(button == GLFW_MOUSE_BUTTON_LEFT ||
		button == GLFW_MOUSE_BUTTON_MIDDLE ||
		button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if(actions == GLFW_PRESS && !m_dragBegin)
		{
			// mouse clicked
			m_dragBegin = !m_dragBegin;
			glfwGetCursorPos(m_window, &m_xpos, &m_ypos);
			eventHandled = true;
		}
		if(actions == GLFW_RELEASE && m_dragBegin)
		{
			// mouse released
			m_dragBegin = !m_dragBegin;
			eventHandled = true;
		}
	}
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if (key == GLFW_KEY_E)
		{
			anime();
			eventHandled = true;
		}
		if (key == GLFW_KEY_I)
		{
			resetPosition();
			eventHandled = true;
		}
		if (key == GLFW_KEY_O)
		{
			resetOrientation();
			eventHandled = true;
		}
		if (key == GLFW_KEY_S)
		{
			resetJoints();
			eventHandled = true;
		}
		if (key == GLFW_KEY_A)
		{
			resetAll();
			eventHandled = true;
		}
		if (key == GLFW_KEY_R)
		{
			redo();
			eventHandled = true;
		}
		if (key == GLFW_KEY_U)
		{
			undo();
			eventHandled = true;
		}
		if( key == GLFW_KEY_M ) {
			show_gui = !show_gui;
			eventHandled = true;
		}
		if (key == GLFW_KEY_Q)
		{
			glfwSetWindowShouldClose(m_window, GL_TRUE);
			eventHandled = true;
		}
		if (key == GLFW_KEY_C)
		{
			updateOperationStatus(OperationType::Circle);
			eventHandled = true;
		}
		if (key == GLFW_KEY_Z)
		{
			updateOperationStatus(OperationType::ZBuffer);
			eventHandled = true;
		}
		if (key == GLFW_KEY_B)
		{
			updateOperationStatus(OperationType::BackfaceCulling);
			eventHandled = true;
		}
		if (key == GLFW_KEY_F)
		{
			updateOperationStatus(OperationType::FrontfaceCulling);
			eventHandled = true;
		}
		if (key == GLFW_KEY_P)
		{
			m_operationStatus[OperationType::Position] = !m_operationStatus[OperationType::Position];
			if(m_operationStatus[OperationType::Position])
			{
				m_operationStatus[OperationType::Joints] = false;
			}
			eventHandled = true;
		}
		if (key == GLFW_KEY_J)
		{
			m_operationStatus[OperationType::Joints] = !m_operationStatus[OperationType::Joints];
			if(m_operationStatus[OperationType::Joints])
			{
				m_operationStatus[OperationType::Position] = false;
			}
			eventHandled = true;
		}
	}
	// Fill in with event handling code...

	return eventHandled;
}
