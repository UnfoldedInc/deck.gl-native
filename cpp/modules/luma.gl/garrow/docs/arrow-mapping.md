# Storing GPU Attributes in Arrow Tables

Columnar tables typically have columns one value per row (say `longitude`, `latitude` and `height`) while GPU shaders typically work on columns (i.e. vertex attributes) with one short vector (`[longitude, latitude, height]`) per row.

While it is possible to efficiently combine the invidual columns to a new column after loading, to maximize efficiency and to avoid using memory to store the temporary, throw-away individual columns, it can be desirable to generate the table with a pre-interleaved column with a short fixed-size array in each row.

Thus, to represent typical GPU attributes as Arrow table columns, we need columns that contains short fixed-size arrays of numbers, e.g. three 32-bit floats per row (representing positions) or four 8-bit unsigned integers per row (representing colors). Arrow API provides `FixedSizeArray` type which can be used for this exact purpose.

`GArrow` provides a simple interface that maps an existing `arrow::Table` into a `garrow::Table`, mapping supported data types to their WebGPU counterparts appropriately while copying data into GPU memory.  

## Arrow to WebGPU Type Map

| Arrow Type                       | WebGPU Vertex Type   |
|--------------------------------  |--------------------  |
| UINT32                           | UInt                 |
| INT32                            | Int                  |
| FLOAT                            | Float                |
| FIXED_SIZE_LIST(BOOL, 2)         | Char2                |
| FIXED_SIZE_LIST(BOOL, 4)         | Char4                |
| FIXED_SIZE_LIST(UINT16, 2)       | UShort2              |
| FIXED_SIZE_LIST(UINT16, 4)       | UShort4              |
| FIXED_SIZE_LIST(INT16, 2)        | Short2               |
| FIXED_SIZE_LIST(INT16, 4)        | Short4               |
| FIXED_SIZE_LIST(UINT32, 2)       | UInt2                |
| FIXED_SIZE_LIST(UINT32, 3)       | UInt3                |
| FIXED_SIZE_LIST(UINT32, 4)       | UInt4                |
| FIXED_SIZE_LIST(INT32, 2)        | Int2                 |
| FIXED_SIZE_LIST(INT32, 3)        | Int3                 |
| FIXED_SIZE_LIST(INT32, 4)        | Int4                 |
| FIXED_SIZE_LIST(HALF_FLOAT, 2)   | Half2                |
| FIXED_SIZE_LIST(HALF_FLOAT, 4)   | Half4                |
| FIXED_SIZE_LIST(FLOAT, 2)        | Float2               |
| FIXED_SIZE_LIST(FLOAT, 3)        | Float3               |
| FIXED_SIZE_LIST(FLOAT, 4)        | Float4               |
