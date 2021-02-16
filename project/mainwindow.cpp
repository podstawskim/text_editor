#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QStateMachine>
#include <QHistoryState>
#include <QFileDialog>
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    auto stateMachine = new QStateMachine{this};

    // TODO: Create states
    auto unlocked = new QState{stateMachine};
    auto locked = new QState{stateMachine};
    auto startup = new QState{unlocked};
    auto open = new QState{unlocked};
    auto view = new QState{unlocked};
    auto error = new QState{unlocked};
    auto edit = new QState{unlocked};
    auto save = new QState{unlocked};



    // TODO: Set appropriate 'assignProperty'

    //UNLOCKED
    unlocked->assignProperty(ui->pbToggle, "text", "Lock");
    unlocked->assignProperty(ui->pbOpen, "enabled", true);
    unlocked->assignProperty(ui->pbSave, "enabled", true);
    unlocked->assignProperty(ui->teText, "enabled", true);
    unlocked->addTransition(ui->pbToggle, SIGNAL(clicked()), locked);

    //LOCKED
    auto historyState = new QHistoryState(unlocked);
    historyState->setDefaultState(startup);
    locked->assignProperty(ui->pbToggle, "text", "Unlock");
    locked->assignProperty(ui->pbOpen, "enabled", false);
    locked->assignProperty(ui->pbSave, "enabled", false);
    locked->assignProperty(ui->teText, "enabled", false);
    locked->addTransition(ui->pbToggle, SIGNAL(clicked()), historyState);

    //STARTUP
    startup->assignProperty(ui->pbOpen, "enabled", true);
    startup->assignProperty(ui->pbSave, "enabled", false);
    startup->assignProperty(ui->teText, "enabled", false);
    startup->assignProperty(ui->teText, "placeholderText", "Open file to start editing...");
    startup->addTransition(ui->pbOpen, SIGNAL(clicked()), open);

    //OPEN
    connect(open, SIGNAL(entered()), this, SLOT(open()));
    open->addTransition(this, SIGNAL(opened()), view);
    open->addTransition(this, SIGNAL(error()), error);

    //VIEW
    view->assignProperty(ui->pbOpen, "enabled", true);
    view->assignProperty(ui->pbSave, "enabled", false);
    view->assignProperty(ui->teText, "enabled", true);
    view->addTransition(ui->pbOpen, SIGNAL(clicked()), open);
    view->addTransition(ui->teText, SIGNAL(textChanged()), edit);

    //ERROR
    error->assignProperty(ui->pbOpen, "enabled", true);
    error->assignProperty(ui->pbSave, "enabled", false);
    error->assignProperty(ui->teText, "enabled", false);
    error->assignProperty(ui->teText, "placeholderText", "Error ocured. Open file to start editing...");
    error->addTransition(ui->pbOpen, SIGNAL(clicked()), open);

    //EDIT
    edit->assignProperty(ui->pbOpen, "enabled", false);
    edit->assignProperty(ui->pbSave, "enabled", true);
    edit->assignProperty(ui->teText, "enabled", true);
    edit->addTransition(ui->pbSave, SIGNAL(clicked()), save);

    //SAVE
    connect(save, SIGNAL(entered()), this, SLOT(save()));
    save->addTransition(this, SIGNAL(saved()), view);
    save->addTransition(this, SIGNAL(error()), error);


    // TODO: Set state transitions including this class events and slots
    // TODO: Set initial state
    stateMachine->setInitialState(unlocked);
    unlocked->setInitialState(startup);
    // TODO: Start state machine
    stateMachine->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open()
{
    // TODO: Show file dialog
    this->fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files(*)"));
    // TODO: Open selected file
    QFile file(fileName);
    if(!file.open(QFile::ReadOnly | QFile::Text) ) {
        // TODO: Emit 'error' if opening failed
        QMessageBox::warning(this, "title", "file not open");
        emit error();
    }

    // TODO: Set text and emit 'opened' if suceeded
    QTextStream in(&file);
    QString text = in.readAll();
    ui->teText->setPlainText(text);
    file.close();
    emit opened();
    // TODO: Save file name in 'fileName'
}

void MainWindow::save()
{
    // TODO: Open 'fileName' for writing
    QFile file(this->fileName);
    // TODO: Emit 'error' if opening failed
    if(!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "title", "file not open");
        emit error();
    }
    QTextStream out(&file);
    QString text = ui->teText->toPlainText();
    out << text;
    file.flush();
    file.close();
    emit saved();


    // TODO: Save file and emit 'saved' if succeeded
}
