# distutils: language=c++
cimport cython
from libcpp cimport bool
from libcpp.string cimport string


cdef extern from *:
    """
    #include <stdio.h>

    typedef struct {
        double priority;
        int count;
        std::string node;
        double dist;
        std::string parent;
        std::string key;

    } queueItem;

    bool operator<(const queueItem t1, const queueItem t2) {
        return t1.priority > t2.priority;
    }
    """
    ctypedef struct queueItem:
        double priority
        int count
        string node
        double dist
        string parent
        string key
