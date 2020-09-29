#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <stdio.h>
#include <QGLWidget>  
#include<QVector>
#include<QTimer>
#include<QtMath> 

QImage texturedata, buffer;
unsigned int cubeTexture, floorTexture, transparentTexture;
const char *vertexShaderSource = "#version 410 core\n"
"layout(location = 0) in vec3 aPos; \n"
"layout(location = 1) in vec3 aNormal; \n"
"layout(location = 2) in vec2 aTexCoords; \n"
"out vec2 TexCoords; \n"
"out VS_OUT{ \n"
"	vec3 FragPos; \n"
"	vec3 Normal; \n"
"	vec2 TexCoords; \n"
"	vec4 FragPosLightSpace; \n"
"} vs_out; \n"
"uniform mat4 model; \n"
"uniform mat4 view; \n"
"uniform mat4 projection; \n"
"uniform mat4 lightSpaceMatrix; \n"
"\n"
"void main()\n"
"{\n"  
"	    vs_out.FragPos = vec3(model * vec4(aPos, 1.0)); \n"
"	   vs_out.Normal = transpose(inverse(mat3(model))) * aNormal; \n"
"	   vs_out.TexCoords = aTexCoords; \n"
"	   vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0); \n"
"	   gl_Position = projection * view * model * vec4(aPos, 1.0); \n"
"}\0";
const char *fragmentShaderSource = "#version 410 core\n"
"out vec4 FragColor; \n"
"\n"
"in VS_OUT{ \n"
"	vec3 FragPos; \n"
"vec3 Normal; \n"
"vec2 TexCoords; \n"
"vec4 FragPosLightSpace; \n"
"} fs_in; \n"

"uniform sampler2D diffuseTexture; \n"
"uniform sampler2D shadowMap; \n"
""
"uniform vec3 lightPos; \n"
"uniform vec3 viewPos; \n"
""
"float ShadowCalculation(vec4 fragPosLightSpace)\n"
"{\n"
"// perform perspective divide\n"
"vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w; \n"
"// transform to [0,1] range\n"
"projCoords = projCoords * 0.5 + 0.5; \n"
"// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)\n"
"float closestDepth = texture(shadowMap, projCoords.xy).r; \n"
"// get depth of current fragment from light's perspective\n"
"float currentDepth = projCoords.z; \n"
"// check whether current frag pos is in shadow\n"
"float shadow = currentDepth > closestDepth ? 1.0 : 0.0; \n"
"		\n"
"return shadow; \n"
"}\n"
"\n"
"void main()\n"
"{\n"
"	vec3 color = texture(diffuseTexture, fs_in.TexCoords).rgb; \n"
"	vec3 normal = normalize(fs_in.Normal); \n"
"	vec3 lightColor = vec3(0.3); \n"
"// ambient\n"
"	vec3 ambient = 0.3 * color; \n"
"// diffuse\n"
"	vec3 lightDir = normalize(lightPos - fs_in.FragPos); \n"
"	float diff = max(dot(lightDir, normal), 0.0); \n"
"	vec3 diffuse = diff * lightColor; \n"
"// specular\n"
"	vec3 viewDir = normalize(viewPos - fs_in.FragPos); \n"
"	vec3 reflectDir = reflect(-lightDir, normal); \n"
"	float spec = 0.0; \n"
"	vec3 halfwayDir = normalize(lightDir + viewDir); \n"
"	spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0); \n"
"	vec3 specular = spec * lightColor; \n"
"// calculate shadow \n"
"	float shadow = ShadowCalculation(fs_in.FragPosLightSpace); \n"
"	vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color; \n"
"		\n"
"	FragColor = vec4(lighting, 1.0); \n"
"}; \0";
const char *vertexShaderSource2 = "#version 410 core\n"
"layout(location = 0) in vec3 aPos; \n"
"\n"
"uniform mat4 lightSpaceMatrix; \n"
"uniform mat4 model; \n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0); \n"

"}\0";
const char *fragmentShaderSource2 = "#version 410 core\n"
"void main()\n"
"{\n"
"	// gl_FragDepth = gl_FragCoord.z;\n"
"}\0";
const char *vertexShaderSource3 = "#version 410 core\n"
"layout(location = 0) in vec3 aPos; \n"
"layout(location = 1) in vec2 aTexCoords; \n"
"\n"
"out vec2 TexCoords; \n"
"\n"
"void main()\n"
"{\n"
"	 TexCoords = aTexCoords;\n"
"	 gl_Position = vec4(aPos, 1.0); \n"
"}\0";
const char *fragmentShaderSource3 = "#version 410 core\n"
"out vec4 FragColor; \n"
"\n"
"in vec2 TexCoords; \n"
"\n"
"uniform sampler2D depthMap; \n"
"uniform float near_plane; \n"
"uniform float far_plane; \n"
"\n"
"// required when using a perspective projection matrix\n"
"float LinearizeDepth(float depth)\n"
"{\n"
"	float z = depth * 2.0 - 1.0; // Back to NDC \n"
"	return (2.0 * near_plane * far_plane) / (far_plane + near_plane - z * (far_plane - near_plane)); \n"
		"}\n"
"\n"
"void main()\n"
"{\n"
"	float depthValue = texture(depthMap, TexCoords).r; \n"
"	// FragColor = vec4(vec3(LinearizeDepth(depthValue) / far_plane), 1.0); // perspective\n"
"	FragColor = vec4(vec3(depthValue), 1.0); // orthographic\n"
"}\0";
float planeVertices[] = {
	// positions            // normals         // texcoords
	 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
	-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

	 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
	-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
	 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
};
/*
float cubeVertices[] = {
	// positions          // texture Coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};
float skyboxVertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};*/

QStringList faces;

QVector<QVector3D> windows=
{
	QVector3D(-1.5f, 0.0f, -0.48f),
	QVector3D(1.5f, 0.0f, 0.51f),
	QVector3D(0.0f, 0.0f, 0.7f),
	QVector3D(-0.3f, 0.0f, -2.3f),
	QVector3D(0.5f, 0.0f, -0.6f)
};
unsigned int depthMapFBO;
unsigned int depthMap;
unsigned int cubeVAO, cubeVBO, planeVBO;
unsigned int skyboxVAO, skyboxVBO;
int WIDTH = 500;
int  HEIGHT = 500;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
GLint modelLoc;
GLint viewLoc;  
GLint projLoc;

// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) 
//so we initially rotate a bit to the left.
GLfloat yaw = -90.0f;	
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
GLfloat fov = 45.0f;
bool keys[1024];

bool firstMouse = true;

// Deltatime
GLfloat deltaTime = 1.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame
// Camera
QVector3D cameraPos = QVector3D(0.0f, 0.0f, 3.0f);
QVector3D cameraFront = QVector3D(0.0f, 0.0f, -1.0f);
QVector3D cameraUp = QVector3D(0.0f, 1.0f, 0.0f);
GLWidget::GLWidget(QWidget* parent /* = nullptr */, Qt::WindowFlags f /* = Qt::WindowFlags() */)
	:QOpenGLWidget(parent, f)
{
    setMinimumSize(WIDTH, HEIGHT);
	QSurfaceFormat surfaceFormat;
	surfaceFormat.setSamples(4);//多重采样
	setFormat(surfaceFormat);
}
unsigned int cubemapTexture;
GLWidget::~GLWidget()
{
   
}

void GLWidget::initializeGL()
{
	faces << "D:/opengltest/minOpenglQtD14/minOpenglQt/helloworld/skybox/right.jpg";
	faces << "D:/opengltest/minOpenglQtD14/minOpenglQt/helloworld/skybox/left.jpg";
	faces << "D:/opengltest/minOpenglQtD14/minOpenglQt/helloworld/skybox/top.jpg";
	faces << "D:/opengltest/minOpenglQtD14/minOpenglQt/helloworld/skybox/bottom.jpg";
	faces << "D:/opengltest/minOpenglQtD14/minOpenglQt/helloworld/skybox/back.jpg";
	faces << "D:/opengltest/minOpenglQtD14/minOpenglQt/helloworld/skybox/front.jpg";

    initializeOpenGLFunctions();
	glViewport(0, 0, WIDTH, HEIGHT);
	// configure global opengl state
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);//抗锯齿 开启多重采样
	//glEnable(GL_BLEND);
	//glEnable(GL_CULL_FACE);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//shader  shaderProgram2=simpleDepthShader
	shaderProgram = initshaderprogramer(vertexShaderSource, fragmentShaderSource);
	shaderProgram2 = initshaderprogramer(vertexShaderSource2, fragmentShaderSource2);
	shaderProgram3 = initshaderprogramer(vertexShaderSource3, fragmentShaderSource3);

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	
	// load textures
   // -------------
	int width, height, nrChannels;
	bool HAdLOAD;

	{
		glGenTextures(1, &woodTexture);
		glBindTexture(GL_TEXTURE_2D, woodTexture);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		QImage buffer2, texturedata2;
		HAdLOAD = buffer2.load("D:/opengltest/minOpenglQtD16/minOpenglQt/helloworld/wall.jpg"); 
		texturedata2 = QGLWidget::convertToGLFormat(buffer2);
		width = texturedata2.width();
		height = texturedata2.height();
		if (!texturedata2.isNull())
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata2.bits());
			glGenerateMipmap(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
			qDebug() << "-------------------succese to load woodTexture" << endl;
		}
		else
		{
			qDebug() << "-------------------Failed to load woodTexture" << endl;
		}
	
	}

	// create depth texture
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	/*我们需要的只是在从光的透视图下渲染场景的时候深度信息，所以颜色缓冲没有用。
	然而帧缓冲对象不是完全不包含颜色缓冲的，所以我们需要显式告诉OpenGL我们不适用任何颜色数据进行渲染。
	我们通过将调用glDrawBuffer和glReadBuffer把读和绘制缓冲设置为GL_NONE来做这件事。*/
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//cubemapTexture = loadCubemap(faces);

	glUseProgram(shaderProgram);
	glUniform1i(glGetUniformLocation(shaderProgram, "diffuseTexture"), 0);
	glUniform1i(glGetUniformLocation(shaderProgram, "shadowMap"), 1); 
	glUseProgram(shaderProgram3);
	glUniform1i(glGetUniformLocation(shaderProgram3, "depthMap"), 0);


	greenValue = 0.0 ;
	up = true;
}

void GLWidget::paintGL()
{
	if ( up) {
		greenValue = greenValue + 1;
		if(greenValue >= 120){
			up = false;
		}
	}
	else if (  !up) {
		greenValue = greenValue - 1;
		if (greenValue <= 0) {
			up = true;
		}
	}
	lightPos= QVector3D(-2.0f, 4.0f, -1.0f);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat radius = 10.0f;
	GLfloat camX = sin(greenValue*0.1f) * radius;
	GLfloat camZ = cos(greenValue*0.1f) * radius;
	dosome();

	QMatrix4x4 lightProjection, lightView;
	QMatrix4x4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection.ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView.lookAt(lightPos, QVector3D(0.0f,0.0f, 0.0f), QVector3D(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	glUseProgram(shaderProgram2);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.constData());

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	renderScene(shaderProgram2);
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

	//下面两个矩阵 如果定义在外面  需要变成单位矩阵
	QMatrix4x4 view; //视图矩阵
	QMatrix4x4 Projection;//投影矩阵  先世界 再视图矩阵  在投影矩阵
	//一个摄像机位置，一个目标位置和一个表示上向量的世界空间中的向量(我们使用上向量计算右向量)
	view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	Projection.perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	QMatrix4x4 model;
	model.translate(QVector3D(-1.0f, 0.0f, -1.0f));
	// reset viewport
	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, Projection.constData());
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, view.constData());
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "lightSpaceMatrix"), 1, GL_FALSE, lightSpaceMatrix.constData());
	glUniform3f(glGetUniformLocation(shaderProgram, " viewPos"), cameraPos[0], cameraPos[1], cameraPos[2]);
	//glUniform3fv(glGetUniformLocation(shaderProgram," viewPos"), 1, &cameraPos[0]);
	glUniform3f(glGetUniformLocation(shaderProgram, " lightPos"), lightPos[0], lightPos[1], lightPos[2]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, woodTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	renderScene(shaderProgram);

	glUseProgram(shaderProgram3);
	glUniform1f(glGetUniformLocation(shaderProgram3, "near_plane"), near_plane);
	glUniform1f(glGetUniformLocation(shaderProgram3, "far_plane"), far_plane); 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, depthMap);

	//draw to screen
	//glBindFramebuffer(GL_FRAMEBUFFER, 0); 
	glUseProgram(0);
	glBindVertexArray(0);
	update();
};
unsigned int GLWidget::loadCubemap(QStringList faces) {
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	int width, height, nrChannels;
	int size = faces.size(); int j = 0;
	for (unsigned int i = 0 ; j< size; j++,i++) {
		QImage buffer333, texturedata333; bool HAdLOAD;
		HAdLOAD = buffer333.load(faces[i]);
		texturedata333 = QGLWidget::convertToGLFormat(buffer333);
		width = texturedata333.width();
		height = texturedata333.height();
		if (!texturedata333.isNull())
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X+i, 0, GL_RGBA, 
				width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata333.bits());
		  
			qDebug() << "-------------------succese to load loadCubemap" << endl;
		}
		else
		{
			qDebug() << "-------------------Failed to load loadCubemap" << endl;
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	return textureID;
}
void  GLWidget::renderScene( unsigned int Program) {
	// floor
	QMatrix4x4 model ;
	QMatrix4x4  model1;
	glUniformMatrix4fv(glGetUniformLocation(Program, "model"),
		1, GL_FALSE, model.constData()); 
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// cubes
	model = QMatrix4x4 (model1);
	model.translate( QVector3D(0.0f, 1.5f, 0.0));
	model.scale(  0.5f);
	glUniformMatrix4fv(glGetUniformLocation(Program, "model"),
		1, GL_FALSE, model.constData()); 
	renderCube();
	model = QMatrix4x4(model1);
	model.translate(QVector3D(2.0f, 0.0f, 1.0));
	model.scale(0.5f);
	glUniformMatrix4fv(glGetUniformLocation(Program, "model"),
		1, GL_FALSE, model.constData());
	renderCube();
	model = QMatrix4x4(model1);
	model.translate(QVector3D(-1.0f, 0.0f, 2.0));
	model.rotate(60.0f, 1.0, 0.0, 1.0);
	model.scale(0.25f);
	glUniformMatrix4fv(glGetUniformLocation(Program, "model"),
		1, GL_FALSE, model.constData());
	renderCube();
}

void GLWidget::renderCube() {
	// initialize (if necessary)
	if (cubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &cubeVAO);
		glGenBuffers(1, &cubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(cubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
void GLWidget::mouseReleaseEvent(QMouseEvent *event) {
	starflage = false;

}
void GLWidget::mousePressEvent(QMouseEvent *event) {
	if (event->button() == Qt::LeftButton) {
		starflage = true;
		lastX = event->x();
		lastY = event->y();
	}
}

void  GLWidget::mouseMoveEvent(QMouseEvent *event) {
	//qDebug()<<"get mouse move "<<endl;
	if (!starflage)
		return;
	if (firstMouse)
	{
		lastX = event->x();
		lastY = event->y();
		firstMouse = false;
	}

	GLfloat xoffset = event->x() - lastX;
	GLfloat yoffset = lastY - event->y(); // Reversed since y-coordinates go from bottom to left
	lastX = event->x();
	lastY = event->y();

	GLfloat sensitivity = 0.5;	// Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	QVector3D front;
	front[0] = cos(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch));
	front[1] = sin(qDegreesToRadians(pitch));
	front[2] = sin(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch));
	front.normalize();
	//qDebug() << "front:  " << front[0] << front[1] << front[2] << endl;
	cameraFront = front;
}
void  GLWidget::dosome() {
	GLfloat cameraSpeed = 1.0f * deltaTime;
	QVector3D bufferver = QVector3D(cameraPos);
	if (keys[Qt::Key_W]) {
		qDebug() << " Key_W  " << endl;
		cameraPos += cameraSpeed * cameraFront;
	}
	if (keys[Qt::Key_A]) {
		qDebug() << " Key_A  " << endl;
		cameraPos -= QVector3D::crossProduct(cameraFront, cameraUp) * cameraSpeed;
		//cameraPos.normalize();
	}
	if (keys[Qt::Key_S]) {
		qDebug() << " Key_S  " << endl;
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (keys[Qt::Key_D]) {
		qDebug() << " Key_D  " << endl;
		cameraPos += QVector3D::crossProduct(cameraFront, cameraUp) * cameraSpeed;
	}
}
void GLWidget::keyReleaseEvent(QKeyEvent *event) {
	qDebug() << "keyReleaseEvent" << endl;
	keys[event->key()] = false;
}
void GLWidget::keyPressEvent(QKeyEvent *event) {
	qDebug() << "keyPressEvent" << endl;
	keys[event->key()] = true;
	
}


unsigned int GLWidget::initshaderprogramer(const char *vshader, const char *fshader) {
	//1.创建顶点着色器
	int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vshader, NULL);
	glCompileShader(vertexShader);
	//检查是否成功
	int  success;
	char infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
	}


	//2.创建片元着色器
	int fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fshader, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);//获取错误信息  如果有的话
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::fragmentShader::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//3.创建着色器程序

	unsigned int shaderProgrambufer = glCreateProgram();
	//4.链接顶点和片元着色器到着色器程序
	glAttachShader(shaderProgrambufer, vertexShader);
	glAttachShader(shaderProgrambufer, fragmentShader);
	glLinkProgram(shaderProgrambufer);
	glGetProgramiv(shaderProgrambufer, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		qDebug() << "ERROR::Program::Program::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//5.链接到程序对象后,删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return shaderProgrambufer;
}
void GLWidget::resizeGL(int w, int h)
{
   
}
/**
unsigned int loadTexture(char const *path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}*/
