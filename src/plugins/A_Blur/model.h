#ifndef MODEL_H
#define MODEL_H

#include "../../model/picture.h"
#include "../../controller/plugin_interface.h"

class filter_a {
private:
  std::vector <std::vector <int>> kernel;
public:
  filter_a(std::vector<std::vector<int>> k) : kernel (k) {}

  inline unsigned get_y_size () const { return kernel.size(); }
  inline unsigned get_x_size () const { return kernel[0].size(); }

  inline int get_element (unsigned i, unsigned j) const {
    return kernel[i][j];
  }

};

class model : public PluginModel {
public:
  model();
  filter_a* generate_filter (unsigned size) {

    std::vector<std::vector<int>> ker (size);
    for (unsigned i = 0; i < size; i++) {
      ker[i].resize(size);
      for (unsigned j = 0; j < size; j++) {
        ker[i][j] = 1;
      }
    }
    return new filter_a (ker);
  }

};

#endif // MODEL_H
