#include "model.h"

model::model() :
current_point (nullptr)
{
  points.push_back(new SyncPoint(0,0));
  points.push_back(new SyncPoint(DEPTH - 1, DEPTH - 1));
}

#include <iostream>

void model::on_release_point() {  
  current_point = nullptr;
}

void model::on_update_point(QPoint original_c, double factor) {
  if (current_point == nullptr) return;

  if (current_point == points.first())
    original_c.setY(0);
  else if (current_point == points.back())
    original_c.setY(DEPTH - 1);
  else if (original_c.y() > DEPTH - 1)
    original_c.setY(DEPTH - 1);
  else if (original_c.y() < 0)
    original_c.setY(0);

  if (original_c.x() > DEPTH - 1)
    original_c.setX(DEPTH - 1);
  else if (original_c.x() < 0)
    original_c.setX(0);


  if (current_point != points.first() && current_point != points.back() ) {
    if (original_c.x()      < prev->x())
      original_c.setX(prev->x());
    else if (original_c.x() > next->x())
      original_c.setX(next->x());
  }


  current_point->setX(original_c.x());
  current_point->setY(original_c.y());
  emit update_chart (points);
}

void model::on_click_point(QPoint point, double factor) {
  prev = points.front();
  next = points.back();

  QPoint* candidate_point = new QPoint(-1,-1);

  for (unsigned i = 1; i < points.size(); i++) {
    SyncPoint* p1 = points.at(i - 1);
    SyncPoint* p2 = points.at(i);

    if (fabs(p1->x() * factor - point.x()) < DISTANCE) {
      candidate_point = p1;
      current_point = points.at(i - 1);
      break;
    } else if (fabs(p2->x() * factor - point.x()) < DISTANCE) {
      candidate_point = p2;
      current_point = points.at(i - 1);
      break;
    } else if (p2->x() * factor > point.x() && p1->x() * factor < point.x()) {
      prev = p1;
      current_point = new SyncPoint(point.x(), point.y());
      points.insert(i, current_point);

      if (current_point != points.back())
        next = points.at(i + 1);
    }
  }
}

