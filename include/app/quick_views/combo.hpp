#pragma once

#include <QFrame>
#include <QString>

class Arbiter;

class ComboQuickView : public QFrame, public QuickView {
    Q_OBJECT

   public:
    ComboQuickView(Arbiter &arbiter);

    void init() override;
};
