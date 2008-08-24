#define SELECTOR_START(name) template<class List> struct name##_selector_struct { 



#define SELECTOR_ERROR(name) }; template<> struct name##_selector_struct<PyCVD::End> {

#define SELECTOR_END };


#define SELECTOR_FUNC_START()  typedef typename List::type type; \
                               typedef typename List::next next;


#define SELECTOR_NEXT(name) name##_selector_struct<next>::fun

#define SELECTOR_INVOKE(name, types) name##_selector_struct<types>::fun

#define SELECTOR_INVOKE_FLOATS(name, types) SELECTOR_INVOKE(name, CVDFloatTypes)
