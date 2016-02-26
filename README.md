SortFilterProxyModel
====================

SortFilterProxyModel is an implementation of `QSortFilterProxyModel` conveniently exposed for QML.

Install
-------
##### With [qpm](https://qpm.io) :
1. `qpm install fr.grecko.sortfilterproxymodel`
2. add `include(vendor/vendor.pri)` in your .pro if it is not already done
3. `import SortFilterProxyModel 0.1` to use this library in your QML files

##### Without qpm :
1. clone or download this repository
2. add `include  (<path/to/SortFilterProxyModel>/SortFilterProxyModel.pri)` in your `.pro`
3. `import SortFilterProxyModel 0.1` to use this library in your QML files

Sample Usage
------------
```qml
import QtQuick 2.2
import QtQuick.Controls 1.2
import SortFilterProxyModel 0.1

ApplicationWindow {
    visible: true
    width: 640
    height: 480

    ListModel {
        id: personModel
        ListElement {
            firstName: "Erwan"
            lastName: "Castex"
        }
        // ...
    }

    TextField {
        id: textField
        anchors { top: parent.top; left: parent.left; right: parent.right }
        height: implicitHeight
    }

    SortFilterProxyModel {
        id: filteredPersonModel
        sourceModel: personModel
        filterRoleName: "lastName"
        filterPattern: textField.text
        filterCaseSensitivity: Qt.CaseInsensitive
    }

    ListView {
        anchors { top: textField.bottom; bottom: parent.bottom; left: parent.left; right: parent.right }
        model: filteredPersonModel
        delegate: Text { text: firstName + " " + lastName}
    }
}
```
Here the `ListView` will only show elements that contains the content of the `TextField` in their `lastName` role.

License
-------
This library is licensed under the MIT License.

Documentation
-------------
This component is a subclass of [`QSortFilterProxyModel`](http://doc.qt.io/qt-5/qsortfilterproxymodel.html), to use it, you need to set the `sourceModel` property to a [`QAbstractItemModel*`](http://doc.qt.io/qt-5/qabstractitemmodel.html) with correct role names.
This means you can use it with custom c++ models or `ListModel`, but not with JavaScript models like arrays, integers or object instances.

You can then define how the source model is filtered or sorted by setting the different provided properties.  
By default the source model is not sorted or filtered by the proxy model.

When filtering, `filterValue` is taken in account first, then `filterPattern` and eventually `filterExpression` if it's a valid expression.
This means you can combine different filtering methods. Prefer using `filterValue` or `filterPattern` when possible, `filterExpression` is slower.  
When sorting, the sort is done with `sortExpression` if it's a valid expression, otherwise it is done with `sortRoleName` in combination with `sortOrder`.

#### Properties
<li>
[__`sourceModel`__](http://doc.qt.io/qt-5/qabstractproxymodel.html#sourceModel-prop) : _`QAbstractItemModel*`_  
This property holds the source model of this proxy model.
</li>

<li>
__`filterRoleName`__ : _string_  
The role name of the source model's data on which `filterPattern` and `filterValue` are tested against.
</li>

<li>
__`filterValue`__ : _any_  
Only contents with their `filterRoleName` data matching exactly this value (or that can be converted to this value) are available.
</li>

<li>
__`filterPattern`__ : _string_  
The pattern used to filter the contents of the source model.
Only the source model's value having their `filterRoleName` data matching this `filterPatern` with the specified `filterPatternSyntax` will be available.
</li>

<li>
__`filterPatternSyntax`__ : _enumeration_  
This property represents the syntax of the pattern used to filter the contents of the source model.
The possible values are mirrored from [`QRegExp::PatternSyntax`](http://doc.qt.io/qt-5/qregexp.html#PatternSyntax-enum) :

| Value                                 | Description                                                                                                                       |
|---------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------|
| `SortFilterProxyModel.RegExp`         |	A rich Perl-like pattern matching syntax. __This is the default__                                                                |
| `SortFilterProxyModel.RegExp2`        |	Like RegExp, but with greedy quantifiers                                                                                          |
| `SortFilterProxyModel.Wildcard`       |	This provides a simple pattern matching syntax similar to that used by shells (command interpreters) for "file globbing"          |
| `SortFilterProxyModel.WildcardUnix`   |	This is similar to Wildcard but with the behavior of a Unix shell. The wildcard characters can be escaped with the character "\"  |
| `SortFilterProxyModel.FixedString`    |	The pattern is a fixed string. This is equivalent to using the RegExp pattern on a string in which all metacharacters are escaped |
| `SortFilterProxyModel.W3CXmlSchema11` |	The pattern is a regular expression as defined by the W3C XML Schema 1.1 specification                                            |
</li>

<li>
__`filterCaseSensitivity`__ : _enumeration_  
This property holds the case sensitivity of the pattern used to filter the contents of the source model.  Can be `Qt.CaseSensitive` or `Qt.CaseInsensitive`.  
This property only affects the filtering made with `filterPattern`, it does not affect `filterValue` or `filterExpression`.  
By default, the filter is case sensitive.
</li>

<li>
__`filterExpression`__ : _expression_  
An expression to implement custom filtering, it must evaluate to a boolean.  
It has the same syntax has a [Property Binding](http://doc.qt.io/qt-5/qtqml-syntax-propertybinding.html)
except it will be evaluated for each of the source model's rows.  
Rows that have their expression evaluating to true will be available in this model.  
Data for each row is exposed like for a delegate of a QML View.
The expression has access to the index of the row as the `index` property, and data for each role are accessible under the `model` property.
###### Examples:
Show only even rows:  
`filterExpression: index % 2 === 0`

Show all tasks when the corresponding checkbox is checked, otherwise show only unfinished tasks :  
```qml
filterExpression: {
    if (showDoneTasksCheckBox.checked)
        return true;
    else
        return !model.done;
}
```
This expression is reevaluated for a row every time its model data changes.
When an external property (not `index` or in `model`, like `showDoneTasksCheckBox.checked` here) the expression depends on changes,
the expression is reevaluated for every row of the source model.
To capture the properties the expression depends on, the expression is first executed with invalid data and each property access is detected by the QML engine.  
This means that if a property is not accessed because of a conditional, it won't be captured and the expression won't be reevaluted when this property changes. For example with the following expression :
```qml
model.releaseYear > minYearSpinbox.value && model.releaseYear < maxYearSpinbox.value
```
The expression will be correctly reevaluated when the minimum year changes but not when the maximum year changes.  

A workaround to this problem is to put the properties the expressions depends on in `var` at the beggining of the expression : 
```qml
filterExpression: {
    var minimumYear = minYearSpinbox.value;
    var maximumYear = maxYearSpinbox.value;
    return model.releaseYear > minimumYear && model.releaseYear < maximumYear;
}
```
Note that it is superfluous here for `minimumYear` since it is accessed no matter what.
</li>

<li>
__`sortRoleName`__ : _string_  
The role name of the source model's data used for the sorting.
</li>

<li>
__`sortOrder`__ : _enumeration_  
The order the source model's data is sorted in. Can be `Qt.AscendingOrder` or `Qt.DescendingOder`.  
By default, sorting is in ascending order.
</li>

<li>
__`sortCaseSensitivity`__ : _enumeration_  
This property holds the case sensitivity setting used for comparing strings when sorting. Can be `Qt.CaseSensitive` or `Qt.CaseInsensitive`.  
By default, sorting is case sensitive.
</li>

<li>
__`isSortLocaleAware`__ : _bool_  
This property holds the local aware setting used for comparing strings when sorting.  
By default, sorting is not local aware.
</li>

<li>
__`sortExpression`__ : _expression_  
An expression to implement custom sortering, it must evaluates to a boolean. To read more in depth info about expressions, see `filterExpression`.  
Model data is accessible for both row with the `indexLeft`, `modelLeft`, `indexRight` and `modelRight` properties.  
The expression should return `true` if the value of the left item is less than the value of the right item, otherwise returns false.
</li>

Contributing
------------
Don't hesitate to open an issue about a suggestion, a bug, a lack of clarity in the documentation, etc.

Pull requests are also welcome, if it's a important change you should open an issue first though.
