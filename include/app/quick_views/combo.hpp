#pragma once

#include <QFrame>

#include "app/quick_views/quick_view.hpp"

class Arbiter;

class ComboQuickView : public QFrame, public QuickView {
    Q_OBJECT

   public:
    ComboQuickView(Arbiter &arbiter);

    void init() override;
};
