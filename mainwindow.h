#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsEllipseItem>
#include <QPushButton>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

struct station_ellipse{
    QPointF coord;
    QGraphicsEllipseItem *ellipse = nullptr;
    QGraphicsTextItem *name_station = nullptr;
    double final_value = 0.0;
    bool block = false;
    bool operator == (const station_ellipse& tmp){
        bool lhs = this->name_station->toPlainText() == tmp.name_station->toPlainText();
        bool rhs = this->coord == tmp.coord;
        return lhs && rhs;
    }
    bool operator != (const station_ellipse& tmp){
        return !(*this == tmp);
    }
};

struct pos_line{
    QPoint pnt_first = {0, 0};
    QPoint pnt_last = {0, 0};
    bool flag_first = false;
    bool flag_last = false;
    double value = 0.0;
    bool block = false;

    bool examination(){
        return (flag_first && flag_last);
    }

    void reset(){
        pnt_first = {0, 0};
        pnt_last = {0, 0};
        flag_first = false;
        flag_last = false;
        line_ptr = nullptr;
        line_close_ptr = nullptr;
    }

    QGraphicsLineItem* line_ptr = nullptr;
    QGraphicsEllipseItem* line_close_ptr = nullptr;
    QGraphicsTextItem* text_val_ptr = nullptr;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    void add_graphics_scene();

    void add_station(const qreal& x, const qreal& y);

    void add_puth(const qreal& x, const qreal& y);

    void calculate_min_val_path(const qreal& x, const qreal& y);

    ~MainWindow();

protected:
    void mousePressEvent(QMouseEvent *e) override;

private slots:
    void station_btn_clicked();
    void path_btn_clicked();
    void calculate_btn_clicked();
    void reset_min_btn_clicked();

private:
    int _mouse_event_val = 1;
    pos_line _line;
    std::vector<pos_line> _arr_line;
    Ui::MainWindow *ui;
    QPen _pen_line;
    QPen _pen_ellipse;
    QPen _pen_red_line;
    QGraphicsScene *_w_scene;
    std::vector<station_ellipse> _ellipse_ptr_list;
    std::pair<station_ellipse, station_ellipse> _pair_statuion;
};

#endif // MAINWINDOW_H
