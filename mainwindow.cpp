#include "mainwindow.h"
#include "courseadvicedialog.h"
#include "assignmentmanager.h"
#include "assignmentdialog.h"
#include "reminderdialog.h"
#include <QMenuBar>
#include <QTableWidget>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QStatusBar>
#include <QMessageBox>
#include <QDebug>
#include <string>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QScreen>
#include <QQueue>
#include <QTimer>
#include <QShowEvent>
#include <sstream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_assignmentManager(nullptr)
{
    this->setWindowTitle("课程管理系统");
    this->resize(1200, 700);  // 增加宽度以容纳右侧面板

    // 加载所有作业
    loadAllAssignments();
    loadReminders();

    createActions();
    createMenus();
    setupScheduleWidget();
    setupCourseAssignmentPanel();

    // 初始化状态栏
    statusBar()->showMessage("就绪 | 第1周");
}

MainWindow::~MainWindow()
{
    delete adviceDialog;
    if(m_assignmentManager) delete m_assignmentManager;
}

void MainWindow::createActions()
{
    // 创建选课建议动作 - 使用this指针
    adviceAction = new QAction("选课建议", this);
    QObject::connect(adviceAction, &QAction::triggered, this, &MainWindow::showCourseAdvice);

    // 创建DDL管理动作
    ddlAction = new QAction("任务管理", this);
    QObject::connect(ddlAction, &QAction::triggered, this, &MainWindow::showDDLManager);

    // 添加提醒事项动作
    reminderAction = new QAction("提醒事项", this);
    QObject::connect(reminderAction, &QAction::triggered, this, &MainWindow::showReminderDialog);
}

void MainWindow::createMenus()
{
    menuBar()->addAction(adviceAction);
    menuBar()->addAction(ddlAction);
    menuBar()->addAction(reminderAction);
}

void MainWindow::setupScheduleWidget()
{
    // 创建主界面容器
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);

    // 周数选择
    QHBoxLayout *weekLayout = new QHBoxLayout;
    prevWeekBtn = new QPushButton("← 上一周", centralWidget);
    nextWeekBtn = new QPushButton("下一周 →", centralWidget);
    weekLabel = new QLabel("当前周数: 第1周", centralWidget);

    // 设置按钮样式
    prevWeekBtn->setStyleSheet("QPushButton { padding: 5px 10px; }");
    nextWeekBtn->setStyleSheet("QPushButton { padding: 5px 10px; }");
    weekLabel->setStyleSheet("font-weight: bold; font-size: 14px;");

    // 连接按钮信号
    QObject::connect(prevWeekBtn, &QPushButton::clicked, this, &MainWindow::goToPrevWeek);
    QObject::connect(nextWeekBtn, &QPushButton::clicked, this, &MainWindow::goToNextWeek);

    weekLayout->addWidget(prevWeekBtn);
    weekLayout->addWidget(weekLabel, 1, Qt::AlignCenter);
    weekLayout->addWidget(nextWeekBtn);

    mainLayout->addLayout(weekLayout);

    // 课表表格
    scheduleTable = new QTableWidget(12, 7, centralWidget); // 12节课 * 7天
    scheduleTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    // 设置表头
    QStringList headers = {"周一", "周二", "周三", "周四", "周五", "周六", "周日"};
    scheduleTable->setHorizontalHeaderLabels(headers);

    // 设置行头（时间）
    QStringList times;
    for (int i = 1; i <= 12; i++) {
        times << QString("第%1节").arg(i);
    }
    scheduleTable->setVerticalHeaderLabels(times);

    // 设置表格样式
    scheduleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    scheduleTable->verticalHeader()->setDefaultSectionSize(60);
    scheduleTable->setStyleSheet(
        "QTableWidget {"
        "   gridline-color: #e0e0e0;"
        "   background-color: white;"
        "}"
        "QTableWidget::item {"
        "   padding: 5px;"
        "   border-bottom: 1px solid #f0f0f0;"
        "}"
        "QTableWidget::item:selected {"
        "   background-color: #e0f0ff;"
        "}"
        );

    // 连接单元格点击信号
    connect(scheduleTable, &QTableWidget::cellClicked, this, &MainWindow::onCourseClicked);

    // 填充示例课程
    updateSchedule();

    // 创建分割器
    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    mainSplitter->addWidget(scheduleTable);

    // 添加课程作业面板（在setupCourseAssignmentPanel中创建）
    assignmentGroup = new QGroupBox("课程作业", centralWidget);
    mainSplitter->addWidget(assignmentGroup);

    // 设置分割比例
    mainSplitter->setSizes(QList<int>() << 700 << 400);

    mainLayout->addWidget(mainSplitter);

    // 设置为中心部件
    this->setCentralWidget(centralWidget);
}

void MainWindow::refreshAllAssignments() {
    // 重新加载所有作业
    loadAllAssignments();

    // 如果有选中的课程，刷新其作业列表
    if (!currentSelectedCourse.isEmpty()) {
        updateCourseAssignmentList(currentSelectedCourse);
    }
}

void MainWindow::setupCourseAssignmentPanel()
{
    // 创建作业面板布局
    QVBoxLayout *assignmentLayout = new QVBoxLayout(assignmentGroup);
    assignmentLayout->setContentsMargins(10, 10, 10, 10);
    assignmentLayout->setSpacing(10);

    // 课程标题
    courseTitleLabel = new QLabel("请点击课表中的课程查看作业", assignmentGroup);
    courseTitleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #2c3e50;");
    courseTitleLabel->setAlignment(Qt::AlignCenter);
    assignmentLayout->addWidget(courseTitleLabel);

    // 分隔线
    QFrame *line = new QFrame(assignmentGroup);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("color: #ddd;");
    assignmentLayout->addWidget(line);

    // 作业列表
    QLabel *assignmentListLabel = new QLabel("作业列表:", assignmentGroup);
    assignmentListLabel->setStyleSheet("font-weight: bold;");
    assignmentLayout->addWidget(assignmentListLabel);

    assignmentList = new QListWidget(assignmentGroup);
    assignmentList->setStyleSheet(
        "QListWidget {"
        "   background-color: white;"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "}"
        "QListWidget::item {"
        "   padding: 8px;"
        "   border-bottom: 1px solid #eee;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #e0f0ff;"
        "}"
        );
    assignmentList->setMinimumHeight(150);
    connect(assignmentList, &QListWidget::itemClicked, this, &MainWindow::onAssignmentClicked);
    assignmentLayout->addWidget(assignmentList, 1);

    // 作业操作按钮
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    addAssignmentBtn = new QPushButton("添加作业", assignmentGroup);
    editAssignmentBtn = new QPushButton("编辑作业", assignmentGroup);
    deleteAssignmentBtn = new QPushButton("删除作业", assignmentGroup);

    // 设置按钮样式 (修复边框和悬停效果)
    QString buttonStyle = "QPushButton {"
                          "   padding: 6px 12px;"
                          "   border-radius: 4px;"
                          "   font-weight: bold;"
                          "   border: 1px solid #ced4da;"
                          "}"
                          "QPushButton:hover {"
                          "   background-color: %1;"
                          "   opacity: 0.9;"
                          "}";

    addAssignmentBtn->setStyleSheet(buttonStyle.arg("#218838") + "background-color: #28a745; color: white;");
    editAssignmentBtn->setStyleSheet(buttonStyle.arg("#138496") + "background-color: #17a2b8; color: white;");
    deleteAssignmentBtn->setStyleSheet(buttonStyle.arg("#c82333") + "background-color: #dc3545; color: white;");

    // 初始禁用编辑和删除按钮
    editAssignmentBtn->setEnabled(false);
    deleteAssignmentBtn->setEnabled(false);

    // 连接按钮信号
    connect(addAssignmentBtn, &QPushButton::clicked, this, &MainWindow::onAddAssignmentForCourse);
    connect(editAssignmentBtn, &QPushButton::clicked, this, &MainWindow::onEditSelectedAssignment);
    connect(deleteAssignmentBtn, &QPushButton::clicked, this, &MainWindow::onDeleteSelectedAssignment);

    buttonLayout->addWidget(addAssignmentBtn);
    buttonLayout->addWidget(editAssignmentBtn);
    buttonLayout->addWidget(deleteAssignmentBtn);
    assignmentLayout->addLayout(buttonLayout);

    // 作业详情
    QLabel *detailsLabel = new QLabel("作业详情:", assignmentGroup);
    detailsLabel->setStyleSheet("font-weight: bold;");
    assignmentLayout->addWidget(detailsLabel);

    assignmentDetails = new QTextBrowser(assignmentGroup);
    assignmentDetails->setStyleSheet(
        "QTextBrowser {"
        "   background-color: #f8f9fa;"
        "   border: 1px solid #ddd;"
        "   border-radius: 4px;"
        "   padding: 10px;"
        "}"
        );
    assignmentDetails->setMinimumHeight(150);
    assignmentLayout->addWidget(assignmentDetails, 2);

    // 初始隐藏作业面板
    assignmentGroup->setVisible(false);
}

void MainWindow::updateSchedule()
{
    // 清除现有课程
    for (int row = 0; row < 12; row++) {
        for (int col = 0; col < 7; col++) {
            if (scheduleTable->item(row, col)) {
                delete scheduleTable->item(row, col);
            }
        }
    }

    // 添加示例课程 - 实际应用中应从数据源加载
    scheduleTable->setItem(0, 0, new QTableWidgetItem("高等数学\n8:00-9:40\nA101"));
    scheduleTable->setItem(1, 0, new QTableWidgetItem("大学英语\n10:00-11:40\nB205"));
    scheduleTable->setItem(3, 2, new QTableWidgetItem("数据结构\n14:00-15:40\nC305"));
    scheduleTable->setItem(4, 4, new QTableWidgetItem("专业导论\n16:00-17:40\nD102"));
}

struct cl
{
    string name;
    int credit;
    int smester;
};
struct bac
{
    string names[100];
    int tota;
};

// 将class.txt内容内置为字符串
const string CLASS_DATA = R"(
1 思想道德与法治 2 1
1 中国近代史纲要 2 2
1 习近平思想概论 2 4
1 马克思主义基本原理 2 3
1 毛泽东思想 2 5
1 信息课程 6 9
1 军事理论 2 1
1 体育课 1 9
2 计算概论A 3 1
2 数据结构与算法A 3 3
2 高等数学A一 5 1
2 高等数学A二 5 2
2 线性代数A(I) 4 1
2 线性代数A(II) 4 2
2 信息科学技术概论 1 1
2 程序设计实习 3 2
2 人工智能基础 3 2
2 计算机系统导论 5 3
2 离散数学基础 3 3
2 算法设计与分析 3 4
2 信息学中的概率统计 3 4
2 计算机组织与体系结构 3 5
2 编译原理 4 6
2 操作系统 4 6
3 力学 3 1
3 电磁学 3 2
3 信号与系统 3 5
3 微电子与电路基础 2 2
3 电子系统基础训练 1 2
3 信息科学中的物理学上 3 1
3 信息科学中的物理学下 3 2
3 集合论与图论 3 5
3 数理逻辑理论 3 5
3 计算机科学基础 3 5
3 密码学基础 3 5
3 数学分析(III) 4 3
3 代数结构与组合数学 3 4
3 软件测试导论 3 4
3 软件分析技术 3 5
3 软件工程 4 5
3 数据库概论 3 6
3 信息安全引论 2 7
3 数字逻辑设计 3 4
3 计算机组织与体系结构实习 2 6
3 并行与分布式计算导论 3 6
3 并行程序设计原理 2 4
3 计算机网络 4 5
3 计算机图形学 3 4
3 机器学习概论 3 5
3 强化学习 3 5
3 智能机器人系统导论 3 5
3 自然语言处理 3 5
3 计算机视觉导论 3 6
4 地震概论 2 1
4 简明量子力学 2 2
4 智能科学研究实践I 2 3
4 信息科学中的数学 2 3
4 智能科学研究实践Ⅱ 3 4
4 程序设计语言概论 2 4
4 视觉艺术与计算美学 2 4
4 视觉计算与深度学习 2 4
4 量子计算 3 5
4 计算影像生成技术 2 6
4 量子信息 3 6
4 智能信息系统 3 7
4 机器智能实验 2 7
0
)";

bac getsuggestion(int semes, int typ)
{
    cl a[100];
    bac m;
    m.tota = 0;
    int cnt = 0;

    // 使用字符串流代替文件输入
    istringstream iss(CLASS_DATA);

    int tmp;
    while (iss >> tmp) {
        if (tmp == 0) break;
        if (tmp != typ) {
            // 跳过这一行的剩余数据
            string dummy;
            getline(iss, dummy);
            continue;
        }

        // 读取课程信息
        iss >> a[cnt].name >> a[cnt].credit >> a[cnt].smester;
        cnt++;
    }

    for (int i = 0; i < cnt; i++) {
        if (a[i].smester == semes) {
            m.names[m.tota] = a[i].name;
            m.tota++;
        }
    }

    return m;
}

void MainWindow::showCourseAdvice()
{
    if (!adviceDialog) {
        adviceDialog = new CourseAdviceDialog(this);
        // 添加信号连接
        connect(adviceDialog, &CourseAdviceDialog::semesterChanged,
                this, &MainWindow::updateCourseAdvice);
    }

    // 调用更新函数
    updateCourseAdvice();
    adviceDialog->exec();
}
void MainWindow::updateCourseAdvice()
{
    if (!adviceDialog) return;

    int se = adviceDialog->getCurrentSemester();

    // 获取不同类型课程的建议
    bac universityCourses = getsuggestion(se, 1);  // 全校必修课
    bac limitedCourses = getsuggestion(se, 2);      // 限选课
    bac majorCourses = getsuggestion(se, 3);        // 专业必修课
    bac generalCourses = getsuggestion(se, 4);      // 通选课

    // 转换全校必修课
    QStringList universityList;
    for (int i = 0; i < universityCourses.tota; i++) {
        universityList << QString::fromStdString(universityCourses.names[i]);
    }

    // 转换限选课
    QStringList limitedList;
    for (int i = 0; i < limitedCourses.tota; i++) {
        limitedList << QString::fromStdString(limitedCourses.names[i]);
    }

    // 转换专业必修课
    QStringList majorList;
    for (int i = 0; i < majorCourses.tota; i++) {
        majorList << QString::fromStdString(majorCourses.names[i]);
    }

    // 转换通选课
    QStringList generalList;
    for (int i = 0; i < generalCourses.tota; i++) {
        generalList << QString::fromStdString(generalCourses.names[i]);
    }

    // 设置对话框数据
    adviceDialog->setUniversityRequired(31, 31, universityList);
    adviceDialog->setLimitedElective(27, 55, limitedList);
    adviceDialog->setMajorRequired(8, 20, majorList);
    adviceDialog->setGeneralElective(5, 14, generalList);
}

void MainWindow::showDDLManager()
{
    m_assignmentManager = new AssignmentManager();
    connect(m_assignmentManager, &AssignmentManager::backToMain, this, [this]() {
        QRect managerRect = m_assignmentManager->geometry();
        this->setGeometry(managerRect);
        this->show();
        m_assignmentManager->deleteLater();
        m_assignmentManager = nullptr;
        refreshAllAssignments();
    });
    m_assignmentManager->setGeometry(this->geometry());
    m_assignmentManager->show();
    this->hide();
}

void MainWindow::goToPrevWeek()
{
    if (currentWeek > 1) {
        currentWeek--;
        weekLabel->setText(QString("当前周数: 第%1周").arg(currentWeek));
        statusBar()->showMessage(QString("就绪 | 第%1周").arg(currentWeek));
        updateSchedule();
    }
}

void MainWindow::goToNextWeek()
{
    currentWeek++;
    weekLabel->setText(QString("当前周数: 第%1周").arg(currentWeek));
    statusBar()->showMessage(QString("就绪 | 第%1周").arg(currentWeek));
    updateSchedule();
}

void MainWindow::onCourseClicked(int row, int column)
{
    QTableWidgetItem *item = scheduleTable->item(row, column);
    if (!item || item->text().isEmpty()) {
        assignmentGroup->setVisible(false);
        return;
    }

    // 正确提取课程名称（第一行是课程名）
    QStringList lines = item->text().split('\n');
    if (lines.isEmpty()) {
        assignmentGroup->setVisible(false);
        return;
    }

    QString courseName = lines[0].trimmed();

    // 更新当前选中的课程
    currentSelectedCourse = courseName;

    // 显示作业面板
    assignmentGroup->setVisible(true);
    courseTitleLabel->setText(courseName + " - 作业");

    // 更新作业列表
    updateCourseAssignmentList(courseName);

    // 清空作业详情
    assignmentDetails->clear();
    editAssignmentBtn->setEnabled(false);
    deleteAssignmentBtn->setEnabled(false);
}

void MainWindow::updateCourseAssignmentList(const QString &courseName)
{
    assignmentList->clear();

    // 筛选属于该课程的作业
    for (const Assignment &assignment : allAssignments) {
        if (assignment.getSubject() == courseName) {
            QString daysLeft = QString::number(assignment.getDaysLeft());
            QString itemText = QString("%1 [%2天] - %3小时")
                                   .arg(assignment.getName())
                                   .arg(daysLeft)
                                   .arg(assignment.getTimeCost());

            QListWidgetItem *item = new QListWidgetItem(itemText, assignmentList);
            item->setData(Qt::UserRole, QVariant::fromValue(assignment));

            // 根据紧急程度设置颜色
            if (assignment.getDaysLeft() <= 1) {
                item->setForeground(Qt::red);
                QFont font = item->font();
                font.setBold(true);
                item->setFont(font);
            } else if (assignment.getDaysLeft() <= 3) {
                item->setForeground(QColor(255, 140, 0)); // 橙色
            }
        }
    }

    // 如果没有作业，添加提示
    if (assignmentList->count() == 0) {
        assignmentList->addItem("该课程暂无作业");
        QListWidgetItem *item = assignmentList->item(0);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        item->setForeground(QColor(150, 150, 150));
    }
}

void MainWindow::onAssignmentClicked(QListWidgetItem *item)
{
    if (!item || !(item->flags() & Qt::ItemIsSelectable)) {
        editAssignmentBtn->setEnabled(false);
        deleteAssignmentBtn->setEnabled(false);
        return;
    }

    // 获取选中的作业
    Assignment assignment = item->data(Qt::UserRole).value<Assignment>();

    // 显示作业详情
    showAssignmentDetails(assignment);

    // 启用编辑和删除按钮
    editAssignmentBtn->setEnabled(true);
    deleteAssignmentBtn->setEnabled(true);
}

void MainWindow::showAssignmentDetails(const Assignment &assignment)
{
    QString daysLeftColor = assignment.getDaysLeft() <= 1 ? "red" :
                                (assignment.getDaysLeft() <= 3 ? "orange" : "green");

    QString details = QString(
                          "<style>"
                          "h3 { color: #2c3e50; margin-top: 0; }"
                          ".detail-item { margin-bottom: 8px; }"
                          ".label { font-weight: bold; color: #495057; }"
                          ".value { color: #212529; }"
                          "</style>"
                          "<h3>%1</h3>"
                          "<div class='detail-item'><span class='label'>科目: </span><span class='value'>%2</span></div>"
                          "<div class='detail-item'><span class='label'>截止日期: </span><span class='value'>%3</span></div>"
                          "<div class='detail-item'><span class='label'>剩余天数: </span><span class='value' style='color:%7;'>%4 天</span></div>"
                          "<div class='detail-item'><span class='label'>重要程度: </span><span class='value'>%5/5</span></div>"
                          "<div class='detail-item'><span class='label'>预计耗时: </span><span class='value'>%6 小时</span></div>"
                          ).arg(assignment.getName())
                          .arg(assignment.getSubject())
                          .arg(assignment.getDdl().toString("yyyy-MM-dd"))
                          .arg(assignment.getDaysLeft())
                          .arg(assignment.getImportance())
                          .arg(assignment.getTimeCost())
                          .arg(daysLeftColor);

    assignmentDetails->setHtml(details);
}

void MainWindow::onAddAssignmentForCourse()
{
    if (currentSelectedCourse.isEmpty()) {
        QMessageBox::warning(this, "错误", "请先选择一门课程");
        return;
    }

    // 使用新的构造函数，传入固定科目
    AssignmentDialog dialog(currentSelectedCourse, this);

    if (dialog.exec() == QDialog::Accepted) {
        Assignment newAssignment = dialog.getAssignment();
        newAssignment.setSubject(currentSelectedCourse);
        allAssignments.append(newAssignment);
        saveAllAssignments();
        updateCourseAssignmentList(currentSelectedCourse);
    }
}

void MainWindow::onEditSelectedAssignment()
{
    QListWidgetItem *item = assignmentList->currentItem();
    // 修复条件判断
    if (!item || !(item->flags() & Qt::ItemIsSelectable)) {
        return;
    }

    Assignment assignment = item->data(Qt::UserRole).value<Assignment>();

    AssignmentDialog dialog(this);
    dialog.setAssignment(assignment);

    if (dialog.exec() == QDialog::Accepted) {
        Assignment updatedAssignment = dialog.getAssignment();

        // 更新作业列表
        for (int i = 0; i < allAssignments.size(); i++) {
            if (allAssignments[i].getName() == assignment.getName() &&
                allAssignments[i].getSubject() == assignment.getSubject()) {
                allAssignments[i] = updatedAssignment;
                break;
            }
        }

        saveAllAssignments();

        // 刷新作业列表
        updateCourseAssignmentList(currentSelectedCourse);

        // 显示更新后的作业详情
        showAssignmentDetails(updatedAssignment);
    }
}

void MainWindow::onDeleteSelectedAssignment()
{
    QListWidgetItem *item = assignmentList->currentItem();
    // 修复条件判断
    if (!item || !(item->flags() & Qt::ItemIsSelectable)) {
        return;
    }

    Assignment assignment = item->data(Qt::UserRole).value<Assignment>();

    int ret = QMessageBox::question(this, "确认删除",
                                    QString("确定要删除作业 '%1' 吗?").arg(assignment.getName()),
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        // 从作业列表中删除
        for (int i = 0; i < allAssignments.size(); i++) {
            if (allAssignments[i].getName() == assignment.getName() &&
                allAssignments[i].getSubject() == assignment.getSubject()) {
                allAssignments.removeAt(i);
                break;
            }
        }

        saveAllAssignments();

        // 刷新作业列表
        updateCourseAssignmentList(currentSelectedCourse);

        // 清空作业详情
        assignmentDetails->clear();
        editAssignmentBtn->setEnabled(false);
        deleteAssignmentBtn->setEnabled(false);
    }
}

void MainWindow::loadAllAssignments()
{
    allAssignments.clear();

    QFile file("assignments.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            Assignment assignment = Assignment::fromFileString(line);
            if (assignment.getName() != "") {
                allAssignments.append(assignment);
            }
        }
    }

    file.close();
}

void MainWindow::saveAllAssignments()
{
    QFile file("assignments.txt");
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "保存失败", "无法保存作业数据");
        return;
    }

    QTextStream out(&file);
    for (const Assignment &assignment : allAssignments) {
        out << assignment.toFileString() << "\n";
    }

    file.close();
}

void MainWindow::showReminderDialog() {
    if (!reminderDialog) {
        reminderDialog = new ReminderDialog(this);
        // 修改信号连接 - 当提醒更新时更新主窗口的列表
        connect(reminderDialog, &ReminderDialog::remindersUpdated,
                [this]() {
                    // 更新主窗口的提醒列表
                    reminderList = reminderDialog->getReminders();
                    // 立即保存到文件
                    updateReminders();
                });
    }

    // 设置当前提醒事项
    reminderDialog->setReminders(reminderList);

    if (reminderDialog->exec() == QDialog::Accepted) {
        // 更新提醒列表
        reminderList = reminderDialog->getReminders();
        updateReminders();
    }
}

void MainWindow::updateReminders() {
    // 保存提醒事项到文件
    QFile file("reminders.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (const QString &reminder : reminderList) {
            out << reminder << "\n";
        }
        file.close();
    }
}

void MainWindow::loadReminders()
{
    reminderList.clear();

    QFile file("reminders.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        while (!in.atEnd()) {
            QString reminder = in.readLine().trimmed();
            if (!reminder.isEmpty()) {
                reminderList.append(reminder);
            }
        }
        file.close();
    }
}

void MainWindow::showReminders()
{
    for (const QString &reminder : reminderList) {
        QMessageBox::information(this, "提醒事项", reminder, QMessageBox::Ok);
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    // 每次显示主窗口时都显示提醒事项
    showRemindersOnStartup();
}

void MainWindow::showRemindersOnStartup()
{
    // 加载提醒事项
    loadReminders();

    // 创建提醒队列
    reminderQueue.clear();
    for (const QString &reminder : reminderList) {
        reminderQueue.enqueue(reminder);
    }

    // 如果队列不为空，显示第一个提醒
    if (!reminderQueue.isEmpty()) {
        QTimer::singleShot(100, this, &MainWindow::showNextReminder);
    }
}

void MainWindow::showNextReminder()
{
    if (!reminderQueue.isEmpty()) {
        QString reminder = reminderQueue.dequeue();
        QMessageBox::information(this, "提醒事项", reminder, QMessageBox::Ok);

        // 显示下一个提醒（如果有）
        if (!reminderQueue.isEmpty()) {
            QTimer::singleShot(100, this, &MainWindow::showNextReminder);
        }
    }
}
