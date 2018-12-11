// Fall 2018

#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "JointNode.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <stack>
#include <map>
#include <vector>

typedef std::vector<std::pair<JointNode::JointRange, JointNode::JointRange >> JointStatus;

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};

//----------------------------------------------------------------------------------------
// Register for operation types;

enum OperationType
{
	Circle = 0,
	ZBuffer,
	BackfaceCulling,
	FrontfaceCulling ,
	Position, 
	Joints
};

class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void analyzeJointNodes(SceneNode & root);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();
	void initColorMap(SceneNode *node);
	void initAnime(SceneNode *node);

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms(bool model = true);
	void renderSceneGraph(SceneNode &node, std::stack<glm::mat4> &transformStack);
	void renderSceneRecursive(SceneNode *node, SceneNode *parent, std::stack<glm::mat4> &transformStack);
	void renderArcCircle();

	// Anime
	void anime();
	void beginAnime();
	void playAnime();
	void endAnime();

	// Application Handler
	void resetPosition();
	void resetOrientation();
	void resetJoints();
	void resetAll();

	// Edit Handler
	void redo();
	void undo();
	void saveJointStatus(JointStatus &status, SceneNode *node);
	void applyJointStatus(JointStatus &status, SceneNode *node);
	void showSelectedNodes(SceneNode *node);

	// Operation Handler
	void updateOperationStatus(OperationType type);
	void jointsHandle(int button, int actions);
	void jointsSelectHandle();
	void positionHandle(int button, int actions);
	void positionUpdateHandler(float updateX, float updateY);
	void positionTrackBallHandler(float m_xpos, float x, float m_ypos, float y);
	
	glm::mat4 m_zTranslate;
	glm::mat4 m_perpsective;
	glm::mat4 m_view;
	std::stack<glm::mat4> m_transformStack;
	std::map<unsigned int, SceneNode *> m_colorMap;
	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// Anime related
	unsigned m_animeLength = 100;
	unsigned m_loopTime = 3;
	unsigned m_startTime = 0;
	unsigned m_animeFrameRate = 1;
	double   m_updateAnimeValue = (double)1.0/(double)180.0;
	bool 	 m_anime = false;
	JointStatus m_animePrevStatus;
	std::vector< std::pair<unsigned, JointNode * >> m_animeNodes;

	// Operation related
	bool m_dragBegin = false;
	double m_xpos = 0.0f;
	double m_ypos = 0.0f;
	std::vector<int> m_operationStatus;
	
	// JointsHandle
	bool m_picking = false;
	double m_rotationUpdateX,m_rotationUpdateY = 0;
	bool m_bodyJoint, m_headJoint = false;
	JointStatus originStatus;
	unsigned m_applyIndex = 0;
	std::stack<JointStatus> undoStack;
	std::stack<JointStatus> redoStack;

	// PositionHandle
	bool m_xTanslate = false;
	bool m_yTanslate = false;
	bool m_zTanslate = false;
	bool m_trackball = false;
	SceneNode *m_trackballNode;

	// World Transform
	float m_xTranslation = 0;
	float m_yTranslation = 0;  
	float m_zTranslation = 0;
	float m_rotateAngle;
	glm::vec3 m_rotateAxis;
	glm::mat4 m_worldRotation;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;
};
