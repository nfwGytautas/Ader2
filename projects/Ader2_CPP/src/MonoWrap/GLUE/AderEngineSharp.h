#pragma once

#include "MonoWrap/MonoManager.h"

/**
 * C++ representation of the AderEngine.cs
 */
struct AderEngineSharp
{
    /// Class of ader engine
    Memory::reference<SharpClass> Klass;

    /// _wndState field
    Memory::reference<SharpField> FieldWndState;

    /// _keyState field
    Memory::reference<SharpField> FieldKeyState;
};

/**
 * C++ representation of the AssetManager.cs
 */
struct AderAssetsSharp
{
    /// Class of ader asset manager
    Memory::reference<SharpClass> Klass;

    /// _CInstance field
    Memory::reference<SharpField> _CInstance;
};
