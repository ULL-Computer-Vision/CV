#include "controller.h"

#include <iostream>

controller::controller(QWidget *mn, PluginModel *mdl) :
  plugin_controller (mn, mdl)
{}

bool controller::operator () (picture* image, LUT* lut, canvas_image_label* canvas) {

  mdl->set_image(image);
  mdl->restore_backup();

  picture* aux = image->make_copy();

  aux->resize(aux->get_size().height(), aux->get_size().width());

  unsigned width  = image->get_image()->size().width();
  unsigned height = image->get_image()->size().height();
  QImage* img = image->get_image();

  aux->each_pixel_modificator_with_index([&](QColor color, unsigned i, unsigned j) -> QColor {
    return (img->pixelColor(width - j, i));
  });

  image->restore_from(aux);

  emit update_inform();

  return true;

}

