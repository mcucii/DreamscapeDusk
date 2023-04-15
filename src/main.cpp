
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(const char *path);
unsigned int loadCubemap(vector<std::string> faces);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//lighting
glm::vec3 lightPos(-5.1f, -0.22f, 0.9f);
glm::vec3 lightPosTable(-1.5f, -0.3f, -2.2f);

glm::vec3 dirLightPos(-0.9f, 0.2f, -3.0f);
//glm::vec3 cubePos(0.0f, -0.29f, 0.0f);




// additional
glm::vec3 birdPos(-2.7f, -0.345f, -1.8f);
int num_of_walls = 10;

// keys
bool spotlightPressed = false;
bool blinnPressed = false;



int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

// glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
//    glfwSetKeyCallback(window, key_callback);

// tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);


    //    stbi_set_flip_vertically_on_load(true);



    // build and compile shaders
    // -------------------------
    Shader skyBoxShader("resources/shaders/skybox.vs", "resources/shaders/skybox.fs");
    Shader lightingShader("resources/shaders/model_lighting.vs", "resources/shaders/model_lighting.fs");
    Shader lightSourceShader("resources/shaders/light_source.vs", "resources/shaders/light_source.fs");
    Shader blendingShader("resources/shaders/blending_shader.vs", "resources/shaders/blending_shader.fs");


    // ground
    float groundVertices[] = {
            // positions          // normals        //texCoords
            0.5f,  0.5f,  0.0f,   0.0f, 0.0f, -1.0f, 1.0f,  1.0f,     // top right
            0.5f, -0.5f,  0.0f,   0.0f, 0.0f, -1.0f, 1.0f,  0.0f,     // bottom right
            -0.5f, -0.5f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  0.0f,     // bottom left
            -0.5f,  0.5f,  0.0f,  0.0f, 0.0f, -1.0f, 0.0f,  1.0f      // top left
    };

    // ground vertices for use in EBO
    unsigned int groundIndices[] = {
            0, 1, 3,  // first Triangle
            1, 2, 3   // second Triangle
    };


    // cube
    float cubeVertices[] = {
            // positions          // normals           // texture coords
            //donja
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,         // suprotno
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            //gornja
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,         // suprotno
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

            // leva
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,


            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,


            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,


            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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

    // skybox coordinates
    float skyBoxVertices[] = {
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


    //ground
    unsigned int groundVAO, groundVBO, groundEBO;
    glGenVertexArrays(1, &groundVAO);
    glGenBuffers(1, &groundVBO);
    glGenBuffers(1, &groundEBO);

    glBindVertexArray(groundVAO);

    glBindBuffer(GL_ARRAY_BUFFER, groundVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(groundVertices), groundVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, groundEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(groundIndices), groundIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void *)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    //cube
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);



    //light cube
    unsigned int lightcubeVAO;
    glGenVertexArrays(1, &lightcubeVAO);
    glBindVertexArray(lightcubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);



    //skyBox
    unsigned int skyBoxVAO, skyBoxVBO;
    glGenVertexArrays(1, &skyBoxVAO);
    glGenBuffers(1, &skyBoxVBO);

    glBindVertexArray(skyBoxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertices), &skyBoxVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);






    // load models
    // -----------
    Model palette("resources/objects/palette/Palette_garden_table.obj");
    palette.SetShaderTextureNamePrefix("material.");

    Model pineapple("resources/objects/pineapple/pineapple.obj");
    pineapple.SetShaderTextureNamePrefix("material.");

    Model bird("resources/objects/bird/13563_Jambu_fruit_dove_v1_L3.obj");
    bird.SetShaderTextureNamePrefix("material.");

    Model apricot("resources/objects/apricot/Apricot_02_hi_poly.obj");
    apricot.SetShaderTextureNamePrefix("material.");

    Model peach("resources/objects/peach/12203_Fruit_v1_L3.obj");
    peach.SetShaderTextureNamePrefix("material.");

    Model chair("resources/objects/chair/chair.obj");
    chair.SetShaderTextureNamePrefix("material.");

    Model guitar("resources/objects/guitar/GipsyGuitar.obj");
    guitar.SetShaderTextureNamePrefix("material.");

    Model statue1("resources/objects/statue1/12329_Statue_v1_l3.obj");
    statue1.SetShaderTextureNamePrefix("material.");

    Model windowModel("resources/objects/window/window_big.obj");
    windowModel.SetShaderTextureNamePrefix("material.");


    // cube texture load
    unsigned int diffuseMapCube = loadTexture(FileSystem::getPath("resources/textures/stonyTex.jpg").c_str());
    unsigned int specularMapCube = loadTexture(FileSystem::getPath("resources/textures/stonyTex_spec.png").c_str());
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);
    lightingShader.setInt("material.specular", 1);

    // floor texture load
    unsigned int diffuseFloorMap = loadTexture(FileSystem::getPath("resources/textures/Pebbles_BaseColor.jpg").c_str());
    unsigned int specularFloorMap = loadTexture(FileSystem::getPath("resources/textures/Pebbles_Height.png").c_str());
    unsigned int transparentTexture = loadTexture(FileSystem::getPath("resources/textures/bush.png").c_str());

    vector<glm::vec3> bushes
    {
        glm::vec3(-0.8f, -0.5f, 0.0f),
        glm::vec3(0.2f, -0.5f, 0.5f),
        glm::vec3(-0.3f, -0.5f, -0.5f),
    };


    // skybox textures load
    vector<std::string> faces
            {
                    FileSystem::getPath("resources/textures/skybox/s1_right.jpg"),
                    FileSystem::getPath("resources/textures/skybox/s1_left.jpg"),
                    FileSystem::getPath("resources/textures/skybox/s1_top.jpg"),
                    FileSystem::getPath("resources/textures/skybox/s1_bottom.jpg"),
                    FileSystem::getPath("resources/textures/skybox/s1_front.jpg"),
                    FileSystem::getPath("resources/textures/skybox/s1_back.jpg")
            };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyBoxShader.use();
    skyBoxShader.setInt("skybox", 0);



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // dirlight setup
        lightingShader.setVec3("dirLight.ambient", glm::vec3(0.1f));
        lightingShader.setVec3("dirLight.diffuse", glm::vec3(0.3f));
        lightingShader.setVec3("dirLight.specular", glm::vec3(0.5f));
        lightingShader.setVec3("dirLight.direction", dirLightPos);

        // pointlight corner setup
        lightingShader.setVec3("pointLights[0].position", lightPos);
        lightingShader.setVec3("pointLights[0].ambient", glm::vec3(1.0f, 0.64f, 0.3f));
        lightingShader.setVec3("pointLights[0].diffuse", glm::vec3(0.5f));
        lightingShader.setVec3("pointLights[0].specular", glm::vec3(0.5f));
        lightingShader.setFloat("pointLights[0].constant", 1.0f);
        lightingShader.setFloat("pointLights[0].linear", 0.05f);
        lightingShader.setFloat("pointLights[0].quadratic", 0.012f);

        // pointlight table setup
        lightingShader.setVec3("pointLights[1].position", lightPosTable);
        lightingShader.setVec3("pointLights[1].ambient", glm::vec3(0.0f, 0.2f, 0.6f));
        lightingShader.setVec3("pointLights[1].diffuse", glm::vec3(0.3f));
        lightingShader.setVec3("pointLights[1].specular", glm::vec3(1.0f));
        lightingShader.setFloat("pointLights[1].constant", 1.0f);
        lightingShader.setFloat("pointLights[1].linear", 0.6f);
        lightingShader.setFloat("pointLights[1].quadratic", 0.5f);

        // spotlight setup
        lightingShader.setVec3("spotLight.position", camera.Position);
        lightingShader.setVec3("spotLight.direction", camera.Front);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setFloat("spotLight.constant", 1.0f);
        lightingShader.setFloat("spotLight.linear", 0.05f);
        lightingShader.setFloat("spotLight.quadratic", 0.1f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(7.2f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(12.0f)));
        lightingShader.setBool("pressed", spotlightPressed);
        lightingShader.setBool("blinn", blinnPressed);

        // material setup
        lightingShader.setFloat("material.shininess", 32.0f);

        //lighting shader setup
        lightSourceShader.use();
        lightSourceShader.setInt("material.diffuse", 0);
        lightSourceShader.setInt("material.specular", 1);




        // view/projection transformations
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        lightingShader.use();
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMapCube);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMapCube);

        glm::mat4 model(1.0f);



        // render cubes/walls
        for(int i = 0; i < num_of_walls; i++){
            // world transformation
            glm::mat4 model(1.0f);
            if(i <= 4)
                model = glm::translate(model, glm::vec3(-5.1f, -0.56f, 1.25f-1.5f*i));
            else
                model = glm::translate(model, glm::vec3(1.7f, -0.56f, 8.75f-1.5f*i));
            model = glm::scale(model, glm::vec3(0.4f, 0.5f, 1.05f));
            lightingShader.setMat4("model", model);

            // render cube/wall
            glEnable(GL_CULL_FACE);
            glFrontFace(GL_CW);
            glCullFace(GL_BACK);
            glBindVertexArray(cubeVAO);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glDisable(GL_CULL_FACE);
        }



         //light cube corner
        lightSourceShader.use();
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightSourceShader.setMat4("model", model);

        //render light cube
        glBindVertexArray(lightcubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // light cube table
        lightSourceShader.use();
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPosTable);
        //model = glm::scale(model, glm::vec3(0.1f));
        model = glm::scale(model, glm::vec3(0.1f));
        lightSourceShader.setMat4("model", model);

        //render light cube
        glBindVertexArray(lightcubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);





        ////// render models

        // palette model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, -0.8f, -2.0f));
        model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.3f));
        lightingShader.use();
        lightingShader.setMat4("model", model);
        palette.Draw(lightingShader);

        // pineapple model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, -0.351f, -1.8f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.015f));
        lightingShader.use();
        lightingShader.setMat4("model", model);
        pineapple.Draw(lightingShader);

        // bird model
        model = glm::mat4(1.0f);
        model = glm::translate(model, birdPos);
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
        model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.05f));
        lightingShader.use();
        lightingShader.setMat4("model", model);
        bird.Draw(lightingShader);

        // apricot model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.2f, -0.29f, -1.7f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
        //model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.033f));
        lightingShader.use();
        lightingShader.setMat4("model", model);
        apricot.Draw(lightingShader);

        // peach model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.2f, -0.36f, -2.0f));
        model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 1.0f));
        //model = glm::rotate(model, glm::radians(-70.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.023f));
        lightingShader.use();
        lightingShader.setMat4("model", model);
        peach.Draw(lightingShader);

        // render 2 chairs
        for(int i = 0; i < 2; i++){
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-3.6f+2.0f*i, -0.77f, -1.7f-2.0*i));
            model = glm::rotate(model, glm::radians(70.0f-i*70.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.013f));
            lightingShader.use();
            lightingShader.setMat4("model", model);
            chair.Draw(lightingShader);
        }

        // guitar model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.6f, 0.23f, -3.9f));
        model = glm::rotate(model, glm::radians(35.5f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.01f));
        lightingShader.use();
        lightingShader.setMat4("model", model);
        guitar.Draw(lightingShader);

        // statue1 model
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.7f, -1.0f, -3.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.005f));
        lightingShader.use();
        lightingShader.setMat4("model", model);
        statue1.Draw(lightingShader);


        // bushes
        blendingShader.use();
        blendingShader.setMat4("view", view);
        blendingShader.setMat4("projection", projection);
        glBindVertexArray(transparentVAO);
        glBindTexture(GL_TEXTURE_2D, transparentTexture);

        for (unsigned int i = 0; i < bushes.size(); i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, bushes[i]);
            //model = glm::scale(model, glm::vec3(i/2+0.5f));
            blendingShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }



        //render ground
        lightingShader.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseFloorMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularFloorMap);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.7f, -0.8f, -1.7f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(7.0f));
        lightingShader.setMat4("model", model);
        lightingShader.setMat4("view", view);
        lightingShader.setMat4("projection", projection);

        glBindVertexArray(groundVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);




        // render skybox
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyBoxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyBoxShader.setMat4("view", view);
        skyBoxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyBoxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default




        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &groundVAO);
    glDeleteVertexArrays(1, &skyBoxVAO);
    glDeleteBuffers(1, &groundVBO);
    glDeleteBuffers(1, &groundEBO);
    glDeleteBuffers(1, &skyBoxVBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
        if(birdPos.x >= -3.0f)
            birdPos = birdPos + glm::vec3(-0.02f,0,0);
    }

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
        if(birdPos.x <= -2.3)
        birdPos = birdPos + glm::vec3(0.02f,0,0.0);
    }

    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
        spotlightPressed = true;
    }

    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE){
        spotlightPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
        blinnPressed = true;
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    {
        blinnPressed = false;
    }

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}


//void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
//    if (key == GLFW_KEY_F1 && action == GLFW_PRESS) {
//        programState->ImGuiEnabled = !programState->ImGuiEnabled;
//        if (programState->ImGuiEnabled) {
//            programState->CameraMouseMovementUpdateEnabled = false;
//            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
//        } else {
//            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//        }
//    }
//}

unsigned int loadCubemap(vector<std::string> faces){
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    unsigned char *data;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            cout << "cube map tex failed to load at path: " << faces[i] << endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


unsigned int loadTexture(char const *path){
    unsigned textureId;
    glGenTextures(1, &textureId);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    //cerr << width << ' ' << height << endl;

    if(data)
    {
        GLenum format = GL_RED;
        if(nrComponents == 1)
            format = GL_RED;
        else if(nrComponents == 3)
            format = GL_RGB;
        else if(nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else{
        cout << "Texture failed to load!" << path << "\n";
        stbi_image_free(data);
    }

    return textureId;
}