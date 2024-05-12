# MCNBT

 一个使用C++编写的易用NBT读写Header-Only库。

## :earth_africa: 依赖库

**gzip-hpp:** 用于解压缩使用GZIP的NBT标签。若不需要使用，可通过在*nbt.hpp*文件中加入以下宏进行禁用。

```cpp
#define NBT_NOGZIP
```

跳转：https://github.com/mapbox/gzip-hpp

## :rocket: 特点

- 简单易用
- 速度快
- 支持不同字节序（对于Java版本的数字标签通常使用大端序，而基岩版则为小端序）
- 支持NBT转换至SNBT
- 可生成单方块实体结构（仅基岩版），用于对特殊实体的导入（目前仅支持结构方块与命令方块）

## :robot: 兼容文件

.dat
.nbt
.mcstructure
.schematic

## :triangular_flag_on_post: 使用方法

对于标准NBT操作，只需要在项目中包含*nbt.hpp*头文件（对于未使用NBT_NOGZIP宏进行禁用GZIP库的情况下，项目还需要配置*cpp-gzip*库）

### 1、从文件中读取NBT

```cpp
#include <nbt.hpp>

int main(){
    //...
    Nbt::Tag nbt = Nbt::Tag(Nbt::Compound, is, false);
    //...
    
    return 0;
}
```

代码中的Nbt::Tag函数是Nbt::Tag类的一个构造函数，此构造函数接受一个Nbt::TagTypes、输入流、布尔值作为参数。

**参数一（Nbt::TagTypes）**：用于指示根标签的类型（根标签可以为List和Compound），通常而言第一个参数选择使用Nbt::Compound。

**参数二（std::istream）**：若是从文件读取（文件必须以二进制形式读取）则应当传递文件的ifstream对象，以表示从文件输入流中读取NBT。

**参数三（bool）**：用于指示当前输入流的字节序，默认为小端序，即默认支持读取基岩版NBT文件。

### 2、构造NBT

```cpp
#include <nbt.hpp>

int main(){
    // 方法一
    Nbt::Tag nbt = Nbt::Tag(Nbt::Compound, "root");
    // 方法二
    Nbt::Tag list = Nbt::gList("version");
    
    return 0;
}
```

**方法一** 构造了一个名为*root*的Compound。

**方法二** 是一个“便利函数”用于快捷创建特定对象，在本例中，其构造了一个名为*version*的List。

所有的便利函数：

- Nbt::gByte()
- Nbt::gShort()
- Nbt::gInt()
- Nbt::gLong()
- Nbt::gFloat()
- Nbt::gDouble()
- Nbt::gByteArray()
- Nbt::gIntArray()
- Nbt::gLongArray()
- Nbt::gList()
- Nbt::gCompound()

以及以上便利函数的PureData版本：

- Nbt::gpByte()
- Nbt::gpShort()
- Nbt::gpInt()
- Nbt::gpLong()
- Nbt::gpFloat()
- Nbt::gpDouble()
- Nbt::gpByteArray()
- Nbt::gpIntArray()
- Nbt::gpLongArray()
- Nbt::gpList()
- Nbt::gpCompound()

PureData版本与普通版本的区别在于，PureData版本构造出的Tag适用于作为List的元素。（普通版本获得的Tag同样可以作为List的元素，但在被加入List时会对Tag的名称进行删除，并将其转换为PureData）
以上函数同样可以通过Nbt::Tag的构造函数实现，如Nbt::Tag(Nbt::Int, false)等同于Nbt::gInt()，Nbt::Tag(Nbt::Int, true)等同于Nbt::gpInt()。

### 3、读取与修改Tag值

对于基本数据类型Tag，直接调用对应的get或set函数即可。
如对于一个Nbt::Int类型的Tag

```cpp
//...
Nbt::Tag num = Nbt::gInt("Num", 1);
num.getInt();	// return 1.
num.setInt(10);
num.getInt();	// return 10.
//...
```

对于数组类型Tag（ByteArray，IntArray，LongArray），可以使用对应的get函数获取整个数据对象（使用std::vector封装）并直接对其操作，对于简单的增删元素可以使用相应的add和remove函数。
如对于一个Nbt::IntArray类型的Tag

```cpp
//...
Nbt::Tag intArray = Nbt::gIntArray("Nums");
intArray.addInt(1);			// add element.
intArray.addInt(2);
intArray.addInt(3);
intArray.remove(0);			// remove element by index.
std::vector<int32> *data = intArray.getIntArray();
int first = (*data)[0];		// first = 2.
//...
```

对于复合数据类型（List，Compound），可以使用addMember与removeMember和<<运算符重载函数进行增删操作，使用getMember或[]运算符重载函数进行读取操作。
对于Compound标签，[]运算符重载函数可以使用Tag在Compound中的位置索引与Tag的名称作为参数（若有同名Tag，则返回索引值最小的Tag，若未找到会引发报错，所以在获取元属前应该使用HasMember()等函数进行判断）。
而List标签的[]运算符重载函数只能使用索引作为参数。

```cpp
//...
Nbt::Tag root = Nbt::gCompound("compound");
root.addMember(gInt("Num1", 1));		 // 使用addMember()函数添加元素。
root << gInt("Num2", 2);				// 使用<<运算符添加元素。
root.removeMember("Num1");				// 使用元素名称移除元素。
root.removeMember(0);			    	// 使用元素索引移除元素。
root.addMember(gString("str", "text"));
root["str"].setString("text2");			// 使用元素名称获取元素。
std::string str = root[0].getString();	 // 使用元素索引获取元素。str = "text2"。

Nbt::Tag list = Nbt::gList("list", Nbt::Int);	// 除了名称外，还需要指定List内元素的数据类型。
list.addMember(gpInt(1));			// 使用addMember()函数添加元素，推荐使用gp系列函数构造List内的元素。
list << gpInt(2);				   // 使用<<运算符添加元素。
int num = list[0].getInt();		    // num = 1
//...
```



