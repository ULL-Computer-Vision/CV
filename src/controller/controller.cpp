#include "controller.h"

controller::controller() {
  main_window = new MainWindow();
  mdl = new model();

  main_window->show();


  op_dock = main_window->get_options_dock();
  vw      = main_window->get_view();
  foot    = main_window->get_footer();

  connect(main_window, SIGNAL(load_image(QString)),SLOT(on_load_image(QString)));
  connect(main_window, SIGNAL(save_image(QString, unsigned)),this,SLOT(on_store_image(QString, unsigned)));
  connect(vw, SIGNAL(delete_image(uint)),  this, SLOT(on_close_image(uint)));
  connect(vw, SIGNAL(image_focused(uint)), this, SLOT(on_set_active_image(uint)));

  connect(this,SIGNAL(update_metadata(QString, QSize)),
          foot,SLOT(on_update_meta_data(QString,QSize)));

  connect(this,SIGNAL(update_rgb_at(QColor)),
          foot,SLOT(on_update_rgb_at(QColor)));

  connect(foot,SIGNAL(get_rgb_at(QPoint)),
          this,SLOT(on_get_rgb_at(QPoint)));

  connect(this, SIGNAL(update_histograms(histogram)),
          op_dock->get_histogram_wid(),
          SLOT(on_update_charts(histogram)));

  connect(this, SIGNAL(update_operation_option(QWidget*)),
          op_dock->get_operation_wid(),
          SLOT(on_set_widget(QWidget*)));

  connect(this,&controller::update_basic_info,op_dock->get_image_wid(),
               &image_info_widget::on_update_basic_info);

  plugin_ctrller = new plugin_controller (op_dock->get_operation_wid(), foot);

  connect(plugin_ctrller,&plugin_controller::generate_image,this,&controller::on_create_image);  
  connect(plugin_ctrller,&plugin_controller::update_histogram,this,&controller::update_histograms);  
  connect(plugin_ctrller,&plugin_controller::update_basic_info,this,&controller::update_basic_info);

  connect(plugin_ctrller,&plugin_controller::request_current_image,this,&controller::on_get_current_image);
  connect(this,&controller::send_current_image,plugin_ctrller,&plugin_controller::on_receive_current_image);

  load_all_plugins(DEFAULT_PLUGINS_LOCATION);
}

void controller::on_load_image(const QString& file_name) {
  op_dock->setEnabled(true);
  mdl->load_image(file_name);
  vw->add_canvas_window(* (mdl->get_picture_at(-1)->get_pixmap()), file_name);
}

void controller::on_store_image(const QString& file_name, unsigned id) {
  mdl->store_image(file_name, id);
}

void controller::on_close_image() {
  mdl->delete_imagepix_at(active_image);
}

void controller::on_set_active_image (unsigned key) {
  active_image = key;
  picture* aux = mdl->get_picture_at(key);
  emit update_histograms(aux->get_histograms());
  emit update_basic_info(aux->get_basic_info());
  emit update_metadata(aux->get_format(), aux->get_size());
}

bool controller::load_all_plugins (const QString& path) {
  QDir pluginsDir(qApp->applicationDirPath());
  pluginsDir.cd(path);
  bool result = true;
  foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
    load_plugin(fileName, pluginsDir);
    std::cout << "\n";
  }
  return result;
}

void controller::on_get_rgb_at(QPoint pos) {
  emit update_rgb_at(mdl->get_current_picture()->get_color(pos));
}

void controller::on_get_current_image() {
  emit send_current_image(mdl->get_picture_at(active_image));
}

bool controller::load_plugin (const QString& path, const QDir& dir) {

  QPluginLoader pluginLoader(dir.absoluteFilePath(path));
  QObject *plugin = pluginLoader.instance();
  std::cout << "Tratando de cargar " << path.toStdString() << " | ";

  if (!plugin) {
    std::cout << "No se ha podido cargar" << std::endl;
      return false;
  } else {
    std::cout << "plugin cargado correctamente";
    PluginInterface* aux = qobject_cast<PluginInterface *>(plugin);
    unsigned index = mdl->add_plugin(aux);
    plugin_metainfo info = aux->get_meta_info();
    indexed_action* plugin_action = main_window->on_add_plugin(info.category, info.name, index);
    connect(plugin_action,SIGNAL(pressed_signal(uint)),this,SLOT(use_plugin(uint)));
  }
}

void controller::on_create_image(picture *pic) {  
  mdl->add_image(pic);
  vw->add_canvas_window(* (mdl->get_picture_at(-1)->get_pixmap()), "file_name");
}

void controller::use_plugin(unsigned index) {  
  if (index < mdl->get_plugins().size()) {
    plugin_ctrller->on_clear();   
    PluginInterface* aux_plugin = mdl->get_plugins().at(index);
    aux_plugin->instance();
    emit update_operation_option(aux_plugin->get_view());   
    picture* aux_pic = mdl->get_picture_at(active_image);
    canvas_window* aux_canvas = vw->get_active_canvas();
    plugin_ctrller->operator ()(aux_canvas, aux_plugin, aux_pic);
  }
}
