GL_PROC(base_opengl,glEnable,,void,(GLenum cap))
GL_PROC(base_opengl,glDisable,,void,(GLenum cap))
GL_PROC(base_opengl,glGetIntegerv,,void,(GLenum pname, GLint *params))
GL_PROC(base_opengl,glGetString,,const GLubyte *,(GLenum name))
GL_PROC(base_opengl,glGetError,,GLenum,(void))
GL_PROC(base_opengl,glDepthMask,,void,(GLboolean flag))
GL_PROC(base_opengl,glDepthRange,,void,(GLclampd zNear, GLclampd zFar))
GL_PROC(base_opengl,glBlendFunc,,void,(GLenum sfactor, GLenum dfactor))
GL_PROC(base_opengl,glAlphaFunc,,void,(GLenum func, GLclampf ref))//**
GL_PROC(base_opengl,glDepthFunc,,void,(GLenum func))
GL_PROC(base_opengl,glMatrixMode,,void,(GLenum mode))//**
GL_PROC(base_opengl,glMultMatrixf,,void,(GLfloat *m))//**
GL_PROC(base_opengl,glLoadIdentity,,void,(void))//**
GL_PROC(base_opengl,glScalef,,void,(GLfloat x, GLfloat y, GLfloat z))//**
GL_PROC(base_opengl,glViewport,,void,(GLint x, GLint y, GLsizei width, GLsizei height))
GL_PROC(base_opengl,glTranslatef,,void,(GLfloat x, GLfloat y, GLfloat z))//**
GL_PROC(base_opengl,glRotatef,,void,(GLfloat angle, GLfloat x, GLfloat y, GLfloat z))//**
GL_PROC(base_opengl,glOrtho,,void,(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar))//**
GL_PROC(base_opengl,glScissor,,void,(GLint x, GLint y, GLsizei width, GLsizei height))
GL_PROC(base_opengl,glClearColor,,void,(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha))
GL_PROC(base_opengl,glClearDepth,,void,(GLclampd depth))
GL_PROC(base_opengl,glClear,,void,(GLbitfield mask))
GL_PROC(base_opengl,glPixelStorei,,void,(GLenum pname, GLint param))
GL_PROC(base_opengl,glBindTexture,,void,(GLenum target, GLuint texture))
GL_PROC(base_opengl,glTexEnvi,,void,(GLenum target, GLenum pname, GLint param))//**
GL_PROC(base_opengl,glTexParameterf,,void,(GLenum target, GLenum pname, GLfloat param))
GL_PROC(base_opengl,glTexParameteri,,void,(GLenum target, GLenum pname, GLint param))
GL_PROC(base_opengl,glTexImage2D,,void,(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels))
GL_PROC(base_opengl,glTexSubImage2D,,void,(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels))
GL_PROC(base_opengl,glGenTextures,,void,(GLsizei n, GLuint *textures))
GL_PROC(base_opengl,glDeleteTextures,,void,(GLsizei n, const GLuint *textures))
GL_PROC(base_opengl,glVertexPointer,,void,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer))//**
GL_PROC(base_opengl,glColorPointer,,void,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer))//**
GL_PROC(base_opengl,glTexCoordPointer,,void,(GLint size, GLenum type, GLsizei stride, const GLvoid *pointer))//**
GL_PROC(base_opengl,glDrawArrays,,void,(GLenum mode, GLint first, GLsizei count))
GL_PROC(base_opengl,glDrawElements,,void,(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices))
GL_PROC(base_opengl,glEnableClientState,,void,(GLenum array))//**
GL_PROC(base_opengl,glFinish,,void,(void))//**
GL_PROC(base_opengl,glReadPixels,,void,(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels))

GL_PROC(GL_EXT_framebuffer_object,glBindRenderbufferEXT,,void,(GLenum target, GLuint name))
GL_PROC(GL_EXT_framebuffer_object,glDeleteRenderbuffersEXT,,void,(GLsizei, const GLuint *))
GL_PROC(GL_EXT_framebuffer_object,glGenRenderbuffersEXT,,void,(GLsizei, GLuint *))
GL_PROC(GL_EXT_framebuffer_object,glRenderbufferStorageEXT,,void,(GLenum, GLenum, GLsizei, GLsizei))
GL_PROC(GL_EXT_framebuffer_object,glBindFramebufferEXT,,void,(GLenum, GLuint))
GL_PROC(GL_EXT_framebuffer_object,glDeleteFramebuffersEXT,,void,(GLsizei, const GLuint *))
GL_PROC(GL_EXT_framebuffer_object,glGenFramebuffersEXT,,void,(GLsizei, GLuint *))
GL_PROC(GL_EXT_framebuffer_object,glCheckFramebufferStatusEXT,,GLenum,(GLenum))
GL_PROC(GL_EXT_framebuffer_object,glFramebufferRenderbufferEXT,,void,(GLenum, GLenum, GLenum, GLuint))
GL_PROC(GL_EXT_framebuffer_object,glFramebufferTexture2DEXT,,void,(GLenum, GLenum, GLenum, GLuint, GLint))

GL_PROC(GL_ARB_vertex_buffer_object,glBindBufferARB,,void,(GLenum, GLuint))
GL_PROC(GL_ARB_vertex_buffer_object,glDeleteBuffersARB,,void,(GLsizei, const GLuint *))
GL_PROC(GL_ARB_vertex_buffer_object,glGenBuffersARB,,void,(GLsizei, GLuint *))
GL_PROC(GL_ARB_vertex_buffer_object,glBufferDataARB,,void,(GLenum, GLsizeiptr, const GLvoid *, GLenum))
GL_PROC(GL_ARB_vertex_buffer_object,glMapBufferARB,,GLvoid*,(GLenum, GLenum))
GL_PROC(GL_ARB_vertex_buffer_object,glUnmapBufferARB,,GLboolean,(GLenum))
