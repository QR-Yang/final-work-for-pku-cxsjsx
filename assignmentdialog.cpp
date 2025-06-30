#include "assignmentdialog.h"
#include <QFormLayout>
#include <QDate>
#include <QLineEdit>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>

AssignmentDialog::AssignmentDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("编辑任务");
    resize(400, 300);
}

AssignmentDialog::AssignmentDialog(const QString &fixedSubject, QWidget *parent)
    : QDialog(parent)
{
    setupUI();
    setWindowTitle("添加作业");
    resize(400, 300);

    // 设置固定科目并禁用编辑
    subjectEdit->setText(fixedSubject);
    subjectEdit->setReadOnly(true);
    subjectEdit->setStyleSheet("background-color: #f0f0f0;");
}

void AssignmentDialog::setupUI() {
    QFormLayout *formLayout = new QFormLayout(this);

    // 所属科目输入框
    subjectEdit = new QLineEdit(this);
    formLayout->addRow("所属科目：", subjectEdit);

    // 作业名称输入框
    nameEdit = new QLineEdit(this);
    formLayout->addRow("任务名称:", nameEdit);

    // 截止日期选择器
    deadlineEdit = new QDateEdit(QDate::currentDate(), this);
    deadlineEdit->setCalendarPopup(true);
    deadlineEdit->setDisplayFormat("yyyy-MM-dd");
    formLayout->addRow("截止日期:", deadlineEdit);

    // 重要性选择器 (1-5级)
    importanceSpinBox = new QSpinBox(this);
    importanceSpinBox->setRange(1, 5);
    importanceSpinBox->setValue(3);
    formLayout->addRow("重要性 (1-5):", importanceSpinBox);

    // 预计完成所需时间（小时）
    timecostEdit = new QDoubleSpinBox(this);
    timecostEdit->setRange(0.5, 24.0);
    timecostEdit->setDecimals(1);
    timecostEdit->setSingleStep(0.5);
    timecostEdit->setValue(2.0);
    formLayout->addRow("预计所需时间（小时）：", timecostEdit);

    // 确定/取消按钮
    buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
        Qt::Horizontal, this);
    formLayout->addRow(buttonBox);

    // 连接信号槽
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

QString AssignmentDialog::assignmentSubject()const{
    return subjectEdit->text().trimmed();
}

QString AssignmentDialog::assignmentName() const {
    return nameEdit->text().trimmed();
}

QDate AssignmentDialog::assignmentDeadline() const {
    return deadlineEdit->date();
}

int AssignmentDialog::assignmentImportance() const {
    return importanceSpinBox->value();
}

double AssignmentDialog::assignmentTimecost() const {
    return timecostEdit->value();
}

Assignment AssignmentDialog::getAssignment() const {
    Assignment assignment;
    assignment.setSubject(subjectEdit->text().trimmed());
    assignment.setName(nameEdit->text().trimmed());
    assignment.setDdl(deadlineEdit->date());
    assignment.setImportance(importanceSpinBox->value());
    assignment.setTimeCost(timecostEdit->value());
    return assignment;
}

void AssignmentDialog::setAssignment(const Assignment &assignment) {
    m_assignment = assignment;
    subjectEdit->setText(assignment.getSubject());
    nameEdit->setText(assignment.getName());
    deadlineEdit->setDate(assignment.getDdl());
    importanceSpinBox->setValue(assignment.getImportance());
    timecostEdit->setValue(assignment.getTimeCost());
}
