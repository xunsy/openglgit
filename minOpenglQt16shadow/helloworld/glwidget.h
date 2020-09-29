#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>
#include <QMouseEvent>
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)
QT_FORWARD_DECLARE_CLASS(QOpenGLShader)
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
    Q_OBJECT

public:
	/*QVector<float> vertices = {
	-0.5f, -0.5f, 0.0f,
	 0.5f, -0.5f, 0.0f,
	 0.0f,  0.5f, 0.0f
	};*/
	/*float vertices[18] = {
		// first triangle
		 0.5f,  0.5f, 0.0f,  // top right
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f,  0.5f, 0.0f,  // top left 
		// second triangle
		 0.5f, -0.5f, 0.0f,  // bottom right
		-0.5f, -0.5f, 0.0f,  // bottom left
		-0.5f,  0.5f, 0.0f   // top left
	};*/
	bool starflage = false;
	int greenValue; bool up;
	unsigned int shaderProgram;
	unsigned int shaderProgram2;
	unsigned int shaderProgram3;
	unsigned int framebuffer;
	unsigned int textureColorbuffer;
	unsigned int rbo;
	QVector3D lightPos; 
	unsigned int woodTexture;
	unsigned int cubeVAO=0;//¶¥µã vertex array object (also known as VAO)
	unsigned int cubeVBO=0;//ÔªËØ vertex buffer objects (VBO)
	unsigned int EBO;//Ë÷Òý
	unsigned int planeVAO, planeVBO;
    GLWidget(QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~GLWidget();

protected:
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void initializeGL() override;
	void keyPressEvent(QKeyEvent *event);
	void keyReleaseEvent(QKeyEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	unsigned int  loadCubemap(QStringList faces);
	void dosome();
	void renderCube();
	void renderScene(unsigned int Program);
	unsigned int initshaderprogramer(const char *vshader, const char *fshader);
};

#endif
