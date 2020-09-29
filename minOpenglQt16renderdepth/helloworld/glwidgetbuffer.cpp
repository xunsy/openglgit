
#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <stdio.h>
#include <QGLWidget>
#include "stb_image.h"
#define STB_IMAGE_IMPLEMENTATION
//#include"vshader.cpp"
//#include"frashader.cpp"
//extern const char *vertexShaderSource;
//extern const char *fragmentShaderSource;
QImage texturedata, buffer;
unsigned int texture1;
const char *vertexShaderSource = "#version 410 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"layout(location = 2) in vec2 aTexCoord;\n"
"out vec3 ourColor; \n"
"out vec2 TexCoord; \n"
"void main()\n"
"{\n"
"	gl_Position = vec4(aPos, 1.0);\n"
"   //gl_Position = vec4(aPos.x,aPos.y+ourColor.y,aPos.z, 1.0)+ourColor;\n"
"   ourColor = aColor;\n"
"	TexCoord = vec2(aTexCoord.x, aTexCoord.y); \n"
"}\0";
const char *fragmentShaderSource = "#version 410 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"in vec2 TexCoord; \n"
"uniform sampler2D texture1; \n"
"void main()\n"
"{\n"
"   FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);\n"
"}\0";
GLWidget::GLWidget(QWidget* parent /* = nullptr */, Qt::WindowFlags f /* = Qt::WindowFlags() */)
	:QOpenGLWidget(parent, f)
{
	setMinimumSize(300, 250);
}

GLWidget::~GLWidget()
{

}

void GLWidget::initializeGL()
{
	initializeOpenGLFunctions();
	//1.创建顶点着色器
	int vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
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
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);//获取错误信息  如果有的话
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		qDebug() << "ERROR::SHADER::fragmentShader::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//3.创建着色器程序

	shaderProgram = glCreateProgram();
	//4.链接顶点和片元着色器到着色器程序
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		qDebug() << "ERROR::Program::Program::COMPILATION_FAILED\n" << infoLog << endl;
	}

	//5.链接到程序对象后,删除着色器对象
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	float vertices[] = {
		// positions          // colors           // texture coords
		 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
	 0, 1, 3, // first triangle
	 1, 2, 3  // second triangle
	};
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	// 生成一个具有缓冲区ID的缓冲区 ID放在引用中

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


	//指定输入数据的哪个部分到顶点着色器中的哪个顶点属性
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute									步幅值		   	 偏移量
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	glGenTextures(1, &texture1);
	glBindTexture(GL_TEXTURE_2D, texture1);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// set texture filtering parameters  放大和缩小时分别如何采集数据
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	// replace it with your own image path.
	//unsigned char *data = stbi_load(QString("./container.jpg").toStdString().c_str(), &width, &height, &nrChannels, 0);


	bool HAdLOAD = buffer.load("D:/opengltest/minOpenglQtD4/minOpenglQt/helloworld/xlpic.jpg");
	texturedata = QGLWidget::convertToGLFormat(buffer);
	width = texturedata.width();
	height = texturedata.height();
	if (!texturedata.isNull())
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, texturedata.bits());
		glGenerateMipmap(GL_TEXTURE_2D);
		qDebug() << "-------------------succese to load texture" << endl;
	}
	else
	{
		qDebug() << "-------------------Failed to load texture" << endl;
	}
	greenValue = 0.0;
	up = true;
}

void GLWidget::paintGL()
{
	if (up) {
		greenValue = greenValue + 1;
		if (greenValue >= 120) {
			up = false;
		}
	}
	else if (!up) {
		greenValue = greenValue - 1;
		if (greenValue <= 0) {
			up = true;
		}
	}
	//5.使用该程序
	glUseProgram(shaderProgram);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glBindTexture(GL_TEXTURE_2D,   texture );
	int vertexColorLocation = glGetUniformLocation(shaderProgram, "texture1");
	glUniform1i(vertexColorLocation, 0);
	glActiveTexture(GL_TEXTURE0);


	glBindTexture(GL_TEXTURE_2D, texture1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}
void GLWidget::keyPressEvent(QKeyEvent *event) {
	if (event->key() == Qt::Key_Q) {

	}
}


void GLWidget::resizeGL(int w, int h)
{

}
