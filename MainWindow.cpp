#include "MainWindow.h"

CMainWindow::CMainWindow(QWidget* parent)
	: QOpenGLWidget(parent), ui(new Ui::CMainWindow)
{
	ui->setupUi(this);
	m_img = QImage("2.jpg");//picture
	m_width = m_img.width();
	m_height = m_img.height();
	m_dataSize = m_width * m_height * 3;
}

CMainWindow::~CMainWindow()
{
	delete ui;
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_VBO);
	glDeleteBuffers(1, &m_EBO);
	glDeleteVertexArrays(1, &m_fVAO);
	glDeleteBuffers(1, &m_fVBO);
	glDeleteBuffers(1, &m_frameBuffer);
	glDeleteBuffers(1, &m_RBO);
}
void CMainWindow::initFBO()
{
	glGenFramebuffers(1, &m_frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);

	glGenTextures(1, &m_textureFBO);
	glBindTexture(GL_TEXTURE_2D, m_textureFBO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);//TODO
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureFBO, 0);

	glGenRenderbuffers(1, &m_RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		qDebug() << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" ;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	char vertexShaderSource[] =
		"#version 450 core\n"
		"layout (location = 0) in vec2 aPos;\n"
		"layout (location = 1) in vec2 aTexCoords;\n"
		"out vec2 TexCoords;\n"
		"void main()\n"
		"{\n"
		"   gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
		"TexCoords = aTexCoords;\n"
		"}\n";
	char fragmentShaderSource[] =
		"#version 450 core\n"
		"out vec4 FragColor;\n"
		"in vec2 TexCoords;\n"
		"uniform sampler2D screenTexture;\n"
		"void main()\n"
		"{\n"
		"   FragColor = texture(screenTexture, TexCoords);\n"
		"}\n";

	m_programScreen = new QOpenGLShaderProgram;
	m_programScreen->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_programScreen->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_programScreen->link();

	m_programScreen->bind();
	m_programScreen->setUniformValue("screenTexture", 1);
	m_programScreen->release();

	float fb_Vertices[] = {
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	glGenVertexArrays(1, &m_fVAO);
	glBindVertexArray(m_fVAO);
	glGenBuffers(1, &m_fVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_fVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fb_Vertices), fb_Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(float)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void CMainWindow::initializeGL()
{
	this->initializeOpenGLFunctions();

	char vertexShaderSource[] =
		"#version 450 core\n"
		"layout(location = 0) in vec3 aPos;\n"
		"layout(location = 1) in vec3 aColor;\n"
		"layout(location = 2) in vec2 aTexcood;\n"
		"out vec3 vertexColor;\n"
		"out vec2 m_tex;\n"
		"void main()\n"
		"{\n"
		"   m_tex = aTexcood;\n"
		"   vertexColor = aColor;\n"
		"   gl_Position = vec4(aPos, 1.0);\n"
		"}\n";

	char fragmentShaderSource[] =
		"#version 450 core\n"
		"out vec4 FragColor;\n"
		"in vec3 vertexColor;\n"
		"in vec2 m_tex;\n"
		"uniform sampler2D ourTexture;\n"
		"void main()\n"
		"{\n"
		"   FragColor = texture2D(ourTexture, m_tex) * vec4(vertexColor, 1.0f);\n"
		"}\n";

	m_shaderProgram = new QOpenGLShaderProgram;
	m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Vertex, vertexShaderSource);
	m_shaderProgram->addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentShaderSource);
	m_shaderProgram->link();

	m_shaderProgram->bind();
	m_shaderProgram->setUniformValue("ourTexture", 0);
	m_shaderProgram->release();

	GLfloat vertices[] = {
		-1.0f, -1.0f,0.0f, 0.0f,0.0f,1.0f, 0.0f,0.0f,
		 1.0f, -1.0f,0.0f, 0.0f,1.0f,0.0f, 1.0f,0.0f,
		 1.0f,  1.0f,0.0f, 1.0f,0.0f,0.0f, 1.0f,1.0f,
		-1.0f,  1.0f,0.0f, 0.0f,1.0f,1.0f, 0.0f,1.0f,
	};

	GLuint indices[] = {
		0,1,2,
		0,2,3
	};

	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenBuffers(1, &m_EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, m_img.bits());//TODO
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	//FBO
	initFBO();//TODO
	
}

void CMainWindow::paintGL()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
	glEnable(GL_DEPTH_TEST);

	glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shaderProgram->bind();
	glBindVertexArray(m_VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_shaderProgram->release();

	/********** 关键之处 ***********/
	GLuint fb = context()->defaultFramebufferObject();
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	glDisable(GL_DEPTH_TEST);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, m_width, m_height);

	m_programScreen->bind();
	glBindVertexArray(m_fVAO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_textureFBO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	m_programScreen->release();

}

void CMainWindow::resizeGL(int w, int h)
{
	glViewport(0, 0, w, h);
}
