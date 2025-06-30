#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QMenu>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QMessageBox>
#include <QSplitter>
#include <QGroupBox>
#include <QListWidget>
#include <QTextBrowser>
#include <QQueue>
#include "assignmentmanager.h"
#include "assignment.h"

// 前置声明
class CourseAdviceDialog;
class ReminderDialog;  // 添加前置声明

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void refreshAllAssignments();

protected:
    void showEvent(QShowEvent *event) override;  // 添加 showEvent 声明

private slots:
    void showCourseAdvice();
    void showDDLManager();
    void goToPrevWeek();
    void goToNextWeek();
    void updateCourseAdvice();
    void onCourseClicked(int row, int column);
    void onAssignmentClicked(QListWidgetItem *item);
    void onAddAssignmentForCourse();
    void onEditSelectedAssignment();
    void onDeleteSelectedAssignment();
    void showReminderDialog();  // 添加提醒事项对话框槽函数
    void showNextReminder();    // 添加显示下一个提醒的槽函数
    void updateReminders();

private:
    void createActions();
    void createMenus();
    void setupScheduleWidget();
    void updateSchedule();
    void setupCourseAssignmentPanel();
    void updateCourseAssignmentList(const QString &courseName);
    void showAssignmentDetails(const Assignment &assignment);
    void loadAllAssignments();
    void saveAllAssignments();
    void loadReminders();
    void showReminders();
    void showRemindersOnStartup();
    AssignmentManager *m_assignmentManager;

    // 主界面组件
    QTableWidget *scheduleTable = nullptr;
    QLabel *weekLabel = nullptr;
    int currentWeek = 1;  // 当前周数

    // 课程作业面板组件
    QSplitter *mainSplitter = nullptr;
    QGroupBox *assignmentGroup = nullptr;
    QLabel *courseTitleLabel = nullptr;
    QListWidget *assignmentList = nullptr;
    QTextBrowser *assignmentDetails = nullptr;
    QPushButton *addAssignmentBtn = nullptr;
    QPushButton *editAssignmentBtn = nullptr;
    QPushButton *deleteAssignmentBtn = nullptr;

    // 当前选中的课程
    QString currentSelectedCourse;

    // 所有作业数据
    QList<Assignment> allAssignments;

    // 功能对话框
    CourseAdviceDialog *adviceDialog = nullptr;
    ReminderDialog *reminderDialog = nullptr;  // 添加提醒事项对话框

    // 菜单动作
    QAction *adviceAction = nullptr;
    QAction *ddlAction = nullptr;
    QAction *reminderAction = nullptr;  // 添加提醒事项动作

    // 周导航按钮
    QPushButton *prevWeekBtn = nullptr;
    QPushButton *nextWeekBtn = nullptr;

    // 提醒事项相关
    QStringList reminderList;   // 存储提醒事项
    QQueue<QString> reminderQueue;  // 用于顺序显示提醒
};

#endif // MAINWINDOW_H
