#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_5_Core>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui {
	class CMainWindow;
}
QT_END_NAMESPACE

class CMainWindow : public QOpenGLWidget, protected QOpenGLFunctions_4_5_Core
{
	Q_OBJECT

public:
	CMainWindow(QWidget *parent = Q_NULLPTR);
	~CMainWindow();

	void initFBO();

protected:
	void initializeGL();
	void paintGL();
	void resizeGL(int w, int h);

private:
	QImage m_img;
	GLsizei m_width = 0;
	GLsizei m_height = 0;
  GLsizeiptr m_dataSize = 0;
	GLuint m_VBO = 0;
	GLuint m_VAO = 0;
	GLuint m_EBO = 0;
	GLuint m_texture = 0;
	GLuint m_frameBuffer = 0;
	GLuint m_RBO = 0;
	GLuint m_fVBO = 0;
	GLuint m_fVAO = 0;
	GLuint m_textureFBO = 0;
	QOpenGLShaderProgram *m_programScreen = nullptr;
	QOpenGLShaderProgram *m_shaderProgram = nullptr;
private:
	Ui::CMainWindow *ui;
};
#endif //MAINWINDOW_H
