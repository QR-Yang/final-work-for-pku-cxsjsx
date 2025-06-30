#include "courseadvicedialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMenu>
#include <QPushButton>

CourseAdviceDialog::CourseAdviceDialog(QWidget *parent)
    : QDialog(parent), currentSemester(1)
{
    setWindowTitle("选课建议");
    setupUI();
    setupSemesterSelector();
    resize(700, 500);
}

void CourseAdviceDialog::setupUI()
{
    QTabWidget *tabWidget = new QTabWidget(this);
    tabWidget->setTabPosition(QTabWidget::North);

    createTab(tabWidget, "全校必修课", universityCreditLabel, universityProgress, universityList);
    createTab(tabWidget, "专业必修课", limitedCreditLabel, limitedProgress, limitedList);
    createTab(tabWidget, "限选课", majorCreditLabel, majorProgress, majorList);
    createTab(tabWidget, "通选课", generalCreditLabel, generalProgress, generalList);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(tabWidget);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // 初始化数据
    setUniversityRequired(0, 0, {});
    setLimitedElective(0, 0, {});
    setMajorRequired(0, 0, {});
    setGeneralElective(0, 0, {});
}

void CourseAdviceDialog::createTab(QTabWidget *tabWidget, const QString &title,
                                   QLabel *&creditLabel, QProgressBar *&progressBar, QListWidget *&courseList)
{
    QWidget *tab = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(tab);
    layout->setSpacing(10);

    // 学分显示区域
    QHBoxLayout *creditLayout = new QHBoxLayout;

    creditLabel = new QLabel("已修学分: 0/0", tab);
    creditLabel->setStyleSheet("font-weight: bold; font-size: 12px;");

    progressBar = new QProgressBar(tab);
    progressBar->setRange(0, 1);
    progressBar->setValue(0);
    progressBar->setFormat("已修学分: %v/%m");
    progressBar->setStyleSheet(
        "QProgressBar {"
        "   border: 1px solid #ccc;"
        "   border-radius: 5px;"
        "   text-align: center;"
        "   height: 20px;"
        "}"
        "QProgressBar::chunk {"
        "   background-color: #4CAF50;"
        "   border-radius: 4px;"
        "}"
        );

    creditLayout->addWidget(creditLabel);
    creditLayout->addWidget(progressBar, 1);
    layout->addLayout(creditLayout);

    // 分隔线
    QFrame *line = new QFrame(tab);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    layout->addWidget(line);

    // 建议课程列表
    QLabel *adviceLabel = new QLabel("下学期建议课程:", tab);
    adviceLabel->setStyleSheet("font-weight: bold; font-size: 12px;");

    courseList = new QListWidget(tab);
    courseList->setStyleSheet(
        "QListWidget {"
        "   background-color: #f9f9f9;"      // 背景色
        "   border: 1px solid #ddd;"         // 边框
        "   border-radius: 3px;"             // 圆角
        "   color: #333333;"                 // 字体颜色 - 深灰色
        "   font-size: 12px;"                // 字体大小
        "}"
        "QListWidget::item {"               // 列表项样式
        "   padding: 8px;"                  // 内边距
        "   border-bottom: 1px solid #eee;" // 底部分隔线
        "}"
        "QListWidget::item:selected {"      // 选中项样式
        "   background-color: #e0f0ff;"      // 选中背景色
        "   color: #0066cc;"                 // 选中字体颜色 - 蓝色
        "}"
        );

    layout->addWidget(adviceLabel);
    layout->addWidget(courseList, 1);

    tabWidget->addTab(tab, title);
}

void CourseAdviceDialog::updateCreditLabel(QLabel *label, int current, int total)
{
    if (current < total) {
        label->setText(QString("<font color='red'>已修学分: %1/%2 (未达标)</font>")
                           .arg(current).arg(total));
    } else {
        label->setText(QString("<font color='green'>已修学分: %1/%2</font>")
                           .arg(current).arg(total));
    }
}

void CourseAdviceDialog::setUniversityRequired(int current, int total, const QStringList &courses)
{
    if (universityCreditLabel && universityProgress && universityList) {
        updateCreditLabel(universityCreditLabel, current, total);
        universityProgress->setRange(0, total);
        universityProgress->setValue(current);
        universityList->clear();
        universityList->addItems(courses);
    }
}

void CourseAdviceDialog::setLimitedElective(int current, int total, const QStringList &courses)
{
    if (limitedCreditLabel && limitedProgress && limitedList) {
        updateCreditLabel(limitedCreditLabel, current, total);
        limitedProgress->setRange(0, total);
        limitedProgress->setValue(current);
        limitedList->clear();
        limitedList->addItems(courses);
    }
}

void CourseAdviceDialog::setMajorRequired(int current, int total, const QStringList &courses)
{
    if (majorCreditLabel && majorProgress && majorList) {
        updateCreditLabel(majorCreditLabel, current, total);
        majorProgress->setRange(0, total);
        majorProgress->setValue(current);
        majorList->clear();
        majorList->addItems(courses);
    }
}

void CourseAdviceDialog::setGeneralElective(int current, int total, const QStringList &courses)
{
    if (generalCreditLabel && generalProgress && generalList) {
        updateCreditLabel(generalCreditLabel, current, total);
        generalProgress->setRange(0, total);
        generalProgress->setValue(current);
        generalList->clear();
        generalList->addItems(courses);
    }
}

//added begin
void CourseAdviceDialog::setupSemesterSelector()
{
    // 创建学期选择菜单（只创建一次）
    semesterMenu = new QMenu(this);
    for (int i = 1; i <= 8; i++) {
        QAction *action = semesterMenu->addAction(QString("第%1学期").arg(i));
        action->setData(i);
        connect(action, &QAction::triggered, [this, i]() {
            currentSemester = i;
            semesterButton->setText(QString("当前学期: 第%1学期").arg(i));
            emit semesterChanged();  // 发出信号
        });
    }

    // 创建学期选择按钮
    semesterButton = new QPushButton(QString("当前学期: 第%1学期").arg(currentSemester), this);
    semesterButton->setMenu(semesterMenu);
    semesterButton->setStyleSheet(
        "QPushButton {"
        "   padding: 5px 10px;"
        "   background-color: #f0f0f0;"
        "   border: 1px solid #ccc;"
        "   border-radius: 4px;"
        "}"
        "QPushButton::menu-indicator { image: none; }"
        );

    // 将按钮添加到布局
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(semesterButton);

    // 将按钮布局添加到主布局
    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout*>(layout());
    if (mainLayout) {
        mainLayout->addLayout(buttonLayout);
    }
}


int CourseAdviceDialog::getCurrentSemester()
{
    return currentSemester;
}
//added end
