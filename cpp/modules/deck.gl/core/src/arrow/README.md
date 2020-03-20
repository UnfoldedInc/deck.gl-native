## Arrow Utilities

### Row

Provides an easy way to extract typed row data out of a table column. Below are the available accessors, together with Arrow types that they map from.

| C++                       | Arrow                                                       | Notes                                                                |
|--------------------- |---------------------------------------------- |----------------------------------------------------  |
| getInt                    | DOUBLE, FLOAT, INT64, INT32               |                                                                           |
| getFloat                | DOUBLE, FLOAT, INT64, INT32               |                                                                           |
| getDouble             | DOUBLE, FLOAT, INT64, INT32               |                                                                           |
| getBool                 | BOOL, DOUBLE, FLOAT, INT64, INT32   |                                                                           |
| getString               | String                                                        |                                                                           |
| getFloatVector2    | FIXED_SIZE_LIST, LIST                            | Must contain only 2 elements of type float       |
| getDoubleVector2 | FIXED_SIZE_LIST, LIST                            | Must contain only 2 elements of type double   |
| getFloatVector3    | FIXED_SIZE_LIST, LIST                            | Must contain only 3 elements of type float        |
| getDoubleVector3 | FIXED_SIZE_LIST, LIST                            | Must contain only 3 elements of type double   |
