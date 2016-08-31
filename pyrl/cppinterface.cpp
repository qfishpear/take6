#include "Python.h"

int main()
{
    Py_Initialize();

    if (!Py_IsInitialized())
    {
        return -1;
    }


    if (PyRun_SimpleString("execfile('./cppinterface.py')") == NULL)
    {
        return -1;
    }

    Py_Finalize();
    return 0;
}

