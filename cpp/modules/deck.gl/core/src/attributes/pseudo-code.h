/*

// PSEUDO CODE TO ILLUSTRATE HOW ATTRIBUTE MANAGER SHOULD WORK

struct Row { string name; float value; };
Row table[1000];


template <>
class TableContainer {
  TableContainer();

  // Defined by user
  length();
  getRow(index);
}


// Inside layer

am = new AttributeManager();
am->addAttribute('positions', ...);

ScatterplotLayer::Props
  getPosition(row: Row) -> Vector3<double>

AttributeManager::initialize() {
  for (attribute of this->attributes) {
    this->gpuColumns[attribute.name] = new GPUColumn(attribute."opts")
  }
}

AttributeManager::update(table, ) {
  for (auto gpuColumn : this->gpuColumns) {
    gpuColum->resize(table.length);
  }

  for (auto row : table) {
    //
    Vector3<double> position = getPosition(row);
    gpuColumn->setValue(position);

    ColorRGBA color = getColor(row);
  }
}
*/