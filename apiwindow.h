#ifndef APIWINDOW_H
#define APIWINDOW_H
#include <QMainWindow>
#include <QWidget>

namespace Ui { class centralwidget; }

class APIWindow : public QWidget
{
    Q_OBJECT
public:
    explicit APIWindow(QWidget *parent = nullptr);

private:
    Ui::centralwidget *ui;
signals:

private slots:
    void on_btn_init_clicked();
    void on_btn_clearTextBox_clicked();
    void on_btn_deinit_clicked();
    void on_btn_getData_clicked();
    void on_btn_showData_clicked();
    void on_btn_getShow_clicked();
    void on_btn_autoFileName_clicked();
    void on_btn_writeFile_clicked();
    void on_textBrowser_textChanged();
};

#endif // APIWINDOW_H
