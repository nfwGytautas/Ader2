#pragma once

#include "MonoWrap/MonoManager.h"

/**
 * C++ representation of the AderEngine.cs
 */
struct AderEngineSharp
{
    /// Class of ader engines
    Memory::reference<SharpClass> Klass;

    /// _wndState field
    Memory::reference<SharpField> FieldWndState;

    /// _keyState field
    Memory::reference<SharpField> FieldKeyState;
};
