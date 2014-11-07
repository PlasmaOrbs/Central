#include "Shader.h"

//Load it from a memory buffer
GLuint loadShaderFromMemory(const char * pMem, SHADER_TYPE enumShaderType)
{
	//Create program with specified shader type
	GLuint gluiProgram = glCreateShader(enumShaderType);

	/*Copy the character array containing the shader source
	  into the shader program */
	glShaderSource(gluiProgram, 1, &pMem, NULL);

	//Compile the shader
	glCompileShader(gluiProgram);

	//Run check to see if shader has compiled correctly
	if (checkForCompilerErrors(gluiProgram))
	{
		return	0;
	}

	//Return the shader
	return gluiProgram;
}

GLuint loadShaderFromFile(const string& fileName, SHADER_TYPE shaderType)
{
	#ifdef __APPLE__
	filename = filename.substr(filename.rfind('/') + 1);
	#endif

	string strFileContents;
	ifstream ifstreamFile; //used in order to open the files
	ifstreamFile.open(fileName.c_str(), ios::in);
	if (!ifstreamFile)
	{
		return	0;
	}
	//calculate file size
	if (ifstreamFile.good()) //if ifstreamFile.open successful
	{
		ifstreamFile.seekg(0, ios::end); // "Seek" to end of file, enables length of file
		unsigned long ulFileLength = ifstreamFile.tellg(); //Set file length
		ifstreamFile.seekg(ios::beg); // "Seek" to the beginning of file

		//if file length = 0 file has no contents
		if (ulFileLength == 0)
		{
			cout << "File has no contents" << endl;
			return	0;
		}

		strFileContents.resize(ulFileLength); //Resizes string to fit file
		ifstreamFile.read(&strFileContents[0], ulFileLength); //READ FILE
		ifstreamFile.close(); //CLOSE FILE
		GLuint gluiProgram = loadShaderFromMemory(strFileContents.c_str(), shaderType);
		return gluiProgram;
	}
	return	0;
}

bool checkForCompilerErrors(GLuint gluiShaderProgram)
{
	//set true or false if Shader has been compiled
	GLint gliIsCompiled = 0;

	/* Gets parameters of a shader object

	   glGetShaderiv(SHADER_OBJECT, ATTEMPTING_TO_RETRIEVE, [1] or [2])
	   
	   -[1] GL_COMPILE_STATUS - returns GL_TRUE if compile was
				successful or GL_FALSE if not
	   -[2] GL_INFO_LOG_LENGTH - returns the number of characters in
				the log that is generated*/
	glGetShaderiv(gluiShaderProgram, GL_COMPILE_STATUS, &gliIsCompiled);

	//if not compiled correctly glIsCompiled = 0
	if (gliIsCompiled == GL_FALSE)
	{
		GLint gliMaxLength = 0; //used to store error message length
		//error found so get length of log
		glGetShaderiv(gluiShaderProgram, GL_INFO_LOG_LENGTH, &gliMaxLength);

		//The maxLength includes the NULL character
		string strInfoLog; //Used to store error message
		strInfoLog.resize(gliMaxLength); //Resize 
		//fills out string which for holding error message
		glGetShaderInfoLog(gluiShaderProgram, gliMaxLength, &gliMaxLength, &strInfoLog[0]);
		cout << "Shader not compiled: " << strInfoLog << endl;

		//We don't need the shader anymore.
		glDeleteShader(gluiShaderProgram);
		return true;

	}
	return false;
}

bool checkForLinkErrors(GLuint gluiProgram)
{
	GLint isLinked = 0;
	glGetProgramiv(gluiProgram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{

		GLint maxLength = 0;
		glGetProgramiv(gluiProgram, GL_INFO_LOG_LENGTH, &maxLength);

		//The maxLength includes the NULL character
		string infoLog;
		glGetShaderInfoLog(gluiProgram, maxLength, &maxLength, &infoLog[0]);

		cout << "Shader	not	linked " << infoLog << endl;

		//We don't need the shader anymore.
		glDeleteProgram(gluiProgram);
		return	true;
	}

	return	false;
}