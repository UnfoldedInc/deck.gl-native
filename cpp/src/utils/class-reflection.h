/////////////////////////////////////////////////////////
/// \file muclass.h
/// \brief Mutil reflection / metatype support
///
/// Include "muclass" if you want to either
/// 1) Define metatype "descriptions" that describe C++ types
/// 2) Traverse and manipulate classes returned by Object::GetClass.
///
/// "muclass" should be seen as an add-on to the basic "Class" and "Namespace"
/// classes defined in "muobject". It allows the programmer to create detailed
/// "descriptions" of C++ classes and types that can be examined at runtime.
/// While intended to be a very general mechanism, the primary use cases for
/// the type information are:
/// - Automatic prettyprinting of complex objects when tracing code
/// - Support creation of elaborate debug system to be created, allowing enumeration of all "described" classes inspection and even manipulation of objects in runtime.
/// - Make it trivial to expose C++ classes into runtime en
///
/// Requirements/Goals:
/// - Make it possible to place all metatype description classes for in CPP files rather than header files, to minimize amount of global code and accordingly, compile times.
/// - It should also be possible to place the metatype definitions
///   temporarily or permanently in header files close to the definitions of
///   the actual c++ types they describe, to make it easier to keep the
///   definitions consistent.
/// - Keep the runtime overhead to zero, the memory overhead
///   from the additional type information small, and also to make it possible
///   to compile out...
/// - Efforts have been made to keep the syntax of the "descriptions" as
///   brief and crisp as possible. As C++11 compiler support improves, it
///   should be possible to eliminate the need for MACROs even in portable code.
/// \author Ib Green
/// \date   2010-2012
////////////////////////////////////////////////////////////



#ifndef CLASS_REFLECTION
#define CLASS_REFLECTION

<<<<<<< HEAD
namespace reflection 
=======
namespace reflection
>>>>>>> prop-types: can now iterate over and compare layer props
{

////////////////////////////////////////////////////////////
// primitive values

class Object;
  class HeapObject;                 // Object allocated on optimized heap allocator
    class RefObject;                // Uses reference counted smart pointers
      class Value;                  // Native object
<<<<<<< HEAD
        class Bool;                 // 
=======
        class Bool;                 //
>>>>>>> prop-types: can now iterate over and compare layer props
        class Integer;              // RangedInteger, Enumeration (signed int/unsigned int)
        class Float;                // RangedFloat
        class Character;            //wchar_t        class Pointer;              // non ref counted
        class CompositeObject;      // For fundamental value-semantics types
          // class Point, Size, Rect
        class Container;            // size(), iteration, foreach()
<<<<<<< HEAD
          class Sequence;      
            class List_;              // std::list<Ref<RefObject>>
            class Array;              // Get(index), Set(index)
              class Vector;           // std::vector<Ref<RefObject>>  
=======
          class Sequence;
            class List_;              // std::list<Ref<RefObject>>
            class Array;              // Get(index), Set(index)
              class Vector;           // std::vector<Ref<RefObject>>
>>>>>>> prop-types: can now iterate over and compare layer props
              class String;           // std::string
      class ScriptObject;         // Hashmap, key-value map etc
// <<LispObject, JavaScriptObject, ...>


////////////////////////////////////////////////////////////
// type names

# define MU_DECLARE_BUILTIN_CLASS() \
    public: \
    static const struct Class : mutil::Class { \
      Class (const Definition &def) : mutil::Class (def) {} \
      void WriteObject (const Object *obj, std::ostream &os, WriteContext &) const override; \
    } class_; \
    auto GetClass_ () const -> const mutil::Class * override { return &class_; }
<<<<<<< HEAD
    
=======

>>>>>>> prop-types: can now iterate over and compare layer props


////////////////////////////////////////////////////////////
// Types

//!Value is the base class for "fundamental" value data types:
// bool, int, float, pointer, std::string
<<<<<<< HEAD
// enumerations are supported as a subclass of int - they can be used where an int is expected, but 
=======
// enumerations are supported as a subclass of int - they can be used where an int is expected, but
>>>>>>> prop-types: can now iterate over and compare layer props

class Value : public RefObject {
public:
  typedef RefObject inherited;
  virtual void SetValue(Ref<Value>) { throw std::runtime_error("illegal conversion"); }

  static const struct Class : mutil::Class {
    Class () : mutil::Class (DefinitionT<Value> (namespace_, "Value")) {}
<<<<<<< HEAD
  } class_; 
=======
  } class_;
>>>>>>> prop-types: can now iterate over and compare layer props
  auto GetClass_ () const -> const mutil::Class * override { return &class_; }
};


class Bool : public Value {
public:
  typedef Value inherited;
<<<<<<< HEAD
  explicit Bool(bool value_) : value(value_) {} 
  // void SetValue(Ref<Value> value) { value = value; }
  bool value;
 
=======
  explicit Bool(bool value_) : value(value_) {}
  // void SetValue(Ref<Value> value) { value = value; }
  bool value;

>>>>>>> prop-types: can now iterate over and compare layer props
  MU_DECLARE_BUILTIN_CLASS();
};


class Integer : public Value {
public:
  typedef Value inherited;
  explicit Integer (int value_) : value(value_) {}
<<<<<<< HEAD
  int Get() const { return value; }    
  virtual void Set(int value_) { value = value_; }
  int value;    
  
=======
  int Get() const { return value; }
  virtual void Set(int value_) { value = value_; }
  int value;

>>>>>>> prop-types: can now iterate over and compare layer props
  MU_DECLARE_BUILTIN_CLASS();
};


class Float : public Value {
public:
  typedef Value inherited;
  float value;

  MU_DECLARE_BUILTIN_CLASS();
};


<<<<<<< HEAD
class Pointer : public Value { 
public:
  typedef Value inherited;
  explicit Pointer(void *value_) : value(value_) {} 
=======
class Pointer : public Value {
public:
  typedef Value inherited;
  explicit Pointer(void *value_) : value(value_) {}
>>>>>>> prop-types: can now iterate over and compare layer props
  void *value;

  MU_DECLARE_BUILTIN_CLASS();
};


class String : public Value {
<<<<<<< HEAD
public:  
  typedef Value inherited;
  // [construct/copy/destroy]
  String();
  explicit String(const std::string &value_) : value(value_) {} 
=======
public:
  typedef Value inherited;
  // [construct/copy/destroy]
  String();
  explicit String(const std::string &value_) : value(value_) {}
>>>>>>> prop-types: can now iterate over and compare layer props

  // [selectors]
  const std::string &GetString() const { return value; }

  // [modifiers]
  void SetString(const std::string &s) { value = s; }
<<<<<<< HEAD
      
=======

>>>>>>> prop-types: can now iterate over and compare layer props
  std::string value;
  bool readOnly;

  MU_DECLARE_BUILTIN_CLASS();
};

<<<<<<< HEAD
 
 
=======


>>>>>>> prop-types: can now iterate over and compare layer props
 ////////////////////////////////////////////////////////////
// @class

class Character : public Value {
public:
  typedef Value inherited;

  enum DisplayName {
      Space = ' '
    , Newline = '\n'
    , Tab = '\t'
    , Page = '\f'
    , Return = '\r'
    , Backspace = '\b'
    , Bell = '\a'
    , Vtab = '\v'
  };

  // [Construction]
  explicit Character (wchar_t c) : value (c) { }

  // [Selectors]
  wchar_t GetCode () const { return value; }
  int GetDigit () const;
  std::string GetName () const;
<<<<<<< HEAD
  
=======

>>>>>>> prop-types: can now iterate over and compare layer props
  bool IsAlpha () const;
  bool IsAlnum () const;
  bool IsUpper () const;
  bool IsLower () const;
  bool IsBothCase () const;
  bool IsDigit () const;
  bool IsGraphic () const;
  bool IsStandard () const;
  int Collate (const Character &, bool considerCase = true) const;

  // [Operations]
  void DownCase ();
  void UpCase ();
<<<<<<< HEAD
  
=======

>>>>>>> prop-types: can now iterate over and compare layer props
  // [static members]
  static int CodeLimit () { return 65536; } //numeric_limitswchar_t::max () + 1;}
  static Ref<Character> CreateFromDigit (unsigned digit);
  static Ref<Character> CreateFromDesignator (const std::string & designator);
  static Ref<Character> CreateFromName (const std::string & name);

  // [implementation]
  //bool Eql (RefObject *s) const;
  //bool Equal (RefObject *s) const;
  //bool Equalp (RefObject *s) const;

  // void WriteObject (std::ostream &, WriteContext &) const override;

  wchar_t value;

  MU_DECLARE_BUILTIN_CLASS();
};
<<<<<<< HEAD
# undef MU_DECLARE_BUILTIN_CLASS  
=======
# undef MU_DECLARE_BUILTIN_CLASS
>>>>>>> prop-types: can now iterate over and compare layer props



////////////////////////////////////////////////////////////
// Enumeration support
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// Enumeration
////////////////////////////////////////////////////////////


/// \brief abstract base class for Enum

class Enumeration : public Integer {
public:
  typedef Integer inherited;
<<<<<<< HEAD
  
=======

>>>>>>> prop-types: can now iterate over and compare layer props
  void Set (int value);
  void SetValue(Ref<Value> value) override;

protected:
  explicit Enumeration (int value) : Integer(value) {}

public:
  struct Class : public inherited::Class {
  public:
    Class (const Definition & def, int invalidNativeValue_ = -1)
      : inherited::Class(def), invalidNativeValue(invalidNativeValue_) {}

    void WriteObject (const Object *obj, std::ostream &os, WriteContext &) const;

    int Count() const;
    const char *GetEnumeratorName(int e) const;
    int GetEnumeratorNativeValue(int enumValue) const;
    int GetEnumeratorFromNativeValue(int nativeValue) const;
    int GetEnumeratorFromName (const std::string &name) const;
    bool IsValidEnumerator (int value) const;
    bool IsValidEnumeratorName (const std::string &name) const;
    const std::list<int> GetEnumerators() const;
    std::list<std::string> GetEnumeratorNames() const;
<<<<<<< HEAD
    
=======

>>>>>>> prop-types: can now iterate over and compare layer props
    // [Enumeration::Class implementation]
    struct Map {
      const char *name;
      int enumerator;
      int nativeValue;
    };
    const Map &operator[] (int i) const { return *std::next (GetMap().begin(), i); }
    typedef boost::iterator_range<const Map *> Range;
    virtual boost::iterator_range<const Map *> GetMap() const;
<<<<<<< HEAD
    
=======

>>>>>>> prop-types: can now iterate over and compare layer props
    int invalidNativeValue;
  };
  static const Class class_;
  const Class *GetClass_() const override { return &class_; }
};



////////////////////////////////////////////////////////////
// EnumT
////////////////////////////////////////////////////////////

/// \brief automatically instantated class for enumerated types
template <typename T>
  class Enum : public Enumeration {
public:
  // [types]
  typedef Enumeration inherited;
  typedef T Type;
<<<<<<< HEAD
  
  // [construct/copy/destroy]
  explicit Enum (T e) : Enumeration (static_cast<int>(e)) {
    if (! GetClass_()->IsValidEnumerator(static_cast<int>(e)))
      throw XBadArgument("invalid enum value", value); 
  }
  
=======

  // [construct/copy/destroy]
  explicit Enum (T e) : Enumeration (static_cast<int>(e)) {
    if (! GetClass_()->IsValidEnumerator(static_cast<int>(e)))
      throw XBadArgument("invalid enum value", value);
  }

>>>>>>> prop-types: can now iterate over and compare layer props
  struct Class : public inherited::Class {
    Class (const Namespace &namespace_, const char *name, Map *map, int count,
      int invalidNativeValue = -1)
      : inherited::Class(DefinitionT<Enum<T>> (namespace_, name), invalidNativeValue)
      , range(boost::iterator_range<const Map *> (map, map + count))
      {}
    boost::iterator_range<const Map *> GetMap() const override { return range; }
    boost::iterator_range<const Map *> range;
  };

  static const Class class_;
  const Class *GetClass_() const override { return &class_; }
};


// TODO - align with method names

template <class T> struct EnumTraits { typedef int NativeType; };

template <class T>
  inline const char *GetEnumeratorName (T e)
    { return Enum<T>::class_.GetEnumeratorName (static_cast<int> (e)); }

template <class T>
  inline typename EnumTraits<T>::Type MapEnumToNativeValue (T e)
    { return static_cast<typename EnumTraits<T>::Type> (Enum<T>::class_.GetEnumeratorNativeValue (static_cast<int> (e))); }

template <class T>
  inline T MapNativeValueToEnum (typename EnumTraits<T>::Type nativeValue)
    { return static_cast<T> (Enum<T>::class_.GetEnumeratorFromNativeValue (nativeValue)); }

template <class T>
  inline T MapStringToEnum (const std::string &name)
    { return Enum<T>::class_.GetEnumeratorFromName (name); }

template <class T>
  inline bool IsValidEnumerator (T e)
    { return Enum<T>::class_.IsValidEnumerator (static_cast<int> (e)); }



////////////////////////////////////////////////////////////
<<<<<<< HEAD
// Class Property Support 
=======
// Class Property Support
>>>>>>> prop-types: can now iterate over and compare layer props
////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////
// TypeTraits

// TODO simplify using base class

template <class T> struct TypeTraits {};

template <typename Type_, typename ObjectType_> struct StructTraitsT {
  typedef ObjectType_ ObjectType;
  typedef Type_ Type;
  typedef const Type & ArgumentType;
  typedef Type ReturnType;
  static ReturnType ConvertArgument(ObjectType *object) { return object->value; }
<<<<<<< HEAD
  static Ref<ObjectType> ConvertReturn(ArgumentType value) { return make_ref<ObjectType> (value); }   
  static bool IsRef() { return false; }   
=======
  static Ref<ObjectType> ConvertReturn(ArgumentType value) { return make_ref<ObjectType> (value); }
  static bool IsRef() { return false; }
>>>>>>> prop-types: can now iterate over and compare layer props
};


template <typename Type_, typename ObjectType_> struct BuiltinTraitsT {
  typedef ObjectType_ ObjectType;
  typedef Type_ Type;
  typedef Type ArgumentType;
  typedef Type ReturnType;
  static ReturnType ConvertArgument(ObjectType *object) { return object->value; }
<<<<<<< HEAD
  static Ref<ObjectType> ConvertReturn(ArgumentType value) { return make_ref<ObjectType> (value); }   
  static bool IsRef() { return false; }   
=======
  static Ref<ObjectType> ConvertReturn(ArgumentType value) { return make_ref<ObjectType> (value); }
  static bool IsRef() { return false; }
>>>>>>> prop-types: can now iterate over and compare layer props
};


template <> struct TypeTraits<bool> : BuiltinTraitsT<bool, Bool> {};

template <> struct TypeTraits<int> : BuiltinTraitsT<int, Integer> {};
<<<<<<< HEAD
 
=======

>>>>>>> prop-types: can now iterate over and compare layer props
template <> struct TypeTraits<unsigned int> : BuiltinTraitsT<unsigned int, Integer> {};

template <> struct TypeTraits<std::string> : StructTraitsT<std::string, String> {};

template <class T> struct TypeTraits<Enum<T>> : BuiltinTraitsT<T, Enum<T>> {
  typedef typename Enum<T>::Type Type;
  typedef Type ArgumentType;
  typedef Type ReturnType;
  typedef Enum<Type> ObjectType;
  static T ConvertArgument(ObjectType *object) { return static_cast<Type> (object->value); }
  static Ref<Enum<Type>> ConvertReturn(T value) { return make_ref<Enum<Type>> (value); }
<<<<<<< HEAD
  static bool IsRef() { return false; }   
};

template <class T> struct TypeTraits<T *> { 
=======
  static bool IsRef() { return false; }
};

template <class T> struct TypeTraits<T *> {
>>>>>>> prop-types: can now iterate over and compare layer props
  typedef T * Type;
  typedef T * ArgumentType;
  typedef const T * ReturnType;
  typedef T ObjectType;
<<<<<<< HEAD
  static ArgumentType ConvertArgument (T *object) { return object; } 
  static Ref<T> ConvertReturn(ReturnType value) { return Ref<ObjectType>(const_cast<ObjectType *> (value)); } // ERROR const cast   
  static bool IsRef() { return false; }   
};

template <class T> struct TypeTraits<Ref<T>> { 
=======
  static ArgumentType ConvertArgument (T *object) { return object; }
  static Ref<T> ConvertReturn(ReturnType value) { return Ref<ObjectType>(const_cast<ObjectType *> (value)); } // ERROR const cast
  static bool IsRef() { return false; }
};

template <class T> struct TypeTraits<Ref<T>> {
>>>>>>> prop-types: can now iterate over and compare layer props
  typedef Ref<T> Type;
  typedef Ref<T> ArgumentType;
  typedef Ref<const T> ReturnType;
  typedef T ObjectType;
<<<<<<< HEAD
  static Ref<T> ConvertArgument (T * object) { return Ref<T> (object); } 
  static Ref<T> ConvertReturn (Ref<const T> value) { return const_pointer_cast<T> (value); }
  static bool IsRef() { return true; }   
};

template <class T> struct TypeTraits<Ref<const T>> { 
=======
  static Ref<T> ConvertArgument (T * object) { return Ref<T> (object); }
  static Ref<T> ConvertReturn (Ref<const T> value) { return const_pointer_cast<T> (value); }
  static bool IsRef() { return true; }
};

template <class T> struct TypeTraits<Ref<const T>> {
>>>>>>> prop-types: can now iterate over and compare layer props
  typedef Ref<T> Type;
  typedef Ref<T> ArgumentType;
  typedef Ref<const T> ReturnType;
  typedef T ObjectType;
  static Ref<T> ConvertArgument (const T *object) { return const_pointer_cast<T> (object); }
  static Ref<T> ConvertReturn (Ref<const T> value) { return const_pointer_cast<T> (value); }
<<<<<<< HEAD
  static bool IsRef() { return true; }   
=======
  static bool IsRef() { return true; }
>>>>>>> prop-types: can now iterate over and compare layer props
};


template <class T>
  std::ostream &operator << (std::ostream &os, const std::list<T> &list)
{
  os << "{";
  for (auto item : list)
    os << *item;
  os << "}";
  return os;
}


template <class T>
class Box : public Value {
public:
  typedef Value inherited;
  typedef T Type;
  explicit Box (const T &value_) : value(value_) {}

  T value;

  static const struct Class : mutil::Class {
<<<<<<< HEAD
    Class (const Definition &def) : mutil::Class (def) {} 
=======
    Class (const Definition &def) : mutil::Class (def) {}
>>>>>>> prop-types: can now iterate over and compare layer props
    void WriteObject (const Object *obj, std::ostream &os, WriteContext &) const override
      { os << dynamic_cast<const Box<T> * >(obj)->value; }
  } class_;
  auto GetClass_ () const -> const mutil::Class * override { return &class_; }
};


// Use Box notation to box any native struct (you still need to define class)
template <class T>
  struct TypeTraits<Box<T>> : StructTraitsT <T, Box<T>> {};
/*
template <class T>
class ContainerBox : public Box<T> {
public:
  typedef Box<T> inherited;
  //typedef T Type;
  explicit ContainerBox (const T &value) : Box(value) {}

  static const struct Class : mutil::Class {
<<<<<<< HEAD
    Class (const Definition &def) : mutil::Class (def) {} 
=======
    Class (const Definition &def) : mutil::Class (def) {}
>>>>>>> prop-types: can now iterate over and compare layer props
    void WriteObject (const Object *obj, std::ostream &os, WriteContext &c) const override
      { WriteObjectList (os, c, "list", dynamic_cast<const Box<T> * >(obj)->value); }
  } class_;
  auto GetClass () const -> const mutil::Class * override { return &class_; }
};

// Use Box notation to box any native struct (you still need to define class)
template <class T>
  struct TypeTraits<ContainerBox<T>> : TypeTraitsT <T, ContainerBox<T>> {};
*/

template <class T>
  struct TypeTraits<std::list<T>> : StructTraitsT <std::list<Ref<RefObject>>, std::list<Ref<RefObject>>> {};


///////////////////////////////////////////////////////////
// PropertyClass

class Property;


class PropertyClass : public Class {
public:
  PropertyClass (const Definition &def) : Class(def) {}

  void WriteObject (const Object *, std::ostream &, WriteContext &) const override;
  void TraverseObject (Object *, TraverseObjectFunc traverseFunc) const override;

  void WriteProperties (std::ostream &,  WriteContext &, const Object *) const;
  const Class *GetBaseClass() const;
<<<<<<< HEAD
  
  mutable EmbeddedList<const Property> propertyList;    
=======

  mutable EmbeddedList<const Property> propertyList;
>>>>>>> prop-types: can now iterate over and compare layer props
};


///////////////////////////////////////////////////////////
/// \brief Property abstract base class

class Property : public EmbeddedList<const Property>::Node {
public:
  enum class Flag {
    None
  , Descriptive   // Property is especially descriptive, use in short form
  , ChildList    // Property contains list of children, recurse into it (long form)
  };
  // [selectors]
  std::string GetName() const { return name_; }
  Flag GetFlag() const { return flag_; }
  // TODO remove const_iterator begin() const {return const_iterator(this); }
<<<<<<< HEAD
          
=======

>>>>>>> prop-types: can now iterate over and compare layer props
  // [implementation]
  virtual const mutil::Class *GetClass() const = 0;
  virtual void Set(Object *object, Ref<RefObject> newValue) const = 0;
  virtual Ref<RefObject> Get(const Object *object) const = 0;
<<<<<<< HEAD
  virtual bool IsReadOnly() const { return false; } 
=======
  virtual bool IsReadOnly() const { return false; }
>>>>>>> prop-types: can now iterate over and compare layer props
  virtual bool IsReference() const { return false; }

protected:
  Property(PropertyClass *, const char *name, Flag flag);
  virtual ~Property() {}
<<<<<<< HEAD
    
=======

>>>>>>> prop-types: can now iterate over and compare layer props
private:
  // MU_NO_COPY(Property); TODO - enable once generalized initializers are supported
  const char *name_;
  Flag flag_;
};

inline Property::Property(PropertyClass *poc, const char *name, Flag flag)
  : EmbeddedList<const Property>::Node (poc->propertyList.front())
  , name_(name)
  , flag_(flag)
{
  poc->propertyList.set (this);
}

<<<<<<< HEAD
template <class T>  
=======
template <class T>
>>>>>>> prop-types: can now iterate over and compare layer props
  inline auto GetProperty(Ref<RefObject> object, const Property *property, T * = nullptr) -> Ref<const T>
{
  if (auto typedValue = dynamic_pointer_cast<const T> (property->Get(object.get())))
    return typedValue;
  return nullptr;
}

inline void SetProperty(Ref<RefObject> object, const Property *property, Ref<RefObject> value)
{
  property->Set(object.get(), value);
}

/* TODO - A bit confusing
inline const Class *GetClass (const Property *property)
{
  return property->GetClass();
}
*/




///////////////////////////////////////////////////////////
/// \brief Class in which can enumerate a list of direct children

/// Affects printing
/// - TODO - should affect construction

struct PolymorphicIterator : public RefObject {
  virtual bool equal (PolymorphicIterator *pi) const = 0;
  virtual bool done () const = 0;
  virtual void increment() = 0;
  virtual auto dereference() -> Ref<RefObject> = 0;
};

template <class WrapIter>
struct WrapIterator : public PolymorphicIterator {
  WrapIterator (WrapIter it, WrapIter end) : it_ {it}, end_ { end } {}
  bool equal (PolymorphicIterator *pi) const override
    { if (auto wi = dynamic_cast<WrapIterator<WrapIter> *> (pi)) { return it_ == wi->it_; } return false; }
  bool done () const override { return it_ == end_; }
  void increment() override { ++it_; }
  auto dereference() -> Ref<RefObject> override { return *it_; }
  WrapIter it_, end_;
};

/*
struct polymorphic_const_iterator
<<<<<<< HEAD
  : public boost::iterator_facade<polymorphic_const_iterator, Ref<const RefObject>, boost::forward_traversal_tag, Ref<const RefObject>> 
=======
  : public boost::iterator_facade<polymorphic_const_iterator, Ref<const RefObject>, boost::forward_traversal_tag, Ref<const RefObject>>
>>>>>>> prop-types: can now iterate over and compare layer props
{
  polymorphic_const_iterator(Ref<PolymorphicIterator> it) : it_(it) {}
  void increment() { it_->increment(); }
  bool equal(polymorphic_const_iterator const& other) const { return it_->equal(other.it_.get()); }
  Ref<const RefObject> dereference() const { return it_->dereference(); }
  Ref<PolymorphicIterator> it_;
};

struct polymorphic_iterator
<<<<<<< HEAD
  : public boost::iterator_facade<polymorphic_iterator, Ref<RefObject>, boost::forward_traversal_tag, Ref<RefObject>> 
=======
  : public boost::iterator_facade<polymorphic_iterator, Ref<RefObject>, boost::forward_traversal_tag, Ref<RefObject>>
>>>>>>> prop-types: can now iterate over and compare layer props
{
  polymorphic_iterator(Ref<PolymorphicIterator> it) : it_(it) {}
  void increment() { it_->increment(); }
  bool equal(polymorphic_iterator const& other) const { return it_->equal(other.it_.get()); }
  Ref<RefObject> dereference() const { return it_->dereference(); }
  Ref<PolymorphicIterator> it_;
};
*/

class ContainerClass : public PropertyClass {
public:
  ContainerClass(const Definition &def) : PropertyClass (def) {}
  void WriteObject (const Object *, std::ostream &, WriteContext &) const override;

  virtual Ref<PolymorphicIterator> Begin(const Object *) const = 0;
  /*
  using iterator = polymorphic_iterator;
  using const_iterator = polymorphic_const_iterator;
  auto begin(Object *obj) -> iterator { return iterator { Begin (obj) }; }
  auto begin(const Object *obj) const -> const_iterator { return const_iterator { Begin(obj) }; }
  auto end(Object *obj) -> iterator { return iterator { End (obj) }; }
  auto end(const Object *obj) const -> const_iterator { return const_iterator { End (obj) }; }

  virtual Ref<PolymorphicIterator> Begin(const Object *) const = 0;
  virtual Ref<PolymorphicIterator> End(const Object *) const = 0;
  */
};



/*
  struct ListIterator : public Iterator {
    ListIterator (std::list<Ref<RefObject>> &list_) : it (list_.begin()), end (list_.end()) {}
    bool equal(Iterator *it) const override { return it == end; }
    void increment() override { return *it_++; }
    auto dereference() override -> Ref<RefObject> { return *it_++; }
    std::list<Ref<RefObject>>::iterator it_;
  };
*/

///////////////////////////////////////////////////////////
// ListContainerClass

class ListContainerClass : public ContainerClass {
public:
  ListContainerClass(const Definition &def) : ContainerClass (def) {}
  virtual std::list<Ref<RefObject>> & GetList(Object *) const = 0;
  using iter = std::list<Ref<RefObject>>::iterator;
  Ref<PolymorphicIterator> Begin(const Object *object) const final
    { auto list = GetList(const_cast<Object *> (object)); return make_ref<WrapIterator<iter>> (list.begin(), list.end()); }
};



///////////////////////////////////////////////////////////
/// \brief Utility to write an attribute that is a list of RefObjects

<<<<<<< HEAD
template <class List> 
  void WriteObjectList (
    std::ostream &os, WriteContext &context, const char *itemName, const List &list)  
{ 
=======
template <class List>
  void WriteObjectList (
    std::ostream &os, WriteContext &context, const char *itemName, const List &list)
{
>>>>>>> prop-types: can now iterate over and compare layer props
  os << " " << itemName << "=( "; // << list.size() << " : ";

  if (context.recursion >= context.maxRecursion)
  {
    os << "... ";
  }
  else
  {
    context.recursion += 1;
    for (auto item : list)
    {
<<<<<<< HEAD
      if (context.indent) 
      {
        os << std::endl;    
        for (int i = 0; i < context.recursion; ++i)
          os << "  ";
      }
      
=======
      if (context.indent)
      {
        os << std::endl;
        for (int i = 0; i < context.recursion; ++i)
          os << "  ";
      }

>>>>>>> prop-types: can now iterate over and compare layer props
      WriteObject (item, os, context);
    }
    context.recursion -= 1;
  }
  os  << ")";
}




////////////////////////////////////////////////////////////
// Propery Adapters


/// \brief Base class for a Property on Object C

/// Main purpose is to reduce generated code in subclass instantiations

template <class C> class ObjectProperty : public Property {
public:
  // [construct/copy/destroy]
  MU_constexpr ObjectProperty (PropertyClass *poc, const char *name, Flag flag)
    : Property(poc, name, flag) {}
<<<<<<< HEAD
  
  // [implementation]
  void Set(Object *object_, Ref<RefObject> newValue) const override {
    if (auto object = dynamic_cast<C *> (object_))
      ObjectSet(object, newValue.get()); 
=======

  // [implementation]
  void Set(Object *object_, Ref<RefObject> newValue) const override {
    if (auto object = dynamic_cast<C *> (object_))
      ObjectSet(object, newValue.get());
>>>>>>> prop-types: can now iterate over and compare layer props
    else
      MU_THROW(XBadArgument("Property write on incompatible object", object));
  }
  Ref<RefObject> Get(const Object *object_) const override {
    if (auto object = dynamic_cast<const C *> (object_))
      return ObjectGet(object);
<<<<<<< HEAD
    MU_THROW(std::logic_error("Property read on incompatible object")); 
=======
    MU_THROW(std::logic_error("Property read on incompatible object"));
>>>>>>> prop-types: can now iterate over and compare layer props
  }
  // [avoids excessive dynamic_cast instantiations to save binary size]
  virtual void ObjectSet (C *, RefObject *value) const = 0;
  virtual Ref<RefObject> ObjectGet (const C *) const = 0;
};


/// \brief Base class for a Property whose value is an Object
template <class C, class T>
<<<<<<< HEAD
  class ValueProperty : public ObjectProperty<C> 
=======
  class ValueProperty : public ObjectProperty<C>
>>>>>>> prop-types: can now iterate over and compare layer props
{
public:
  // [types]
  typedef typename TypeTraits<T>::Type Type;
  typedef typename TypeTraits<T>::ObjectType ObjectType;
<<<<<<< HEAD
  typedef Type C::* MemberObjectPointer;   
=======
  typedef Type C::* MemberObjectPointer;
>>>>>>> prop-types: can now iterate over and compare layer props
  typedef Property::Flag Flag;


  // [construct/copy/destroy]
  MU_constexpr ValueProperty (PropertyClass *poc, const char *name,
      MemberObjectPointer pMember_, Flag flag = Flag::None)
    : ObjectProperty<C>(poc, name, flag), pMember(pMember_) {}

  // [implementation]
<<<<<<< HEAD
  const Class *GetClass() const // override 
    { return &ObjectType::class_; }
  bool IsReference() const // override 
    { return TypeTraits<T>::IsRef(); }
  Ref<RefObject> ObjectGet(const C *object) const // override 
    { return TypeTraits<T>::ConvertReturn(object->*pMember); }
  void ObjectSet(C *object, RefObject *newValue_) const // override
  {
    if (ObjectType *newValue = dynamic_cast<ObjectType *> (newValue_)) 
=======
  const Class *GetClass() const // override
    { return &ObjectType::class_; }
  bool IsReference() const // override
    { return TypeTraits<T>::IsRef(); }
  Ref<RefObject> ObjectGet(const C *object) const // override
    { return TypeTraits<T>::ConvertReturn(object->*pMember); }
  void ObjectSet(C *object, RefObject *newValue_) const // override
  {
    if (ObjectType *newValue = dynamic_cast<ObjectType *> (newValue_))
>>>>>>> prop-types: can now iterate over and compare layer props
      object->*pMember = TypeTraits<T>::ConvertArgument(newValue);
    Throw(XBadArgument("Property write on incompatible object", newValue_));
  }

protected:
  MemberObjectPointer pMember;
};


/*
template <class C, class MemberObjectPointer>
<<<<<<< HEAD
  class ValueProperty2 : public ValueProperty<C, decltype(((Object *)nullptr)->*MemberObjectPointer);> 
{
public:
  static_assert(std::is_member_object_pointer(MemberPointer), "Supplied type not a member pointer"); 
  
=======
  class ValueProperty2 : public ValueProperty<C, decltype(((Object *)nullptr)->*MemberObjectPointer);>
{
public:
  static_assert(std::is_member_object_pointer(MemberPointer), "Supplied type not a member pointer");

>>>>>>> prop-types: can now iterate over and compare layer props
  // [types]
  typedef decltype(((Object *)nullptr)->*MemberObjectPointer) PrimType;
  typedef typename TypeTraits<T>::Type Type;
  typedef typename TypeTraits<T>::ObjectType  ObjectType;
<<<<<<< HEAD
  
  typedef Type C::* MemberObjectPointer;   
=======

  typedef Type C::* MemberObjectPointer;
>>>>>>> prop-types: can now iterate over and compare layer props

  // [construct/copy/destroy]
  ValueProperty2 (PropertyObject::Class *poc, const char *name)
    : ObjectProperty<C>(poc, name), pMember(pMember_) {}

  // [implementation]
<<<<<<< HEAD
  const Class *GetClass() const // override 
    { return &ObjectType::class_; }
  bool IsReference() const // override 
    { return TypeTraits<T>::IsRef(); }
  Ref<RefObject> ObjectGet(const C *object) const // override 
    { return TypeTraits<T>::ConvertReturn(object->*pMember); }
  void ObjectSet(C *object, RefObject *newValue_) // override 
  {
    if (ObjectType *newValue = dynamic_cast<ObjectType *> (newValue_)) 
=======
  const Class *GetClass() const // override
    { return &ObjectType::class_; }
  bool IsReference() const // override
    { return TypeTraits<T>::IsRef(); }
  Ref<RefObject> ObjectGet(const C *object) const // override
    { return TypeTraits<T>::ConvertReturn(object->*pMember); }
  void ObjectSet(C *object, RefObject *newValue_) // override
  {
    if (ObjectType *newValue = dynamic_cast<ObjectType *> (newValue_))
>>>>>>> prop-types: can now iterate over and compare layer props
      object->*pMember = TypeTraits<T>::ConvertArgument(newValue);
    Throw(XBadArgument("Property write on incompatible object", newValue_));
  }
};
*/


/// \brief Property with value is a pointer - ERROR why can't this be handled by above case?
template <class C, class T>
class PointerProperty : public Property {
public:
<<<<<<< HEAD
  typedef T * C::* MemberPointer;   
   
=======
  typedef T * C::* MemberPointer;

>>>>>>> prop-types: can now iterate over and compare layer props
  // [construct/copy/destroy]
  PointerProperty (PropertyClass *poc, const char *name,
      MemberPointer pMember_, Flag flag = Flag::None)
    : Property(poc, name, flag), pMember(pMember_)
    { C::class_.propertyList.push_front (this); }
  // [implementation]
  const Class *GetClass() const override
    { return &Pointer::class_; }
  void Set(Object *, Ref<RefObject>) const override
    { throw std::logic_error("Can't set native pointer property"); }
  Ref<RefObject> Get(const Object *object_) const override
<<<<<<< HEAD
    { 
      if (const C *object = dynamic_cast<const C *> (object_)) 
        return make_ref<Pointer> (object->*pMember);
      throw XBadArgument("Property read on incompatible object"); 
    }

protected:
  MemberPointer pMember; 
=======
    {
      if (const C *object = dynamic_cast<const C *> (object_))
        return make_ref<Pointer> (object->*pMember);
      throw XBadArgument("Property read on incompatible object");
    }

protected:
  MemberPointer pMember;
>>>>>>> prop-types: can now iterate over and compare layer props
};


/// \brief Property that is implemented through accessor functions
template <class C, class T, class ValueT = typename TypeTraits<T>::ObjectType>
class SyntheticProperty : public ObjectProperty<C> {
public:
  typedef typename TypeTraits<T>::ArgumentType ArgumentType;
  typedef typename TypeTraits<T>::ReturnType ReturnType;
  typedef typename TypeTraits<T>::ObjectType ObjectType;
<<<<<<< HEAD
  
  typedef ReturnType (C::*MemberPointerGet) () const;
  typedef void (C::*MemberPointerSet) (ArgumentType);
  typedef Property::Flag Flag;
  
=======

  typedef ReturnType (C::*MemberPointerGet) () const;
  typedef void (C::*MemberPointerSet) (ArgumentType);
  typedef Property::Flag Flag;

>>>>>>> prop-types: can now iterate over and compare layer props
  // [construct/copy/destroy]
  MU_constexpr SyntheticProperty (PropertyClass *poc, const char *name,
      MemberPointerGet pMemberGet_, MemberPointerSet pMemberSet_,
      Flag flag = Flag::None)
    : ObjectProperty<C> (poc, name, flag)
    , pMemberGet(pMemberGet_), pMemberSet(pMemberSet_)
    {}

  // [implementation]
  const Class *GetClass() const // override
    { return &ObjectType::class_; }
<<<<<<< HEAD
  bool IsReadOnly() const // override 
    { return pMemberSet == 0; } 
  Ref<RefObject> ObjectGet(const C *object) const // override 
  { 
    if (pMemberGet == nullptr) 
      MU_THROW(XBadArgument("Attempt to read write-only propery", object)); 
    return TypeTraits<T>::ConvertReturn((object->*pMemberGet) ()); // ERROR
  }
  void ObjectSet(C *object, RefObject * value) const // override
  { 
    if (pMemberSet != 0) 
      MU_THROW(XBadArgument("Attempt to write read-only property", object));
    ObjectType *newValue = dynamic_cast<ObjectType *> (value);
    if (newValue == nullptr)
      MU_THROW(XBadArgument("Attempt to write property with incompatible type", value)); 
    (object->*pMemberSet) (TypeTraits<T>::ConvertArgument(newValue));
  }
  
protected:
  MemberPointerGet pMemberGet; 
  MemberPointerSet pMemberSet; 
=======
  bool IsReadOnly() const // override
    { return pMemberSet == 0; }
  Ref<RefObject> ObjectGet(const C *object) const // override
  {
    if (pMemberGet == nullptr)
      MU_THROW(XBadArgument("Attempt to read write-only propery", object));
    return TypeTraits<T>::ConvertReturn((object->*pMemberGet) ()); // ERROR
  }
  void ObjectSet(C *object, RefObject * value) const // override
  {
    if (pMemberSet != 0)
      MU_THROW(XBadArgument("Attempt to write read-only property", object));
    ObjectType *newValue = dynamic_cast<ObjectType *> (value);
    if (newValue == nullptr)
      MU_THROW(XBadArgument("Attempt to write property with incompatible type", value));
    (object->*pMemberSet) (TypeTraits<T>::ConvertArgument(newValue));
  }

protected:
  MemberPointerGet pMemberGet;
  MemberPointerSet pMemberSet;
>>>>>>> prop-types: can now iterate over and compare layer props
};



/// \brief Animated property that is implemented through accessor functions
template <class C, class T, class ValueT = typename TypeTraits<T>::ObjectType>
class AnimatedProperty : public ObjectProperty<C> {
public:
  typedef typename TypeTraits<T>::ArgumentType ArgumentType;
  typedef typename TypeTraits<T>::ReturnType ReturnType;
  typedef typename TypeTraits<T>::ObjectType ObjectType;
<<<<<<< HEAD
  
  typedef ReturnType (C::*MemberPointerGet) () const;
  typedef void (C::*MemberPointerSet) (ArgumentType, bool animate);
  typedef Property::Flag Flag;
   
=======

  typedef ReturnType (C::*MemberPointerGet) () const;
  typedef void (C::*MemberPointerSet) (ArgumentType, bool animate);
  typedef Property::Flag Flag;

>>>>>>> prop-types: can now iterate over and compare layer props
  // [construct/copy/destroy]
  AnimatedProperty (PropertyClass *poc, const char *name,
    MemberPointerGet pMemberGet_, MemberPointerSet pMemberSet_, Flag flag = Flag::None)
    : ObjectProperty<C> (poc, name, flag)
    , pMemberGet(pMemberGet_), pMemberSet(pMemberSet_)
  {}

  // [implementation]
  const Class *GetClass() const // override
    { return &ObjectType::class_; }
<<<<<<< HEAD
  bool IsReadOnly() const // override 
    { return pMemberSet == 0; } 
  Ref<RefObject> ObjectGet(const C *object) const // override 
  { 
    if (pMemberGet == nullptr) 
      MU_THROW(XBadArgument("Attempt to read write-only propery", object)); 
    return TypeTraits<T>::ConvertReturn((object->*pMemberGet) ()); // TODO ERROR
  }
  void ObjectSet(C *object, RefObject * value) const // override
  { 
    if (pMemberSet != 0) 
      MU_THROW(XBadArgument("Attempt to write read-only property", object));
    ObjectType *newValue = dynamic_cast<ObjectType *> (value);
    if (newValue == nullptr)
      MU_THROW(XBadArgument("Attempt to write property with incompatible type", value)); 
    (object->*pMemberSet) (TypeTraits<T>::ConvertArgument(newValue), true);
  }
  void ObjectSetAnimated(C *object, RefObject * value, bool animate) const // override
  { 
    if (pMemberSet != 0) 
      MU_THROW(XBadArgument("Attempt to write read-only property", object));
    ObjectType *newValue = dynamic_cast<ObjectType *> (value);
    if (newValue == nullptr)
      MU_THROW(XBadArgument("Attempt to write property with incompatible type", value)); 
    (object->*pMemberSet) (TypeTraits<T>::ConvertArgument(newValue), animate);
  }
  
protected:
  MemberPointerGet pMemberGet; 
  MemberPointerSet pMemberSet; 
=======
  bool IsReadOnly() const // override
    { return pMemberSet == 0; }
  Ref<RefObject> ObjectGet(const C *object) const // override
  {
    if (pMemberGet == nullptr)
      MU_THROW(XBadArgument("Attempt to read write-only propery", object));
    return TypeTraits<T>::ConvertReturn((object->*pMemberGet) ()); // TODO ERROR
  }
  void ObjectSet(C *object, RefObject * value) const // override
  {
    if (pMemberSet != 0)
      MU_THROW(XBadArgument("Attempt to write read-only property", object));
    ObjectType *newValue = dynamic_cast<ObjectType *> (value);
    if (newValue == nullptr)
      MU_THROW(XBadArgument("Attempt to write property with incompatible type", value));
    (object->*pMemberSet) (TypeTraits<T>::ConvertArgument(newValue), true);
  }
  void ObjectSetAnimated(C *object, RefObject * value, bool animate) const // override
  {
    if (pMemberSet != 0)
      MU_THROW(XBadArgument("Attempt to write read-only property", object));
    ObjectType *newValue = dynamic_cast<ObjectType *> (value);
    if (newValue == nullptr)
      MU_THROW(XBadArgument("Attempt to write property with incompatible type", value));
    (object->*pMemberSet) (TypeTraits<T>::ConvertArgument(newValue), animate);
  }

protected:
  MemberPointerGet pMemberGet;
  MemberPointerSet pMemberSet;
>>>>>>> prop-types: can now iterate over and compare layer props
};



/*
// TODO - is this needed?
template <class C, class T, class ValueT = typename TypeTraits<T>::ObjectType>
class SyntheticRefProperty : public ObjectProperty<C> {
public:
  typedef typename TypeTraits<T>::ArgumentType ArgumentType;
  typedef typename TypeTraits<T>::ReturnType ReturnType;
  typedef typename TypeTraits<T>::ObjectType ObjectType;
<<<<<<< HEAD
  
  typedef ReturnType (C::*MemberPointerGet) () ;   
  typedef void (C::*MemberPointerSet) (ArgumentType) ;   
   
  // [construct/copy/destroy]
  SyntheticRefProperty (PropertyClass *poc, const char *name, MemberPointerGet pMemberGet_, MemberPointerSet pMemberSet_)
    : ObjectProperty<C> (poc, name), pMemberGet(pMemberGet_), pMemberSet(pMemberSet_) 
=======

  typedef ReturnType (C::*MemberPointerGet) () ;
  typedef void (C::*MemberPointerSet) (ArgumentType) ;

  // [construct/copy/destroy]
  SyntheticRefProperty (PropertyClass *poc, const char *name, MemberPointerGet pMemberGet_, MemberPointerSet pMemberSet_)
    : ObjectProperty<C> (poc, name), pMemberGet(pMemberGet_), pMemberSet(pMemberSet_)
>>>>>>> prop-types: can now iterate over and compare layer props
    { C::class_.propertyList.push_front (this); }

  // [implementation]
  const Class *GetClass() const // override
    { return &ObjectType::class_; }
<<<<<<< HEAD
  bool IsReadOnly() const // override 
    { return pMemberSet == 0; } 
  Ref<RefObject> ObjectGet(const C *object) const // override 
  { 
    if (pMemberGet == nullptr) 
      MU_THROW(XBadArgument("Attempt to read write-only propery", object)); 
    return TypeTraits<T>::ConvertReturn((const_cast<C *> (object)->*pMemberGet) ()); // ERROR
  }
  void ObjectSet(C *object, RefObject * value) const // override
  { 
    if (pMemberSet != 0) 
      MU_THROW(XBadArgument("Attempt to write read-only property", object));
    ObjectType *newValue = dynamic_cast<ObjectType *> (value);
    if (newValue == nullptr)
      MU_THROW(XBadArgument("Attempt to write property with incompatible type", value)); 
    (object->*pMemberSet) (TypeTraits<T>::ConvertArgument(newValue));
  }
  
protected:
  MemberPointerGet pMemberGet; 
  MemberPointerSet pMemberSet; 
=======
  bool IsReadOnly() const // override
    { return pMemberSet == 0; }
  Ref<RefObject> ObjectGet(const C *object) const // override
  {
    if (pMemberGet == nullptr)
      MU_THROW(XBadArgument("Attempt to read write-only propery", object));
    return TypeTraits<T>::ConvertReturn((const_cast<C *> (object)->*pMemberGet) ()); // ERROR
  }
  void ObjectSet(C *object, RefObject * value) const // override
  {
    if (pMemberSet != 0)
      MU_THROW(XBadArgument("Attempt to write read-only property", object));
    ObjectType *newValue = dynamic_cast<ObjectType *> (value);
    if (newValue == nullptr)
      MU_THROW(XBadArgument("Attempt to write property with incompatible type", value));
    (object->*pMemberSet) (TypeTraits<T>::ConvertArgument(newValue));
  }

protected:
  MemberPointerGet pMemberGet;
  MemberPointerSet pMemberSet;
>>>>>>> prop-types: can now iterate over and compare layer props
};
*/


/*
///////////////////////////////////////////////////////////
// Intended use
//  : constructor ()
//  insertElement ("insert-element", "element")
//Method1<C, InsertElement> insertElement;
//Method2

template <class T>
  typedef TypeTraits<T> TypeMap;


class Method : public Property {
public:
  const char *name;
  int args;
//  virtual void Invoke (Object *object, Params &args, Returns &returns) = 0;
};

<<<<<<< HEAD
template<typename C, typename Return, typename Param1, typename Param2> 
class Method2 : public Method {
public:
  typedef Return (C::*) (Param1, Param2) MethodPointer;
  Method2(MethodPointer pM, const char *name, const char *param1, const char *param2) : param1(param1), param2(param2) {} 
=======
template<typename C, typename Return, typename Param1, typename Param2>
class Method2 : public Method {
public:
  typedef Return (C::*) (Param1, Param2) MethodPointer;
  Method2(MethodPointer pM, const char *name, const char *param1, const char *param2) : param1(param1), param2(param2) {}
>>>>>>> prop-types: can now iterate over and compare layer props
//  void Invoke (Object *object, Params &Params, Returns &returns) {
//    Ref<RefObject> param1 = params.Pop<TypeMap<Param1>::ObjectType> (param1);
//    Ref<RefObject> param2 = params.Pop<TypeMap<Param2>::ObjectType> (param2);
//    params.End();
<<<<<<< HEAD
//    returns.Add<TypeMap<Return>::ObjectType>(Get<C>(object)->pMethodPointer_(param1, param2)); 
//  }
    
=======
//    returns.Add<TypeMap<Return>::ObjectType>(Get<C>(object)->pMethodPointer_(param1, param2));
//  }

>>>>>>> prop-types: can now iterate over and compare layer props
private:
  MethodPointer pMethodPointer_;
  const char *name;
  const char *param1;
  const char *param2;
};

*/




} //  mutil


///////////////////////////////////////////////////////////
// Class macros

# define MU_DECLARE_CLASS(TYPE, NAME) \
    public: \
    static const struct Class : mutil::Class { \
      Class () : mutil::Class (Definition ( \
        namespace_, (NAME), &inherited::class_, typeid(TYPE), sizeof (TYPE), sizeof (TYPE::Class))) {} \
    } class_; \
    const mutil::Class *GetClass_ () const override { return &class_; }

# define MU_BEGIN_PROPERTY_CLASS(TYPE, NAME) \
    public: \
    struct Class : PropertyClass { \
      typedef TYPE T; \
      Class () : PropertyClass (DefinitionT<TYPE> (namespace_, (NAME))) {}
// using namespace mutil;
//  template <typename T>
//    static auto ConvertList (std::list<Ref<T>> *list, RefObject * =  (T *) nullptr) -> std::list<Ref<RefObject>> *
//    { return reinterpret_cast<std::list<Ref<RefObject>> *> (list); }
# define MU_BEGIN_LIST_CONTAINER_CLASS(TYPE, NAME, LIST) \
    public: \
    struct Class : ListContainerClass { \
      typedef TYPE T; \
      Class () : ListContainerClass (DefinitionT<TYPE> (namespace_, (NAME))) {} \
      std::list<Ref<RefObject>> & GetList(Object *object) const override \
        { return ConvertList (((TYPE *) object)->LIST); }


# define MU_VALUE_PROPERTY(TYPE,VARIABLE,NAME,VALUE) \
    ValueProperty<T, TYPE> VARIABLE {this, NAME, VALUE};

# define MU_SYNTHETIC_PROPERTY(TYPE,VARIABLE,NAME,GETTER,SETTER) \
    SyntheticProperty<T, TYPE> VARIABLE {this, NAME, GETTER, SETTER};
# define MU_SYNTHETIC_PROPERTYF(TYPE,VARIABLE,NAME,GETTER,SETTER,FLAG) \
    SyntheticProperty<T, TYPE> VARIABLE {this, NAME, GETTER, SETTER, FLAG};

# define MU_ANIMATED_PROPERTY(TYPE,VARIABLE,NAME,GETTER,SETTER) \
    AnimatedProperty<T, TYPE> VARIABLE {this, NAME, GETTER, SETTER};

# define MU_END_CLASS() \
    }; \
    static const Class class_; \
    const PropertyClass *GetClass_ () const override { return &class_; }
<<<<<<< HEAD
  
=======

>>>>>>> prop-types: can now iterate over and compare layer props
# define MU_DEFINE_CLASS(TYPE) \
    const TYPE::Class TYPE::class_

# define MU_DEFINE_CLASS2(NAMESPACE, TYPE, NAME) \
<<<<<<< HEAD
    const TYPE::Class TYPE::class_ = Class::DefinitionT<TYPE> (NAMESPACE, NAME) 
=======
    const TYPE::Class TYPE::class_ = Class::DefinitionT<TYPE> (NAMESPACE, NAME)
>>>>>>> prop-types: can now iterate over and compare layer props


///////////////////////////////////////////////////////////
// Class macros NEW

# define MU_FORWARD_CLASS () \
    public: \
    struct Class; \
    static Class class_; \
    const mutil::Class *GetClass_ () const override { return &class_; }

# define MU_DEFINE_SIMPLE_CLASS(TYPE, NAME) \
    struct TYPE::Class : mutil::Class { \
      Class () : mutil::Class (Definition ( \
        namespace_, (NAME), &inherited::class_, typeid(TYPE), sizeof (TYPE), sizeof (TYPE::Class))) {} \
    } class_; \
    const mutil::Class *TYPE::GetClass_ () const override { return &class_; }

# define MU_BEGIN_PROPERTY_CLASS_EXT(TYPE, NAME) \
    struct TYPE::Class : PropertyClass { \
      typedef TYPE T; \
      Class () : PropertyClass (DefinitionT<TYPE> (namespace_, (NAME))) {}
<<<<<<< HEAD
 
=======

>>>>>>> prop-types: can now iterate over and compare layer props
# define MU_BEGIN_LIST_CONTAINER_CLASS_EXT(TYPE, NAME) \
    struct TYPE::Class : ListContainerClass { \
      typedef TYPE T; \
      Class () : ListContainerClass (DefinitionT<TYPE> (namespace_, (NAME))) {} \
      std::list<Ref<RefObject>> & GetList(Object *object) const override \
        { return reinterpret_cast<std::list<Ref<RefObject>> &> (LIST); }

# define MU_END_CLASS_EXT() \
    }; \
    const PropertyClass *TYPE::GetClass_ () const override { return &class_; }
<<<<<<< HEAD
  
=======

>>>>>>> prop-types: can now iterate over and compare layer props
/*
# define MU_DEFINE_CLASS(TYPE) \
    const TYPE::Class TYPE::class_

# define MU_DEFINE_CLASS2(NAMESPACE, TYPE, NAME) \
<<<<<<< HEAD
    const TYPE::Class TYPE::class_ = Class::DefinitionT<TYPE> (NAMESPACE, NAME) 
=======
    const TYPE::Class TYPE::class_ = Class::DefinitionT<TYPE> (NAMESPACE, NAME)
>>>>>>> prop-types: can now iterate over and compare layer props
*/

///////////////////////////////////////////////////////////
// Enum macros

# define MU_DECLARE_ENUM(NAMESPACE,ENUM,NATIVE_TYPE) \
  template<> struct mutil::EnumTraits<NAMESPACE::ENUM> { typedef NATIVE_TYPE Type; }; \

# define MU_DECLARE_ENUMX(NAMESPACE,ENUM,NATIVE_TYPE) \
  } \
  MU_DECLARE_ENUM(NAMESPACE,ENUM,NATIVE_TYPE) \
  namespace NAMESPACE {


# define MU_DEFINE_ENUM(NAMESPACE,ENUM,NAME,MAP_NAME) \
  template<> const mutil::Enum<ENUM>::Class mutil::Enum<ENUM>::class_ ( \
    NAMESPACE::namespace_, NAME, MAP_NAME, numberof(MAP_NAME)); \

# define MU_DEFINE_ENUMX(NAMESPACE,ENUM,NAME,MAP_NAME) \
  } \
  MU_DEFINE_ENUM(NAMESPACE,ENUM,NAME,MAP_NAME) \
  namespace NAMESPACE {


# define MU_ENUM_BEGIN(MAP_NAME) \
  static Enumeration::Class::Map enumMap_##MAP_NAME[] = {

# define MU_ENUM_END(NAMESPACE,ENUM,NAME,NATIVE_TYPE,MAP_NAME) \
  }; \
  MU_DEFINE_ENUM(NAMESPACE,ENUM,NAME,enumMap_##MAP_NAME) \
  MU_DECLARE_ENUM(NAMESPACE,ENUM,NATIVE_TYPE)

# define MU_ENUM_ENDX(NAMESPACE,ENUM,NAME,NATIVE_TYPE,MAP_NAME) \
  }; \
  } \
  MU_DEFINE_ENUM(NAMESPACE,ENUM,NAME,enumMap_##MAP_NAME)); \
  MU_DECLARE_ENUM(NAMESPACE,ENUM,NATIVE_TYPE) \
  namespace NAMESPACE {

# define MU_ENUM1(STRING,ENUM,NATIVE) { STRING, (int) ENUM, (int) NATIVE }
# define MU_ENUM2(STRING,ENUM,NATIVE) , { STRING, (int) ENUM, (int) NATIVE }

#endif // CLASS_REFLECTION
