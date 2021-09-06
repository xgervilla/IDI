#include <QLabel>
class MyLabel:public QLabel {
    Q_OBJECT
    public:
        MyLabel(QWidget *parent);
        int cont;
        bool apagat;
        
    public slots:
        //moviments del robot
        void move_left();
        void move_front();
        void move_back();
        void move_right();
        void gir();
        
        //Apagar/encendre el robot
        void on_off();
        
    signals:
        void enviaCont(const int &);
};
