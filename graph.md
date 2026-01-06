# Workspace

```` mermaid
classDiagram
    %% 核心类
    class Game {
        +GameState State : 游戏当前状态（活动、菜单、加载、切换）
        +bool Keys[1024] : 键盘按键状态
        +bool mousePressed : 鼠标按下状态
        +unsigned int Width, Height : 游戏窗口尺寸
        +int towerNum, towerLevel : 塔数量和盘子数量
        +Object2D* exit : 退出按钮
        +MessageBox* messageBox : 消息框
        +TextInput* textInput : 文本输入框
        
        +init() : 初始化游戏资源和状态
        +update() : 更新游戏逻辑
        +render() : 渲染游戏画面
        +processMouse() : 处理鼠标输入
        +processKeyboard() : 处理键盘输入
        +Reenter() : 重新进入游戏
        +enter() : 进入游戏
        +beginRecord() : 开始记录步骤
        +isMoveValid() : 判断移动是否有效
        
        核心游戏类，管理游戏的整体流程、状态和用户输入
    }
    
    class SpriteRenderer {
        +Camera camera : 3D摄像机
        +glm::mat4 projection, view : 投影和视图矩阵
        +Shader shader, lightingShader, skyboxShader : 各种渲染着色器
        +Texture2D diffuseMap, specularMap : 材质纹理
        
        +drawSprite() : 绘制2D精灵
        +drawRectangle() : 绘制矩形
        +drawLine() : 绘制线条
        +drawCylinder() : 绘制圆柱体（塔结构）
        +drawCube() : 绘制立方体
        +drawGround() : 绘制地面
        +drawSkyBox() : 绘制天空盒
        +mouseUpdate() : 更新鼠标输入
        +mouseScroll() : 处理鼠标滚轮输入
        
        负责游戏中所有图形元素的渲染，包括2D和3D对象
    }
    
    class Camera {
        +glm::vec3 Position, Front, Up, Right, WorldUp : 摄像机位置和方向向量
        +float Yaw, Pitch, MovementSpeed, MouseSensitivity, Zoom : 摄像机参数
        
        +getViewMatrix() : 获取视图矩阵
        +processKeyboard() : 处理键盘移动
        +processMouseMovement() : 处理鼠标移动
        +processMouseScroll() : 处理鼠标滚轮缩放
        
        3D摄像机类，控制游戏视角和观察方式
    }
    
    %% 渲染相关类
    class Shader {
        +use() : 使用着色器
        +setInteger() : 设置整数 uniforms
        +setFloat() : 设置浮点数 uniforms
        +setVector3f() : 设置三维向量 uniforms
        +setMatrix4() : 设置4x4矩阵 uniforms
        
        着色器类，管理OpenGL着色器程序
    }
    
    class Texture2D {
        +bind() : 绑定纹理
        +unbind() : 解绑纹理
        
        纹理类，管理游戏中的纹理资源
    }
    
    class TextRenderer {
        +renderText() : 渲染文本
        +renderTextInBox() : 在指定区域内渲染文本
        
        文本渲染类，负责游戏中的文字显示
    }
    
    class PostProcessor {
        +beginRender() : 开始后处理渲染
        +endRender() : 结束后处理渲染
        +render() : 应用后处理效果
        
        后处理效果类，用于应用特效（如模糊、抖动等）
    }
    
    %% UI相关基类
    class Object2D {
        +glm::vec2 Position, Velocity : 位置和速度
        +glm::vec3 Color : 颜色
        +float Rotation : 旋转角度
        +float Width, Height : 尺寸
        +Texture2D Sprite : 纹理
        
        +draw() : 绘制对象
        +drawText() : 绘制文本
        +isChosen() : 判断是否被选中
        
        所有2D对象的基类，提供通用的2D对象属性和方法
    }
    
    %% UI组件（继承 Object2D）
    class Button {
        +draw() : 绘制按钮
        +isChosen() : 判断按钮是否被点击
        
        按钮类，用于用户交互，继承自Object2D
    }
    
    class Menu {
        +Button* groundOn, towerAdd, towerSub, diskAdd, diskSub, soundOn, volumeAdd, volumeSub, okButton : 菜单按钮
        +bool groundEnabled, soundEnabled : 地面和声音状态
        +int towerCount, diskCount : 塔和盘子数量
        +float volume : 音量
        
        +draw() : 绘制菜单
        +mouseClick() : 处理菜单鼠标点击
        +SetCallback() : 设置菜单回调函数
        
        游戏菜单类，用于设置游戏参数
    }
    
    class MessageBox {
        +Object2D* background, buttonArea : 消息框UI元素
        +std::string message : 消息内容
        +bool active : 是否激活
        
        +draw() : 绘制消息框
        +processMouseClick() : 处理消息框鼠标点击
        +setMessage() : 设置消息内容
        +isActive() : 判断是否激活
        +setOnConfirmCallback() : 设置确认回调
        
        消息框类，用于显示提示信息，继承自Object2D
    }
    
    class TextInput {
        +draw() : 绘制文本输入框
        +processKey() : 处理键盘输入
        +isActive() : 判断是否处于激活状态
        
        文本输入类，用于用户输入文本，继承自Object2D
    }
    
    %% 游戏逻辑类
class Move {
    +int from
    +int to
}

class StepManager {
    +bool isRecording
    +Form* form
    +std::map<std::string, std::vector<Move>> data
    
    +insert(from, to)
    +record(name)
    +endRecord()
    +update()
    +copy(source)
    +switchNum(source, raw, target)
    +Render()
    +onMouseReleased()
    +movesToString()
    
    步骤管理器类，用于记录和管理游戏步骤
}
    
    class EventBus {
        +addEvent() : 添加事件
        +addHighPriorityEvent() : 添加高优先级事件
        +getCurrentMessage() : 获取当前消息
        +IsDisplayingEvent() : 判断是否正在显示事件
        
        事件总线类，用于管理游戏中的事件通知
    }
    
    class Hanoi {
        +int numDisks : 盘子数量
        +std::map<int, Plate3D> disks : 盘子映射
        +Object3D pole, base : 塔的3D模型（柱子和底座）
        +glm::vec2 pos, size : 塔的位置和尺寸
        
        +Draw() : 渲染汉诺塔
        +PushTop() : 压入顶层盘子
        +PopTop() : 弹出顶层盘子
        +isEmpty() : 判断塔是否为空
        +getTopPlate() : 获取顶层盘子
        +isFull() : 判断塔是否已满
        
        汉诺塔类，管理单个塔的盘子和3D模型
    }
    
    class Plate3D {
        +int level : 盘子层级
        +glm::vec3 color : 盘子颜色
        +glm::vec2 pos, velocity : 位置和速度
        +float sizeX, sizeY : 盘子尺寸
        
        +draw() : 绘制3D盘子
        +isChosen() : 判断是否被选中
        +isSelect() : 判断是否处于选中状态
        +select() : 选择盘子
        
        3D盘子类，继承自Plate，代表汉诺塔中的一个盘子
    }
    
    %% 工具类
    class ResourceManager {
        +loadShader() : 加载着色器
        +loadTexture() : 加载纹理
        +clear() : 清除资源
        
        资源管理类，统一管理游戏中的着色器和纹理资源
    }
    
    class CoordinateTrans {
        +getInstance() : 获取单例实例
        +setDimensions() : 设置尺寸
        
        坐标转换类，用于2D和3D坐标之间的转换
    }
    
    class Timer {
        +start() : 开始计时
        +stop() : 停止计时
        +reset() : 重置计时器
        +getElapsedTime() : 获取已流逝时间
        
        计时器类，用于游戏中的时间管理
    }
    
    %% 继承关系
    Button --|> Object2D : 继承自2D对象基类
    Menu --|> Object2D : 继承自2D对象基类
    MessageBox --|> Object2D : 继承自2D对象基类
    TextInput --|> Object2D : 继承自2D对象基类
    Plate3D --|> Plate : 继承自盘子基类
    
    %% 依赖/使用关系（用 ..> 表示）
    Game ..> SpriteRenderer : 使用渲染器渲染游戏
    Game ..> TextRenderer : 使用文本渲染器显示文字
    Game ..> PostProcessor : 使用后处理器应用特效
    Game ..> StepManager : 使用步骤管理器记录和回放
    Game ..> EventBus : 使用事件总线处理通知
    Game ..> Button : 使用按钮组件
    Game ..> Menu : 使用菜单组件
    Game ..> MessageBox : 使用消息框组件
    Game ..> TextInput : 使用文本输入组件
    Game ..> Hanoi : 包含多个汉诺塔实例
    Game ..> ResourceManager : 使用资源管理器加载资源
    Game ..> CoordinateTrans : 使用坐标转换工具
    
    SpriteRenderer ..> Camera : 使用摄像机确定视角
    SpriteRenderer ..> Shader : 使用各种着色器进行渲染
    SpriteRenderer ..> Texture2D : 使用纹理进行材质渲染
    SpriteRenderer ..> Plate3D : 渲染3D盘子
    SpriteRenderer ..> CoordinateTrans : 使用坐标转换工具
    
    TextRenderer ..> Shader : 使用文本着色器
    TextRenderer ..> Texture2D : 使用字体纹理
    
    PostProcessor ..> Shader : 使用后处理着色器
    PostProcessor ..> Texture2D : 使用渲染纹理
    
    Object2D ..> SpriteRenderer : 使用渲染器绘制
    Object2D ..> TextRenderer : 使用文本渲染器显示文字
    
    Hanoi ..> Plate3D : 包含多个3D盘子
    
    ResourceManager ..> Shader : 管理着色器资源
    ResourceManager ..> Texture2D : 管理纹理资源
    
    StepManager ..> Object2D : 使用2D对象构建界面
    StepManager ..> SpriteRenderer : 使用渲染器绘制界面
    StepManager ..> TextRenderer : 使用文本渲染器显示文字
````