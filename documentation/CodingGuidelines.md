# Coding Guidelines

First and foremost it is of major importance that **everything is written in English!**

For classes which inherit explicitly from 3rd party libraries, then it is acceptable to maintain the interface of that said library.
However, the implementations should still follow the guidelines used in this project.
If the 3rd party library does not have an explicit definition of the interface style then that from this library should be used.


## Naming Convention and Coding Style
Defines a set of common naming conventions for certain identifiers to avoid confusions.
Note, the examples do not include comments since these are explained [in the documentation section.](#documentation)


### Classes and Structs
Class and struct names use the UpperCamelCaseStyle e.g. 

```C++
class ThisIsMyClass {
	...
};

struct ThisIsMyStruct {
	...
};
```

**Member variables** follow the `_lowerCamelCaseStyle`, but are preceeded by an underscore.
**Member functions** use the generic style of [functions](#functions).
```C++
class MyClass {
	double getMemberVariable() const;

protected:
	double _thisIsMyMemberVariable = 0.0;
};
```


The number of included files in the header files should be reduced as far as possible using forward definitions.
Instead the needed files can be included in the source files instead when needed.

Classes that are intended to be passed around with pointers must have defined a typdef for `MyClass::Ptr` (pointer to class) and `MyClass::CPtr` (Pointer to class of const type) using the `std::shared_ptr<>` unless it inherits a class from another library using a different convention.
```C++
class MyClass {
	typedef std::shared_ptr< MyClass > Ptr;
	typedef std::shared_ptr< const MyClass > CPtr;
};
```


### Enumerators
Enum names use the UpperCamelCaseStyle and their enum elements use SCREAMING_SNAKE_CASE (all caps snake case) style e.g.

```C++
enum ThisIsMyEnum {
	ENUM_ONE, ENUM_TWO
};
```


### Functions
Function names use the lowerCamelCaseStyle e.g.

```C++
void thisIsMyFunction( ... );
```

Function parameters do not follow a specific naming convention, but they should be understandable name where camel case and/or underscore can be used to help with the meaning of the variable.
Function inputs that are integral types (like int, char, double, etc) should be passed-by-value and non-integral types as a constant pass-by-reference.
Excepetions to this rule is when they are also used as an output to the user or the input is intentionally modified for the user.


### Namespaces
Namespaces use the lowerCamelCaseStyle e.g.

```C++
namespace thisIsMyNamespace {
	namespace thisIsMyNestedNamespace {
		...
	}
}
```

Header files may not use the 'using namespace XYZ;' keyword, but must be typed explicitly.


### Interfaces, Abstractions and Proxies
When coding **Interfaces**, **Abstractions** or **Proxy** interfaces the names are to be used to describe the class accuratly.
The identifyer is to be appended to the class name such `class MyCodeInterface;`, `class MyCodeAbstraction;` and `class MyCodeProxy;`.

A **Interface** class describes the basic functions as a base class to be used by its children, but without specifying their implementation.
The word **Interface** is hence only to be used if a class has only pure virtual functions (`virtual myFunc() = 0;`).

An **Abstraction** is a class much like the Interface, but does also contain some implemented function bodies.
It is however still required that an **Abstraction** has at least one pure virtual function.


## Documentation
The documentation of functions, classes, etc uses the doxygen standard with java-style using `/** ... */` and `@...` for commands such as `brief`, `param` and `return`.
Extra supported commands for automatic listing: `todo`, `hack` and `warning`.

The in-code comments likewise follow the `/** ... */` comment style.


## Language Usage
Language usage defines a set of do's and do not's when it comes the C++ language features.


### Type Casts
Type casts may not be c-style casts like type(object) or (type)object.
Casting must hence be done with static_cast<>(), dynamic_cast<>() or similar.
[Why c-style cast is not used.](https://stackoverflow.com/a/332086)


### Increments
Whenever post-increment (`i++;`) is not strictly needed, then pre-increment (`++i;`) MUST be used.
As example in for loops the syntax would be as follows:

```C++
for ( size_t i = 0; i < 4; ++i ) {
	...
}
```


### Pass by Reference
Function parameters are to be passed by reference where applicable.
A good rule of thumb is that objects to be passed as reference are:
 1) those which size is larger than the reference itself and
 2) objects which have a copy constructor that is not trivial.

 Example would be (for 1) all stl containers and objects whith multiple member variables, but not integral types.
 Furhtermore, (for 2) all smart pointers.

 Note that when the pass by reference is applied usually also a const is adequate (except for when the object is to be used as an output parameter).


### Consts
Const is also to be applied when appropiate.
If the const does not make a difference it should be left out such that it des not confuses more than it gains.

Usually const applies to classes passed as **function parameters** by reference or pointer (unless modification of such is needed).
Note that in the case of a pointer, there is a difference as well if it is the object or pointer that is const.
A smart pointer should by default be passed as a const reference ([see Pass by Reference](#Pass by Reference)) and the object const if it is not to be modified by the function.

When using const as the qualifyer to a *member function* itself (to signal it does not change the object) a good idication of it being applicable is when
the function name starts with either 'get', 'is' or any other name that signals only reading the state.
Otherwise it does depend on the function itself (what it does and which name it has), if it makes sense to const qualify it.
