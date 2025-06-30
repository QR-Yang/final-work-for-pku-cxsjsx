#ifndef COURSEADVICEDIALOG_H
#define COURSEADVICEDIALOG_H

#include <QDialog>  // 确保包含QDialog
#include <QTabWidget>
#include <QLabel>
#include <QListWidget>
#include <QProgressBar>
#include <QStringList>
#include <QVBoxLayout>  // 添加
#include <QHBoxLayout>  // 添加

class CourseAdviceDialog : public QDialog
{
    Q_OBJECT

signals:
    void semesterChanged();
public:
    explicit CourseAdviceDialog(QWidget *parent = nullptr);

    void setUniversityRequired(int current, int total, const QStringList &courses);
    void setLimitedElective(int current, int total, const QStringList &courses);
    void setMajorRequired(int current, int total, const QStringList &courses);
    void setGeneralElective(int current, int total, const QStringList &courses);
    int getCurrentSemester();//added
private:


    //added
    QPushButton *semesterButton;  // 添加学期选择按钮
    int currentSemester;           // 当前学期
    QMenu *semesterMenu;           // 学期选择菜单
    // 添加新函数
    void setupSemesterSelector();  // 设置学期选择器
    //added end


    void setupUI();
    void createTab(QTabWidget *tabWidget, const QString &title,
                   QLabel *&creditLabel, QProgressBar *&progressBar, QListWidget *&courseList);
    void updateCreditLabel(QLabel *label, int current, int total);

    QLabel *universityCreditLabel = nullptr;
    QLabel *limitedCreditLabel = nullptr;
    QLabel *majorCreditLabel = nullptr;
    QLabel *generalCreditLabel = nullptr;

    QProgressBar *universityProgress = nullptr;
    QProgressBar *limitedProgress = nullptr;
    QProgressBar *majorProgress = nullptr;
    QProgressBar *generalProgress = nullptr;

    QListWidget *universityList = nullptr;
    QListWidget *limitedList = nullptr;
    QListWidget *majorList = nullptr;
    QListWidget *generalList = nullptr;
};

#endif // COURSEADVICEDIALOG_H
