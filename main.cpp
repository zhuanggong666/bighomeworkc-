#include "mainscene.h"
#include<QResource>
#include <QApplication>
#include"config.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //注册二进制资源文件信息
    QResource::registerResource(GAME_RES_PATH);
    MainScene w;
    w.show();
    return a.exec();
}
