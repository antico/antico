////////////////////////////////////////
//  File      : deskapp.cpp           //
//  Written by: g_cigala@virgilio.it  //
//  Copyright : GPL                   //
////////////////////////////////////////

#include "deskapp.h"

////////////////////////////////////////

Deskapp::Deskapp(const QString &app_nm, const QString &app_exe, const QString &app_pix, QWidget *parent) : QWidget(parent)
{
    setFixedSize(100, 50);
    read_settings();
    app_name = app_nm;
    app_exec = app_exe;
    setToolTip(app_exec + app_name);
    d_app_pix = QPixmap(app_pix);
    zoom = false;
    show();
}

Deskapp::~Deskapp()
{
    delete &app_name;
    delete &app_exec;
    delete &d_app_pix;
    delete &d_app_col;
    delete &delete_link_pix;
}

void Deskapp::read_settings()
{
    // get style path
    antico = new QSettings(QCoreApplication::applicationDirPath() + "/antico.cfg", QSettings::IniFormat, this);
    antico->beginGroup("Style");
    QString stl_name = antico->value("name").toString();
    QString stl_path = antico->value("path").toString();
    antico->endGroup(); //Style
    // get style values
    style = new QSettings(stl_path + stl_name, QSettings::IniFormat,this);
    style->beginGroup("Deskapp");
    d_app_col = style->value("name_color").value<QColor>();
    style->endGroup(); //Deskapp
    style->beginGroup("Other");
    delete_link_pix = stl_path + style->value("delete_link_pix").toString();
    style->endGroup(); //Other
}

void Deskapp::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setWindow(-50, -50, 100, 50);
    painter.setPen(d_app_col);
    
    if (zoom)
    {
        painter.drawPixmap(QRect(-18, -50, 36, 36), d_app_pix, QRect(0, 0, d_app_pix.width(), d_app_pix.height()));// deskapp pix
    }
    else
    {
        painter.drawPixmap(QRect(-16, -50, 32, 32), d_app_pix, QRect(0, 0, d_app_pix.width(), d_app_pix.height()));// deskapp pix
    }
    
    QString name = QApplication::fontMetrics().elidedText(app_name, Qt::ElideRight, 90); // if app_name is too long, add ... at the end
    painter.drawText(-50, -15, 100, 20, Qt::AlignHCenter, name); // deskapp name
}

void Deskapp::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        menu = new QMenu(this);
        QAction *del_app = menu->addAction(QIcon(delete_link_pix), tr("Delete link"));
        menu->popup(event->globalPos());
        connect(del_app, SIGNAL(triggered()), this, SLOT(del_app()));
    }
    if (event->button() == Qt::LeftButton)
    {
        mousepos = event->pos();
        grabMouse(QCursor(Qt::SizeAllCursor));
    }
}

void Deskapp::mouseMoveEvent(QMouseEvent *event)
{
    QPoint p = event->globalPos()-mousepos;
    move(p.x(), p.y());
}

void Deskapp::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    releaseMouse();
    // save new deskapp position on desk writing on Anfico.conf
    antico->beginGroup("Desktop");
    antico->beginGroup("App");
    antico->beginGroup(app_name); // App name
    antico->setValue("pos", pos());
    antico->endGroup(); // Icon name
    antico->endGroup(); // App
    antico->endGroup(); // Desktop
}

void Deskapp::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QProcess::startDetached(app_exec + app_name); //launch the application
    }
}

void Deskapp::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = true;
    update();
}

void Deskapp::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    zoom = false;
    update();
}

void Deskapp::del_app()
{
    emit destroy_deskapp(this);
    // remove the deskapp from desk and from Antico.conf
    antico->beginGroup("Desktop");
    antico->beginGroup("App");
    antico->remove(app_name);
    antico->endGroup(); // App
    antico->endGroup(); // Desktop
}

void Deskapp::update_style()
{
    read_settings();
    update();
}

