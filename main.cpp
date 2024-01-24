//Opis: Primjer ucitavanja modela upotrebom ASSIMP biblioteke
//Preuzeto sa learnOpenGL

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "model.hpp"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
glm::vec3 calculatePyramidNormal(glm::vec3 x, glm::vec3 y, glm::vec3 z);
unsigned int loadTexture(char const* path);

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 2.5f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.5f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);

glm::vec3 fishLocation = glm::vec3(-35.0f, -90.0f, -30.0f);
glm::vec3 fishOffSet = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 spotLightIntensity = glm::vec3(1.0f, 0.0f, 0.0f);

const unsigned int wWidth = 1280;
const unsigned int wHeight = 720;

float fov = 40.0f;

float deltaTime = 0.0f;	
float lastFrame = 0.0f;
float rotationAngle = 0.0f;

int main()
{
    if(!glfwInit())
    {
        std::cout << "GLFW fail!\n" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    float aspectRatio = static_cast<float>(wWidth) / wHeight;
    const char wTitle[] = "Pustinja";
    GLFWwindow* window = glfwCreateWindow(wWidth, wHeight, wTitle, glfwGetPrimaryMonitor(), NULL);
    if (window == NULL)
    {
        std::cout << "Window fail!\n" << std::endl;
        glfwTerminate();
        return -2;
    }
    glfwMakeContextCurrent(window);
    glViewport(0, 0, wWidth, wHeight);
    glfwSetScrollCallback(window, scroll_callback);

    if (glewInit() !=GLEW_OK)
    {
        std::cout << "GLEW fail! :(\n" << std::endl;
        return -3;
    }


    unsigned int VAO[3];
    glGenVertexArrays(3, VAO);
    unsigned int VBO[3];
    glGenBuffers(3, VBO);

    Shader unifiedShader("basic.vert", "basic.frag");
    Shader lightShader("lightCube.vert", "lightCube.frag");
    Shader modelShader("model.vert", "model.frag");
    Shader textureShader("texture.vert", "texture.frag");


    // --------------------------- IME I PREZIME ------------------------------
    float nameVertices[] =
    {   //X    Y      S    T 
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f 
    };

    unsigned int stride = (2 + 2) * sizeof(float);

    unsigned int indices[] =
    {
        0, 1, 3,
        0, 2, 1
    };

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(nameVertices), nameVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int EBO[3];
    glGenBuffers(3, EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned nameTexture = loadTexture("res/ime.png");
    glBindTexture(GL_TEXTURE_2D, nameTexture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    textureShader.setInt("uTex", 0);

    // ------------------- PIRAMIDE -----------------------------------
    glm::vec3 negZNormal = calculatePyramidNormal(glm::vec3(-0.25, 0.0, -0.25), glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.25, 0.0, -0.25));
    glm::vec3 posXNormal = calculatePyramidNormal(glm::vec3(0.25, 0.0, -0.25), glm::vec3(0.0, 0.5, 0.0), glm::vec3(0.25, 0.0, 0.25));
    glm::vec3 posZNormal = calculatePyramidNormal(glm::vec3(0.25, 0.0, 0.25), glm::vec3(0.0, 0.5, 0.0), glm::vec3(-0.25, 0.0, 0.25));
    glm::vec3 negXNormal = calculatePyramidNormal(glm::vec3(-0.25, 0.0, 0.25), glm::vec3(0.0, 0.5, 0.0), glm::vec3(-0.25, 0.0, -0.25));
    /*float pyramidVertices[] =
    {
       -0.25, 0.0, -0.25, -negZNormal.x,-negZNormal.y, -negZNormal.z,
        0.0,  0.5,  0.0,  -negZNormal.x,-negZNormal.y, -negZNormal.z,
        0.25, 0.0, -0.25, -negZNormal.x,-negZNormal.y, -negZNormal.z,
                          
        0.25, 0.0, -0.25, -posXNormal.x,-posXNormal.y, -posXNormal.z,
        0.0,  0.5,  0.0,  -posXNormal.x,-posXNormal.y, -posXNormal.z,
        0.25, 0.0,  0.25, -posXNormal.x,-posXNormal.y, -posXNormal.z,
                         
        0.25, 0.0, 0.25,  -posZNormal.x,-posZNormal.y, -posZNormal.z,
        0.0,  0.5, 0.0,   -posZNormal.x,-posZNormal.y, -posZNormal.z,
       -0.25, 0.0, 0.25,  -posZNormal.x,-posZNormal.y, -posZNormal.z,
                         
       -0.25, 0.0,  0.25, -negXNormal.x,-negXNormal.y, -negXNormal.z,
        0.0,  0.5,  0.0,  -negXNormal.x,-negXNormal.y, -negXNormal.z,
       -0.25, 0.0, -0.25, -negXNormal.x,-negXNormal.y, -negXNormal.z,
    };*/
    float pyramidVertices[] =
    {
       -0.25, 0.0, -0.25, 0.0, 0.7071, -0.7071,
        0.0,  0.5,  0.0,  0.0, 0.7071, -0.7071,
        0.25, 0.0, -0.25, 0.0, 0.7071, -0.7071,

        0.25, 0.0, -0.25, 0.7071, 0.7071, 0.0,
        0.0,  0.5,  0.0,  0.7071, 0.7071, 0.0,
        0.25, 0.0,  0.25, 0.7071, 0.7071, 0.0,

        0.25, 0.0, 0.25,  0.0, 0.7071, 0.7071,
        0.0,  0.5, 0.0,   0.0, 0.7071, 0.7071,
       -0.25, 0.0, 0.25,  0.0, 0.7071, 0.7071,

       -0.25, 0.0,  0.25, -0.7071, 0.7071, 0.0,
        0.0,  0.5,  0.0,  -0.7071, 0.7071, 0.0,
       -0.25, 0.0, -0.25, -0.7071, 0.7071, 0.0,
    };

    unsigned int stridePyramid = (3 + 3) * sizeof(float);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidVertices), pyramidVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stridePyramid, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stridePyramid, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // --------------------------- KOCKA ----------------------------
    float cubeVertices[] =
    {  
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int strideCube = (3 + 3) * sizeof(float);

    glBindVertexArray(VAO[2]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, strideCube, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, strideCube, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    Model ribica("Fish/12265_Fish_v1_L2.obj");
    
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    bool isPerspective = true;
    float lightBlue[] = { 0.6f, 0.8f, 1.0f };
    float darkBlue[] = { 0.0f, 0.0f, 0.4f };
    float rgb[3];
    float initialTime = glfwGetTime();
    bool pauseAnimation = false;
    float pauseStartTime = 0;
    float speed = 0.31f;
    float moonX, moonY, sunX, sunY;
    float starA = 0.0;
    float lightIntensity = 0.1f;
    bool movingDown = true;
    glm::vec3 lightDirection = glm::vec3(0.0, 0.0, 0.0);
    glm::vec3 lightPos1 = glm::vec3(0.0, 10.0, 0.0);
    glm::vec3 lightPos2 = glm::vec3(2.3f, 0.5f, -1.3f);
    glm::vec3 lightPos3 = glm::vec3(1.5f, 0.5f, 1.0f);

    glm::vec3 cubePositions[] = {
        glm::vec3(-52.0f,  60.4f,  -30.9f),
        glm::vec3(42.0f,  45.0f, -19.5f),
        glm::vec3(-30.5f, 50.2f, -33.5f),
        glm::vec3(10.5f, 50.2f, -33.5f),
        glm::vec3(-20.2f, 33.3f, -19.3f),
        glm::vec3(20.2f, 33.3f, -19.3f),
        glm::vec3(5.1f,  43.5f, -21.5f),
        glm::vec3(-26.7f, 51.4f, 26.5f),
        glm::vec3(52.3f, 47.0f, 36.3f),
        glm::vec3(-48.5f,  32.0f, 32.7f),
        glm::vec3(-34.5f,  29.9f, 21.1f),
        glm::vec3(51.3f,  36.2f, 0.9f),
        glm::vec3(9.3f,  26.2f, 10.0f)
    };

    while (!glfwWindowShouldClose(window))
    {
        glViewport(0, 0, wWidth, wHeight);
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
        {
            isPerspective = true;
        }
        if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
        {
            isPerspective = false;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        {
            initialTime = glfwGetTime();
            pauseAnimation = false;
            starA = 0.0;
        }
        if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !pauseAnimation)
        {
            pauseAnimation = true;
            pauseStartTime = glfwGetTime();
        }
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        {
            spotLightIntensity.x -= 0.3 * deltaTime;
        }
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !pauseAnimation)
        {
            spotLightIntensity.x += 0.3 * deltaTime;
        }

        glfwSetKeyCallback(window, key_callback);

        //-------------------KAMERA------------------------------

        float angleCameraRotation = glm::radians(rotationAngle);
        glm::mat3 rotationMatrix5 = glm::mat3(glm::rotate(glm::mat4(1.0f), angleCameraRotation, glm::vec3(0.0, 1.0, 0.0)));
        cameraPos = rotationMatrix5 * cameraPos;

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // ------------------NEBO-----------------------

            if (!pauseAnimation) {
                float t = 0.5f * (1.0f + sin((glfwGetTime() - initialTime) * speed));

                for (int i = 0; i < 3; ++i) {
                    rgb[i] = (1.0f - t) * darkBlue[i] + t * lightBlue[i];
                }


            }
            else {
                float t = 0.5f * (1.0f + sin((pauseStartTime - initialTime) * speed));

                for (int i = 0; i < 3; ++i) {
                    rgb[i] = (1.0f - t) * darkBlue[i] + t * lightBlue[i];
                }
            }

        glClearColor(rgb[0], rgb[1], rgb[2], 1.0);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!pauseAnimation) {
            sunX = -(11.05 * cos((glfwGetTime() - initialTime) * speed));
            sunY = 5.5 * sin((glfwGetTime() - initialTime) * speed);
        }
        else {
            sunX = -(11.05 * cos((pauseStartTime - initialTime) * speed));
            sunY = 5.5 * sin((pauseStartTime - initialTime) * speed);
        }

        if (!pauseAnimation) {
            moonX = -(13.05 * cos(3.14 + (glfwGetTime() - initialTime) * speed));
            moonY = 5.5 * sin(3.14 + (glfwGetTime() - initialTime) * speed);
        }
        else {
            moonX = -(13.05 * cos(3.14 + (pauseStartTime - initialTime) * speed));
            moonY = 5.5 * sin(3.14 + (pauseStartTime - initialTime) * speed);
        }

        if (sunY >= 0) {
            lightDirection = -glm::vec3(sunX, sunY, 0.0);
            
            if (sunX <= 0){
                lightIntensity += 0.004 * 30.0 * deltaTime;
            }
            if (sunX > 0) {
                lightIntensity -= 0.004 * 30.0 * deltaTime;
            }
        }
        else {
            lightDirection = -glm::vec3(moonX, moonY, 0.0);
            if (sunX >= 0) {
                lightIntensity += 0.001 * 20.0 * deltaTime;
            }
            if (sunX < 0){
                lightIntensity -= 0.001 * 20.0 * deltaTime;
            }
        }

        unifiedShader.use();
        unifiedShader.setVec3("light.direction", lightDirection);
        unifiedShader.setVec3("viewPos", cameraPos);

        // light properties
        unifiedShader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
        unifiedShader.setVec3("light.diffuse", lightIntensity, lightIntensity, lightIntensity);
        unifiedShader.setVec3("light.specular", 0.5f, 0.5f, 0.5f);

        unifiedShader.setFloat("pointLights[0].constant", 1.0f);
        unifiedShader.setFloat("pointLights[0].linear", 1.9f);
        unifiedShader.setFloat("pointLights[0].quadratic", 0.2f);
        unifiedShader.setVec3("pointLights[0].ambient", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[0].diffuse", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[0].specular", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[0].position", 0.0f, 0.5f, 0.0f);

        unifiedShader.setFloat("pointLights[1].constant", 1.0f);
        unifiedShader.setFloat("pointLights[1].linear", 1.9f);
        unifiedShader.setFloat("pointLights[1].quadratic", 0.2f);
        unifiedShader.setVec3("pointLights[1].ambient", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[1].diffuse", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[1].specular", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[1].position", 0.5f, 0.3f, 0.35f);

        unifiedShader.setFloat("pointLights[2].constant", 1.0f);
        unifiedShader.setFloat("pointLights[2].linear", 1.9f);
        unifiedShader.setFloat("pointLights[2].quadratic", 0.2f);
        unifiedShader.setVec3("pointLights[2].ambient", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[2].diffuse", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[2].specular", 0.0f, 0.3f, 0.0f);
        unifiedShader.setVec3("pointLights[2].position", 1.0f, 0.3f, -0.55f);

        glm::vec3 spotlightPosition = glm::vec3(0.0f, 2.0f, 0.0f);
        glm::vec3 fishPos = glm::vec3(fishLocation + glm::vec3(0.0, 0.0, fishOffSet.x));
        glm::vec3 spotlightDirection = glm::normalize(fishPos - spotlightPosition);

        unifiedShader.setVec3("spotLight.position", spotlightPosition);
        unifiedShader.setVec3("spotLight.direction", spotlightDirection);
        unifiedShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(4.5f)));
        unifiedShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(5.0f)));
        unifiedShader.setVec3("spotLight.ambient", spotLightIntensity);
        unifiedShader.setVec3("spotLight.diffuse", spotLightIntensity);
        unifiedShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        unifiedShader.setFloat("spotLight.constant", 1.0f);
        unifiedShader.setFloat("spotLight.linear", 0.09f);
        unifiedShader.setFloat("spotLight.quadratic", 0.032f);

        // view
        processInput(window);
        glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
        unifiedShader.setMat4("uV", view);
       
        // projection
        if (isPerspective) {
            glm::mat4 projectionP = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
            unifiedShader.setMat4("uP", projectionP);
        }
        else {
            glm::mat4 projectionO = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, 0.1f, 100.0f);
            unifiedShader.setMat4("uP", projectionO);
        }

        glm::vec3 changingPosition = glm::vec3(0.0f, 0.0f, 0.0f);
        unifiedShader.setVec3("uLoc", changingPosition);

        glBindVertexArray(VAO[2]);
        // ------------------------------ PUSTINJA ------------------------------------
        // material properties
        unifiedShader.setVec3("material.ambient", 0.3, 0.3, 0.3);
        unifiedShader.setVec3("material.diffuse", 0.76, 0.54, 0.0);
        unifiedShader.setVec3("material.specular", 0.2f, 0.2f, 0.2f); 
        unifiedShader.setFloat("material.shininess", 16.0f);
        
        glm::mat4 modelDesert = glm::mat4(1.0f);
        modelDesert = glm::scale(modelDesert, glm::vec3(2.5, 1/5.0, 2.0));
        modelDesert = glm::translate(modelDesert, glm::vec3(0.3, -0.5, 0.0));
        unifiedShader.setMat4("uM", modelDesert);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // -------------------------- OSTRVA --------------------------------
        glm::mat4 modelIsland1 = glm::mat4(1.0f);
        modelIsland1 = glm::scale(modelIsland1, glm::vec3(1/5.0, 1 / 10.0, 1/3.0));
        modelIsland1 = glm::translate(modelIsland1, glm::vec3(-7.0, -0.3, -0.5));
        unifiedShader.setMat4("uM", modelIsland1);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 modelIsland2 = glm::mat4(1.0f);
        modelIsland2 = glm::scale(modelIsland2, glm::vec3(1 / 4.0, 1 / 10.0, 1 / 5.0));
        modelIsland2 = glm::translate(modelIsland2, glm::vec3(-4.2, -0.3, 3.0));
        unifiedShader.setMat4("uM", modelIsland2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 modelIsland3 = glm::mat4(1.0f);
        modelIsland3 = glm::scale(modelIsland3, glm::vec3(1 / 9.0, 1 / 10.0, 1 / 9.0));
        modelIsland3 = glm::translate(modelIsland3, glm::vec3(-10.2, -0.3, -6.0));
        unifiedShader.setMat4("uM", modelIsland3);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glBindVertexArray(VAO[1]);
        // ------------------------------ PIRAMIDA 1 ----------------------------------------------
        // material properties
        unifiedShader.setVec3("material.ambient", 0.3, 0.3, 0.3);
        unifiedShader.setVec3("material.diffuse", 0.75, 0.58, 0.0);
        unifiedShader.setVec3("material.specular", 0.7f, 0.7f, 0.7f);
        unifiedShader.setFloat("material.shininess", 32.0f);

        
        glm::mat4 modelPyr1 = glm::mat4(1.0f);
        modelPyr1 = glm::scale(modelPyr1, glm::vec3(1 / 1.5, 1 / 1.5, 1 / 1.5));
        unifiedShader.setMat4("uM", modelPyr1);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        // ------------------------------ PIRAMIDA 2 ----------------------------------------------
        glm::mat4 modelPyr2 = glm::mat4(1.0f);
        modelPyr2 = glm::scale(modelPyr2, glm::vec3(1 / 2.3, 1 / 2.3, 1 / 2.3));
        modelPyr2 = glm::translate(modelPyr2, glm::vec3(2.3, 0.0, -1.3));
        unifiedShader.setMat4("uM", modelPyr2);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        // ------------------------------ PIRAMIDA 3 ----------------------------------------------
        glm::mat4 modelPyr3 = glm::mat4(1.0f);
        modelPyr3 = glm::scale(modelPyr3, glm::vec3(1 / 3.0, 1 / 3.0, 1 / 3.0));
        modelPyr3 = glm::translate(modelPyr3, glm::vec3(1.5, 0.0, 1.0));
        unifiedShader.setMat4("uM", modelPyr3);
        glDrawArrays(GL_TRIANGLES, 0, 12);

        modelShader.use();
        modelShader.setMat4("uV", view);
        if (isPerspective) {
            glm::mat4 projectionP1 = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
            modelShader.setMat4("uP", projectionP1);
        }
        else {
            glm::mat4 projectionO1 = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, 0.1f, 100.0f);
            modelShader.setMat4("uP", projectionO1);
        }

        // light properties
        modelShader.setVec3("light.ambient", 0.2f, 0.2f, 0.2f);
        modelShader.setVec3("light.diffuse", lightIntensity, lightIntensity, lightIntensity);
        modelShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        modelShader.setVec3("uViewPos", cameraPos);
        modelShader.setVec3("light.direction", lightDirection);

        modelShader.setInt("material.diffuse", 0);
        modelShader.setInt("material.specular", 1);
        modelShader.setFloat("material.shininess", 32.0f);

        modelShader.setVec3("spotLight.position", spotlightPosition);
        modelShader.setVec3("spotLight.direction", spotlightDirection);
        modelShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(4.5f)));
        modelShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(5.0f)));
        modelShader.setVec3("spotLight.ambient", spotLightIntensity);
        modelShader.setVec3("spotLight.diffuse", spotLightIntensity);
        modelShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        modelShader.setFloat("spotLight.constant", 1.0f);
        modelShader.setFloat("spotLight.linear", 0.09f);
        modelShader.setFloat("spotLight.quadratic", 0.032f);


        if (movingDown) {
            fishOffSet.x += 0.10 * 100.0 * deltaTime;
            modelShader.setVec3("uLoc", fishOffSet);
            if ((-30.0 + fishOffSet.x) >= 25.0) {
                movingDown = !movingDown;
            }
        }
        if (!movingDown) {
            fishOffSet.x -= 0.10 * 100 * deltaTime;
            modelShader.setVec3("uLoc", fishOffSet);
            if ((25 + fishOffSet.x) <= 30) {
                movingDown = !movingDown;
            }
        }
        modelShader.setVec3("uLoc", fishOffSet);

        // --------------------------- RIBICA -------------------------------------
        glm::mat4 modelFish = glm::mat4(1.0f);
        modelFish = glm::rotate(modelFish, glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
        modelFish = glm::rotate(modelFish, glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
        modelFish = glm::rotate(modelFish, glm::radians(-180.0f), glm::vec3(0.0, 1.0, 0.0));
        modelFish = glm::scale(modelFish, glm::vec3(1 / 60.0, 1 / 60.0, 1 / 60.0));
        modelFish = glm::translate(modelFish, glm::vec3(-30.0, -50.0, 0.0));
        modelShader.setMat4("uM", modelFish);
       
        ribica.Draw(modelShader);
        
        lightShader.use();
        lightShader.setMat4("uV", view);
        if (isPerspective) {
            glm::mat4 projectionP1 = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.0f);
            lightShader.setMat4("uP", projectionP1);
        }
        else {
            glm::mat4 projectionO1 = glm::ortho(-1.0f * aspectRatio, 1.0f * aspectRatio, -1.0f, 1.0f, 0.1f, 100.0f);
            lightShader.setMat4("uP", projectionO1);
        }
        lightShader.setVec3("uLoc", changingPosition);


        glBindVertexArray(VAO[2]);
        // ------------------------------ VODA ------------------------------------
        glm::mat4 modelWater = glm::mat4(1.0f);
        modelWater = glm::scale(modelWater, glm::vec3(1.3, 1/5.0, 2.0));
        modelWater = glm::translate(modelWater, glm::vec3(-0.89, -0.5, 0.0));
        lightShader.setMat4("uM", modelWater);
        lightShader.setVec4("uColor", 0.6, 0.8, 0.9, 0.6);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // --------------------- TACKASTA OSVETLJENJA ----------------------------
        lightShader.setVec4("uColor", 0.0, 1.0, 0.0, 1.0);
        
        glm::mat4 modelLight1 = glm::mat4(1.0f);
        modelLight1 = glm::scale(modelLight1, glm::vec3(1 / 50.0, 1 / 50.0, 1 / 50.0));
        modelLight1 = glm::translate(modelLight1, glm::vec3(0.0f, 0.5f, 0.0f) * 50.0f);
        lightShader.setMat4("uM", modelLight1);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 modelLight2 = glm::mat4(1.0f);
        modelLight2 = glm::scale(modelLight2, glm::vec3(1 / 50.0, 1 / 50.0, 1 / 50.0));
        modelLight2 = glm::translate(modelLight2, glm::vec3(0.5f, 0.3f, 0.35f) * 50.0f);
        lightShader.setMat4("uM", modelLight2);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glm::mat4 modelLight3 = glm::mat4(1.0f);
        modelLight3 = glm::scale(modelLight3, glm::vec3(1 / 50.0, 1 / 50.0, 1 / 50.0));
        modelLight3 = glm::translate(modelLight3, glm::vec3(1.0f, 0.3f, -0.55f) * 50.0f);
        lightShader.setMat4("uM", modelLight3);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        
        
        if (moonY >= 0.0 && moonX <= 0.0 && starA < 1.0) {
            starA += 0.2 * deltaTime;
        }
        else if (moonY >= 0.0 && moonX <= 1.0 && starA == 1.0) {
            starA = 1.0;
        }
        else if (moonY >= 0.0 && moonX >= 1.0) {
            starA -= 0.3 * deltaTime;
        }
        else if (moonY < 0.0) {
            starA = 0.0;
        }

        lightShader.setVec4("uColor", 1.0, 1.0, 1.0, starA);

        // --------------------- ZVEZDE --------------------------------
        for (unsigned int i = 0; i < 12; i++)
        {
            glm::mat4 modelStar = glm::mat4(1.0f);
            modelStar = glm::scale(modelStar, glm::vec3(1 / 50.0, 1 / 50.0, 1 / 50.0));
            modelStar = glm::translate(modelStar, cubePositions[i]);
            lightShader.setMat4("uM", modelStar);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }


        // ------------------ SUNCE --------------------------
        lightShader.setVec3("uLoc", sunX, sunY, 0.0);
        glm::mat4 modelSun = glm::mat4(1.0f);
        modelSun = glm::scale(modelSun, glm::vec3(1 / 5.0, 1 / 5.0, 1 / 5.0));
        lightShader.setMat4("uM", modelSun);
        lightShader.setVec4("uColor", 1.0, 1.0, 0.8, 1.0);
        if(sunY >= -2)
            glDrawArrays(GL_TRIANGLES, 0, 36);

        // --------------------- MESEC ----------------------------
        lightShader.setVec3("uLoc", moonX, moonY, 0.0);
        glm::mat4 modelMoon = glm::mat4(1.0f);
        modelMoon = glm::scale(modelMoon, glm::vec3(1 / 6.0, 1 / 6.0, 1 / 6.0));
        lightShader.setMat4("uM", modelMoon);
        lightShader.setVec4("uColor", 1.0, 1.0, 1.0, 1.0);
        if(moonY >= -2)
            glDrawArrays(GL_TRIANGLES, 0, 36);

        // ime i prezime
        glViewport(-1.0, 0.7, 400, 50);

        textureShader.use();
        glBindVertexArray(VAO[0]);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, nameTexture);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(0 * sizeof(unsigned int)));
        glBindTexture(GL_TEXTURE_2D, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(3, VBO);
    glDeleteVertexArrays(3, VAO);

    glfwTerminate();
    return 0;
}

glm::vec3 calculatePyramidNormal(glm::vec3 first, glm::vec3 second, glm::vec3 third) {
    return glm::normalize(glm::cross((second - first), (third - first)));
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos.z -= 1.0 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos.z += 1.0 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos.x -= 1.0 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos.x += 1.0 * deltaTime;
    

    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        rotationAngle -= deltaTime * 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        rotationAngle += deltaTime * 0.1;
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_L && action == GLFW_RELEASE) {
        rotationAngle = 0.0f;
    }
    if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
        rotationAngle = 0.0f;
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 40.0f)
        fov = 40.0f;
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
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
}
