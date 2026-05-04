#include "mainwindow.h"
#include "ui_mainwindow.h"

#define DEBUG_LOCATION qDebug().nospace() << "[" << Q_FUNC_INFO << " @ " << QFileInfo(__FILE__).fileName() << ":" << __LINE__ << "]"


MainWindow::MainWindow( QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::MainWindow(QString token, const UserInfo& userInfo, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_token(token)
    , m_userInfo(userInfo)
{
    ui->setupUi(this);
    initUI();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{
    DEBUG_LOCATION;

    QString title = QString("地面站系统 - [%1]").arg(m_userInfo.role);
    setWindowTitle(title);

    QString welcome = QString("欢迎用户: %1 (%2)").arg(m_userInfo.username , m_userInfo.role);
    statusBar()->showMessage(welcome, 5000);

    initButtonsByRole();
    initTableTask();

    connect(ui->tableTask, &QTableWidget::itemClicked,
            this, &MainWindow::onTaskItemClicked);

}

void MainWindow::initButtonsByRole()
{
    // 根据角色设置按钮可见性
    switch(UserRole::roleFromString(m_userInfo.role)) {
    case UserRole::Admin:
        // Admin可以看到所有按钮
        ui->btnExecute->setVisible(true);
        ui->btnObtain->setVisible(true);
        ui->btnLogs->setVisible(true);
        break;

    case UserRole::Engineer:
        // Engineer只能看到Execute和Obtain
        ui->btnExecute->setVisible(true);
        ui->btnObtain->setVisible(true);
        ui->btnLogs->setVisible(false);
        break;

    case UserRole::Operator:
        // Operator只能看到Execute
        ui->btnExecute->setVisible(true);
        ui->btnObtain->setVisible(false);
        ui->btnLogs->setVisible(false);
        break;

    default:
        break;
    }
}

void MainWindow::initTableTask()
{
    // 设置表头
    QStringList headers = {"选择", "任务ID", "类型", "描述", "优先级", "状态",
                           "创建时间", "开始时间", "结束时间", "创建者"};
    ui->tableTask->setColumnCount(headers.size());
    ui->tableTask->setHorizontalHeaderLabels(headers);

    // 设置列宽
    ui->tableTask->setColumnWidth(0, 50);   // 选择列
    ui->tableTask->setColumnWidth(1, 120);  // 任务ID
    ui->tableTask->setColumnWidth(2, 80);   // 类型
    ui->tableTask->setColumnWidth(3, 200);  // 描述
    ui->tableTask->setColumnWidth(4, 60);   // 优先级
    ui->tableTask->setColumnWidth(5, 80);   // 状态
    ui->tableTask->setColumnWidth(6, 140);  // 创建时间
    ui->tableTask->setColumnWidth(7, 140);  // 开始时间
    ui->tableTask->setColumnWidth(8, 140);  // 结束时间
    ui->tableTask->setColumnWidth(9, 80);   // 创建者

    // 让最后一列填满剩余空间
    ui->tableTask->horizontalHeader()->setStretchLastSection(true);

    // 设置表格样式
    ui->tableTask->setAlternatingRowColors(true);
    ui->tableTask->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableTask->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableTask->setEditTriggers(QAbstractItemView::NoEditTriggers);  // 只读

    // 设置行高
    ui->tableTask->verticalHeader()->setDefaultSectionSize(30);
}

void MainWindow::updateTaskList(const QList<TaskBasicInfo> &tasks)
{
    // 设置行数
    ui->tableTask->setRowCount(tasks.size());

    // 逐行添加数据
    for (int i = 0; i < tasks.size(); ++i) {
        addTaskToTable(tasks[i], i);
    }
}

void MainWindow::addTaskToTable(const TaskBasicInfo &task, int row)
{
    // 0: 选择 (复选框)
    QTableWidgetItem *checkItem = new QTableWidgetItem();
    checkItem->setCheckState(Qt::Unchecked);
    ui->tableTask->setItem(row, 0, checkItem);

    // 1: 任务ID
    ui->tableTask->setItem(row, 1, new QTableWidgetItem(task.taskId));

    // 2: 任务类型
    ui->tableTask->setItem(row, 2, new QTableWidgetItem(TaskType::toString(task.taskType)));

    // 3: 描述
    ui->tableTask->setItem(row, 3, new QTableWidgetItem(task.description));

    // 4: 优先级
    QTableWidgetItem *priorityItem = new QTableWidgetItem(QString::number(task.priority));

    if (task.priority >= 8) {
        priorityItem->setForeground(QBrush(Qt::red));
        priorityItem->setFont(QFont("", -1, QFont::Bold));
    }
    ui->tableTask->setItem(row, 4, priorityItem);

    // 5: 状态
    QTableWidgetItem *statusItem = new QTableWidgetItem(getStatusText(task.status));
    statusItem->setForeground(QBrush(getStatusColor(task.status)));
    statusItem->setFont(QFont("", -1, QFont::Bold));
    ui->tableTask->setItem(row, 5, statusItem);

    // 6：创建时间
    ui->tableTask->setItem(row, 6, new QTableWidgetItem(task.createTime.toString("yyyy-MM-dd hh:mm:ss")));

    // 7：开始时间
    ui->tableTask->setItem(row, 7, new QTableWidgetItem(task.startTime.isValid() ? task.startTime.toString("yyyy-MM-dd hh:mm:ss"): "-"));

    // 8：结束时间
    ui->tableTask->setItem(row, 8, new QTableWidgetItem(task.startTime.isValid() ? task.endTime.toString("yyyy-MM-dd hh:mm:ss"): "-"));

    // 9：创建者
    ui->tableTask->setItem(row, 9, new QTableWidgetItem(task.creator));

    // 存储完整的任务信息到第0列（隐藏数据）
    ui->tableTask->item(row, 0)->setData(Qt::UserRole,QVariant::fromValue(task));  // 需要注册自定义类型
}

QString MainWindow::getStatusText(TaskStatus::Status status)
{
    switch(status) {
    case TaskStatus::Pending:   return "待执行";
    case TaskStatus::Running:   return "执行中";
    case TaskStatus::Completed: return "已完成";
    case TaskStatus::Failed:    return "失败";
    case TaskStatus::Cancelled: return "已取消";
    default: return "未知";
    }
}

QColor MainWindow::getStatusColor(TaskStatus::Status status)
{
    switch(status) {
    case TaskStatus::Pending:   return QColor(255, 165, 0);  // 橙色
    case TaskStatus::Running:   return QColor(0, 120, 215);  // 蓝色
    case TaskStatus::Completed: return QColor(0, 150, 0);    // 绿色
    case TaskStatus::Failed:    return QColor(220, 20, 60);  // 红色
    case TaskStatus::Cancelled: return QColor(128, 128, 128); // 灰色
    default: return Qt::black;
    }
}

void MainWindow::onTaskItemClicked(QTableWidgetItem *item)
{
    if (!item) return;

    int row = item->row();

    // 获取该行的任务ID
    QString taskId = ui->tableTask->item(row, 0)->text();

    // 或者获取完整的任务信息（如果存储了）
    // TaskBasicInfo task = ui->taskTable->item(row, 0)->data(Qt::UserRole).value<TaskBasicInfo>();

    qDebug() << "选中任务:" << taskId;

    // 可以打开任务详情对话框
    // showTaskDetailDialog(taskId);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!m_userInfo.username.isEmpty()) {
        int ret = QMessageBox::question(this, "退出确认",
                                        QString("用户 %1 正在使用程序，是否退出账户并关闭程序？").arg(m_userInfo.username),
                                        QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) {
            event->ignore();
            return;
        }
    }

    event->accept();    // 接受关闭事件
    DEBUG_LOCATION << QString("用户 %1 退出程序").arg(m_userInfo.username);
}

void MainWindow::on_btnExecute_clicked()
{
   ui->stackedWidget ->setCurrentIndex(0);
}
void MainWindow::on_btnObtain_clicked()
{
    ui->stackedWidget ->setCurrentIndex(1);
}
void MainWindow::on_btnLogs_clicked()
{
    ui->stackedWidget ->setCurrentIndex(2);
}


void MainWindow::on_btnConnectToDevices_clicked()
{
    emit openDeviceConnector();
}


void MainWindow::on_btnLogout_clicked()
{
    emit logoutFromMainWindow();
}

