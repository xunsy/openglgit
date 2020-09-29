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
"layout(location = 0) in vec2 aPos; \n"
"layout(location = 1) in vec2 aTexCoords; \n"
"out vec2 TexCoords; \n"
"\n"
"void main()\n"
"{\n"
"	gl_Position =  vec4(aPos.x, aPos.y, 0.0, 1.0); \n"
"	TexCoords  =aTexCoords; \n"
"}\0";
const char *fragmentShaderSource2 = "#version 410 core\n"
"in vec2 TexCoords; \n"
"out vec4 FragColor; \n"
"uniform sampler2D screenTexture; \n"
"void main() \n"
"{ \n"
"	vec3 col = texture(screenTexture, TexCoords).rgb; \n"
"	FragColor = vec4(col, 1.0); \n"
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
float planeVertices[] = {
	// positions          // texture Coords 
	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};
float transparentVertices[] = {
	// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
	0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
	0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
	1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

	0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
	1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
	1.0f,  0.5f,  0.0f,  1.0f,  0.0f
}; 
float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords

		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		- 1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
		
};

QVector<QVector3D> windows=
{
	QVector3D(-1.5f, 0.0f, -0.48f),
	QVector3D(1.5f, 0.0f, 0.51f),
	QVector3D(0.0f, 0.0f, 0.7f),
	QVector3D(-0.3f, 0.0f, -2.3f),
	QVector3D(0.5f, 0.0f, -0.6f)
};
unsigned int quadVAO, quadVBO;
unsigned int transparentVAO, transparentVBO;
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
}

GLWidget::~GLWidget()
{
   
}
void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
	glViewport(0, 0, WIDTH, HEIGHT);
	glDisable(GL_CULL_FACE);
	// configure global opengl state
	//glEnable(GL_DEPTH_TEST);
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

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	// screen VAO
	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(4 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
	
	// load textures
   // -------------
	int width, height, nrChannels;
	bool HAdLOAD;
	{
		glGenTextures(1,&floorTexture);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters  放大和缩小时分别如何采集数据
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		 HAdLOAD=buffer.load("D:/opengltest/minOpenglQtD10/minOpenglQt/helloworld/toy_box_diffuse.png");  
		texturedata = QGLWidget::convertToGLFormat(buffer);
		width = texturedata.width();
		height = texturedata.height();
		if (!texturedata.isNull())
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,GL_RGBA, GL_UNSIGNED_BYTE, texturedata.bits());
			glGenerateMipmap(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.

			qDebug() << "-------------------succese to load texture" << endl;
		}
		else
		{
			qDebug()<< "-------------------Failed to load texture" <<  endl;
		}
	}
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
		HAdLOAD = buffer2.load("D:/opengltest/minOpenglQtD10/minOpenglQt/helloworld/marble.jpg"); 
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
	
		glGenTextures(1, &transparentTexture);
		glBindTexture(GL_TEXTURE_2D, transparentTexture);
		QImage buffer3, texturedata3;
		HAdLOAD = buffer3.load("D:/opengltest/minOpenglQtD10/minOpenglQt/helloworld/window.png");
		texturedata3 = QGLWidget::convertToGLFormat(buffer3);
		width = texturedata3.width();
		height = texturedata3.height();
		if (!texturedata3.isNull())
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texturedata3.bits());
			glGenerateMipmap(GL_TEXTURE_2D);
			//glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture when done, so we won't accidentily mess up our texture.
			qDebug() << "-------------------succese to load transparentTexture" << endl;
		}
		else
		{
			qDebug() << "-------------------Failed to load transparentTexture" << endl;
		}
	}


	glUseProgram(shaderProgram); 
	glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
	glUseProgram(shaderProgram2);
	glUniform1i(glGetUniformLocation(shaderProgram2, "screenTexture"), 0);

	// framebuffer configuration
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	// create a color attachment texture
	glGenTextures(1,& textureColorbuffer);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH ,HEIGHT , 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	// create a renderbuffer object for depth and stencil attachment (we won't be sampling these)
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	// use a single renderbuffer object for both a depth AND stencil buffer.
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	// now actually attach it
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);


	// now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	// bind to framebuffer and draw scene as we normally would to color texture 
	//glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
 
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT );
	
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
	// floor
	glBindVertexArray(planeVAO);
	glBindTexture(GL_TEXTURE_2D, floorTexture); 
	glDrawArrays(GL_TRIANGLES, 0, 6);

	//draw to screen
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f,0.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(shaderProgram2);
	glBindVertexArray(quadVAO);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);	// use the color attachment texture as the texture of the quad plane
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
	glUseProgram(0);
	glBindVertexArray(0);
	update();
};
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
