#ifndef MENU_H
#define MENU_H

#include <QWidget>
#include <QPushButton>
#include <QBoxLayout>

#define PAL_TXT "PAL"
#define NTSC_TXT "NTSC"

class view_interface : public QWidget {
  Q_OBJECT
public:
  view_interface (QWidget* parent) : QWidget (parent) {}
};

class menu : public view_interface {
  Q_OBJECT
private:
  QPushButton* bttn_PAL;
  QPushButton* bttn_NTSC;
public:
  explicit menu(QWidget *parent = nullptr);
  inline QPushButton* get_pal_bttn () { return bttn_PAL; }
  inline QPushButton* get_ntsc_bttn () { return bttn_NTSC; }
private slots:
  void set_ntsc_checked (bool);
  void set_pal_checked (bool);
signals:
  void pal (bool);  
};

#endif // MENU_H

