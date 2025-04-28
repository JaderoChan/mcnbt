# NBT data of Minecraft Bedrock Edition

**[简体中文](README.md) | English**

The data is from the Minecraft Wikipedia. Some less frequently used data and more common data have not been specially implemented. You can obtain relevant information through Wikipedia or other channels and implement possible NBT data blocks by inheriting the structure of the relevant base class.

## Regulation of NBT data structure

### Data structure

- The data structure must is `struct` not `class`.
- All the member variables that need to be written to `savegame` (referring to NBT, the same below) should be `public` variables.

### Member variables of the data structure

- It should be named with a name that conforms to its function instead of using its `savegame id`.
- Variables of basic types must be their corresponding data types in `savegame`. For example, `Int` in `savegame` corresponds to `nbt::Int32`, and `Float` corresponds to `nbt::Fp32`.

  - Even when the available values are known and limited hard-coded values, `enum` cannot be used to replace its original data type. Instead, an `enum` based on the variable data type should be provided, while the data type of the variable itself remains unchanged.
  - When the available values are a known finite hard-coded String, the variable type must be `nbt::String`, and at the same time provide a `enum` and static conversion function from `enum values` to `nbt::String`.
  - This entry does not apply to the `bool` type. Although the `bool` type in `savegame` is actually stored using `Byte`, the `bool` type must still be used in the code implementation.
  - For non-basic type variables (such as `Compound`, `List`, etc. in `savegame`), `STL containers` can be used as substitutes.
