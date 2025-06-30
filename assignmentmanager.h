#ifndef ASSIGNMENTMANAGER_H
#define ASSIGNMENTMANAGER_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include "assignment.h"

class AssignmentManager : public QWidget {
    Q_OBJECT

public:
    explicit AssignmentManager(QWidget *parent = nullptr);
    ~AssignmentManager();

    void loadAssignments();
    void saveAssignments();

signals:
    void backToMain();

private slots:
    void onAddAssignment();
    void onEditAssignment();
    void onDeleteAssignment();
    void onGeneratePlan();
    void onAssignmentSelected(QListWidgetItem *item);
    void onItemDoubleClicked(QListWidgetItem *item);

private:
    // UI 控件
    QListWidget *assignmentList;
    QGroupBox *detailsGroup;
    QLabel *detailsLabel;
    QPushButton *addButton;
    QPushButton *editButton;
    QPushButton *deleteButton;
    QPushButton *planButton;
    QPushButton *backButton;

    // 布局
    QVBoxLayout *mainLayout;
    QHBoxLayout *buttonLayout;
    QHBoxLayout *topButtonLayout;

    // 数据
    QList<Assignment> m_assignments;
    QString m_dataFile = "assignments.txt";

    void createUI();
    void refreshAssignmentList();
    void showAssignmentDetails(const Assignment &assignment);
    QList<Assignment> generateDailyPlan();
};

#endif // ASSIGNMENTMANAGER_H
