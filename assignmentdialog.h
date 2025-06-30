#ifndef ASSIGNMENTDIALOG_H
#define ASSIGNMENTDIALOG_H

#include <QDialog>
#include <QVector>
#include <QDoubleSpinBox>
#include "assignment.h"

// 前置声明
class QLineEdit;
class QSpinBox;
class QDateEdit;
class QDialogButtonBox;

class AssignmentDialog : public QDialog {
    Q_OBJECT

public:
    explicit AssignmentDialog(QWidget *parent = nullptr);
    explicit AssignmentDialog(const QString &fixedSubject, QWidget *parent = nullptr);

    // 获取用户输入的作业信息
    QString assignmentSubject() const;
    QString assignmentName() const;
    QDate assignmentDeadline() const;
    double assignmentTimecost() const;
    int assignmentImportance() const;

    Assignment getAssignment() const;
    void setAssignment(const Assignment& assignment);

private:
    void setupUI(); // 初始化界面

    // 界面组件
    QLineEdit *subjectEdit;
    QLineEdit *nameEdit;
    QDateEdit *deadlineEdit;
    QDoubleSpinBox *timecostEdit;
    QSpinBox *importanceSpinBox;
    QDialogButtonBox *buttonBox;

    Assignment m_assignment;
};

#endif // ASSIGNMENTDIALOG_H
