# Using garrow::Table

The `Table` manages GPU buffer(s) representing a table of binary columns, where "table" implies that the columns have the same number of "rows" (though not necessarily the same byte length).

The `Table` class carefully designed to support Apache Arrow formatted tables:
- Supports initialization from arrow tables, building GPU buffer(s) from the chunked columns)
- Attempts to cover a large subset of the data types that can be represented as columns in Arrow tables.
- Defines mapping between Arrow data types and WebGPU
- Defines transformations

`garrow` API is modeled after `arrow`, and currently is identical when it comes to the key functionality, including `Table`. As this is a work in progress, only a subset of API has been implemented thus far for usage by `luma.gl` native implementation. As of right now, the goal is to provide complete functionality defined by `arrow` API reference that can be found [here](https://arrow.apache.org/docs/cpp/api/table.html#tables).  

## References

This guide describes basic usage of `Table`, see separate article for more advanced use cases, including:
- Variable Length Rows
- Constant columns...
- Index column...

It can be useful to understand how `GPUTable` compares with other similar APIs:
- The Apache Arrow `Table` class.
- The Python pandas [`DataFrame`](https://pandas.pydata.org/pandas-docs/stable/reference/frame.html) class.
- The [GPU Data Frame](http://gpuopenanalytics.com/#/PROJECTS) in the [GoAi](http://gpuopenanalytics.com/#/) GPU Open Analytics Initiative.
