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
"layout(location = 1) in vec2 aTexCoords; \n"
"out vec2 TexCoords; \n"
"uniform mat4 model; \n"
"uniform mat4 view; \n"
"uniform mat4 projection; \n"
"\n"
"void main()\n"
"{\n"
"	gl_Position = projection * view * model * vec4(aPos, 1.0f); \n"
"	TexCoords  =aTexCoords; \n"
"}\0";
const char *fragmentShaderSource = "#version 410 core\n"
"in vec2 TexCoords; \n"
"out vec4 FragColor; \n"
"uniform sampler2D texture1; \n"
"void main() \n"
"{ \n"
"	FragColor =texture(texture1, TexCoords); \n"
"}\0"; 
const char *vertexShaderSource2 = "#version 410 core\n"
"layout(location = 0) in vec3 aPos; \n"
"out vec3 TexCoords; \n"
"uniform mat4 projection; \n"
"uniform mat4 view; \n"
"\n"
"void main()\n"
"{\n"
"	TexCoords = aPos; \n"
"	vec4 pos = projection * view * vec4(aPos, 1.0); \n"
"	gl_Position   =pos.xyww; \n"
"}\0";
const char *fragmentShaderSource2 = "#version 410 core\n"
"in vec3 TexCoords; \n"
"out vec4 FragColor; \n"
"uniform samplerCube  skybox; \n"
"void main() \n"
"{ \n" 
"	FragColor =texture(skybox, TexCoords); \n"
"}\0";
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
};

QStringList faces;

QVector<QVector3D> windows=
{
	QVector3D(-1.5f, 0.0f, -0.48f),
	QVector3D(1.5f, 0.0f, 0.51f),
	QVector3D(0.0f, 0.0f, 0.7f),
	QVector3D(-0.3f, 0.0f, -2.3f),
	QVector3D(0.5f, 0.0f, -0.6f)
};
unsigned int cubeVAO, cubeVBO;
unsigned int skyboxVAO, skyboxVBO;
int WIDTH = 500;
int  HEIGHT = 500;
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

	shaderProgram = initshaderprogramer(vertexShaderSource, fragmentShaderSource);
	shaderProgram2 = initshaderprogramer(vertexShaderSource2, fragmentShaderSource2);

	glGenVertexArrays(1, &cubeVAO);
	glBindVertexArray(cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0); 
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glGenVertexArrays(1, &skyboxVAO);
	glBindVertexArray(skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	
	// load textures
   // -------------
	int width, height, nrChannels;
	bool HAdLOAD;

	{
		glGenTextures(1, &cubeTexture);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		QImage buffer2, texturedata2;
		HAdLOAD = buffer2.load("D:/opengltest/minOpenglQtD14/minOpenglQt/helloworld/love.jpg"); 
		texturedata2 = QGLWidget::convertToGLFormat(buffer2);
		width = texturedata2.width();
		height = texturedata2.height();
		if (!texturedata2.isNull())
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata2.bits());
			glGenerateMipmap(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
			qDebug() << "-------------------succese to load cubeTexture" << endl;
		}
		else
		{
			qDebug() << "-------------------Failed to load cubeTexture" << endl;
		}
	
	}

	cubemapTexture = loadCubemap(faces);

	glUseProgram(shaderProgram); 
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	glUseProgram(shaderProgram2);
	glUniform1i(glGetUniformLocation(shaderProgram2, "skybox"), 0);

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
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
	
	//5.使用该程序
	glUseProgram(shaderProgram); 
	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc = glGetUniformLocation(shaderProgram, "view");
	projLoc = glGetUniformLocation(shaderProgram, "projection");

	GLfloat radius = 10.0f;
	GLfloat camX = sin(greenValue*0.1f) * radius;
	GLfloat camZ = cos(greenValue*0.1f) * radius;
	dosome();
	//下面两个矩阵 如果定义在外面  需要变成单位矩阵
	QMatrix4x4 view; //视图矩阵
	QMatrix4x4 Projection;//投影矩阵  先世界 再视图矩阵  在投影矩阵
	//一个摄像机位置，一个目标位置和一个表示上向量的世界空间中的向量(我们使用上向量计算右向量)
	view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	Projection.perspective(fov, (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	QMatrix4x4 model;
	model.translate(QVector3D(-1.0f, 0.0f, -1.0f) );

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model.constData());
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view.constData());
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, Projection.constData());

	// cubes
	glBindVertexArray(cubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, cubeTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	

	QMatrix4x4 model2;
	model2.translate(QVector3D(2.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model2.constData());
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	// draw skybox as last
	// change depth function so depth test passes when values are equal to depth buffer's content
	glDepthFunc(GL_LEQUAL);  

	glUseProgram(shaderProgram2);
	float *xsy=view.data();

	QMatrix4x4 view2= QMatrix4x4(xsy[0], xsy[1],xsy[2],0 ,
		xsy[4], xsy[5], xsy[6], 0  ,
		xsy[8], xsy[9], xsy[10], 0  ,
		xsy[12], xsy[13], xsy[14],0) ;

	//转换成3*3的  去掉 
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "view") ,
		1, GL_FALSE, view2.constData());
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "projection"),
		1, GL_FALSE, Projection.constData());
	// skybox cube
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
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
