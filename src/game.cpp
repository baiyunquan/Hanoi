#include <algorithm>
#include <sstream>
#include <iostream>

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "object_2D.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"
#include "step_manager.h"
#include "eventbus.h"
#include "button.h"
#include "menu.h"


// Game-related State data
SpriteRenderer* Renderer;
ParticleGenerator* Particles;
PostProcessor* Effects;


ISoundEngine* SoundEngine;
ISoundSource* trigger;

TextRenderer* Text;
std::map<int, Hanoi*> towers;
StepManager* stepManager;
EventBus eventBus{};

Button* RecordButton;
Button* StopButton;
Object2D* LoadButton;

Menu* menu;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Step(0)
{}

Game::~Game()
{
    // 释放成员变量
    if (textInput) {
        delete textInput;
        textInput = nullptr;
    }
    
    if (messageBox) {
        delete messageBox;
        messageBox = nullptr;
    }
    
    // 释放按钮资源
    if (RecordButton) {
        delete RecordButton;
        RecordButton = nullptr;
    }
    
    if (StopButton) {
        delete StopButton;
        StopButton = nullptr;
    }
    
    if (LoadButton) {
        delete LoadButton;
        LoadButton = nullptr;
    }
    
    // 释放塔资源
    for (auto& [id, tower] : towers) {
        if (tower) {
            delete tower;
        }
    }
    towers.clear();
    
    // 释放StepManager
    if (stepManager) {
delete stepManager;
        stepManager = nullptr;
    }
    
    // 释放Menu
    if (menu) {
        delete menu;
        menu = nullptr;
    }
    
    // 释放渲染相关资源
    if (Particles) {
        delete Particles;
        Particles = nullptr;
    }
    
    if (Renderer) {
        delete Renderer;
        Renderer = nullptr;
    }
    
    if (Effects) {
        delete Effects;
        Effects = nullptr;
    }
    
    // 释放TextRenderer
    if (Text) {
        delete Text;
        Text = nullptr;
    }
    
    // 释放SoundEngine（使用drop而不是delete）
    if (SoundEngine) {
        SoundEngine->drop();
        SoundEngine = nullptr;
    }
}

void Game::Init()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // 确保使用默认的深度测试函数

    // Load freetype
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("resources/fonts/arial.ttf", 24);

    // Load shaders
    ResourceManager::LoadShader("shaders/sprite/vertShader.glsl", "shaders/sprite/fragShader.glsl", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particles/vertShader.glsl", "shaders/particles/fragShader.glsl", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processor/vertShader.glsl", "shaders/post_processor/fragShader.glsl", nullptr, "post_processor");
    ResourceManager::LoadShader("shaders/rectangle/vertShader.glsl", "shaders/rectangle/fragShader.glsl", nullptr, "rectangle");
    ResourceManager::LoadShader("shaders/light/vertShader.glsl", "shaders/multiple_light/fragShader.glsl", nullptr, "lighting");
    ResourceManager::LoadShader("shaders/light/defaultVertShader.glsl", "shaders/light/lightShader.glsl", nullptr, "lightCube");
    ResourceManager::LoadShader("shaders/ground/groundVert.glsl", "shaders/ground/groundFrag.glsl", nullptr, "ground");
    ResourceManager::LoadShader("shaders/lightMaterial/vert.glsl", "shaders/lightMaterial/frag.glsl", nullptr, "lightMaterial");
    ResourceManager::LoadShader("shaders/cylinder/diskVert.glsl", "shaders/multiple_light/materialFrag.glsl", nullptr, "disk");
    ResourceManager::LoadShader("shaders/cylinder/cylinderSideVert.glsl", "shaders/multiple_light/materialFrag.glsl", nullptr, "cylinderSide");
    ResourceManager::LoadShader("shaders/skybox/skyboxVert.glsl", "shaders/skybox/skyboxFrag.glsl", nullptr , "skybox");
    // Configure shaders
    glm::mat4 projection = glm::ortho(
        0.0f,
        static_cast<float>(this->Width),
        static_cast<float>(this->Height),
        0.0f,
        -1.0f,
        1.0f
    );
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    ResourceManager::GetShader("rectangle").Use().SetMatrix4("projection", projection);
    // 加载纹理
    ResourceManager::LoadTexture("resources/textures/background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture("resources/textures/block.png", GL_FALSE, "block");
    ResourceManager::LoadTexture("resources/textures/container2.png", GL_TRUE, "diffuseMap");
    ResourceManager::LoadTexture("resources/textures/container2_specular.png", GL_TRUE, "specularMap");
    ResourceManager::LoadTexture("resources/textures/alien.png", GL_TRUE, "alien");

    // Set render-specific controls
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
    Renderer = new SpriteRenderer(ResourceManager::Shaders ,
        ResourceManager::GetTexture("diffuseMap") , ResourceManager::GetTexture("specularMap") , this->Width , this->Height);
    Effects = new PostProcessor(ResourceManager::GetShader("post_processor"), this->Width, this->Height);

    messageBox = new MessageBox(this->Width, this->Height);
    menu = new Menu(this->Width, this->Height);

    // 设置回调函数
    menu->SetCallback([this](bool ground ,int towers, int disks, bool sound, float volume) {
        std::cout << "Starting game with: " << towers << " towers, "
            << disks << " disks, sound " << (sound ? "ON" : "OFF")
            << ", volume: " << volume << std::endl;
        this->towerNum = towers;
        this->towerLevel = disks;
        this->volume = volume;
        this->sound = sound;
        this->ground = ground;
        if (!initialized) {
            this->enter();
        }
        else {
            this->Reenter();
        }

        State = GAME_ACTIVE;
    });
}

void Game::Reenter() {
    SoundEngine = createIrrKlangDevice();
    if (!SoundEngine)
    {
        std::cout << "Fail to load SoundEngine" << std::endl;
    }
    // Load Sound
    if (sound) {
        ISoundSource* background = SoundEngine->addSoundSourceFromFile("resources/audio/funky_stars.mp3");
        background->setDefaultVolume(volume / 3);
        SoundEngine->play2D(background, true);
		trigger = SoundEngine->addSoundSourceFromFile("resources/audio/trigger.wav");
    }

    float tenH = this->Height * 0.1f;
    float eightH = this->Height * 0.08f;

    float twentyW = this->Width * 0.2f;
    float twelfthW = this->Width * 0.12f;
    float twoW = this->Width * 0.02f;
    float sixW = this->Width * 0.06f;

    float towerWidth = (this->Width - twentyW) / static_cast<float>(this->towerNum);
    float towerhHeight = this->Height - tenH;

    towers.emplace(0, new Hanoi(towerLevel, glm::vec2(twoW, eightH), glm::vec2(towerWidth, towerhHeight), false));
    for (int i = 1; i < this->towerNum; i++) {
        float posX = twoW + (static_cast<float>(i) * towerWidth) + (sixW / static_cast<float>(towerNum - 1)) * static_cast<float>(i);
        float posY = eightH;
        towers.emplace(i, new Hanoi(towerLevel, glm::vec2(posX, posY), glm::vec2(towerWidth, towerhHeight), true));
    }

    for (auto& [i, tower] : towers) {
        tower->base.setText(std::to_string(i));
    }

    messageBox->setOnConfirmCallback([&]() {});
    messageBox->setMessage(std::string("Specifically we want you to move piles ") + "of radioactive disks from an old reactor. Just be sure"
        + " not to put a bigger disk on top of a smaller disk or the whole ship will blow up.", ResourceManager::GetTexture("alien"));
}

void Game::enter() {
    SoundEngine = createIrrKlangDevice();
    if (!SoundEngine)
    {
        std::cout << "Fail to load SoundEngine" << std::endl;
    }
    // Load Sound
    if (sound) {
        ISoundSource* background = SoundEngine->addSoundSourceFromFile("resources/audio/funky_stars.mp3");
        background->setDefaultVolume(volume / 3);
        SoundEngine->play2D(background, true);
        trigger = SoundEngine->addSoundSourceFromFile("resources/audio/trigger.wav");
    }


    float tenH = this->Height * 0.1f;
    float eightH = this->Height * 0.08f;

    float twentyW = this->Width * 0.2f;
    float twelfthW = this->Width * 0.12f;
    float twoW = this->Width * 0.02f;
    float sixW = this->Width * 0.06f;

    float towerWidth = (this->Width - twentyW) / static_cast<float>(this->towerNum);
    float towerhHeight = this->Height - tenH;

    towers.emplace(0, new Hanoi(towerLevel, glm::vec2(twoW, eightH), glm::vec2(towerWidth, towerhHeight), false));
    for (int i = 1; i < this->towerNum; i++) {
        float posX = twoW + (static_cast<float>(i) * towerWidth) + (sixW / static_cast<float>(towerNum - 1)) * static_cast<float>(i);
        float posY = eightH;
        towers.emplace(i, new Hanoi(towerLevel, glm::vec2(posX, posY), glm::vec2(towerWidth, towerhHeight), true));
    }

    for (auto& [i, tower] : towers) {
        tower->base.setText(std::to_string(i));
    }

    topBarHeight = eightH;
    sideBarWidth = twelfthW;
    sideBarX = this->Width - sideBarWidth;
    sideBarHeight = this->Height - topBarHeight;

    // Add buttons
    // 计算按钮的尺寸和位置
    float buttonWidth = this->Width - sideBarX;  // 按钮宽度为侧边栏宽度
    float totalAreaHeight = this->Height - topBarHeight; // 可用总高度
    float spacing = totalAreaHeight * 0.05f; // 间距为总高度的5%
    float buttonHeight = (totalAreaHeight - 2 * spacing) / 3; // 每个按钮的高度

    // 计算每个按钮的Y坐标
    float recordButtonY = topBarHeight;
    float stopButtonY = topBarHeight + buttonHeight + spacing;
    float loadButtonY = topBarHeight + 2 * (buttonHeight + spacing);

    // 创建按钮对象
    RecordButton = new Button(
        glm::vec2(sideBarX, recordButtonY),
        glm::vec2(buttonWidth, buttonHeight),
        ResourceManager::GetTexture("block")
    );
    RecordButton->setText("Record");

    StopButton = new Button(
        glm::vec2(sideBarX, stopButtonY),
        glm::vec2(buttonWidth, buttonHeight),
        ResourceManager::GetTexture("block")
    );
    StopButton->setText("Stop");

    LoadButton = new Object2D(
        glm::vec2(sideBarX, loadButtonY),
        glm::vec2(buttonWidth, buttonHeight),
        ResourceManager::GetTexture("block")
    );
    LoadButton->setText("Load");

    // Load Step Manager
    stepManager = new StepManager(this->towerLevel);

    // Input Record Name
    textInput = new TextInput(this->Width, this->Height);
    textInput->setOnSubmitCallback([this](const std::string& result) {
        // 处理输入完成后的逻辑
        std::cout << "Input result: " << result << std::endl;
        if (result.size() == 0) {
            messageBox->setMessage("ERROR : Please Enter Something");
            return;
        }
        if (!this->beginRecord(result)) {
            messageBox->setMessage("ERROR : Please Choose Another Name");
        }
        });

    stepManager->regViewCall([this](const std::string& result) {
        // 处理输入完成后的逻辑
        std::cout << "Display result: " << result << std::endl;
        messageBox->setMessage(result);
        });
    stepManager->regSwCall([this](const std::string& result) {
        // 处理输入完成后的逻辑
        std::cout << "Display result: " << result << std::endl;
        eventBus.AddHighPriorityEvent("Please Choose Source Tower", 3.0f);
        switchTemp = result;
        State = GAME_SWITCH;
        });

    stepManager->regLoadCall([this](std::vector<Move>* load) {
        timer.init(load);
        clearPlateSelections();
        State = GAME_ACTIVE;
        });

    stepManager->regExitCall([this]() {
        State = GAME_ACTIVE;
        });

    timer.setCallBack([this](Move move) {
        Hanoi* source = towers[move.from];
        Hanoi* target = towers[move.to];
        if (isMoveValid(source, target)) {
            source->getTopPlate()->select();
            movePlate(*source, move.from, *target, move.to);
        }
        else {
            eventBus.AddHighPriorityEvent("ERROR : Fail to Load Memory", 5.0f);
            timer.reset();
        }
        });

    messageBox->setOnConfirmCallback([&]() {});
    messageBox->setMessage(std::string("Specifically we want you to move piles ") + "of radioactive disks from an old reactor. Just be sure"
        + " not to put a bigger disk on top of a smaller disk or the whole ship will blow up.", ResourceManager::GetTexture("alien"));
    initialized = true;
}

bool Game::beginRecord(std::string name) {
    return stepManager->record(name);
}

void Game::clearPlateSelections() {
    for (auto& [i, tower] : towers) {
        for (auto& plate : tower->disks) {
            if (plate.second.isSelect()) {
                plate.second.select();
            }
        }
    }
}

void Game::Update(float dt)
{
    eventBus.Update(dt);
    timer.update(dt);

    if (State == GAME_ACTIVE) {
        RecordButton->UpdateBounceAnimation(dt);
        StopButton->UpdateBounceAnimation(dt);
    }
}


void Game::ProcessInput(float dt)
{
    Renderer->process_keyboard_input(Keys, dt);
}

int from = -1, to = -1;

void Game::ProcessMouse(float dt, GLFWwindow* window) {
    // 检测是否为完整的鼠标点击（按下并释放）
    const bool isCompleteClick = (!mousePressed && mouseWasPressed);
    mouseWasPressed = mousePressed;

    // 获取鼠标位置
    double dmouseX, dmouseY;
    glfwGetCursorPos(window, &dmouseX, &dmouseY);

    float cursorX = static_cast<float>(dmouseX);
    float cursorY = static_cast<float>(dmouseY);

    // camera mouse update
    Renderer->MouseUpdate(cursorX, cursorY);

    // check button click
    if (!isCompleteClick) return;

    if (State == GAME_MENU) {
        menu->mouseClick(cursorX, cursorY);
        return;
    }

    if (textInput->isActive()) return;

    if (messageBox->isActive()) {
        messageBox->ProcessMouseClick(static_cast<float>(cursorX), static_cast<float>(cursorY));
        return;
    }

    if (State == GAME_ACTIVE) {
        // 检查是否已经有盘子被选中
        bool hasSelectedPlate = false;
        for (auto& [towerId, tower] : towers) {
            if (!tower->isEmpty()) {
                Plate3D& plate = tower->disks.begin()->second;
                if (plate.isSelect()) {
                    hasSelectedPlate = true;
                    break;
                }
            }
        }

        // 如果有盘子被选中，优先检查柱子点击
        if (hasSelectedPlate) {
            // 先尝试处理柱子点击
            bool towerClicked = false;
            for (auto& [towerId, tower] : towers) {
                if (tower->pole.isChosen(cursorX, cursorY, &Renderer->camera)) {
                    handleTowerClick(cursorX, cursorY);
                    towerClicked = true;
                    break;
                }
            }
            
            // 如果点击了柱子，直接返回，不再检查盘子点击
            if (towerClicked) {
                // 继续处理其他按钮点击
                if (RecordButton->isChosen(cursorX, cursorY, NULL)) {
                    textInput->setActive(true);
                    RecordButton->StartBounceAnimation();
                    soundTrigger();
                }

                if (StopButton->isChosen(cursorX, cursorY, NULL)) {
                    stepManager->endRecord();
                    StopButton->StartBounceAnimation();
                    soundTrigger();
                }

                if (LoadButton->isChosen(cursorX, cursorY, NULL)) {
                    State = GAME_LOAD;
                    soundTrigger();
                }
                return;
            }
        }

        // 尝试选择点击的盘子（只有在没有选中盘子或没有点击柱子时执行）
        Plate3D* clickedPlate = nullptr;
        Hanoi* sourceTower = nullptr;

        for (auto& [towerId, tower] : towers) {
            if (tower->isEmpty()) continue;

            Plate3D* topPlate = tower->getTopPlate();
            if (topPlate->isChosen(cursorX, cursorY, &Renderer->camera)) {
                soundTrigger();
                // 取消其他塔顶盘子的选中状态
                clearOtherPlateSelections(towerId);

                topPlate->select();
                clickedPlate = topPlate;
                sourceTower = tower;
                break;
            }
        }

        // 如果没有点击到盘子，检查柱子点击（当没有选中盘子时执行）
        if (!clickedPlate && !hasSelectedPlate) {
            handleTowerClick(cursorX, cursorY);
        }

        if (RecordButton->isChosen(cursorX, cursorY, NULL)) {
            textInput->setActive(true);
            RecordButton->StartBounceAnimation();
            soundTrigger();
        }

        if (StopButton->isChosen(cursorX, cursorY, NULL)) {
            stepManager->endRecord();
            StopButton->StartBounceAnimation();
            soundTrigger();
        }

        if (LoadButton->isChosen(cursorX, cursorY, NULL)) {
            State = GAME_LOAD;
            soundTrigger();
        }
    }

    if (State == GAME_LOAD) {
        stepManager->onMouseReleased(cursorX, cursorY);
    }

    if (State == GAME_SWITCH) {
        for (auto& [i, tower] : towers) {
            if (tower->base.isChosen(cursorX, cursorY, &Renderer->camera)) {
                if (from < 0) {
                    from = i;
                    tower->base.setText("From");
                    eventBus.AddHighPriorityEvent("Please Choose Target Tower", 3.0f);
                }
                else {
                    if (to < 0) {
                        to = i;
                        towers[from]->base.setText(std::to_string(from));

                        stepManager->switchNum(switchTemp, from, to);

                        eventBus.ClearAll();
                        eventBus.AddHighPriorityEvent("Switch Complete", 2.0f);

                        from = -1 , to = -1;
                        State = GAME_ACTIVE;
                    }
                }
            }
        }
    }
}

void Game::MouseScroll(double yoffset)
{
    Renderer->MouseScroll(yoffset);
}

// 清除其他塔的盘子选中状态
void Game::clearOtherPlateSelections(int currentTowerId) {
    for (auto& [towerId, tower] : towers) {
        if (towerId != currentTowerId && !tower->isEmpty()) {
            Plate3D& otherPlate = tower->disks.begin()->second;
            if (otherPlate.isSelect()) {
                otherPlate.select();
            }
        }
    }
}

// 处理塔点击事件
void Game::handleTowerClick(float cursorX, float cursorY) {
    // 查找当前选中的盘子
    Plate3D* selectedPlate = nullptr;

    int sourceId;
    Hanoi* sourceTower = nullptr;

    for (auto& [towerId, tower] : towers) {
        if (!tower->isEmpty()) {
            Plate3D& plate = tower->disks.begin()->second;
            if (plate.isSelect()) {
                selectedPlate = &plate;
                sourceTower = tower;
                sourceId = towerId;
                break;
            }
        }
    }

    if (!selectedPlate) return;

    // 检查是否点击了目标塔
    for (auto& [towerId, targetTower] : towers) {
        if (targetTower->pole.isChosen(cursorX, cursorY, &Renderer->camera)) {
            // 验证移动是否合法
            if (isMoveValid(*targetTower, *selectedPlate)) {
                movePlate(*sourceTower, sourceId, *targetTower, towerId);
            }
            else {
                messageBox->setOnConfirmCallback([&]() {ResetLevel(); });
                messageBox->setMessage("You Fail!", ResourceManager::GetTexture("alien"));
            }
            break;
        }
    }

}

// 检查移动是否合法
bool Game::isMoveValid(Hanoi& targetTower, Plate3D& plate) {
    return targetTower.isEmpty() || targetTower.getTop() > plate.level;
}

bool Game::isMoveValid(Hanoi* sourceTower, Hanoi* targetTower) {
    return targetTower->isEmpty() || targetTower->getTop() > sourceTower->getTop();
}

// 辅助函数：执行盘子移动
void Game::movePlate(Hanoi& sourceTower, int sourceId, Hanoi& targetTower, int targetId) {
    auto [plateLevel, plateObj] = sourceTower.PopTop();

    // 更新盘子位置
    const float xOffset = targetTower.pos.x - sourceTower.pos.x;
    plateObj.Position.x += xOffset;

    // 创建并添加事件
    std::string eventMsg = "Switch Tower " + std::to_string(sourceId) +
        " To Tower " + std::to_string(targetId);
    eventBus.AddMediumPriorityEvent(eventMsg, 2.0f);

    stepManager->insert(sourceId, targetId);
    Step++;

    plateObj.select(); // 取消选中状态
    targetTower.PushTop(plateObj, plateLevel);
}

void Game::Render(float deltaTime)
{
    static glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    //Renderer->DrawLine(glm::vec2(0.0f, 0.0f), glm::vec2(100.0f, 100.0f), 3.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    if (State == GAME_MENU) {
        glEnable(GL_DEPTH_TEST);
        Renderer->DrawMenuSkyBox(deltaTime, 45.0f, Width / Height);
        glDisable(GL_DEPTH_TEST);
        menu->Draw(*Renderer, *Text, this->Width, this->Height);
        return;
    }

    if (State == GAME_ACTIVE || State == GAME_SWITCH) {

        glEnable(GL_DEPTH_TEST);
        if(ground)
        {
            Renderer->DrawGround(-0.1f);
        }
        Renderer->DrawLightCube();
        //for (unsigned int i = 0; i < 10; i++)
        //{
        //    Renderer->DrawCube(cubePositions[i]);
        //}
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(3.0f, -3.0f, 3.0f));
        //Renderer->DrawCylinder(model , glm::vec3(1.0f , 1.0f , 1.0f));
        //Renderer->DrawCylinder2D(glm::vec2(0.0f , 0.0f), glm::vec2(300.0f, 300.0f) , 0.0f , glm::vec3(1.0f , 0.0f , 0.0f));
        //Renderer->DrawCylinder2D(glm::vec2(0.0f, 0.0f), glm::vec2(600.0f, 600.0f));

        for (auto& [num, tower] : towers) {
            tower->Draw(*Renderer, *Text);
        }
        Renderer->DrawSkyBox();
        glDisable(GL_DEPTH_TEST);

        // 渲染UI和文本
        Text->RenderTextInBox("Front: " + std::to_string(Renderer->camera.Front.x) + " " + std::to_string(Renderer->camera.Front.y) + " " + std::to_string(Renderer->camera.Front.z)
            , 0.0f , this->Height - 60.0f ,400.0f , 30.0f , 0.9f , glm::vec3(1.0f));
        Text->RenderTextInBox("Position: " + std::to_string(Renderer->camera.Position.x) + " " + std::to_string(Renderer->camera.Position.y) + " " + std::to_string(Renderer->camera.Position.z)
            , 0.0f, this->Height - 30.0f, 400.0f, 30.0f, 0.9f, glm::vec3(1.0f));

        // Render topbar
        std::string tbText{ "Step: " };
        tbText.append(std::to_string(Step));

        Text->RenderTextInBox(tbText, sideBarX, 0, sideBarWidth, topBarHeight, 1.5f, glm::vec3(1.0f, 1.0f, 1.0f));

        if (eventBus.IsDisplayingEvent()) {
            std::string str = eventBus.GetCurrentMessage();
            std::cout << str << std::endl;
            Text->RenderTextInBox(str, 0, 0, this->Width - sideBarWidth, topBarHeight, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
        }

        RecordButton->Draw(*Renderer);
        StopButton->Draw(*Renderer);
        LoadButton->Draw(*Renderer);

        RecordButton->DrawText(*Text);
        StopButton->DrawText(*Text);
        LoadButton->DrawText(*Text);

        textInput->Draw(*Renderer, *Text);

        // Display Cross
        Renderer->DrawLine(glm::vec2(-30.0f + this->Width / 2, this->Height / 2), glm::vec2(30.0f + this->Width / 2, this->Height / 2), 4.0f, glm::vec3(1.0f));
        Renderer->DrawLine(glm::vec2(this->Width / 2, -30.0f + this->Height / 2), glm::vec2(this->Width / 2, 30.0f + this->Height / 2), 4.0f, glm::vec3(1.0f));
    }

    messageBox->Draw(*Renderer, *Text);

    if (State == GAME_LOAD) {
        stepManager->Render(*Renderer, *Text, this->Width, this->Height);
    }
}

void Game::soundTrigger()
{
    if (sound)
        SoundEngine->play2D(trigger, false);
}

void Game::ResetLevel()
{
    stepManager->endRecord();
    stepManager->clear();
    // 释放塔资源
    for (auto& [id, tower] : towers) {
        if (tower) {
            delete tower;
        }
    }
    towers.clear();
    SoundEngine->drop();
    State = GAME_MENU;
}
