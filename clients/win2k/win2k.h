/*
  Gallium-Win2k KWin client
  
  Copyright 2001
    Karol Szwed <karlmail@usa.net>
    http://gallium.n3.net/

  Based on the default KWin client.

  Major code cleanups, bug fixes and updates to support toolwindows 3/2001 - KS
*/

#ifndef __KDEGALLIUM_WIN2K_H
#define __KDEGALLIUM_WIN2K_H

#include <qbutton.h>
#include <qbitmap.h>
#include <kpixmap.h>
#include "../../client.h"
class QLabel;
class QSpacerItem;
class QHBoxLayout;

namespace KWinInternal {

class GalliumButton : public QButton
{

public:
    GalliumButton(Client *parent=0, const char *name=0, const unsigned char *bitmap=NULL,
                  bool menuButton=false, bool isMini=false );
    void setBitmap(const unsigned char *bitmap);
    void setPixmap(const QPixmap &p);
    void reset();

    QSize sizeHint() const;
    int   last_button;

protected:
    void mousePressEvent( QMouseEvent* e )
    {
	   last_button = e->button();
       QMouseEvent me ( e->type(), e->pos(), e->globalPos(),
                        LeftButton, e->state() );
	   QButton::mousePressEvent( &me );
    }

    void mouseReleaseEvent( QMouseEvent* e )
    {
	   last_button = e->button();
	   QMouseEvent me ( e->type(), e->pos(), e->globalPos(), 
                        LeftButton, e->state() );
	   QButton::mouseReleaseEvent( &me );
    }

    virtual void drawButton(QPainter *p);
    void drawButtonLabel(QPainter *){;}

    QBitmap  deco;
    QPixmap  pix;
    Client*  client;
    bool     menuBtn;
    bool     miniBtn;
};


class GalliumClient : public KWinInternal::Client
{
    Q_OBJECT

public:
    enum Buttons{ BtnHelp=0, BtnMax, BtnIconify, BtnClose, BtnMenu, BtnCount };
    GalliumClient( Workspace *ws, WId w, QWidget *parent=0, const char *name=0 );
    ~GalliumClient() {;}
    int titleHeight;

protected:
    void resizeEvent( QResizeEvent* );
    void paintEvent( QPaintEvent* );
    void showEvent( QShowEvent* );
    void mouseDoubleClickEvent( QMouseEvent * );
    void captionChange( const QString& name );
    void maximizeChange(bool m);
    void activeChange(bool);
    void iconChange();

    void calcHiddenButtons();

protected slots:
    void slotReset();
    void slotMaximize();
    void menuButtonPressed();

private:
    GalliumButton* button[ GalliumClient::BtnCount ];
    int            lastButtonWidth;
    QSpacerItem*   titlebar;
    bool           hiddenItems;
    QHBoxLayout*   hb;
    bool           smallButtons;
};

};

#endif
