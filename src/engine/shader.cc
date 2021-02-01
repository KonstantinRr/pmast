/// MIT License
/// 
/// Copyright (c) 2020 Konstantin Rolf
/// 
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
/// 
/// The above copyright notice and this permission notice shall be included in all
/// copies or substantial portions of the Software.
/// 
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
/// SOFTWARE.
/// 
/// Written by Konstantin Rolf (konstantin.rolf@gmail.com)
/// July 2020

#include <engine/shader.hpp>
#include <engine/resource.hpp>

#include <spdlog/spdlog.h>
#include <glm/gtc/type_ptr.hpp>

#include <limits>

NYREM_USE_NAMESPACE

RenderContext::RenderContext(size_t w, size_t h, float s)
    : w(w), h(h), s(s) { }

void Renderable::render(const RenderContext &context) {
    storeContext(context);
}

void Renderable::storeContext(const RenderContext &context) {
    this->context = context;
}


ShaderBase::ShaderBase(bool hasVertexShader, bool hasFragmentShader)
{
    if (hasVertexShader)
        flags |= BIT_HAS_VERT_SHADER;
    if (hasFragmentShader)
        flags |= BIT_HAS_FRAG_SHADER;
}

ShaderBase::ShaderBase(ShaderBase&& sh)
    : flags(std::exchange(sh.flags, 0))
    , program(std::exchange(sh.program, std::numeric_limits<GLuint>::max()))
{
}

ShaderBase& ShaderBase::operator=(ShaderBase&& sh)
{
    cleanUp();
    flags = std::exchange(sh.flags, false);
    program = std::exchange(sh.program, std::numeric_limits<GLuint>::max());
    return *this;
}

ShaderBase::~ShaderBase() { cleanUp(); }
void ShaderBase::cleanUp()
{
    cleanupProgram();
}

void showShaderLog(GLuint shader) {
    GLint logSize = 0;
    CGL(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize));
    if (logSize > 0) {
        spdlog::info("Shader compilation output:");
        std::vector<GLchar> errorLog(static_cast<size_t>(logSize + 1));
        errorLog[static_cast<size_t>(logSize)] = '\0';
        CGL(glGetShaderInfoLog(shader, logSize, &logSize, &errorLog[0]));
        spdlog::error("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", errorLog.data());
    }
}

void showInfoLog(GLuint program) {
    GLint logSize = 0;
    CGL(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize));
    if (logSize > 0) {
        spdlog::info("Shader linking output");
        std::vector<GLchar> errorLog(static_cast<size_t>(logSize) + 1);
        errorLog[static_cast<size_t>(logSize)] = '\0';
        CGL(glGetProgramInfoLog(program, logSize, &logSize, &errorLog[0]));
        spdlog::error("ERROR::SHADER::PROGRAM::LINKING_FAILED\n{}", errorLog.data());
    }
}

void ShaderBase::cleanupParts(GLuint vertex_shader, GLuint fragment_shader) {
    if ((flags & BIT_HAS_VERT_SHADER) && (flags && BIT_CREATED_VERT)) {
        CGL(glDeleteShader(vertex_shader));
        flags &= ~BIT_CREATED_VERT;
    }
    if ((flags & BIT_HAS_FRAG_SHADER) && (flags && BIT_CREATED_FRAG)) {
        CGL(glDeleteShader(fragment_shader));
        flags &= ~BIT_CREATED_FRAG;
    }
}

void ShaderBase::cleanupProgram() {
    if (flags & BIT_CREATED_PROG) {
        glDeleteProgram(program);
        flags &= ~BIT_CREATED_PROG;
    }
}

void ShaderBase::create() {
    try {
        int success;

        program = glCreateProgram();
        flags |= BIT_CREATED_PROG;

        GLuint vertex_shader = 0;
        GLuint fragment_shader = 0;

        if (flags & BIT_HAS_VERT_SHADER) {
            spdlog::info("Creating vertex shader");
            auto src = retrieveVertexShader();
            spdlog::info("Retrieved shader source \n'{}'", src.data());

            vertex_shader = glCreateShader(GL_VERTEX_SHADER);
            flags |= BIT_CREATED_VERT;
            const GLchar *srcPointer = (const GLchar*)src.data();
            CGL(glShaderSource(vertex_shader, 1, &(srcPointer), NULL));
            CGL(glCompileShader(vertex_shader));
            CGL(glAttachShader(program, vertex_shader));

            CGL(glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success));
            showShaderLog(vertex_shader);
            if(!success) {
                cleanupParts(vertex_shader, fragment_shader);
                cleanupProgram();
                throw std::runtime_error("Could not load vertex shader");
            }
            spdlog::info("Compiled vertex shader successful");
        } else {
            spdlog::warn("Shaders programs without vertex stage are not allowed");
        }
        
        if (flags & BIT_HAS_FRAG_SHADER) {
            spdlog::info("Creating Fragment Shader");
            auto src = retrieveFragmentShader();
            spdlog::info("Source {}", src.data());

            fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
            flags |= BIT_CREATED_FRAG;
            const GLchar *srcPointer = (const GLchar *)src.data();
            CGL(glShaderSource(fragment_shader, 1, &srcPointer, NULL));
            CGL(glCompileShader(fragment_shader));
            CGL(glAttachShader(program, fragment_shader));

            CGL(glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success));
            showShaderLog(fragment_shader);
            if(!success) {
                cleanupParts(vertex_shader, fragment_shader);
                cleanupProgram();
                throw std::runtime_error("Could not load fragment shader");
            };
            spdlog::info("Compiled fragment shader successful");
        } else {
            spdlog::warn("Shaders programs without vertex stage are not allowed");
        }

        CGL(glLinkProgram(program));
        CGL(glGetProgramiv(program, GL_LINK_STATUS, &success));
        showInfoLog(program);
        if(!success) {
            cleanupParts(vertex_shader, fragment_shader);
            cleanupProgram();
            throw std::runtime_error("Could not link shader");
        }
        flags |= BIT_LINKED;

        cleanupParts(vertex_shader, fragment_shader);
        spdlog::info("Shaders successfully linked");
        initializeUniforms();
        spdlog::info("Uniforms successfully loaded");
    } catch (const std::runtime_error&excp) {
        spdlog::error("Could not create shader {}", excp.what());
        throw;
    }
}

void ShaderBase::bind() {
    if (!valid()) {
        throw std::runtime_error("Shader is not valid()");
    }
    CGL(glUseProgram(program));
}

void ShaderBase::release() {
    CGL(glUseProgram(0));
}

bool ShaderBase::valid() {
    return flags & BIT_LINKED != 0;
}

void ShaderBase::loadFloat(GLint location, float value) { CGL(glUniform1f(location, value)); }
void ShaderBase::loadBool(GLint location, bool value) { CGL(glUniform1i(location, value)); }
void ShaderBase::loadInt(GLint location, int value) { CGL(glUniform1i(location, value)); }

void ShaderBase::loadVec1(GLint location, const glm::vec1 &vec) { CGL(glUniform1f(location, vec.x)); }
void ShaderBase::loadVec2(GLint location, const glm::vec2 &vec) { CGL(glUniform2f(location, vec.x, vec.y)); }
void ShaderBase::loadVec3(GLint location, const glm::vec3 &vec) { CGL(glUniform3f(location, vec.x, vec.y, vec.y)); }
void ShaderBase::loadVec4(GLint location, const glm::vec4 &vec) { CGL(glUniform4f(location, vec.x, vec.y, vec.z, vec.w)); }

void ShaderBase::loadMat2x2(GLint location, const glm::mat2x2 &mat) { CGL(glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
void ShaderBase::loadMat2x3(GLint location, const glm::mat2x3 &mat) { CGL(glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
void ShaderBase::loadMat2x4(GLint location, const glm::mat2x4 &mat) { CGL(glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }

void ShaderBase::loadMat3x2(GLint location, const glm::mat3x2 &mat) { CGL(glUniformMatrix3x2fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
void ShaderBase::loadMat3x3(GLint location, const glm::mat3x3 &mat) { CGL(glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
void ShaderBase::loadMat3x4(GLint location, const glm::mat3x4 &mat) { CGL(glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }

void ShaderBase::loadMat4x2(GLint location, const glm::mat4x2 &mat) { CGL(glUniformMatrix4x2fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
void ShaderBase::loadMat4x3(GLint location, const glm::mat4x3 &mat) { CGL(glUniformMatrix4x3fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }
void ShaderBase::loadMat4x4(GLint location, const glm::mat4x4 &mat) { CGL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat))); }


GLint ShaderBase::uniformLocation(const std::string &name, bool required) {
    GLint v = glGetUniformLocation(program, name.c_str());
    if (required && v == -1) {
        NYREM_DEBUG_BREAK;
        throw std::runtime_error("Could not load uniform '" + name + '\'');
    }
    return v;
}

GLuint ShaderBase::getShaderID() {
    return program;
}

// ---- Resource ShaderBase ----

ResourceShader::ResourceShader(bool hasVertexShader, bool hasFragmentShader)
    : ShaderBase(hasVertexShader, hasFragmentShader) { }

std::vector<char> ResourceShader::retrieveVertexShader() {
    return readFile(getVertexSource());
}

std::vector<char> ResourceShader::retrieveFragmentShader() {
    return readFile(getFragmentSource());
}


void RenderPipeline::addStage(const std::shared_ptr<Renderable> &component) {
    renders.push_back(component);
}
void RenderPipeline::clear() {
    renders.clear();
}

void RenderPipeline::render(const RenderContext &context) {
    for (auto v : renders)
        v->render(context);
}


template<typename EntityType>
void RenderList<EntityType>::add(const std::shared_ptr<EntityType>& entity) {
    entities.push_back(entity);
}

template<typename EntityType>
void RenderList<EntityType>::remove(const std::shared_ptr<EntityType>& entity) {
    for (auto it = entities.begin(); it != entities.end(); ++it) {
        if ((*it)->getID() == entity->getID()) {
            entities.erase(it);
        }
    }
}

template<typename EntityType>
void RenderList<EntityType>::clear() {
    entities.clear();
}

// ---- RenderBatch ---- //
template<typename EntityType>
void RenderBatch<EntityType>::add(const std::shared_ptr<EntityType>& entity) {
    map[entity->getTexture()->getTexture()].add(entity);
}

template<typename EntityType>
void RenderBatch<EntityType>::remove(const std::shared_ptr<EntityType>& entity) {
    map[entity->getTexture()->getTexture()].remove(entity);
}

// ---- TickerList ---- //

void TickerList::add(const std::shared_ptr<Tickable>& ticker) {
    tickables.push_back(ticker);
}
void TickerList::clear() {
    tickables.clear();
}
float TickerList::getCurrentTime() const {
    return time;
}
void TickerList::updateAll(float dt) {
    for (const auto& ticker : tickables) {
        ticker->update(time, dt);
    }
    time += dt;
}

RectStageBuffer::RectStageBuffer(
    const RenderList<TransformableEntity2D>& renderList,
    const std::shared_ptr<const ViewTransformer> &camera)
    : renderList(renderList), camera(camera) { }


// ---- RectShader ---- //
RectShader::RectShader()
    : ShaderBase(true, true),
    uniformTexture(-1), uniformTransform(-1),
    uniformColor(-1), uniformUseColor(-1) { }

RectShader::RectShader(RectShader &&sh) : ShaderBase(std::move(sh)),
    uniformTexture(std::exchange(sh.uniformTexture, -1)),
    uniformTransform(std::exchange(sh.uniformTransform, -1)),
    uniformColor(std::exchange(sh.uniformColor, -1)),
    uniformUseColor(std::exchange(sh.uniformUseColor, -1)) { }


RectShader& RectShader::operator=(RectShader &&sh)
{
    ShaderBase::operator=(std::move(sh));
    uniformTexture = std::exchange(sh.uniformTexture, -1);
    uniformTransform =std::exchange(sh.uniformTransform, -1);
    uniformColor = std::exchange(sh.uniformColor, -1);
    uniformUseColor = std::exchange(sh.uniformUseColor, -1);
    return *this;
}

void RectShader::render(
    const RenderList<TransformableEntity2D>& renderList,
    const std::shared_ptr<const ViewTransformer> &camera) {
    bind();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);
    loadTexture(0);

    nyrem::mat4x4 transformCam;
    if (camera)
        transformCam = camera->matrix();
    
    for (auto& entity : renderList) {
        if (camera)
            loadTransform(transformCam * entity->calculateTransformationMatrix());
        else
            loadTransform(entity->calculateTransformationMatrix());
        
        if (entity->getColorStorage().hasColor()) {
            loadUseColor(true);
            loadColor(entity->getColorStorage()[0]);
        }
        else {
            loadUseColor(false);
            entity->getTexture()->bind(); // binds texture
        }

        entity->getModel()->bind(); // binds the model
        glDrawArrays(GL_TRIANGLES, 0, entity->getModel()->getSize());
    }

    release();
}

void RectShader::render(const RectStageBuffer& renderList) {
    render(renderList.renderList, renderList.camera);
}

void RectShader::loadColor(nyrem::vec3 color) { loadVec3(uniformColor, color); }
void RectShader::loadUseColor(bool value) { loadBool(uniformUseColor, value); }
void RectShader::loadTransform(const glm::mat4x4& matrix) { loadMat4x4(uniformTransform, matrix); }
void RectShader::loadTexture(GLint texture) { loadInt(uniformTexture, texture); }

void RectShader::initializeUniforms() {
    uniformColor = uniformLocation("u_color");
    uniformUseColor = uniformLocation("u_useColor");
    uniformTexture = uniformLocation("textureSampler");
    uniformTransform = uniformLocation("mvp");
}

// ---- SimpleShader ---- //

// ---- SimpleMVPShader ---- //
MVPListStageBuffer::MVPListStageBuffer() :
    camera(std::make_shared<ViewTransformer>()),
    list(std::make_shared<RenderList<Entity>>()) { }
MVPListStageBuffer::MVPListStageBuffer(
    const std::shared_ptr<ViewTransformer>& pCamera,
    const std::shared_ptr<RenderList<Entity>>& pList)
    : camera(pCamera), list(pList) { }

// ---- SimpleMVPShader ---- //
SimpleMVPShader::SimpleMVPShader() : ShaderBase(true, true) { }

SimpleMVPShader::SimpleMVPShader(SimpleMVPShader&& sh) : ShaderBase(std::move(sh)),
    location_mvp(std::exchange(sh.location_mvp, -1))
{ }

void SimpleMVPShader::initializeUniforms() {
    location_mvp = uniformLocation("mvp");
}

SimpleMVPShader& SimpleMVPShader::operator=(SimpleMVPShader&& sh)
{
    ShaderBase::operator=(std::move(sh));
    location_mvp = std::exchange(sh.location_mvp, -1);
    return *this;
}

// TODO
void SimpleMVPShader::render(const ViewTransformer& camera, const RenderList<Entity>& list) {
    bind();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    // loads the uniforms that stay the same during drawcall
    

    glm::mat4x4 cam = camera.matrix();
    for (auto& entity : list) {
        if (!entity->hasModel()) continue;

        loadMVPMatrix(cam * entity->getTransformationMatrix());

        entity->getModel()->bind(); // binds model
        glDrawArrays(GL_TRIANGLES, 0, entity->getModel()->getSize());
    }
    release();
}
void SimpleMVPShader::render(const MVPListStageBuffer& stageBuffer) {
    render(*stageBuffer.camera, *stageBuffer.list);
}


void SimpleMVPShader::loadMVPMatrix(const glm::mat4& mat) {
    loadMat4x4(location_mvp, mat);
}

// ---- PhongShader ---- //
PhongListStageBuffer::PhongListStageBuffer() :
    camera(std::make_shared<ViewPipeline>()),
    renderList(std::make_shared<RenderList<Entity>>()),
    lightPosition({0.0f, 0.0f, 0.0f}),
    lightColor({1.0f, 1.0f, 1.0f})
{ }
PhongListStageBuffer::PhongListStageBuffer(
    const std::shared_ptr<ViewPipeline>& pCamera,
    const std::shared_ptr<RenderList<Entity>>& pRenderList,
    const glm::vec3& pLightPosition, const glm::vec3& pLightColor)
    : camera(pCamera), renderList(pRenderList),
    lightPosition(pLightPosition), lightColor(pLightColor)
{ }

PhongBatchStageBuffer::PhongBatchStageBuffer() :
    camera(std::make_shared<ViewPipeline>()),
    renderList(std::make_shared<RenderBatch<Entity>>()),
    lightPosition({0.0f, 0.0f, 0.0f}),
    lightColor({1.0f, 1.0f, 1.0f})
{ }
PhongBatchStageBuffer::PhongBatchStageBuffer(
    const std::shared_ptr<ViewPipeline>& pCamera,
    const std::shared_ptr<RenderBatch<Entity>>& pRenderList,
    const glm::vec3& pLightPosition, const glm::vec3& pLightColor)
    : camera(pCamera), renderList(pRenderList),
    lightPosition(pLightPosition), lightColor(pLightColor)
{ }

// ---- PhongShader ---- //
PhongShader::PhongShader()
    : ShaderBase(true, true) { }

PhongShader::PhongShader(PhongShader&& sh) : ShaderBase(std::move(sh)),
    uniformModelViewTransformPhong(std::exchange(sh.uniformModelViewTransformPhong, -1)),
    uniformProjectionTransformPhong(std::exchange(sh.uniformProjectionTransformPhong, -1)),
    uniformNormalTransformPhong(std::exchange(sh.uniformNormalTransformPhong, -1)),
    uniformMaterialPhong(std::exchange(sh.uniformMaterialPhong, -1)),
    uniformLightPositionPhong(std::exchange(sh.uniformLightPositionPhong, -1)),
    uniformLightColorPhong(std::exchange(sh.uniformLightColorPhong, -1)),
    uniformTextureSamplerPhong(std::exchange(sh.uniformTextureSamplerPhong, -1))
{
}

PhongShader& PhongShader::operator=(PhongShader&& sh)
{
    ShaderBase::operator=(std::move(sh));
    uniformModelViewTransformPhong = std::exchange(sh.uniformModelViewTransformPhong, -1);
    uniformProjectionTransformPhong =std::exchange(sh.uniformProjectionTransformPhong, -1);
    uniformNormalTransformPhong = std::exchange(sh.uniformNormalTransformPhong, -1);
    uniformMaterialPhong =std::exchange(sh.uniformMaterialPhong, -1);
    uniformLightPositionPhong = std::exchange(sh.uniformLightPositionPhong, -1);
    uniformLightColorPhong = std::exchange(sh.uniformLightColorPhong, -1);
    uniformTextureSamplerPhong = std::exchange(sh.uniformTextureSamplerPhong, -1);
    return *this;
}

void PhongShader::initializeUniforms() {
    uniformModelViewTransformPhong = uniformLocation("modelViewTransform");
    uniformProjectionTransformPhong = uniformLocation("projectionTransform");
    uniformNormalTransformPhong = uniformLocation("normalTransform");
    uniformMaterialPhong = uniformLocation("material");
    uniformLightPositionPhong = uniformLocation("lightPosition");
    uniformLightColorPhong = uniformLocation("lightColor");
    uniformColorPhong = uniformLocation("color");

    //uniformUseNormalTexture = uniformLocation("useNormalTexture");
    uniformUseTexture = uniformLocation("useTexture");
    uniformTextureSamplerPhong = uniformLocation("textureSampler");
}


void PhongShader::render(const ViewPipeline& camera, const RenderList<Entity>& list,
    const glm::vec3& lightPosition, const glm::vec3& lightColor)
{
    bind();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glActiveTexture(GL_TEXTURE0);
    // loads the uniforms that stay the same during drawcall
    loadProjection(camera.projectionMatrix());
    loadLightPosition(lightPosition);
    loadLightColor(lightColor);
    loadTexture(0);

    glm::mat4x4 cam = camera.viewMatrix();
    for (const auto& entity : list) {
        if (!entity->hasModel()) continue; // entities without model are skipped
        // loads the view transformations
        loadModelView(cam * entity->getTransformationMatrix());
        loadNormalMatrix(entity->getNormalMatrix());

        // loads the entity material or uses a default one
        if (entity->hasMaterial())
            loadMaterial(entity->getMaterial()->getMaterial());
        else
            loadMaterial({0.5f, 0.5f, 0.5f, 5.0f});
        
        // 
        loadHasTexture(entity->hasTexture());
        if (entity->hasTexture()) {
            entity->getTexture()->bind();
            loadHasTexture(true);
        } else {
            loadHasTexture(false);
            const auto &colors = entity->getColorStorage();
            if (colors.hasColor()) {
                loadColor(colors[0]);
            } else {
                loadColor({1.0f, 1.0f, 1.0f});
            }
        }

        entity->getModel()->bind(); // binds the model
        glDrawArrays(GL_TRIANGLES, 0,
            entity->getModel()->getSize());
    }
    release();
}

void PhongShader::render(const ViewPipeline& camera, const RenderBatch<Entity>& batch,
    const glm::vec3& lightPosition, const glm::vec3& lightColor) {
    bind();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);

    glActiveTexture(GL_TEXTURE0);
    // loads the uniforms that stay the same
    loadProjection(camera.projectionMatrix());
    loadLightPosition(lightPosition);
    loadLightColor(lightColor);
    loadTexture(0);

    glm::mat4x4 view = camera.viewMatrix();
    for (auto& renderListIt : batch) {
        glBindTexture(GL_TEXTURE_2D, renderListIt.first);

        for (auto& entity : renderListIt.second) {
            loadModelView(view * entity->getTransformationMatrix());
            loadNormalMatrix(entity->getNormalMatrix());
            loadMaterial(entity->getMaterial()->getMaterial());

            entity->getModel()->bind(); // binds the model only
            glDrawArrays(GL_TRIANGLES, 0,
                entity->getModel()->getSize());
        }
    }
    release();
}

void PhongShader::render(const PhongListStageBuffer& stageBuffer) {
    render(*stageBuffer.camera, *stageBuffer.renderList, stageBuffer.lightPosition, stageBuffer.lightColor);
}
void PhongShader::render(const PhongBatchStageBuffer& stageBuffer) {
    render(*stageBuffer.camera, *stageBuffer.renderList, stageBuffer.lightPosition, stageBuffer.lightColor);
}

void PhongShader::loadModelView(const glm::mat4x4& matrix) { loadMat4x4(uniformModelViewTransformPhong, matrix); }
void PhongShader::loadProjection(const glm::mat4x4& matrix) { loadMat4x4(uniformProjectionTransformPhong, matrix); }
void PhongShader::loadNormalMatrix(const glm::mat3x3& matrix) { loadMat3x3(uniformNormalTransformPhong, matrix); }

void PhongShader::loadHasTexture(bool value) { loadBool(uniformUseTexture, value); }
void PhongShader::loadHasNormalTexture(bool value) { loadBool(uniformUseNormalTexture, value); }
void PhongShader::loadColor(const glm::vec3& color) { loadVec3(uniformColorPhong, color); }

void PhongShader::loadMaterial(const glm::vec4& vector) { loadVec4(uniformMaterialPhong, vector); }
void PhongShader::loadLightPosition(const glm::vec3& vector) { loadVec3(uniformLightPositionPhong, vector); }
void PhongShader::loadLightColor(const glm::vec3& vector) { loadVec3(uniformLightColorPhong, vector); }
void PhongShader::loadTexture(GLint unit) { loadInt(uniformTextureSamplerPhong, unit); }

std::vector<char> toArray(const char* raw) {
    std::string str(raw);
    std::vector<char> v(str.length() + 1);
    std::copy(str.begin(), str.end(), v.begin());
    v.back() = '\0';
    return v;
}

std::vector<char> PhongMemoryShader::retrieveVertexShader()
{
    const char * frag = R"(
    #version 330 core

    // Specify the input locations of attributes.
    layout (location = 0) in vec3 vertCoordinates_in;
    layout (location = 1) in vec3 vertNormals_in;
    layout (location = 2) in vec2 texCoords_in;

    // Specify the uniforms of the vertex shader.
    uniform mat4 modelViewTransform;
    uniform mat4 projectionTransform;
    uniform mat3 normalTransform;
    uniform vec3 lightPosition;

    // Specify the output of the vertex stage.
    out vec3 vertNormal;
    out vec3 vertPosition;
    out vec3 relativeLightPosition;
    out vec2 texCoords;

    void main()
    {
        gl_Position  = projectionTransform * modelViewTransform * vec4(vertCoordinates_in, 1.0F);

        // Pass the required information to the fragment shader stage.
        relativeLightPosition = vec3(modelViewTransform * vec4(lightPosition, 1.0F));
        vertPosition = vec3(modelViewTransform * vec4(vertCoordinates_in, 1.0F));
        vertNormal   = normalize(normalTransform * vertNormals_in);
        texCoords    = texCoords_in;
    }
    )";
    return toArray(frag);
}

std::vector<char> PhongMemoryShader::retrieveFragmentShader()
{
    const char * frag = R"(
    #version 330 core

    // The input from the vertex shader.
    in vec3 vertNormal;
    in vec3 vertPosition;
    in vec3 relativeLightPosition;
    in vec2 texCoords;

    // Illumination model constants.
    uniform vec4 material;
    uniform vec3 lightColor;
    uniform vec3 color;
    uniform int useTexture;

    // Texture sampler.
    uniform sampler2D textureSampler;

    // Specify the output of the fragment shader.
    out vec4 vertColor;

    void main()
    {
        // Ambient color does not depend on any vectors.
        vec3 texColor;
        if (useTexture != 0)
            texColor = texture(textureSampler, texCoords).xyz;
        else
            texColor = color;
        //texColor = vec3(0.5, 0.3, 0.5);
        vec3 color = material.x * texColor;

        // Calculate light direction vectors in the Phong illumination model.
        vec3 lightDirection = normalize(relativeLightPosition - vertPosition);
        vec3 normal = normalize(vertNormal);

        // Diffuse color.
        float diffuseIntensity = max(dot(normal, lightDirection), 0.0F);
        color += texColor * material.y * diffuseIntensity;

        // Specular color.
        vec3 viewDirection = normalize(-vertPosition); // The camera is always at (0, 0, 0).
        vec3 reflectDirection = reflect(-lightDirection, normal);
        float specularIntensity = max(dot(reflectDirection, viewDirection), 0.0F);
        color += lightColor * material.z * pow(specularIntensity, material.w);

        vertColor = vec4(color, 1.0F);
    }
    )";
    return toArray(frag);
}

/*
#version 330

uniform mat4 mvp;

layout(location = 0) in vec2 vVertex;
layout(location = 1) in vec3 color;
out vec3 mixedColor;

void main(void) {
    gl_Position = mvp * vec4(vVertex, 0.0, 1.0);
    mixedColor = color;
})";
*/

//// ---- MemoryRectShader ---- ////
MemoryRectShader::MemoryRectShader() : RectShader() { }

std::vector<char> MemoryRectShader::retrieveFragmentShader()
{
    const char * frag = R"(
    #version 330

    in vec2 texturePosition;
    out vec4 color;
    
    uniform sampler2D textureSampler;
    uniform vec3 u_color;
    uniform int u_useColor;

    void main(){
        if (u_useColor == 0) {
            color = texture(textureSampler, texturePosition);
        } else {
            color = vec4(u_color, 1.0);
        }
    })";
    return toArray(frag);
}

std::vector<char> MemoryRectShader::retrieveVertexShader()
{
    const char * vert = R"(
    #version 330

    uniform mat4 mvp;

    layout(location = 0) in vec2 vertexPosition;
    layout(location = 1) in vec2 vertexTexturePosition;

    out vec2 texturePosition;
  
    void main() {
        gl_Position = mvp * vec4(vertexPosition, 0.0, 1.0);
        texturePosition = vertexTexturePosition;
    })";
    return toArray(vert);
}

// ---- TriangleShader ---- //
TriangleShader::TriangleShader()
    : ShaderBase(true, true) { }

TriangleShader::TriangleShader(TriangleShader&& sh) : ShaderBase(std::move(sh)),
    uniformMVP(std::exchange(sh.uniformMVP, -1)) { }

TriangleShader& TriangleShader::operator=(TriangleShader&& sh)
{
    ShaderBase::operator=(std::move(sh));
    uniformMVP = std::exchange(sh.uniformMVP, -1);
    return *this;
}

void TriangleShader::initializeUniforms()
{
    uniformMVP = uniformLocation("mvp");
}

void TriangleShader::loadMVP(const glm::mat4x4 &mat) { loadMat4x4(uniformMVP, mat); }
void TriangleShader::render(const TriangleStageBuffer &stageBuffer)
{
    bind();
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    
    for (const auto& entity : stageBuffer.renderList) {
        loadMVP(entity->getTransformationMatrix());

        entity->getModel()->bind();
        CGL(glDrawArrays(GL_TRIANGLES, 0, entity->getModel()->getSize()));
    }
    release();
}

//// ---- TriangleMemoryShader ---- ////
TriangleMemoryShader::TriangleMemoryShader()
    : TriangleShader() { }

std::vector<char> TriangleMemoryShader::retrieveVertexShader()
{
    const char * vert = R"(
    #version 330
    
    uniform mat4 mvp;
    layout (location = 0) in vec2 vVertex;
    layout (location = 1) in vec3 color;
    out vec3 mixedColor;

    void main(void) {
	    gl_Position = mvp * vec4(vVertex, 0.0, 1.0);
	    mixedColor = color;
    })";
    return toArray(vert);
}

std::vector<char> TriangleMemoryShader::retrieveFragmentShader()
{
    const char * frag = R"(
    #version 330
    in vec3 mixedColor;

    out vec4 color;

    void main() {
        color = vec4(mixedColor, 1.0);
    })";
    return toArray(frag);
}


// ---- Simple MVP Shader ---- //
std::vector<char> SimpleMVPMemoryShader::retrieveVertexShader()
{
    const char * vert = R"(
    #version 330 core

    layout(location = 0) in vec3 vertexPos;
  
    // Values that stay constant for the whole mesh.
    uniform mat4 mvp;
  
    void main(){
      // Output position of the vertex, in clip space : MVP * position
      gl_Position =  mvp * vec4(vertexPos, 1.0);
    }
    )";
    return toArray(vert);
}

std::vector<char> SimpleMVPMemoryShader::retrieveFragmentShader()
{
    const char * frag = R"(
    #version 330 core

    out vec3 color;

    void main(){
        color = vec3(1,0,0);
    }
    )";
    return toArray(frag);
}

//// ---- LineShader ---- ////
LineShader::LineShader()
    : ShaderBase(true, true) { }

LineShader::LineShader(LineShader&& sh) : ShaderBase(std::move(sh)),
    uniformMVP(std::exchange(sh.uniformMVP, -1)) { }

LineShader& LineShader::operator=(LineShader&& sh)
{
    ShaderBase::operator=(std::move(sh));
    uniformMVP = std::exchange(sh.uniformMVP, -1);
    return *this;
}

void LineShader::initializeUniforms()
{
    uniformMVP = uniformLocation("mvp");
}

void LineShader::loadMVP(const glm::mat4x4& mat) { loadMat4x4(uniformMVP, mat); }

void LineShader::render(const LineStageBuffer& stageBuffer)
{
    bind();
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    
    for (const auto& entity : stageBuffer.renderList) {
        loadMVP(entity->getTransformationMatrix());

        entity->getModel()->bind();
        CGL(glDrawArrays(GL_LINES, 0, entity->getModel()->getSize()));
    }
    release();
}
//// ---- LineMemoryShader ---- ////
LineMemoryShader::LineMemoryShader()
    : LineShader() { }

std::vector<char> LineMemoryShader::retrieveVertexShader()
{
    const char * vert = R"(
    #version 330

    uniform mat4 mvp;

    layout (location = 0) in vec2 vVertex;
    layout (location = 1) in vec3 color;
    out vec3 mixedColor;

    void main(void) {
	    gl_Position = mvp * vec4(vVertex, 0.0, 1.0);
	    mixedColor = color;
    })";
    return toArray(vert);
}

std::vector<char> LineMemoryShader::retrieveFragmentShader()
{
    const char * frag = R"(
    #version 330
    in vec3 mixedColor;

    out vec4 color;

    void main() {
        color = vec4(mixedColor, 1.0);
    })";
    return toArray(frag);
}

// Explicit instantiations
template class nyrem::RenderList<Entity>;
template class nyrem::RenderBatch<Entity>;

template class nyrem::RenderList<Entity2D>;
template class nyrem::RenderList<MatrixBufferedEntity2D>;
template class nyrem::RenderList<TransformableEntity2D>;
template class nyrem::RenderList<TransformedEntity2D>;
template class nyrem::RenderBatch<Entity2D>;
template class nyrem::RenderBatch<MatrixBufferedEntity2D>;
template class nyrem::RenderBatch<TransformableEntity2D>;
template class nyrem::RenderBatch<TransformedEntity2D>;