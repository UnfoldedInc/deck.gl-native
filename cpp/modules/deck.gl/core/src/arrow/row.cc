#include "row.h"

#include <arrow/array.h>

using namespace deckgl;
using namespace mathgl;

Row::Row(const std::shared_ptr<arrow::Table>& table, int rowIndex) : _table{table}, _rowIndex{rowIndex} {
  this->_chunkIndex = this->_getChunkIndex(table, rowIndex);
}

// NOTE: Accessors largely based on https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html

auto Row::getInt(const std::string& columnName, int nullValue) -> int {
  if (!this->isValid(columnName)) {
    return nullValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue.has_value()) {
    return static_cast<int>(doubleValue.value());
  }

  // TODO: Log out a warning once logging system is in place
  //  throw std::logic_error("Invalid column type. Expected int, found: " + chunk->type()->ToString());

  return nullValue;
}

auto Row::getFloat(const std::string& columnName, float nullValue) -> float {
  if (!this->isValid(columnName)) {
    return nullValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue.has_value()) {
    return static_cast<float>(doubleValue.value());
  }

  // TODO: Log out a warning once logging system is in place
  //  throw std::logic_error("Invalid column type. Expected float, found: " + chunk->type()->ToString());

  return nullValue;
}

auto Row::getDouble(const std::string& columnName, double nullValue) -> double {
  if (!this->isValid(columnName)) {
    return nullValue;
  }

  auto chunk = this->_getChunk(columnName);
  auto doubleValue = this->_getDouble(chunk);
  if (doubleValue.has_value()) {
    return doubleValue.value();
  }

  // TODO: Log out a warning once logging system is in place
  //  throw std::logic_error("Invalid column type. Expected double, found: " + chunk->type()->ToString());

  return nullValue;
}

auto Row::getBool(const std::string& columnName, bool nullValue) -> bool {
  if (!this->isValid(columnName)) {
    return nullValue;
  }
  if (!this->isValid(columnName)) {
    return nullValue;
  }

  auto chunk = this->_getChunk(columnName);
  if (auto boolArray = std::dynamic_pointer_cast<arrow::BooleanArray>(chunk)) {
    return boolArray->Value(this->_chunkIndex);
  } else if (auto doubleValue = this->_getDouble(chunk)) {
    return static_cast<bool>(doubleValue.value());
  }

  // TODO: Log out a warning once logging system is in place
  //  throw std::logic_error("Invalid column type. Expected bool, found: " + chunk->type()->ToString());

  return nullValue;
}

auto Row::getString(const std::string& columnName, const std::string& nullValue) -> std::string {
  if (!this->isValid(columnName)) {
    return nullValue;
  }

  auto chunk = this->_getChunk(columnName);
  if (auto stringArray = std::dynamic_pointer_cast<arrow::StringArray>(chunk)) {
    return stringArray->GetString(this->_chunkIndex);
  }

  // TODO: Log out a warning once logging system is in place
  //  throw std::logic_error("Invalid column type. Expected string, found: " + chunk->type()->ToString());

  return nullValue;
}

auto Row::getFloatVector2(const std::string& columnName, const Vector2<float>& nullValue) -> Vector2<float> {
  auto chunk = this->_getChunk(columnName);
  switch (chunk->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto offsets = std::static_pointer_cast<arrow::FixedSizeListArray>(chunk);
      auto thisOffset = offsets->value_offset(this->_rowIndex);
      auto nextOffset = offsets->value_offset(this->_rowIndex + 1);

      auto length = nextOffset - thisOffset;
      if (length != 2) {
        // TODO: Log out a warning once logging system is in place
        // throw std::logic_error("Vector does not have 2 values");
        return nullValue;
      }

      auto values = std::static_pointer_cast<arrow::FloatArray>(offsets->values());
      return this->_vector2FromFloatArray(values, nullValue);
    }
    case arrow::Type::LIST: {
      auto offset = std::static_pointer_cast<arrow::ListArray>(chunk);
      auto thisOffset = offset->value_offset(this->_rowIndex);
      auto nextOffset = offset->value_offset(this->_rowIndex + 1);

      auto length = nextOffset - thisOffset;
      if (length != 2) {
        // TODO: Log out a warning once logging system is in place
        // throw std::logic_error("Vector does not have 2 values");

        return nullValue;
      }

      auto values = std::static_pointer_cast<arrow::FloatArray>(offset->values());
      return this->_vector2FromFloatArray(values, nullValue);
    }

    default:
      // TODO: Log out a warning once logging system is in place
      // throw std::logic_error("Invalid column type. Expected list, found: " + chunk->type()->ToString());
      return nullValue;
  }
}

auto Row::getDoubleVector2(const std::string& columnName, const Vector2<double>& nullValue) -> Vector2<double> {
  auto chunk = this->_getChunk(columnName);
  switch (chunk->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto offsets = std::static_pointer_cast<arrow::FixedSizeListArray>(chunk);
      auto thisOffset = offsets->value_offset(this->_rowIndex);
      auto nextOffset = offsets->value_offset(this->_rowIndex + 1);

      auto length = nextOffset - thisOffset;
      if (length != 2) {
        // TODO: Log out a warning once logging system is in place
        // throw std::logic_error("Vector does not have 2 values");
        return nullValue;
      }

      auto values = std::static_pointer_cast<arrow::DoubleArray>(offsets->values());
      return this->_vector2FromDoubleArray(values, nullValue);
    }
    case arrow::Type::LIST: {
      auto offset = std::static_pointer_cast<arrow::ListArray>(chunk);
      auto thisOffset = offset->value_offset(this->_rowIndex);
      auto nextOffset = offset->value_offset(this->_rowIndex + 1);

      auto length = nextOffset - thisOffset;
      if (length != 2) {
        // TODO: Log out a warning once logging system is in place
        // throw std::logic_error("Vector does not have 2 values");

        return nullValue;
      }

      auto values = std::static_pointer_cast<arrow::DoubleArray>(offset->values());
      return this->_vector2FromDoubleArray(values, nullValue);
    }

    default:
      // TODO: Log out a warning once logging system is in place
      // throw std::logic_error("Invalid column type. Expected list, found: " + chunk->type()->ToString());
      return nullValue;
  }
}

auto Row::getFloatVector3(const std::string& columnName, const Vector3<float>& nullValue) -> Vector3<float> {
  auto chunk = this->_getChunk(columnName);
  switch (chunk->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto offsets = std::static_pointer_cast<arrow::FixedSizeListArray>(chunk);
      auto thisOffset = offsets->value_offset(this->_rowIndex);
      auto nextOffset = offsets->value_offset(this->_rowIndex + 1);

      auto length = nextOffset - thisOffset;
      if (length != 3) {
        // TODO: Log out a warning once logging system is in place
        // throw std::logic_error("Vector does not have 3 values");
        return nullValue;
      }

      auto values = std::static_pointer_cast<arrow::FloatArray>(offsets->values());
      return this->_vector3FromFloatArray(values, nullValue);
    }
    case arrow::Type::LIST: {
      auto offset = std::static_pointer_cast<arrow::ListArray>(chunk);
      auto thisOffset = offset->value_offset(this->_rowIndex);
      auto nextOffset = offset->value_offset(this->_rowIndex + 1);

      auto length = nextOffset - thisOffset;
      if (length != 3) {
        // TODO: Log out a warning once logging system is in place
        // throw std::logic_error("Vector does not have 3 values");

        return nullValue;
      }

      auto values = std::static_pointer_cast<arrow::FloatArray>(offset->values());
      return this->_vector3FromFloatArray(values, nullValue);
    }

    default:
      // TODO: Log out a warning once logging system is in place
      // throw std::logic_error("Invalid column type. Expected list, found: " + chunk->type()->ToString());
      return nullValue;
  }
}

auto Row::getDoubleVector3(const std::string& columnName, const Vector3<double>& nullValue) -> Vector3<double> {
  auto chunk = this->_getChunk(columnName);
  switch (chunk->type_id()) {
    case arrow::Type::FIXED_SIZE_LIST: {
      auto offsets = std::static_pointer_cast<arrow::FixedSizeListArray>(chunk);
      auto thisOffset = offsets->value_offset(this->_rowIndex);
      auto nextOffset = offsets->value_offset(this->_rowIndex + 1);

      auto length = nextOffset - thisOffset;
      if (length != 3) {
        // TODO: Log out a warning once logging system is in place
        // throw std::logic_error("Vector does not have 3 values");
        return nullValue;
      }

      auto values = std::static_pointer_cast<arrow::DoubleArray>(offsets->values());
      return this->_vector3FromDoubleArray(values, nullValue);
    }
    case arrow::Type::LIST: {
      auto offset = std::static_pointer_cast<arrow::ListArray>(chunk);
      auto thisOffset = offset->value_offset(this->_rowIndex);
      auto nextOffset = offset->value_offset(this->_rowIndex + 1);

      auto length = nextOffset - thisOffset;
      if (length != 3) {
        // TODO: Log out a warning once logging system is in place
        // throw std::logic_error("Vector does not have 3 values");

        return nullValue;
      }

      auto values = std::static_pointer_cast<arrow::DoubleArray>(offset->values());
      return this->_vector3FromDoubleArray(values, nullValue);
    }

    default:
      // TODO: Log out a warning once logging system is in place
      // throw std::logic_error("Invalid column type. Expected list, found: " + chunk->type()->ToString());
      return nullValue;
  }
}

auto Row::isValid(const std::string& columnName) -> bool {
  auto chunk = this->_getChunk(columnName);
  return chunk->IsValid(this->_rowIndex);
}

auto Row::_getChunk(const std::string& columnName) -> std::shared_ptr<arrow::Array> {
  auto column = this->_table->GetColumnByName(columnName);
  if (column == nullptr) {
    throw std::logic_error("Invalid column name");
  }

  // Iterate over column chunks and find the chunk that contains row data
  int64_t offset = 0;
  for (auto chunk : column->chunks()) {
    if (_rowIndex >= offset && _rowIndex < offset + chunk->length()) {
      // TODO: Cache
      return chunk;
    }

    offset += chunk->length();
  }

  throw std::logic_error("Invalid chunk lookup");
}

auto Row::_getChunkIndex(const std::shared_ptr<arrow::Table>& table, int rowIndex) -> int {
  if (rowIndex >= table->num_rows()) {
    throw std::range_error("Invalid row index");
  }

  // NOTE: Columns are chunked in the same way, so we pick an arbitrary column
  // https://arrow.apache.org/docs/cpp/tables.html#tables
  auto column = table->column(0);

  // Iterate over column chunks and find the chunk that contains row data
  int chunkIndex = rowIndex;
  for (auto chunk : column->chunks()) {
    if (chunkIndex < chunk->length()) {
      return chunkIndex;
    }

    chunkIndex -= chunk->length();
  }

  throw std::logic_error("Invalid chunk lookup");
}

// TODO: Sacrificing performance and precision for conciseness, revisit
auto Row::_getDouble(const std::shared_ptr<arrow::Array>& chunk) -> std::optional<double> {
  switch (chunk->type_id()) {
    case arrow::Type::DOUBLE:
      return std::static_pointer_cast<arrow::DoubleArray>(chunk)->Value(this->_chunkIndex);
    case arrow::Type::FLOAT:
      return static_cast<double>(std::static_pointer_cast<arrow::FloatArray>(chunk)->Value(this->_chunkIndex));
    case arrow::Type::INT64:
      return static_cast<double>(std::static_pointer_cast<arrow::Int64Array>(chunk)->Value(this->_chunkIndex));
    case arrow::Type::INT32:
      return static_cast<double>(std::static_pointer_cast<arrow::Int32Array>(chunk)->Value(this->_chunkIndex));

    default:
      return std::nullopt;
  }
}

auto Row::_vector2FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, const Vector2<float>& nullValue)
    -> Vector2<float> {
  if (values->type_id() != arrow::Type::FLOAT) {
    // TODO: Log out a warning once logging system is in place
    // throw std::logic_error("Inconsistent vector type, expected float got: " + values->type()->ToString());
    return nullValue;
  }

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<float>(1);
  const float* first = listPointer;
  const float* second = listPointer + 1;

  return Vector2<float>(*first, *second);
}

auto Row::_vector2FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, const Vector2<double>& nullValue)
    -> Vector2<double> {
  if (values->type_id() != arrow::Type::DOUBLE) {
    // TODO: Log out a warning once logging system is in place
    // throw std::logic_error("Inconsistent vector type, expected double got: " + values->type()->ToString());
    return nullValue;
  }

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<double>(1);
  const double* first = listPointer;
  const double* second = listPointer + 1;

  return Vector2<double>(*first, *second);
}

auto Row::_vector3FromFloatArray(const std::shared_ptr<arrow::FloatArray>& values, const Vector3<float>& nullValue)
    -> Vector3<float> {
  if (values->type_id() != arrow::Type::FLOAT) {
    // TODO: Log out a warning once logging system is in place
    // throw std::logic_error("Inconsistent vector type, expected double got: " + values->type()->ToString());
    return nullValue;
  }

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<double>(1);
  const double* first = listPointer;
  const double* second = listPointer + 1;
  const double* third = listPointer + 2;

  return Vector3<float>(*first, *second, *third);
}

auto Row::_vector3FromDoubleArray(const std::shared_ptr<arrow::DoubleArray>& values, const Vector3<double>& nullValue)
    -> Vector3<double> {
  if (values->type_id() != arrow::Type::DOUBLE) {
    // TODO: Log out a warning once logging system is in place
    // throw std::logic_error("Inconsistent vector type, expected double got: " + values->type()->ToString());
    return nullValue;
  }

  // Needed for some reason as seen in https://arrow.apache.org/docs/cpp/examples/row_columnar_conversion.html
  auto listPointer = values->data()->GetValues<double>(1);
  const double* first = listPointer;
  const double* second = listPointer + 1;
  const double* third = listPointer + 2;

  return Vector3<double>(*first, *second, *third);
}
