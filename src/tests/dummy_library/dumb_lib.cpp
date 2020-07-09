/*
Dis be a dummy library for RLL's test framework.
*/

#ifdef WIN32
    #define API_EXPORT __declspec(dllexport)
#else 
    #define API_EXPORT 
#endif

extern "C" {

API_EXPORT int add(int a, int b) {
    return a + b;
}

API_EXPORT extern const char abc[4] = "abc";

}
