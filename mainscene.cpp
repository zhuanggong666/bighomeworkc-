#include "mainscene.h"
#include  "config.h"
#include<QIcon>
#include<QMouseEvent>
#include<ctime>
#include <QMediaPlayer>
#include<QMessageBox>
#include<QApplication>

MainScene::MainScene(QWidget *parent)
    : QWidget(parent)
{   //控制音频输出
    m_audioOutput = new QAudioOutput(this);
    m_audioOutput->setVolume(0.5); // 50%音量

    m_bgMusicPlayer = new QMediaPlayer(this);
    m_bgMusicPlayer->setAudioOutput(m_audioOutput);
    m_bgMusicPlayer->setSource(QUrl("qrc:/diwut/guimi.mp3"));
    //循环播放
    m_bgMusicPlayer->setLoops(QMediaPlayer::Infinite);

    m_collisionCoolDown=true;
    //开始界面
    initStartInterface();
    //游戏界面
    initScene();

}

MainScene::~MainScene() {
    delete m_bgMusicPlayer;
    delete m_audioOutput;
}



void MainScene::initScene(){
    setFixedSize(GAME_WIDTH,GAME_HEIGHT);
    setWindowTitle(GAME_TITLE);
    //定时器设置
    m_Timer.setInterval(10);

    m_recorder1=0;
    //随机数种子
    srand((unsigned int)time(NULL));
    //播放音乐按钮
    createMusicButtons();
    // 生命值
    m_life = 3;
    //游戏结束状态
    m_gameOver = false;

}

void MainScene::playGame()
{
    //播放音乐
    playMusic();
    //启动定时器
    m_Timer.start();

    //监听定时器发送的信号
    connect(&m_Timer,&QTimer::timeout,[=](){
        //出场
        appearance();
        //更新游戏中元素的坐标
        updatePosition();
        //绘制到屏幕中
        update();
        //碰撞检测
        collisionDelete();
    });
}

void MainScene::updatePosition()
{
    //更新地图坐标
    m_map.mapPosition();
    //障碍物更新
    for(int i=0;i<30;i++){
        if(m_enemys1[i].m_Free==false){
            m_enemys1[i].updatePosition();
        }
    }
}

void MainScene::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    //绘制地图
    painter.drawPixmap(0,m_map.m_map1_posY,m_map.m_map1);
    painter.drawPixmap(0,m_map.m_map2_posY,m_map.m_map2);

    //绘制自己
    painter.drawPixmap(m_jieke.m_X,m_jieke.m_Y,m_jieke.m_jieke);

    //绘制障碍
    for(int i=0;i<30;i++){
        if(m_enemys1[i].m_Free==false){
            painter.drawPixmap(m_enemys1[i].m_X,m_enemys1[i].m_Y,m_enemys1[i].m_enemy[i]);
        }
    }
    // 绘制生命值
    painter.setPen(Qt::red);
    painter.setFont(QFont("华文楷体", 20, QFont::Bold));
    painter.drawText(20, 40, QString("生命值: %1").arg(m_life));

    // 绘制心形图标
    QPixmap heart(":/diwut/heart");
    for(int i=0; i<m_life; i++){
        painter.drawPixmap(20 + i*40, 50, 30, 30, heart);
    }
}
void MainScene::mouseMoveEvent(QMouseEvent *event)
{
    if(m_gameOver) return;
    int x=event->x()-m_jieke.m_Rect.width()*0.5;
    int y=event->y()-m_jieke.m_Rect.height()*0.5;
     //边界检测
    if(x<=0){
        x=0;
    }
    if(x>=GAME_WIDTH-m_jieke.m_Rect.width()){
        x=GAME_WIDTH-m_jieke.m_Rect.width();
    }
    if(y<=0){
        y=0;
    }
    if(y>=GAME_HEIGHT-m_jieke.m_Rect.height()){
        y=GAME_HEIGHT-m_jieke.m_Rect.height();
    }
    m_jieke.setPosition(x,y);
}


void MainScene::appearance()
{
    m_recorder1++;
    if(m_recorder1<30){
        return;
    }
    m_recorder1=0;
    for(int i=0;i<30;i++){
        //如果空闲，出场
        if(m_enemys1[i].m_Free){
            m_enemys1[i].m_Free=false;
            //坐标
            m_enemys1[i].m_X=rand()%(GAME_WIDTH-m_enemys1[i].m_Rect.width());
            m_enemys1[i].m_Y=-m_enemys1[i].m_Rect.height();
            break;
        }
    }
}

void MainScene::collisionDelete()
{

   //遍历非空闲障碍
    for(int i=0;i<30;i++){
        //如果空闲，执行下一次
       if(m_enemys1[i].m_Free){
            continue;
       }
           //如果矩形相交，则发生碰撞
           if(m_enemys1[i].m_Rect.intersects(m_jieke.m_Rect)){
           m_collisionCoolDown = true;
           QTimer::singleShot(1000, [this](){
               m_collisionCoolDown = false;
           });

           m_life--;
           update();

           m_enemys1[i].m_Free = true;
            QString message;
           // 生命值耗尽处理
           if(m_life <= 0){
               m_gameOver = true;
               m_Timer.stop();

               // 使用模态对话框并确保只创建一个实例
               QMessageBox msgBox(this);
               msgBox.setWindowTitle("游戏结束");
               msgBox.setText("生命值耗尽！");
               QPushButton *restartBtn = msgBox.addButton("重新开始", QMessageBox::AcceptRole);
               QPushButton *exitBtn = msgBox.addButton("退出游戏", QMessageBox::RejectRole);

               int result = msgBox.exec();

               if(result == QMessageBox::AcceptRole){
                   resetGameState();
                   m_Timer.start();
               } else {
                   qApp->exit();
               }
               return; // 直接返回避免后续处理
               }

               else{
               m_enemys1[i].m_Free=true;
               switch (i%6) {
               case 0:
                   message = "你被前锋的超长螺旋球撞晕了，请眩晕10秒钟！";
                   break;
               case 1:
                   message = "你被古董商的三连棍打中啦!18秒内不能攻击！";
                   break;
               case 2:
                   message = "你被勘探员的素质三连晕住了！";
                   break;
               case 3:
                   message = "你被骑士猜了普攻！你将无法使用普通攻击！";
                   break;
               case 4:
                   message = "你被空军的信号枪击中了！请眩晕5.5秒！";
                   break;
               case 5:
                   message = "你被火箭撞上了！请原地罚站！";
                   break;
               default:
                   break;
               }

           }

           // 创建对话框实例
           QMessageBox msgBox;
           msgBox.setWindowTitle("温馨提示");  // 标题
           msgBox.setText(message);          // 内容

           // 添加按钮
           QPushButton *customOkBtn = msgBox.addButton("继续游戏", QMessageBox::AcceptRole);
           msgBox.setDefaultButton(customOkBtn);  // 设置为默认按钮

           // 显示对话框
           msgBox.exec();
           }
    }
}


void MainScene::initStartInterface(){
    // 创建启动界面容器
    startInterface = new QWidget(this);  // 父部件是MainScene，跟随主窗口生命周期
    startInterface->setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    startInterface->setStyleSheet("background-image: url(:/diwut/jiazai.jpg); background-repeat: no - repeat;");
    QVBoxLayout *layout = new QVBoxLayout(startInterface);
    layout->setContentsMargins(0, 100, 0, 0);  // 上下左右边距（顶部留100px空白）
    layout->setSpacing(30);  // 控件之间的垂直间距30px



    // 创建开始按钮
    startBtn = new QPushButton("开始游戏",startInterface);
    startBtn->setFixedSize(170, 50);  // 按钮大小
    //按钮样式表
    startBtn->setStyleSheet(""
                           "font-size: 18px;"
                            "background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #66ccff, stop:1 #0099ff);"
                           "color: red;"
                           "border-radius: 10px;"
                            "padding: 5px;"
                            "");
    startBtn->setFont(QFont("华文楷体",50,10,1));
    startBtn->setCursor(Qt::PointingHandCursor);  // 鼠标移到按钮上时显示手型
    startBtn->move(410,600);
    //连接按钮的点击信号到切换场景的槽函数
    connect(startBtn, &QPushButton::clicked, this, &MainScene::switchToGameScene);
}

void MainScene::switchToGameScene()
{
    // 隐藏启动界面容器
    startInterface->hide();
    //显示按钮
    toggleMusicButtons(true);
    // 启动游戏
    playGame();
}

void MainScene::resetGameState()
{
    // 重置玩家
    m_jieke.resetPosition();

    // 重置障碍
    for(auto& enemy : m_enemys1){
        enemy.m_Free = true;
    }

    // 重置地图
    m_map.m_map1_posY=-GAME_HEIGHT;
    m_map.m_map2_posY=0;

    // 重置其他状态
    m_life = 3;
    m_gameOver = false;
    m_collisionCoolDown = false;

    // 强制重绘
    update();
}

void MainScene::playMusic()
{
    if(m_bgMusicPlayer->playbackState() != QMediaPlayer::PlayingState) {
        m_bgMusicPlayer->play();
    }
}

void MainScene::stopMusic()
{
    if(m_bgMusicPlayer->playbackState() == QMediaPlayer::PlayingState) {
        m_bgMusicPlayer->stop();
    }
}
void MainScene::createMusicButtons()
{
    // 创建音乐控制按钮
    m_playBtn = new QPushButton("开始播放", this);
    m_stopBtn = new QPushButton("停止播放", this);

    // 设置样式
    const QString btnStyle =
        "QPushButton {"
        "   font: bold 18px '微软雅黑';"
        "   padding: 4px;"
        "   background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(136, 255, 136, 0.7), stop:1 rgba(0, 204, 0, 0.7));"
        "   border-radius: 15px;"
        "   min-width: 40px;"
        "   min-height: 40px;"
        "   color: rgba(255, 255, 255, 0.9);"
        "}"
        "QPushButton:hover { background: rgba(0, 255, 0, 0.8); }";

    m_playBtn->setStyleSheet(btnStyle);
    m_stopBtn->setStyleSheet(btnStyle);


    m_playBtn->move(GAME_WIDTH-120 ,20);
    m_stopBtn->move(GAME_WIDTH-120 ,80);

    // 初始状态隐藏
    m_playBtn->hide();
    m_stopBtn->hide();

    // 连接信号
    connect(m_playBtn, &QPushButton::clicked, this, &MainScene::playMusic);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainScene::stopMusic);
}

void MainScene::toggleMusicButtons(bool visible)
{
    m_playBtn->setVisible(visible);
    m_stopBtn->setVisible(visible);
}
