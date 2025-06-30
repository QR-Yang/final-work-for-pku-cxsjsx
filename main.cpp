#include "mainwindow.h"
#include <QApplication>
#include <QFont>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 设置应用全局样式
    a.setStyle("Fusion");

    // 设置全局调色板
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(240, 240, 240));
    palette.setColor(QPalette::WindowText, QColor(33, 37, 41));
    palette.setColor(QPalette::Base, QColor(255, 255, 255));
    palette.setColor(QPalette::AlternateBase, QColor(248, 249, 250));
    palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 255));
    palette.setColor(QPalette::ToolTipText, QColor(33, 37, 41));
    palette.setColor(QPalette::Text, QColor(33, 37, 41));
    palette.setColor(QPalette::Button, QColor(233, 236, 239));
    palette.setColor(QPalette::ButtonText, QColor(33, 37, 41));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Link, QColor(13, 110, 253));
    palette.setColor(QPalette::Highlight, QColor(13, 110, 253));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    a.setPalette(palette);

    // 设置全局字体
    QFont font("Microsoft YaHei", 9);
    a.setFont(font);

    MainWindow w;
    w.show();

    return a.exec();
}
