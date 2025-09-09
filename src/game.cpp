#include <algorithm>
#include <sstream>
#include <iostream>

#include "game.h"
#include "resource_manager.h"
#include "sprite_renderer.h"
#include "game_object.h"
#include "particle_generator.h"
#include "post_processor.h"
#include "text_renderer.h"
#include "step_manager.h"
#include "evenbus.h"
#include "button.h"
#include "menu.h"

// Game-related State data
SpriteRenderer* Renderer;
ParticleGenerator* Particles;
PostProcessor* Effects;
ISoundEngine* SoundEngine;
TextRenderer* Text;
std::map<int, Hanoi*> towers;
StepManager* stepManager;
EventBus eventBus{};

Button* RecordButton;
Button* StopButton;
GameObject* LoadButton;

Menu* menu;

Game::Game(unsigned int width, unsigned int height)
    : State(GAME_MENU), Keys(), KeysProcessed(), Width(width), Height(height), Step(0)
{
}

Game::~Game()
{
    // �ͷų�Ա����
    if (textInput) {
        delete textInput;
        textInput = nullptr;
    }
    
    if (messageBox) {
        delete messageBox;
        messageBox = nullptr;
    }
    
    // �ͷŰ�ť��Դ
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
    
    // �ͷ�����Դ
    for (auto& [id, tower] : towers) {
        if (tower) {
            delete tower;
        }
    }
    towers.clear();
    
    // �ͷ�StepManager
    if (stepManager) {
        delete stepManager;
        stepManager = nullptr;
    }
    
    // �ͷ�Menu
    if (menu) {
        delete menu;
        menu = nullptr;
    }
    
    // �ͷ���Ⱦ�����Դ
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
    
    // �ͷ�TextRenderer
    if (Text) {
        delete Text;
        Text = nullptr;
    }
    
    // �ͷ�SoundEngine��ʹ��drop������delete��
    if (SoundEngine) {
        SoundEngine->drop();
        SoundEngine = nullptr;
    }
    
    // ���������Դ�������е���Դ
    ResourceManager::Clear();
}
{
}

void Game::Init()
{
    SoundEngine = createIrrKlangDevice();
    // Load Sound
    SoundEngine->play2D("resources/audio/funky_stars.mp3", GL_TRUE);
    // Load freetype
    Text = new TextRenderer(this->Width, this->Height);
    Text->Load("resources/fonts/arial.ttf", 24);

    // Load shaders
    ResourceManager::LoadShader("shaders/sprite/vertShader.glsl", "shaders/sprite/fragShader.glsl", nullptr, "sprite");
    ResourceManager::LoadShader("shaders/particles/vertShader.glsl", "shaders/particles/fragShader.glsl", nullptr, "particle");
    ResourceManager::LoadShader("shaders/post_processor/vertShader.glsl", "shaders/post_processor/fragShader.glsl", nullptr, "post_processor");
    ResourceManager::LoadShader("shaders/rectangle/vertShader.glsl", "shaders/rectangle/fragShader.glsl", nullptr, "rectangle");
    // Configure shaders
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width), static_cast<float>(this->Height), 0.0f, -1.0f, 1.0f);
    ResourceManager::GetShader("sprite").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);
    ResourceManager::GetShader("particle").Use().SetInteger("sprite", 0);
    ResourceManager::GetShader("particle").SetMatrix4("projection", projection);

    ResourceManager::GetShader("rectangle").Use().SetMatrix4("projection", projection);
    // ��������
    ResourceManager::LoadTexture("resources/textures/background.jpg", GL_FALSE, "background");
    ResourceManager::LoadTexture("resources/textures/block.png", GL_FALSE, "block");

    // Set render-specific controls
    Particles = new ParticleGenerator(ResourceManager::GetShader("particle"), ResourceManager::GetTexture("particle"), 500);
    Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"), ResourceManager::GetShader("rectangle"));
    Effects = new PostProcessor(ResourceManager::GetShader("post_processor"), this->Width, this->Height);

    messageBox = new MessageBox(this->Width, this->Height);
    menu = new Menu(this->Width, this->Height);

    // ���ûص�����
    menu->SetCallback([this](int towers, int disks, bool sound, float volume) {
        std::cout << "Starting game with: " << towers << " towers, "
            << disks << " disks, sound " << (sound ? "ON" : "OFF")
            << ", volume: " << volume << std::endl;
        this->towerNum = towers;
        this->towerLevel = disks;
        this->enter();
        State = GAME_ACTIVE;
    });
}

void Game::enter() {

    float tenH = this->Height * 0.1f;
    float eightH = this->Height * 0.08f;

    float twentyW = this->Width * 0.2f;
    float twelfthW = this->Width * 0.12f;
    float twoW = this->Width * 0.02f;
    float sixW = this->Width * 0.06f;

    float towerWidth = (this->Width - twentyW) / (float)this->towerNum;
    float towerhHeight = this->Height - tenH;

    towers.emplace(0, new Hanoi(towerLevel, glm::vec2(twoW, eightH), glm::vec2(towerWidth, towerhHeight), false));
    for (int i = 1; i < this->towerNum; i++) {
        float posX = twoW + (i * towerWidth) + (sixW / (towerNum - 1)) * i;
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
    // ���㰴ť�ĳߴ��λ��
    float buttonWidth = this->Width - sideBarX;  // ��ť���Ϊ��������
    float totalAreaHeight = this->Height - topBarHeight; // �����ܸ߶�
    float spacing = totalAreaHeight * 0.05f; // ���Ϊ�ܸ߶ȵ�5%
    float buttonHeight = (totalAreaHeight - 2 * spacing) / 3; // ÿ����ť�ĸ߶�

    // ����ÿ����ť��Y����
    float recordButtonY = topBarHeight;
    float stopButtonY = topBarHeight + buttonHeight + spacing;
    float loadButtonY = topBarHeight + 2 * (buttonHeight + spacing);

    // ������ť����
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

    LoadButton = new GameObject(
        glm::vec2(sideBarX, loadButtonY),
        glm::vec2(buttonWidth, buttonHeight),
        ResourceManager::GetTexture("block")
    );
    LoadButton->setText("Load");

    // Load Step Manager
    stepManager = new StepManager();

    // Input Record Name
    textInput = new TextInput(this->Width, this->Height);
    textInput->setOnSubmitCallback([this](const std::string& result) {
        // ����������ɺ���߼�
        std::cout << "Input result: " << result << std::endl;
        if (result.size() == 0) {
            messageBox->setMessage("ERROR : Please Choose Another Name");
            messageBox->setActive(true);
            return;
        }
        if (!this->beginRecord(result)) {
            messageBox->setMessage("ERROR : Please Choose Another Name");
            messageBox->setActive(true);
        }
        });

    stepManager->regViewCall([this](const std::string& result) {
        // ����������ɺ���߼�
        std::cout << "Display result: " << result << std::endl;
        messageBox->setMessage(result);
        messageBox->setActive(true);
        });
    stepManager->regSwCall([this](const std::string& result) {
        // ����������ɺ���߼�
        std::cout << "Display result: " << result << std::endl;
        eventBus.AddHighPriorityEvent("Please Choose Source Tower", 3.0f);
        switchTemp = result;
        State = GAME_SWITCH;
        });

    stepManager->regLoadCall([this](std::vector<Move>* load) {
        timer.init(load);
        for (auto& [i, tower] : towers) {
            for (auto& plate : tower->disks) {
                if (plate.second.isSelect()) {
                    plate.second.select();
                }
            }

        }
        State = GAME_ACTIVE;
        });

    timer.setCallBack([this](Move move) {
        Hanoi* source = towers[move.from];
        Hanoi* target = towers[move.to];
        if (isMoveValid(source, target)) {
            movePlate(*source, move.from, *target, move.to);
        }
        else {
            eventBus.AddHighPriorityEvent("ERROR : Fail to Load Memory", 5.0f);
            timer.reset();
        }
        });

}

bool Game::beginRecord(std::string name) {
    return stepManager->record(name);
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

}

int from = -1, to = -1;

void Game::ProcessMouse(float dt, GLFWwindow* window) {
    // ����Ƿ�Ϊ����������������²��ͷţ�
    const bool isCompleteClick = (!mousePressed && mouseWasPressed);
    mouseWasPressed = mousePressed;

    if (!isCompleteClick) return;

    // ��ȡ���λ��
    double cursorX, cursorY;
    glfwGetCursorPos(window, &cursorX, &cursorY);

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
        // ����ѡ����������
        Plate* clickedPlate = nullptr;
        Hanoi* sourceTower = nullptr;

        for (auto& [towerId, tower] : towers) {
            if (tower->isEmpty()) continue;

            Plate& topPlate = tower->disks.begin()->second;
            if (topPlate.isChosen(cursorX, cursorY)) {
                // ȡ�������������ӵ�ѡ��״̬
                clearOtherPlateSelections(towerId);

                topPlate.select();
                clickedPlate = &topPlate;
                sourceTower = tower;
                break;
            }
        }

        // ���û�е�������ӣ�������ӵ��
        if (!clickedPlate) {
            handleTowerClick(cursorX, cursorY);
        }

        if (RecordButton->isChosen(cursorX, cursorY)) {
            textInput->setActive(true);
            RecordButton->StartBounceAnimation();
        }

        if (StopButton->isChosen(cursorX, cursorY)) {
            stepManager->endRecord();
            StopButton->StartBounceAnimation();
        }

        if (LoadButton->isChosen(cursorX, cursorY)) {
            State = GAME_LOAD;
        }
    }

    if (State == GAME_LOAD) {
        stepManager->onMouseReleased(cursorX, cursorY);
    }

    if (State == GAME_SWITCH) {
        for (auto& [i, tower] : towers) {
            if (tower->base.isChosen(cursorX, cursorY)) {
                if (from < 0) {
                    from = i;
                    tower->base.setText("From");
                    eventBus.AddHighPriorityEvent("Please Choose Target Tower", 3.0f);
                }
                else {
                    if (to < 0) {
                        to = i;
                        stepManager->switchNum(switchTemp, from, to);
                        State = GAME_ACTIVE;
                    }
                }
            }
        }
    }
}

// ���������������ѡ��״̬
void Game::clearOtherPlateSelections(int currentTowerId) {
    for (auto& [towerId, tower] : towers) {
        if (towerId != currentTowerId && !tower->isEmpty()) {
            Plate& otherPlate = tower->disks.begin()->second;
            if (otherPlate.isSelect()) {
                otherPlate.select();
            }
        }
    }
}

// ����������¼�
void Game::handleTowerClick(double cursorX, double cursorY) {
    // ���ҵ�ǰѡ�е�����
    Plate* selectedPlate = nullptr;

    int sourceId;
    Hanoi* sourceTower = nullptr;

    for (auto& [towerId, tower] : towers) {
        if (!tower->isEmpty()) {
            Plate& plate = tower->disks.begin()->second;
            if (plate.isSelect()) {
                selectedPlate = &plate;
                sourceTower = tower;
                sourceId = towerId;
                break;
            }
        }
    }

    if (!selectedPlate) return;

    // ����Ƿ�����Ŀ����
    for (auto& [towerId, targetTower] : towers) {
        if (targetTower->pole.isChosen(cursorX, cursorY)) {
            // ��֤�ƶ��Ƿ�Ϸ�
            if (isMoveValid(*targetTower, *selectedPlate)) {
                movePlate(*sourceTower, sourceId, *targetTower, towerId);
            }
            break;
        }
    }

}

// ����ƶ��Ƿ�Ϸ�
bool Game::isMoveValid(Hanoi& targetTower, Plate& plate) {
    return targetTower.isEmpty() || targetTower.getTop() > plate.level;
}

bool Game::isMoveValid(Hanoi* sourceTower, Hanoi* targetTower) {
    return targetTower->isEmpty() || targetTower->getTop() > sourceTower->getTop();
}

// ����������ִ�������ƶ�
void Game::movePlate(Hanoi& sourceTower, int sourceId, Hanoi& targetTower, int targetId) {
    auto [plateLevel, plateObj] = sourceTower.PopTop();

    // ��������λ��
    const float xOffset = targetTower.pos.x - sourceTower.pos.x;
    plateObj.Position.x += xOffset;

    // ����������¼�
    std::string eventMsg = "Switch Tower " + std::to_string(sourceId) +
        " To Tower " + std::to_string(targetId);
    eventBus.AddMediumPriorityEvent(eventMsg, 2.0f);

    stepManager->insert(sourceId, targetId);
    Step++;

    plateObj.select(); // ȡ��ѡ��״̬
    targetTower.PushTop(plateObj, plateLevel);
}

void Game::Render()
{
    if (State == GAME_MENU) {
        menu->Draw(*Renderer, *Text, this->Width, this->Height);
        return;
    }

    messageBox->Draw(*Renderer, *Text);
    if (State == GAME_ACTIVE || State == GAME_SWITCH) {

        // Render towers
        for (auto& [num, tower] : towers) {
            tower->Draw(*Renderer, *Text);
        }

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
    }

    if (State == GAME_LOAD) {
        stepManager->Render(*Renderer, *Text, this->Width, this->Height);
    }


}