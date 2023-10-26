#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Конструктор основного окна
    ui->setupUi(this);
    add_graphics_scene();
    _pen_line.setBrush(Qt::black);
    _pen_line.setWidth(3);
    _pen_ellipse.setBrush(Qt::black);
    _pen_red_line.setBrush(Qt::red);
    _pen_red_line.setWidth(3);
    connect(ui->station_btn, SIGNAL(clicked()), this, SLOT(station_btn_clicked()));
    connect(ui->path_btn, SIGNAL(clicked()), this, SLOT(path_btn_clicked()));
    connect(ui->calcul_btn, SIGNAL(clicked()), this, SLOT(calculate_btn_clicked()));
    connect(ui->reset_min_btn, SIGNAL(clicked()), this, SLOT(reset_min_btn_clicked()));
}

void MainWindow::add_station(const qreal& x, const qreal& y){
    //Добавление и удаление точек станцый на графическую сцену. А так-же их имен.
    bool renge = (x > 5 && x < 690 && y > 5 && y < 550);
    if(!renge) return void();
    auto ellipse_it = std::find_if(_ellipse_ptr_list.begin(),
                                   _ellipse_ptr_list.end(),
                                   [x, y]
                                   (const station_ellipse& tmp_ellipse){
        bool first_b = (tmp_ellipse.ellipse->rect().x() >= x-5 &&
                        tmp_ellipse.ellipse->rect().x() <= x+5);
        bool second_b = (tmp_ellipse.ellipse->rect().y() >= y-5 &&
                         tmp_ellipse.ellipse->rect().y() <= y+5);
        return (first_b && second_b);
    });
    if(ellipse_it != _ellipse_ptr_list.end()) {
        //Удаление станции и прекрепленных к ней линий
        if(!_arr_line.empty()){
            auto line_it = _arr_line.begin();
            while (line_it != _arr_line.end()) {
                line_it = std::find_if(_arr_line.begin(),
                                       _arr_line.end(),
                                       [&ellipse_it]
                                       (const auto& line){
                    bool first = (ellipse_it->ellipse->rect().x() == line.pnt_last.x() &&
                                  ellipse_it->ellipse->rect().y() == line.pnt_last.y());
                    bool second = (ellipse_it->ellipse->rect().x() == line.pnt_first.x() &&
                                   ellipse_it->ellipse->rect().y() == line.pnt_first.y());
                    return (first||second);
                });
                if(line_it != _arr_line.end()){
                    _w_scene->removeItem(line_it->line_ptr);
                    _w_scene->removeItem(line_it->line_close_ptr);
                    _w_scene->removeItem(line_it->text_val_ptr);
                    _arr_line.erase(line_it);
                }
            }
        }
        if(ellipse_it->ellipse != nullptr)
            _w_scene->removeItem(ellipse_it->ellipse);
        else
            qDebug() << "ОШИБКА УДАЛЕНИЯ СТАНЦИИ!!!!";
        if(ellipse_it->name_station != nullptr)
            _w_scene->removeItem(ellipse_it->name_station);
        _ellipse_ptr_list.erase(ellipse_it);
    }
    else{
        //Добавление станции
        QGraphicsEllipseItem *ellipse_ptr = new QGraphicsEllipseItem();
        station_ellipse tmp_st;
        ellipse_ptr->setRect(x, y, 10, 10);
        ellipse_ptr->setPen(_pen_ellipse);
        ellipse_ptr->setBrush(QBrush(Qt::blue));
        ellipse_ptr->setZValue(1);
        _w_scene->addItem(ellipse_ptr);
        tmp_st.ellipse = std::move(ellipse_ptr);
        tmp_st.coord = tmp_st.ellipse->rect().topLeft();
        //Добавление Названий станций
        bool ok;
        QString str = QInputDialog::getText(this, "Название станции", "", QLineEdit::Normal, "", &ok);
        if(ok){
            QGraphicsTextItem *text = new QGraphicsTextItem(str);
            _w_scene->addItem(text);
            text->setPos(x - str.size()*3, y - 20);
            tmp_st.name_station = std::move(text);
        }
        else{
            _w_scene->removeItem(ellipse_ptr);
            return void();
        }
        _ellipse_ptr_list.emplace_back(tmp_st);
    }
}

void MainWindow::add_puth(const qreal& x, const qreal& y){
    //Добавление путей
    bool renge = (x > 5 && x < 690 && y > 5 && y < 550);
    if(!renge) return void();

    auto ellipse_it = std::find_if(_ellipse_ptr_list.begin(),
                                   _ellipse_ptr_list.end(),
                                   [x, y]
                                   (const station_ellipse& tmp_ellipse){
        bool first_b = (tmp_ellipse.ellipse->rect().x() >= x-5 &&
                        tmp_ellipse.ellipse->rect().x() <= x+5);
        bool second_b = (tmp_ellipse.ellipse->rect().y() >= y-5 &&
                         tmp_ellipse.ellipse->rect().y() <= y+5);
        return (first_b && second_b);
    });

    auto line_it = std::find_if(_arr_line.begin(),
                                _arr_line.end(),
                                [x, y]
                                (const pos_line& line){
        bool first = (x >= line.line_close_ptr->rect().x() - 3 &&
                      x < line.line_close_ptr->rect().x() + 3);
        bool last = (y >= line.line_close_ptr->rect().y() - 3 &&
                     y < line.line_close_ptr->rect().y() + 3);
        return (first && last);
    });

    if(line_it != _arr_line.end()){
        //Удаление пути
        _w_scene->removeItem(line_it->line_close_ptr);
        _w_scene->removeItem(line_it->line_ptr);
        _w_scene->removeItem(line_it->text_val_ptr);
        _arr_line.erase(line_it);
        return void();
    }

    if(ellipse_it == _ellipse_ptr_list.end()) return void();

    if(!_line.flag_first){
        _line.flag_first = true;
        _line.pnt_first = {int(ellipse_it->ellipse->rect().x()), int(ellipse_it->ellipse->rect().y())};
        return void();
    }

    if(_line.flag_first && !_line.flag_last && _line.pnt_first !=
            QPoint(int(ellipse_it->ellipse->rect().x()), int(ellipse_it->ellipse->rect().y()))){
        auto arr_it = std::find_if(_arr_line.begin(), _arr_line.end(), [&](const pos_line& point){
            QPoint tmp_pnt = QPoint{int(ellipse_it->ellipse->rect().x()), int(ellipse_it->ellipse->rect().y())};
            bool tmp_first = (point.pnt_first == _line.pnt_first && point.pnt_last == tmp_pnt);
            bool tmp_last = (point.pnt_first == tmp_pnt && point.pnt_last == _line.pnt_first);
            return (tmp_first || tmp_last);
        });

        if(arr_it == _arr_line.end()){
            _line.flag_last = true;
            _line.pnt_last = {int(ellipse_it->ellipse->rect().x()), int(ellipse_it->ellipse->rect().y())};
        }
    }

    if(_line.examination()){
        //Отрисовка пути
        QGraphicsLineItem *line_ptr = new QGraphicsLineItem();
        QGraphicsEllipseItem *close_ptr = new QGraphicsEllipseItem();
        auto line = QLine(_line.pnt_first.x() + 5, _line.pnt_first.y() + 5,
                          _line.pnt_last.x() + 5, _line.pnt_last.y() + 5);
        line_ptr->setLine(line);
        line_ptr->setPen(_pen_line);
        close_ptr->setRect(line.center().x() - 3, line.center().y() - 3, 6, 6);
        QPen pen;
        pen.setBrush(Qt::black);
        close_ptr->setBrush(QBrush(Qt::red));
        close_ptr->setPen(pen);
        _w_scene->addItem(line_ptr);
        _w_scene->addItem(close_ptr);
        _line.line_ptr = std::move(line_ptr);
        _line.line_close_ptr = std::move(close_ptr);
        bool ok;
        double val = QInputDialog::getDouble(this, "Название станции", "", 0, 0, 100000.0, 1, &ok);
        if(ok){
            std::string str = std::to_string(val);
            QGraphicsTextItem *text = new QGraphicsTextItem(QString::number(val));
            _w_scene->addItem(text);
            qreal mod_x = std::abs(line.p1().x() - line.p2().x());
            qreal mod_y = std::abs(line.p1().y() - line.p2().y());
            if(mod_y == 0) mod_y = 1;
            if(mod_x/mod_y > 1)
                text->setPos(line.center().x() - str.size()*1.2, line.center().y() - 25);
            else
                text->setPos(line.center().x() - str.size()*3, line.center().y() - 20);
            _line.value = val;
            _line.text_val_ptr = std::move(text);
        }
        else{
            _w_scene->removeItem(line_ptr);
            _w_scene->removeItem(close_ptr);
            _line.reset();
            return void();
        }
        _arr_line.emplace_back(std::move(_line));
        _line.reset();
    }
}

void MainWindow::calculate_min_val_path(const qreal& x, const qreal& y){
    //Подсчет минимальной стоимости пути.
    bool renge = (x > 5 && x < 690 && y > 5 && y < 550);
    if(!renge) return void();
    auto ellipse_it = std::find_if(_ellipse_ptr_list.begin(), _ellipse_ptr_list.end(), [x, y](const station_ellipse& tmp_ellipse){
        bool first_b = (tmp_ellipse.ellipse->rect().x() >= x-5 && tmp_ellipse.ellipse->rect().x() <= x+5);
        bool second_b = (tmp_ellipse.ellipse->rect().y() >= y-5 && tmp_ellipse.ellipse->rect().y() <= y+5);
        return (first_b && second_b);
    });
    if(ellipse_it != _ellipse_ptr_list.end()){
        if(!_pair_statuion.first.ellipse){
            _pair_statuion.first = *ellipse_it;
            ellipse_it->ellipse->setBrush(QBrush(Qt::red));
            return void();
        }
        else{
            _pair_statuion.second = *ellipse_it;
            ellipse_it->ellipse->setBrush(QBrush(Qt::red));
        }
    }
    auto pnt_begin = _pair_statuion.first.coord.toPoint();
    std::list <QPointF> target_list;
    QStringList final_path;
    int i = 1; // Ограничитель от зацикливания
    while (target_list.size() < 10 && i <= 100 ) {
        if(_arr_line.size() == 0 || _ellipse_ptr_list.size() == 0){
            qDebug() << "ERROR vectors";
            return void();
        }
        auto line_it = std::find_if(_arr_line.begin(), _arr_line.end(), [&pnt_begin](const auto& line){
            return (line.pnt_first == pnt_begin || line.pnt_last == pnt_begin) && !line.block;
        });
        if(line_it != _arr_line.end())
            line_it->block = true;
        QPoint pnt_end;
        if(line_it == _arr_line.end()){
            pnt_begin = target_list.front().toPoint();
            if(target_list.size() > 0)
                target_list.pop_front();
            ++i;
            continue;
        }
        else{
            pnt_end = (pnt_begin == line_it->pnt_first) ? line_it->pnt_last : line_it->pnt_first;
        }

        auto ell_begin_it = std::find_if(_ellipse_ptr_list.begin(), _ellipse_ptr_list.end(), [&pnt_begin](const auto& ell){
            return pnt_begin == ell.coord.toPoint();
        });

        for(auto& ell : _ellipse_ptr_list){
            if(ell.ellipse->rect().topLeft() == pnt_end && !ell.block){
                if (ell.final_value == 0 || (ell.final_value > (line_it->value + ell_begin_it->final_value)))
                    ell.final_value = line_it->value + ell_begin_it->final_value;
                auto target_it = std::find(target_list.begin(), target_list.end(), ell.ellipse->rect().topLeft());
                if(target_it == target_list.end()){
                    target_list.push_back(ell.ellipse->rect().topLeft());
                }
            }
        }
        auto anti_block_it = std::find_if(_arr_line.begin(), _arr_line.end(), [&pnt_begin](const auto& line){
            return (line.pnt_first == pnt_begin || line.pnt_last == pnt_begin) && !line.block;
        });
        if(anti_block_it == _arr_line.end()){
            ell_begin_it->block = true;
        }
        QStringList test_str;
        for(auto a : target_list){
            auto it = std::find_if(_ellipse_ptr_list.begin(), _ellipse_ptr_list.end(),[a](auto b){
                return b.ellipse->rect().topLeft() == a;
            });
            test_str << it->name_station->toPlainText();
        }
        ++i;
    }
    std::vector<pos_line> red_line;
    station_ellipse build_station = _pair_statuion.second;
    int j = 0;
    final_path += _pair_statuion.second.name_station->toPlainText();
    while(build_station != _pair_statuion.first || j > 100){
    ++j;
    for(auto& ell : _ellipse_ptr_list){
        if (ell == build_station){
            for(auto& line : _arr_line){
                if(line.pnt_first == ell.coord || line.pnt_last == ell.coord){
                    QPoint tmp_ell = (line.pnt_first != ell.coord) ? line.pnt_first : line.pnt_last;
                    auto need_ell_it = std::find_if(_ellipse_ptr_list.begin(), _ellipse_ptr_list.end(), [&tmp_ell](const station_ellipse& ell){
                        return ell.coord == tmp_ell;
                    });
                    if (need_ell_it != _ellipse_ptr_list.end() && need_ell_it->final_value == ell.final_value - line.value){
                        build_station = *need_ell_it;//
                        final_path += need_ell_it->name_station->toPlainText();
                        red_line.push_back(line);
                        break;
                    }
                }
            }
        }
    }
    }
    QString value_path_str;
    for(auto& ell : _ellipse_ptr_list){
        if(ell == _pair_statuion.second){
            value_path_str = QString::number(ell.final_value);
        }
    }
    for(auto& line : red_line){
        line.line_ptr->setPen(_pen_red_line);
    }
    QMessageBox *box = new QMessageBox;
    std::reverse(final_path.begin(), final_path.end());
    box->setWindowTitle("Ответ");
    box->setText("Самый короткий путь\n" + final_path.join(" -> ") + "\n" + "Объщая продолжительность пути составляет " + value_path_str);
    box->show();
}

void MainWindow::add_graphics_scene(){
    QGraphicsScene *w_scene = new QGraphicsScene(0, 0, 705, 555);
    QGraphicsView *w_gview = new QGraphicsView();
    w_gview->setScene(w_scene);
    ui->brash_layout->addWidget(w_gview);
    _w_scene = w_scene;
}

void MainWindow::station_btn_clicked(){
    _mouse_event_val = 1;
}

void MainWindow::path_btn_clicked(){
    _mouse_event_val = 2;
}

void MainWindow::calculate_btn_clicked(){
    _mouse_event_val = 3;
}

void MainWindow::reset_min_btn_clicked(){
    for(auto& ell : _ellipse_ptr_list){
        ell.final_value = 0;
        ell.block = false;
        ell.ellipse->setBrush(QBrush(Qt::blue));
    }
    _pair_statuion = {};
    for(auto& line : _arr_line){
        line.block = false;
        line.line_ptr->setPen(_pen_line);
    }

}

void MainWindow::mousePressEvent(QMouseEvent *event){
    switch(_mouse_event_val){
    case 1:
        add_station(event->pos().x() - 17, event->pos().y() - 7);
        break;
    case 2:
        add_puth(event->pos().x() - 17, event->pos().y() - 7);
        break;
    case 3:
        calculate_min_val_path(event->pos().x() - 17, event->pos().y() - 7);
        break;
    default:
        qDebug() << "mousePressEvent не определен";
        break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

