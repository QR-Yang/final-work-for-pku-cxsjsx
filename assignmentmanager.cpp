#include "assignmentmanager.h"
#include "assignmentdialog.h"
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QDate>
#include <QScrollBar>
#include <algorithm>
#include <QFont>
#include <QPalette>
#include <QSpacerItem>

AssignmentManager::AssignmentManager(QWidget *parent) : QWidget(parent) {
    createUI();
    loadAssignments();
    refreshAssignmentList();
}

AssignmentManager::~AssignmentManager() {
    // 不需要手动删除，Qt 的父子对象关系会自动处理
}

void AssignmentManager::createUI() {
    // 设置主布局
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    // 添加弹性空间确保内容居中
    mainLayout->addStretch(1);
    // 设置背景色
    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(240, 245, 250));
    setPalette(pal);
    setAutoFillBackground(true);

    // 创建顶部按钮布局
    topButtonLayout = new QHBoxLayout();

    // 返回按钮
    backButton = new QPushButton("返回主界面", this);
    backButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #6c757d;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #5a6268;"
        "}"
        );
    backButton->setCursor(Qt::PointingHandCursor);
    topButtonLayout->addWidget(backButton);

    topButtonLayout->addStretch();

    // 添加任务按钮
    addButton = new QPushButton("添加新任务", this);
    addButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #28a745;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #218838;"
        "}"
        );
    addButton->setCursor(Qt::PointingHandCursor);
    topButtonLayout->addWidget(addButton);

    mainLayout->addLayout(topButtonLayout);

    // 创建任务列表
    assignmentList = new QListWidget(this);
    assignmentList->setStyleSheet(
        "QListWidget {"
        "   background-color: white;"
        "   border: 1px solid #ced4da;"
        "   border-radius: 5px;"
        "}"
        "QListWidget::item {"
        "   padding: 12px 15px;"
        "   border-bottom: 1px solid #e9ecef;"
        "}"
        "QListWidget::item:selected {"
        "   background-color: #e2e6ea;"
        "   color: #212529;"
        "}"
        );
    assignmentList->setFont(QFont("Segoe UI", 10));
    assignmentList->setMinimumHeight(400);
    mainLayout->addWidget(assignmentList);

    // 创建详情组
    detailsGroup = new QGroupBox("任务详情", this);
    detailsGroup->setStyleSheet(
        "QGroupBox {"
        "   background-color: white;"
        "   border: 1px solid #ced4da;"
        "   border-radius: 5px;"
        "   margin-top: 10px;"
        "   font-weight: bold;"
        "   font-size: 14px;"
        "}"
        "QGroupBox::title {"
        "   subcontrol-origin: margin;"
        "   subcontrol-position: top left;"
        "   padding: 0 5px;"
        "   background-color: #f8f9fa;"
        "   color: #495057;"
        "}"
        );
    detailsGroup->setVisible(false);

    QVBoxLayout *detailsLayout = new QVBoxLayout(detailsGroup);
    detailsLayout->setContentsMargins(15, 25, 15, 15);

    detailsLabel = new QLabel(this);
    detailsLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 12px;"
        "   color: #212529;"
        "}"
        );
    detailsLabel->setWordWrap(true);
    detailsLayout->addWidget(detailsLabel);

    mainLayout->addWidget(detailsGroup);

    // 创建按钮布局
    buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(15);

    // 编辑按钮
    editButton = new QPushButton("修改任务", this);
    editButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #17a2b8;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #138496;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #ced4da;"
        "}"
        );
    editButton->setCursor(Qt::PointingHandCursor);
    editButton->setEnabled(false);
    buttonLayout->addWidget(editButton);

    // 删除按钮
    deleteButton = new QPushButton("删除任务", this);
    deleteButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #dc3545;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #c82333;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #ced4da;"
        "}"
        );
    deleteButton->setCursor(Qt::PointingHandCursor);
    deleteButton->setEnabled(false);
    buttonLayout->addWidget(deleteButton);

    buttonLayout->addStretch();

    // 生成计划按钮
    planButton = new QPushButton("一键生成今日计划", this);
    planButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #007bff;"
        "   color: white;"
        "   padding: 8px 16px;"
        "   border-radius: 4px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #0069d9;"
        "}"
        );
    planButton->setCursor(Qt::PointingHandCursor);
    buttonLayout->addWidget(planButton);

    mainLayout->addLayout(buttonLayout);

    // 连接信号
    connect(addButton, &QPushButton::clicked, this, &AssignmentManager::onAddAssignment);
    connect(editButton, &QPushButton::clicked, this, &AssignmentManager::onEditAssignment);
    connect(deleteButton, &QPushButton::clicked, this, &AssignmentManager::onDeleteAssignment);
    connect(planButton, &QPushButton::clicked, this, &AssignmentManager::onGeneratePlan);
    connect(backButton, &QPushButton::clicked, this, &AssignmentManager::backToMain);
    connect(assignmentList, &QListWidget::itemClicked, this, &AssignmentManager::onAssignmentSelected);
    connect(assignmentList, &QListWidget::itemDoubleClicked, this, &AssignmentManager::onItemDoubleClicked);

    mainLayout->addStretch(1);

    // 设置布局对齐方式
    mainLayout->setAlignment(Qt::AlignCenter);

    // 强制重新计算布局
    this->layout()->activate();
    this->adjustSize();
}

void AssignmentManager::loadAssignments() {
    m_assignments.clear();

    QFile file(m_dataFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            Assignment assignment = Assignment::fromFileString(line);
            if (assignment.getName() != "") {
                m_assignments.append(assignment);
            }
        }
    }

    file.close();

    // 按ddl排序
    std::sort(m_assignments.begin(), m_assignments.end());
}

void AssignmentManager::saveAssignments() {
    QFile file(m_dataFile);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "保存失败", "无法保存任务数据");
        return;
    }

    QTextStream out(&file);
    for (const Assignment &assignment : m_assignments) {
        out << assignment.toFileString() << "\n";
    }

    file.close();
}

void AssignmentManager::refreshAssignmentList() {
    assignmentList->clear();

    // 按ddl排序
    std::sort(m_assignments.begin(), m_assignments.end());

    for (const Assignment &assignment : m_assignments) {
        QString daysLeft = QString::number(assignment.getDaysLeft());
        QString itemText = QString("%1: %2 [%3天] - %4小时")
                               .arg(assignment.getSubject())
                               .arg(assignment.getName())
                               .arg(daysLeft)
                               .arg(assignment.getTimeCost());

        QListWidgetItem *item = new QListWidgetItem(itemText, assignmentList);
        item->setData(Qt::UserRole, QVariant::fromValue(assignment));

        // 设置字体
        QFont font = item->font();
        font.setPointSize(10);
        item->setFont(font);

        // 根据紧急程度设置颜色
        if (assignment.getDaysLeft() <= 1) {
            item->setForeground(Qt::red);
            font.setBold(true);
            item->setFont(font);
        } else if (assignment.getDaysLeft() <= 3) {
            item->setForeground(QColor(255, 140, 0)); // 橙色
        }
    }

    // 如果没有任务，显示提示
    if (m_assignments.isEmpty()) {
        assignmentList->addItem("没有任务，点击右上角按钮添加新任务");
        QListWidgetItem *item = assignmentList->item(0);
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        item->setForeground(QColor(108, 117, 125));
    }
}

void AssignmentManager::onAddAssignment() {
    AssignmentDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Assignment newAssignment = dialog.getAssignment();
        m_assignments.append(newAssignment);
        saveAssignments();
        refreshAssignmentList();

        // 自动选择新添加的任务
        for (int i = 0; i < assignmentList->count(); i++) {
            QListWidgetItem *item = assignmentList->item(i);
            Assignment a = item->data(Qt::UserRole).value<Assignment>();
            if (a.getName() == newAssignment.getName() &&
                a.getSubject() == newAssignment.getSubject()) {
                assignmentList->setCurrentItem(item);
                onAssignmentSelected(item);
                break;
            }
        }

        emit backToMain();
    }
}

void AssignmentManager::onEditAssignment() {
    QListWidgetItem *item = assignmentList->currentItem();
    if (!item) return;

    AssignmentDialog dialog(this);

    Assignment assignment = item->data(Qt::UserRole).value<Assignment>();

    if (dialog.exec() == QDialog::Accepted) {
        Assignment updatedAssignment = dialog.getAssignment();

        // 更新任务
        for (int i = 0; i < m_assignments.size(); i++) {
            if (m_assignments[i].getName() == assignment.getName() &&
                m_assignments[i].getSubject() == assignment.getSubject()) {
                m_assignments[i] = updatedAssignment;
                break;
            }
        }

        saveAssignments();
        refreshAssignmentList();
        showAssignmentDetails(updatedAssignment);

        // 重新选择更新后的任务
        for (int i = 0; i < assignmentList->count(); i++) {
            QListWidgetItem *newItem = assignmentList->item(i);
            Assignment a = newItem->data(Qt::UserRole).value<Assignment>();
            if (a.getName() == updatedAssignment.getName() &&
                a.getSubject() == updatedAssignment.getSubject()) {
                assignmentList->setCurrentItem(newItem);
                break;
            }
        }
    }
}

void AssignmentManager::onDeleteAssignment() {
    QListWidgetItem *item = assignmentList->currentItem();
    if (!item) return;

    Assignment assignment = item->data(Qt::UserRole).value<Assignment>();

    int ret = QMessageBox::question(this, "确认删除",
                                    QString("确定要删除任务 '%1: %2' 吗?").arg(assignment.getSubject()).arg(assignment.getName()),
                                    QMessageBox::Yes | QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        // 删除任务
        for (int i = 0; i < m_assignments.size(); i++) {
            if (m_assignments[i].getName() == assignment.getName() &&
                m_assignments[i].getSubject() == assignment.getSubject()) {
                m_assignments.removeAt(i);
                break;
            }
        }

        saveAssignments();
        refreshAssignmentList();

        // 重置详情显示
        detailsGroup->setVisible(false);
        editButton->setEnabled(false);
        deleteButton->setEnabled(false);
    }
}

void AssignmentManager::onGeneratePlan() {
    QList<Assignment> plan = generateDailyPlan();

    QString planText = "<h3 style='color:#007bff; margin-top:0;'>今日推荐计划 (总时长约4小时)</h3>";
    planText += "<ul style='margin-top:10px; margin-bottom:10px;'>";

    if (plan.isEmpty()) {
        planText += "<li style='margin-bottom:8px;'>恭喜！没有需要紧急完成的任务</li>";
    } else {
        double totalHours = 0;
        for (const Assignment &a : plan) {
            QString color = a.getDaysLeft() <= 1 ? "red" : (a.getDaysLeft() <= 3 ? "orange" : "green");
            planText += QString("<li style='margin-bottom:8px;'><b>%1: %2</b> - <span style='color:%7;'>%3小时</span> "
                                "(剩余<span style='color:%7;'>%4天</span>, 优先级: %5)</li>")
                            .arg(a.getSubject())
                            .arg(a.getName())
                            .arg(a.getTimeCost())
                            .arg(a.getDaysLeft())
                            .arg(QString::number(a.getPriorityScore(), 'f', 2))
                            .arg(color);
            totalHours += a.getTimeCost();
        }
        planText += QString("<li style='margin-top:10px; font-weight:bold;'><b>总时长: %1 小时</b></li>").arg(totalHours);
    }

    planText += "</ul>";
    planText += "<p style='color:#6c757d; font-size:12px;'>提示：计划基于任务紧急度、重要性和耗时自动生成</p>";

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("今日计划");
    msgBox.setText(planText);
    msgBox.setIcon(QMessageBox::Information);
    msgBox.setStyleSheet(
        "QMessageBox { background-color: white; }"
        "QLabel { min-width: 400px; }"
        );
    msgBox.exec();
}

QList<Assignment> AssignmentManager::generateDailyPlan() {
    // 复制任务列表并按优先级排序
    QList<Assignment> sortedAssignments = m_assignments;
    std::sort(sortedAssignments.begin(), sortedAssignments.end(), [](const Assignment &a, const Assignment &b) {
        return a.getPriorityScore() > b.getPriorityScore();
    });

    QList<Assignment> plan;
    double totalHours = 0;
    const double MAX_HOURS = 4.6;

    for (const Assignment &a : sortedAssignments) {

        if (totalHours + a.getTimeCost() <= MAX_HOURS) {
            plan.append(a);
            totalHours += a.getTimeCost();
        }

        if (totalHours >= MAX_HOURS) {
            break;
        }
    }

    return plan;
}

void AssignmentManager::onAssignmentSelected(QListWidgetItem *item) {
    if (item && item->flags() & Qt::ItemIsSelectable) {
        Assignment assignment = item->data(Qt::UserRole).value<Assignment>();
        showAssignmentDetails(assignment);
        editButton->setEnabled(true);
        deleteButton->setEnabled(true);
    }
}

void AssignmentManager::onItemDoubleClicked(QListWidgetItem *item) {
    if (item && item->flags() & Qt::ItemIsSelectable) {
        onEditAssignment();
    }
}

void AssignmentManager::showAssignmentDetails(const Assignment &assignment) {
    QString daysLeftColor = assignment.getDaysLeft() <= 1 ? "red" :
                                (assignment.getDaysLeft() <= 3 ? "orange" : "green");

    QString details = QString(
                          "<style>"
                          ".detail-item { margin-bottom: 8px; }"
                          ".label { font-weight: bold; color: #495057; }"
                          ".value { color: #212529; }"
                          "</style>"
                          "<div class='detail-item'><span class='label'>科目: </span><span class='value'>%1</span></div>"
                          "<div class='detail-item'><span class='label'>任务名称: </span><span class='value'>%2</span></div>"
                          "<div class='detail-item'><span class='label'>截止日期: </span><span class='value'>%3</span></div>"
                          "<div class='detail-item'><span class='label'>剩余天数: </span><span class='value' style='color:%7;'>%4 天</span></div>"
                          "<div class='detail-item'><span class='label'>重要程度: </span><span class='value'>%5/5</span></div>"
                          "<div class='detail-item'><span class='label'>预计耗时: </span><span class='value'>%6 小时</span></div>"
                          "<div class='detail-item'><span class='label'>优先级评分: </span><span class='value'>%8</span></div>"
                          ).arg(assignment.getSubject())
                          .arg(assignment.getName())
                          .arg(assignment.getDdl().toString("yyyy-MM-dd"))
                          .arg(assignment.getDaysLeft())
                          .arg(assignment.getImportance())
                          .arg(assignment.getTimeCost())
                          .arg(daysLeftColor)
                          .arg(QString::number(assignment.getPriorityScore(), 'f', 2));

    detailsLabel->setText(details);
    detailsGroup->setVisible(true);
}
