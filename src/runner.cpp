////////////////////////////////////////
//  File      : runner.cpp            //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "runner.h"

////////////////////////////////////////

Runner::Runner(QWidget *parent) : QDialog(parent)
{
    setSizeGripEnabled(true);
    setContentsMargins(0, 10, 0, 0);
    setWindowModality(Qt::WindowModal);
    read_settings();
    init();
    show();
}

Runner::~Runner()
{
    delete &ok_button_pix_path;
    delete &close_button_pix_path;
}

void Runner::read_settings()
{
    QSettings *antico = new QSettings(QCoreApplication::applicationDirPath() + "/antico.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    QSettings *style = new QSettings(stl_path + stl_name, QSettings::IniFormat, this);
    style->beginGroup("Message");
    ok_button_pix_path = stl_path + style->value("ok_button_pix").toString();
    close_button_pix_path = stl_path + style->value("close_button_pix").toString();
    style->endGroup(); // Message
}

void Runner::init()
{
    QGridLayout *layout = new QGridLayout(this);
    setLayout(layout);
    QLabel *win_lab = new QLabel(tr("<b>RUNNER</b>"), this);
    win_lab->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    QLabel *text = new QLabel(tr("Type command line:"), this);
    command = new QLineEdit("", this);
    QPushButton *run_but = new QPushButton(QIcon(QPixmap(ok_button_pix_path)), tr("Run"), this);
    QPushButton *close_but = new QPushButton(QIcon(QPixmap(close_button_pix_path)), tr("Close"), this);
    layout->addWidget(win_lab, 0, 0, 1, 0);
    layout->addWidget(text, 1, 0, 1, 0);
    layout->addWidget(command, 2, 0, 1, 0);
    layout->addWidget(run_but, 3, 0);
    layout->addWidget(close_but, 3, 1);

    connect(run_but, SIGNAL(pressed()), this, SLOT(run_pressed()));
    connect(close_but, SIGNAL(pressed()), this, SLOT(close_pressed()));
}

void Runner::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::darkGray, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawRect(0, 0, width(), height());
}

void Runner::mousePressEvent(QMouseEvent *event)
{
    mousepos = event->pos();
    grabMouse(QCursor(Qt::SizeAllCursor));
    raise();
}

void Runner::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Runner::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
}

void Runner::run_pressed()
{
    QString cmd = command->text();

    if ((cmd != ""))
    {
        if (QProcess::startDetached(cmd))
            close();
        else
        {
            Msgbox msg;
            msg.set_header(tr("<b>COMMAND INCORRECT</b>"));
            msg.set_info(tr("Check the command syntax. If the app is not in your $PATH, type the absolute app path."));
            msg.set_icon("Critical");
            msg.exec();
        }
    }
}

void Runner::close_pressed()
{
    close();
}
