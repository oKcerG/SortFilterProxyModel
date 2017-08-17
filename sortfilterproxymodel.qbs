import qbs 1.0
import qbs.File
import qbs.FileInfo

Product
{
    name: "SortFilterProxyModel"
    type: ["dynamiclibrary", "copied qbs resources"]
    targetName: "sortfilterproxymodel"

    destinationDirectory: project.deployDirectory + "/" + name

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
            "qqmlsortfilterproxymodel_plugin.cpp",
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
            "qqmlsortfilterproxymodelexport.h",
            "qqmlsortfilterproxymodel_plugin.h",
            "sorter.h"
        ]
    }

    Group
    {
        name: "Other files"
        files:
        [
            "qmldir"
        ]
        fileTags: "qbs resources"
    }

    //FIXME: Replace this to actually install files in the right location
    Rule
    {
        inputs:
        [
            "qbs resources"
        ]

        Artifact
        {
            filePath: FileInfo.joinPaths(product.destinationDirectory,
                                         "qmldir")
            fileTags:
            [
                "copied qbs resources"
            ]
        }

        prepare:
        {
            var cmd = new JavaScriptCommand()

            cmd.description = "Copying " + input.fileName + " to " + output.filePath
            cmd.highlight = "codegen"
            cmd.sourceCode = function()
            {
                File.copy(input.filePath,
                          output.filePath)
            }

            return cmd
        }
    }
}
