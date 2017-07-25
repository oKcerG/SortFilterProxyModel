import qbs 1.0

Product
{
    name: "sortfilterproxymodel"
    type: "dynamiclibrary"
    targetName: "sortfilterproxymodel"

    destinationDirectory: project.deployDirectory

    readonly property stringList qtModules:
    [
        "core",
        "qml",
        "gui"
    ]

    cpp.defines:
    {
        var defines = []
        
        defines.push('QT_NO_KEYWORDS')
        defines.push('SORT_FILTER_PROXY_MODEL_LIBRARY_BUILD')

        if (qbs.buildVariant === "release")
        {
            defines.push('QT_NO_DEBUG_OUTPUT=1')
        }
        
        return defines
    }

    cpp.includePaths: [ "." ]
    
    Depends { name: "cpp" }
    Depends { name: "Qt"; submodules: product.qtModules }

    Export
    {
        cpp.defines:
        {
            var defines = []
            
            defines.push('QT_NO_KEYWORDS')
            
            return defines
        }

        cpp.includePaths: [ "." ]

        Depends { name: "cpp" }
        Depends { name: "Qt"; submodules: product.qtModules }
    }

    Group
    {
        name: "C++ Sources"
        files:
        [
            "filter.cpp",
            "qqmlsortfilterproxymodel.cpp",
            "sorter.cpp"
        ]
    }

    Group
    {
        name: "C++ Headers"
        files:
        [
            "filter.h",
            "qqmlsortfilterproxymodel.h",
            "sorter.h"
        ]
    }    
}
