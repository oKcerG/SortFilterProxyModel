import QtQuick 2.0
import QtQml 2.2
import QtTest 1.1
import SortFilterProxyModel 0.2
import SortFilterProxyModel.Test 0.2
import QtQml 2.2

Item {
    ListModel {
        id: listModel
        ListElement { test: "first"; keep: true }
        ListElement { test: "second"; keep: true }
        ListElement { test: "third"; keep: true }
    }

    SortFilterProxyModel {
        id: testModel
        sourceModel: listModel
        filters: [
            ValueFilter {
                roleName: "keep"
                value: true
            },
            ValueFilter {
                inverted: true
                roleName: "staticRole"
                value: "filterMe"
            }
        ]

        proxyRoles: [
            StaticRole {
                id: staticRole
                name: "staticRole"
                value: "foo"
            },
            SourceIndexRole {
                name: "sourceIndexRole"
            }
        ]
    }

    Instantiator {
        id: instantiator
        model: testModel
        QtObject {
            property string staticRole: model.staticRole
            property int sourceIndexRole: model.sourceIndexRole
        }
    }

    TestCase {
        name: "ProxyRoles"

        function test_proxyRole() {
            compare(instantiator.object.staticRole, "foo");
            staticRole.value = "bar";
            compare(instantiator.object.staticRole, "bar");
            compare(instantiator.object.sourceIndexRole, 0);
            compare(testModel.get(1, "sourceIndexRole"), 1);
            listModel.setProperty(1, "keep", false);
            compare(testModel.get(1, "sourceIndexRole"), 2);
            staticRole.value = "filterMe";
            compare(testModel.count, 0)
        }
    }
}
